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

#include "native_token_info_inner.h"

#include "data_translator.h"
#include "data_validator.h"
#include "field_const.h"
#include "nlohmann/json.hpp"

#include "accesstoken_log.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_ACCESSTOKEN, "NativeTokenInfoInner"};
}

NativeTokenInfoInner::NativeTokenInfoInner() : isRemote_(false)
{
    tokenInfoBasic_.ver = DEFAULT_TOKEN_VERSION;
    tokenInfoBasic_.tokenID = 0;
    tokenInfoBasic_.tokenAttr = 0;
    tokenInfoBasic_.apl = APL_NORMAL;
}

NativeTokenInfoInner::NativeTokenInfoInner(NativeTokenInfo& native,
    const std::vector<PermissionStateFull>& permStateList) : isRemote_(false)
{
    tokenInfoBasic_ = native;
    std::vector<PermissionDef> permDefList = {};
    permPolicySet_ = PermissionPolicySet::BuildPermissionPolicySet(native.tokenID,
        permDefList, permStateList);
}

NativeTokenInfoInner::~NativeTokenInfoInner()
{
    ACCESSTOKEN_LOG_DEBUG(LABEL,
        "tokenID: %{public}u destruction", tokenInfoBasic_.tokenID);
}

int NativeTokenInfoInner::Init(AccessTokenID id, const std::string& processName,
    int apl, const std::vector<std::string>& dcap,
    const std::vector<PermissionStateFull> &permStateList)
{
    tokenInfoBasic_.tokenID = id;
    if (!DataValidator::IsProcessNameValid(processName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL,
            "tokenID: %{public}u process name is null", tokenInfoBasic_.tokenID);
        return RET_FAILED;
    }
    tokenInfoBasic_.processName = processName;
    if (!DataValidator::IsAplNumValid(apl)) {
        ACCESSTOKEN_LOG_ERROR(LABEL,
            "tokenID: %{public}u init failed, apl %{public}d is invalid",
            tokenInfoBasic_.tokenID, apl);
        return RET_FAILED;
    }
    tokenInfoBasic_.apl = (ATokenAplEnum)apl;
    tokenInfoBasic_.dcap = dcap;

    std::vector<PermissionDef> permDefList = {};
    permPolicySet_ = PermissionPolicySet::BuildPermissionPolicySet(id,
        permDefList, permStateList);
    return RET_SUCCESS;
}

std::string NativeTokenInfoInner::DcapToString(const std::vector<std::string>& dcap) const
{
    std::string dcapStr;
    for (auto iter = dcap.begin(); iter != dcap.end(); iter++) {
        dcapStr.append(*iter);
        if (iter != (dcap.end() - 1)) {
            dcapStr.append(",");
        }
    }
    return dcapStr;
}

int NativeTokenInfoInner::TranslationIntoGenericValues(GenericValues& outGenericValues) const
{
    outGenericValues.Put(FIELD_TOKEN_ID, tokenInfoBasic_.tokenID);
    outGenericValues.Put(FIELD_PROCESS_NAME, tokenInfoBasic_.processName);
    outGenericValues.Put(FIELD_APL, tokenInfoBasic_.apl);
    outGenericValues.Put(FIELD_TOKEN_VERSION, tokenInfoBasic_.ver);
    outGenericValues.Put(FIELD_DCAP, DcapToString(tokenInfoBasic_.dcap));
    outGenericValues.Put(FIELD_TOKEN_ATTR, tokenInfoBasic_.tokenAttr);

    return RET_SUCCESS;
}

