/*
 * Copyright (c) 2021-2022 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "permission_policy_set.h"

#include "accesstoken_log.h"
#include "data_storage.h"
#include "data_translator.h"
#include "field_const.h"
#include "permission_definition_cache.h"
#include "permission_validator.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_ACCESSTOKEN, "PermissionPolicySet"};
}

PermissionPolicySet::~PermissionPolicySet()
{
    ACCESSTOKEN_LOG_DEBUG(LABEL,
        "%{public}s called, tokenID: 0x%{public}x destruction", __func__, tokenId_);
}

std::shared_ptr<PermissionPolicySet> PermissionPolicySet::BuildPermissionPolicySet(
    AccessTokenID tokenId, const std::vector<PermissionDef>& permList,
    const std::vector<PermissionStateFull>& permStateList)
{
    std::shared_ptr<PermissionPolicySet> policySet = std::make_shared<PermissionPolicySet>();
    if (policySet != nullptr) {
        PermissionValidator::FilterInvalidPermisionDef(permList, policySet->permList_);
        PermissionValidator::FilterInvalidPermisionState(permStateList, policySet->permStateList_);
        policySet->tokenId_ = tokenId;
    }
    return policySet;
}

void PermissionPolicySet::UpdatePermStateFull(const PermissionStateFull& permOld, PermissionStateFull& permNew)
{
    if (permNew.isGeneral == permOld.isGeneral) {
        permNew.resDeviceID = permOld.resDeviceID;
        permNew.grantStatus = permOld.grantStatus;
        permNew.grantFlags = permOld.grantFlags;
    }
}

void PermissionPolicySet::Update(const std::vector<PermissionDef>& permList,
    const std::vector<PermissionStateFull>& permStateList)
{
    std::vector<PermissionDef> permFilterList;
    std::vector<PermissionStateFull> permStateFilterList;

    PermissionValidator::FilterInvalidPermisionDef(permList, permFilterList);
    PermissionValidator::FilterInvalidPermisionState(permStateList, permStateFilterList);

    Utils::UniqueWriteGuard<Utils::RWLock> infoGuard(this->permPolicySetLock_);
    for (const PermissionDef& permNew : permFilterList) {
        bool found = false;
        for (PermissionDef& permOld : permList_) {
            if (permNew.permissionName == permOld.permissionName) {
                permOld = permNew;
                found = true;
                break;
            }
        }
        if (!found) {
            permList_.emplace_back(permNew);
        }
    }

    for (PermissionStateFull& permStateNew : permStateFilterList) {
        for (const PermissionStateFull& permStateOld : permStateList_) {
            if (permStateNew.permissionName == permStateOld.permissionName) {
                UpdatePermStateFull(permStateOld, permStateNew);
                break;
            }
        }
    }
    permStateList_ = permStateFilterList;
}

std::shared_ptr<PermissionPolicySet> PermissionPolicySet::RestorePermissionPolicy(AccessTokenID tokenId,
    const std::vector<GenericValues>& permDefRes, const std::vector<GenericValues>& permStateRes)
{
    std::shared_ptr<PermissionPolicySet> policySet = std::make_shared<PermissionPolicySet>();
    if (policySet == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenId 0x%{public}x new failed.", tokenId);
        return nullptr;
    }
    policySet->tokenId_ = tokenId;

    for (GenericValues defValue : permDefRes) {
        if ((AccessTokenID)defValue.GetInt(FIELD_TOKEN_ID) == tokenId) {
            PermissionDef def;
            int ret = DataTranslator::TranslationIntoPermissionDef(defValue, def);
            if (ret == RET_SUCCESS) {
                policySet->permList_.emplace_back(def);
            } else {
                ACCESSTOKEN_LOG_ERROR(LABEL, "tokenId 0x%{public}x permDef is wrong.", tokenId);
            }
        }
    }

    for (GenericValues stateValue : permStateRes) {
        if ((AccessTokenID)stateValue.GetInt(FIELD_TOKEN_ID) == tokenId) {
            PermissionStateFull state;
            int ret = DataTranslator::TranslationIntoPermissionStateFull(stateValue, state);
            if (ret == RET_SUCCESS) {
                MergePermissionStateFull(policySet->permStateList_, state);
            } else {
                ACCESSTOKEN_LOG_ERROR(LABEL, "tokenId 0x%{public}x permState is wrong.", tokenId);
            }
        }
    }
    return policySet;
}

void PermissionPolicySet::MergePermissionStateFull(std::vector<PermissionStateFull>& permStateList,
    const PermissionStateFull& state)
{
    for (auto iter = permStateList.begin(); iter != permStateList.end(); iter++) {
        if (state.permissionName == iter->permissionName) {
            iter->resDeviceID.emplace_back(state.resDeviceID[0]);
            iter->grantStatus.emplace_back(state.grantStatus[0]);
            iter->grantFlags.emplace_back(state.grantFlags[0]);
            return;
        }
    }
    permStateList.emplace_back(state);
}

void PermissionPolicySet::StorePermissionDef(std::vector<GenericValues>& valueList) const
{
    for (auto permissionDef : permList_) {
        GenericValues genericValues;
        genericValues.Put(FIELD_TOKEN_ID, tokenId_);
        DataTranslator::TranslationIntoGenericValues(permissionDef, genericValues);
        valueList.emplace_back(genericValues);
    }
}

void PermissionPolicySet::StorePermissionState(std::vector<GenericValues>& valueList) const
{
    for (auto permissionState : permStateList_) {
        if (permissionState.isGeneral) {
            GenericValues genericValues;
            genericValues.Put(FIELD_TOKEN_ID, tokenId_);
            DataTranslator::TranslationIntoGenericValues(permissionState, 0, genericValues);
            valueList.emplace_back(genericValues);
            continue;
        }

        unsigned int stateSize = permissionState.resDeviceID.size();
        for (unsigned int i = 0; i < stateSize; i++) {
            GenericValues genericValues;
            genericValues.Put(FIELD_TOKEN_ID, tokenId_);
            DataTranslator::TranslationIntoGenericValues(permissionState, i, genericValues);
            valueList.emplace_back(genericValues);
        }
    }
}

void PermissionPolicySet::StorePermissionPolicySet(std::vector<GenericValues>& permDefValueList,
    std::vector<GenericValues>& permStateValueList)
{
    Utils::UniqueReadGuard<Utils::RWLock> infoGuard(this->permPolicySetLock_);
    StorePermissionDef(permDefValueList);
    StorePermissionState(permStateValueList);
}

int PermissionPolicySet::VerifyPermissStatus(const std::string& permissionName)
{
    Utils::UniqueReadGuard<Utils::RWLock> infoGuard(this->permPolicySetLock_);
    for (auto perm : permStateList_) {
        if (perm.permissionName == permissionName) {
            if (perm.isGeneral) {
                return perm.grantStatus[0];
            } else {
                return PERMISSION_DENIED;
            }
        }
    }
    return PERMISSION_DENIED;
}

void PermissionPolicySet::GetDefPermissions(std::vector<PermissionDef>& permList)
{
    Utils::UniqueReadGuard<Utils::RWLock> infoGuard(this->permPolicySetLock_);
    permList.assign(permList_.begin(), permList_.end());
}

void PermissionPolicySet::GetPermissionStateFulls(std::vector<PermissionStateFull>& permList)
{
    Utils::UniqueReadGuard<Utils::RWLock> infoGuard(this->permPolicySetLock_);
    permList.assign(permStateList_.begin(), permStateList_.end());
}

int PermissionPolicySet::QueryPermissionFlag(const std::string& permissionName)
{
    Utils::UniqueReadGuard<Utils::RWLock> infoGuard(this->permPolicySetLock_);
    for (auto perm : permStateList_) {
        if (perm.permissionName == permissionName) {
            if (perm.isGeneral) {
                return perm.grantFlags[0];
            } else {
                return PERMISSION_DEFAULT_FLAG;
            }
        }
    }
    return PERMISSION_DEFAULT_FLAG;
}

void PermissionPolicySet::UpdatePermissionStatus(const std::string& permissionName, bool isGranted, int flag)
{
    Utils::UniqueWriteGuard<Utils::RWLock> infoGuard(this->permPolicySetLock_);
    for (auto& perm : permStateList_) {
        if (perm.permissionName == permissionName) {
            if (perm.isGeneral) {
                perm.grantStatus[0] = isGranted ? PERMISSION_GRANTED : PERMISSION_DENIED;
                perm.grantFlags[0] = flag;
            } else {
                return;
            }
        }
    }
}

void PermissionPolicySet::GetPermissionStateList(std::vector<PermissionStateFull>& stateList)
{
    Utils::UniqueReadGuard<Utils::RWLock> infoGuard(this->permPolicySetLock_);
    for (auto& state : permStateList_) {
        stateList.emplace_back(state);
    }
}

void PermissionPolicySet::PermDefToString(const PermissionDef& def, std::string& info) const
{
    info.append(R"(    {)");
    info.append("\n");
    info.append(R"(      "permissionName": ")" + def.permissionName + R"(")" + ",\n");
    info.append(R"(      "bundleName": ")" + def.bundleName + R"(")" + ",\n");
    info.append(R"(      "grantMode": )" + std::to_string(def.grantMode) + ",\n");
    info.append(R"(      "availableLevel": )" + std::to_string(def.availableLevel) + ",\n");
    info.append(R"(      "provisionEnable": )" + std::to_string(def.provisionEnable) + ",\n");
    info.append(R"(      "distributedSceneEnable": )" + std::to_string(def.distributedSceneEnable) + ",\n");
    info.append(R"(      "label": ")" + def.label + R"(")" + ",\n");
    info.append(R"(      "labelId": )" + std::to_string(def.labelId) + ",\n");
    info.append(R"(      "description": ")" + def.description + R"(")" + ",\n");
    info.append(R"(      "descriptionId": )" + std::to_string(def.descriptionId) + ",\n");
    info.append(R"(    })");
}

void PermissionPolicySet::PermStateFullToString(const PermissionStateFull& state, std::string& info) const
{
    info.append(R"(    {)");
    info.append("\n");
    info.append(R"(      "permissionName": ")" + state.permissionName + R"(")" + ",\n");
    info.append(R"(      "isGeneral": )" + std::to_string(state.isGeneral) + ",\n");
    info.append(R"(      "resDeviceIDList": [ )");
    for (auto iter = state.resDeviceID.begin(); iter != state.resDeviceID.end(); iter++) {
        info.append("\n");
        info.append(R"(        { "resDeviceID": ")" + *iter + R"(")" + " }");
        if (iter != (state.resDeviceID.end() - 1)) {
            info.append(",");
        }
    }
    info.append("\n      ],\n");

    info.append(R"(      "grantStatusList": [)");
    for (auto iter = state.grantStatus.begin(); iter != state.grantStatus.end(); iter++) {
        info.append("\n");
        info.append(R"(        { "grantStatus": )" + std::to_string(*iter) + " }");
        if (iter != (state.grantStatus.end() - 1)) {
            info.append(",");
        }
    }
    info.append("\n      ],\n");

    info.append(R"(      "grantFlagsList": [)");
    for (auto iter = state.grantFlags.begin(); iter != state.grantFlags.end(); iter++) {
        info.append("\n");
        info.append(R"(        { "grantFlag": )" + std::to_string(*iter) + " }");
        if (iter != (state.grantFlags.end() - 1)) {
            info.append(",");
        }
    }
    info.append("\n      ],\n");

    info.append(R"(    })");
}

void PermissionPolicySet::ToString(std::string& info)
{
    Utils::UniqueReadGuard<Utils::RWLock> infoGuard(this->permPolicySetLock_);
    info.append(R"(  "permDefList": [)");
    info.append("\n");
    for (auto iter = permList_.begin(); iter != permList_.end(); iter++) {
        PermDefToString(*iter, info);
        if (iter != (permList_.end() - 1)) {
            info.append(",\n");
        }
    }
    info.append("\n  ],\n");

    info.append(R"(  "permStateList": [)");
    info.append("\n");
    for (auto iter = permStateList_.begin(); iter != permStateList_.end(); iter++) {
        PermStateFullToString(*iter, info);
        if (iter != (permStateList_.end() - 1)) {
            info.append(",\n");
        }
    }
    info.append("\n  ]\n");
}

bool PermissionPolicySet::IsPermissionReqValid(int32_t tokenApl, const std::string &permissionName)
{
    PermissionDef permissionDef;
    int ret = PermissionDefinitionCache::GetInstance().FindByPermissionName(
        permissionName, permissionDef);
    if (ret != RET_SUCCESS) {
        return false;
    }
    if (tokenApl < permissionDef.availableLevel) {
        return false;
    }
    return true;
}

void PermissionPolicySet::PermStateToString(int32_t tokenApl, std::string& info)
{
    Utils::UniqueReadGuard<Utils::RWLock> infoGuard(this->permPolicySetLock_);

    std::vector<std::string> invaildPermList = {};
    info.append(R"(  "permStateList": [)");
    info.append("\n");
    for (auto iter = permStateList_.begin(); iter != permStateList_.end(); iter++) {
        if (!IsPermissionReqValid(tokenApl, iter->permissionName)) {
            invaildPermList.emplace_back(iter->permissionName);
            continue;
        }
        PermStateFullToString(*iter, info);
        if (iter != (permStateList_.end() - 1)) {
            info.append(",\n");
        }
    }
    info.append("\n  ]\n");

    if (invaildPermList.size() == 0) {
        return;
    }

    info.append(R"(  "invaildPermList": [)");
    info.append("\n");
    for (auto iter = invaildPermList.begin(); iter != invaildPermList.end(); iter++) {
        info.append(R"(      "permissionName": ")" + *iter + R"(")" + ",\n");
    }
    info.append("\n  ]\n");
}
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