int NativeTokenInfoInner::RestoreNativeTokenInfo(AccessTokenID tokenId, const GenericValues& inGenericValues,
    const std::vector<GenericValues>& permStateRes)
{
    tokenInfoBasic_.tokenID = tokenId;
    tokenInfoBasic_.processName = inGenericValues.GetString(FIELD_PROCESS_NAME);
    if (!DataValidator::IsProcessNameValid(tokenInfoBasic_.processName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL,
            "tokenID: %{public}u process name is null", tokenInfoBasic_.tokenID);
        return RET_FAILED;
    }
    int aplNum = inGenericValues.GetInt(FIELD_APL);
    if (!DataValidator::IsAplNumValid(aplNum)) {
        ACCESSTOKEN_LOG_ERROR(LABEL,
            "tokenID: %{public}u apl is error, value %{public}d",
            tokenInfoBasic_.tokenID, aplNum);
        return RET_FAILED;
    }
    tokenInfoBasic_.apl = (ATokenAplEnum)aplNum;
    tokenInfoBasic_.ver = (char)inGenericValues.GetInt(FIELD_TOKEN_VERSION);
    if (tokenInfoBasic_.ver != DEFAULT_TOKEN_VERSION) {
        ACCESSTOKEN_LOG_ERROR(LABEL,
            "tokenID: %{public}u version is error, version %{public}d",
            tokenInfoBasic_.tokenID, tokenInfoBasic_.ver);
        return RET_FAILED;
    }

    SetDcaps(inGenericValues.GetString(FIELD_DCAP));
    tokenInfoBasic_.tokenAttr = (uint32_t)inGenericValues.GetInt(FIELD_TOKEN_ATTR);

    std::vector<GenericValues> permDefRes = {};
    permPolicySet_ = PermissionPolicySet::RestorePermissionPolicy(tokenId,
        permDefRes, permStateRes);
    return RET_SUCCESS;
}

void NativeTokenInfoInner::TranslateToNativeTokenInfo(NativeTokenInfo& InfoParcel) const
{
    InfoParcel.apl = tokenInfoBasic_.apl;
    InfoParcel.ver = tokenInfoBasic_.ver;
    InfoParcel.processName = tokenInfoBasic_.processName;
    InfoParcel.dcap = tokenInfoBasic_.dcap;
    InfoParcel.tokenID = tokenInfoBasic_.tokenID;
    InfoParcel.tokenAttr = tokenInfoBasic_.tokenAttr;
}

void NativeTokenInfoInner::StoreNativeInfo(std::vector<GenericValues>& valueList,
    std::vector<GenericValues>& permStateValues) const
{
    if (isRemote_) {
        return;
    }
    GenericValues genericValues;
    TranslationIntoGenericValues(genericValues);
    valueList.emplace_back(genericValues);

    if (permPolicySet_ != nullptr) {
        std::vector<GenericValues> permDefValues;
        permPolicySet_->StorePermissionPolicySet(permDefValues, permStateValues);
    }
}

AccessTokenID NativeTokenInfoInner::GetTokenID() const
{
    return tokenInfoBasic_.tokenID;
}

std::vector<std::string> NativeTokenInfoInner::GetDcap() const
{
    return tokenInfoBasic_.dcap;
}

std::string NativeTokenInfoInner::GetProcessName() const
{
    return tokenInfoBasic_.processName;
}

std::shared_ptr<PermissionPolicySet> NativeTokenInfoInner::GetNativeInfoPermissionPolicySet() const
{
    return permPolicySet_;
}

bool NativeTokenInfoInner::IsRemote() const
{
    return isRemote_;
}

void NativeTokenInfoInner::SetRemote(bool isRemote)
{
    isRemote_ = isRemote;
}

void NativeTokenInfoInner::SetDcaps(const std::string& dcapStr)
{
    std::string::size_type start = 0;
    while (true) {
        std::string::size_type offset = dcapStr.find(',', start);
        if (offset == std::string::npos) {
            tokenInfoBasic_.dcap.push_back(dcapStr.substr(start));
            break;
        }
        tokenInfoBasic_.dcap.push_back(dcapStr.substr(start, offset));
        start = offset + 1;
    }
}

void NativeTokenInfoInner::ToString(std::string& info) const
{
    info.append(R"({)");
    info.append("\n");
    info.append(R"(  "tokenID": )" + std::to_string(tokenInfoBasic_.tokenID) + ",\n");
    info.append(R"(  "tokenAttr": )" + std::to_string(tokenInfoBasic_.tokenAttr) + ",\n");
    info.append(R"(  "ver": )" + std::to_string(tokenInfoBasic_.ver) + ",\n");
    info.append(R"(  "processName": ")" + tokenInfoBasic_.processName + R"(")" + ",\n");
    info.append(R"(  "apl": )" + std::to_string(tokenInfoBasic_.apl) + ",\n");
    info.append(R"(  "dcap": ")" + DcapToString(tokenInfoBasic_.dcap) + R"(")" + ",\n");
    info.append(R"(  "isRemote": )" + std::to_string(isRemote_? 1 : 0) + ",\n");
    if (permPolicySet_ != nullptr) {
        permPolicySet_->PermStateToString(tokenInfoBasic_.apl, info);
    }
    info.append("}");
}
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
