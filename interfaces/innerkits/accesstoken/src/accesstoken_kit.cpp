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

#include "accesstoken_kit.h"

#include <string>
#include <vector>

#include "accesstoken_log.h"
#include "accesstoken_manager_client.h"
#include "data_validator.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_ACCESSTOKEN, "AccessTokenKit"};
} // namespace

AccessTokenIDEx AccessTokenKit::AllocHapToken(const HapInfoParams& info, const HapPolicyParams& policy)
{
    AccessTokenIDEx res = {0};
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if ((!DataValidator::IsUserIdValid(info.userID)) || !DataValidator::IsAppIDDescValid(info.appIDDesc) ||
        !DataValidator::IsBundleNameValid(info.bundleName) || !DataValidator::IsAplNumValid(policy.apl) ||
        !DataValidator::IsDomainValid(policy.domain) || !DataValidator::IsDlpTypeValid(info.dlpType)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "input param failed");
        return res;
    }

    return AccessTokenManagerClient::GetInstance().AllocHapToken(info, policy);
}

AccessTokenID AccessTokenKit::AllocLocalTokenID(const std::string& remoteDeviceID, AccessTokenID remoteTokenID)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called, deviceID=%{public}s tokenID=%{public}d",
        __func__, remoteDeviceID.c_str(), remoteTokenID);
#ifdef DEBUG_API_PERFORMANCE
    ACCESSTOKEN_LOG_INFO(LABEL, "api_performance:start call");
    AccessTokenID resID = AccessTokenManagerClient::GetInstance().AllocLocalTokenID(remoteDeviceID, remoteTokenID);
    ACCESSTOKEN_LOG_INFO(LABEL, "api_performance:end call");
    return resID;
#else
    return AccessTokenManagerClient::GetInstance().AllocLocalTokenID(remoteDeviceID, remoteTokenID);
#endif
}

int AccessTokenKit::UpdateHapToken(AccessTokenID tokenID, const std::string& appIDDesc, const HapPolicyParams& policy)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if ((tokenID == 0) || (!DataValidator::IsAppIDDescValid(appIDDesc)) ||
        (!DataValidator::IsAplNumValid(policy.apl))) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "input param failed");
        return RET_FAILED;
    }
    return AccessTokenManagerClient::GetInstance().UpdateHapToken(tokenID, appIDDesc, policy);
}

int AccessTokenKit::DeleteToken(AccessTokenID tokenID)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return RET_FAILED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d", tokenID);
    return AccessTokenManagerClient::GetInstance().DeleteToken(tokenID);
}

ATokenTypeEnum AccessTokenKit::GetTokenType(AccessTokenID tokenID)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return TOKEN_INVALID;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d", tokenID);
    return AccessTokenManagerClient::GetInstance().GetTokenType(tokenID);
}

ATokenTypeEnum AccessTokenKit::GetTokenTypeFlag(AccessTokenID tokenID)
{
    if (tokenID == 0) {
        return TOKEN_INVALID;
    }
    AccessTokenIDInner *idInner = reinterpret_cast<AccessTokenIDInner *>(&tokenID);
    return (ATokenTypeEnum)(idInner->type);
}

int AccessTokenKit::CheckNativeDCap(AccessTokenID tokenID, const std::string& dcap)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return RET_FAILED;
    }
    if (!DataValidator::IsDcapValid(dcap)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "dcap is invalid");
        return RET_FAILED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d, dcap=%{public}s", tokenID, dcap.c_str());
    return AccessTokenManagerClient::GetInstance().CheckNativeDCap(tokenID, dcap);
}

AccessTokenID AccessTokenKit::GetHapTokenID(int userID, const std::string& bundleName, int instIndex)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (!DataValidator::IsUserIdValid(userID) || !DataValidator::IsBundleNameValid(bundleName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "hap token param failed");
        return 0;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "int userID=%{public}d, bundleName=%{public}s, instIndex=%{public}d",
        userID, bundleName.c_str(), instIndex);
    return AccessTokenManagerClient::GetInstance().GetHapTokenID(userID, bundleName, instIndex);
}

int AccessTokenKit::GetHapTokenInfo(AccessTokenID tokenID, HapTokenInfo& hapTokenInfoRes)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return RET_FAILED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d", tokenID);

    return AccessTokenManagerClient::GetInstance().GetHapTokenInfo(tokenID, hapTokenInfoRes);
}

int AccessTokenKit::GetNativeTokenInfo(AccessTokenID tokenID, NativeTokenInfo& nativeTokenInfoRes)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d", tokenID);

    return AccessTokenManagerClient::GetInstance().GetNativeTokenInfo(tokenID, nativeTokenInfoRes);
}

PermissionOper AccessTokenKit::GetSelfPermissionsState(std::vector<PermissionListState>& permList)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "called.");
    return AccessTokenManagerClient::GetInstance().GetSelfPermissionsState(permList);
}

int AccessTokenKit::VerifyAccessToken(AccessTokenID tokenID, const std::string& permissionName)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return PERMISSION_DENIED;
    }
    if (!DataValidator::IsPermissionNameValid(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "permissionName is invalid");
        return PERMISSION_DENIED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d, permissionName=%{public}s", tokenID, permissionName.c_str());
    return AccessTokenManagerClient::GetInstance().VerifyAccessToken(tokenID, permissionName);
}

int AccessTokenKit::VerifyAccessToken(
    AccessTokenID callerTokenID, AccessTokenID firstTokenID, const std::string& permissionName)
{
    int ret = AccessTokenKit::VerifyAccessToken(callerTokenID, permissionName);
    if (ret != PERMISSION_GRANTED) {
        return ret;
    }
    if (firstTokenID == FIRSTCALLER_TOKENID_DEFAULT) {
        return ret;
    }
    return AccessTokenKit::VerifyAccessToken(firstTokenID, permissionName);
}

int AccessTokenKit::VerifyNativeToken(AccessTokenID tokenID, const std::string& permissionName)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID=%{public}d is invalid", tokenID);
        return PERMISSION_DENIED;
    }
    if (!DataValidator::IsPermissionNameValid(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "permissionName is invalid");
        return PERMISSION_DENIED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d, permissionName=%{public}s", tokenID, permissionName.c_str());
    return AccessTokenManagerClient::GetInstance().VerifyNativeToken(tokenID, permissionName);
}

int AccessTokenKit::GetDefPermission(const std::string& permissionName, PermissionDef& permissionDefResult)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (!DataValidator::IsPermissionNameValid(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "permissionName is invalid");
        return RET_FAILED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "permissionName=%{public}s", permissionName.c_str());

    int ret = AccessTokenManagerClient::GetInstance().GetDefPermission(permissionName, permissionDefResult);
    ACCESSTOKEN_LOG_INFO(LABEL, "GetDefPermission bundleName = %{public}s", permissionDefResult.bundleName.c_str());

    return ret;
}

int AccessTokenKit::GetDefPermissions(AccessTokenID tokenID, std::vector<PermissionDef>& permDefList)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return RET_FAILED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d", tokenID);

    return AccessTokenManagerClient::GetInstance().GetDefPermissions(tokenID, permDefList);
}

int AccessTokenKit::GetReqPermissions(
    AccessTokenID tokenID, std::vector<PermissionStateFull>& reqPermList, bool isSystemGrant)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return RET_FAILED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d, isSystemGrant=%{public}d", tokenID, isSystemGrant);

    return AccessTokenManagerClient::GetInstance().GetReqPermissions(tokenID, reqPermList, isSystemGrant);
}

int AccessTokenKit::GetPermissionFlag(AccessTokenID tokenID, const std::string& permissionName)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return PERMISSION_DEFAULT_FLAG;
    }
    if (!DataValidator::IsPermissionNameValid(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "permissionName is invalid");
        return PERMISSION_DEFAULT_FLAG;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d, permissionName=%{public}s", tokenID, permissionName.c_str());
    return AccessTokenManagerClient::GetInstance().GetPermissionFlag(tokenID, permissionName);
}

int AccessTokenKit::GrantPermission(AccessTokenID tokenID, const std::string& permissionName, int flag)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return RET_FAILED;
    }
    if (!DataValidator::IsPermissionNameValid(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "permissionName is invalid");
        return RET_FAILED;
    }
    if (!DataValidator::IsPermissionFlagValid(flag)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "flag is invalid");
        return RET_FAILED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d, permissionName=%{public}s, flag=%{public}d",
        tokenID, permissionName.c_str(), flag);
    return AccessTokenManagerClient::GetInstance().GrantPermission(tokenID, permissionName, flag);
}

int AccessTokenKit::RevokePermission(AccessTokenID tokenID, const std::string& permissionName, int flag)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return RET_FAILED;
    }
    if (!DataValidator::IsPermissionNameValid(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "permissionName is invalid");
        return RET_FAILED;
    }
    if (!DataValidator::IsPermissionFlagValid(flag)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "flag is invalid");
        return RET_FAILED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d, permissionName=%{public}s, flag=%{public}d",
        tokenID, permissionName.c_str(), flag);
    return AccessTokenManagerClient::GetInstance().RevokePermission(tokenID, permissionName, flag);
}

int AccessTokenKit::ClearUserGrantedPermissionState(AccessTokenID tokenID)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return RET_FAILED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d", tokenID);
    return AccessTokenManagerClient::GetInstance().ClearUserGrantedPermissionState(tokenID);
}

#ifdef TOKEN_SYNC_ENABLE
int AccessTokenKit::GetHapTokenInfoFromRemote(AccessTokenID tokenID, HapTokenInfoForSync& hapSync)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    if (tokenID == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "tokenID is invalid");
        return RET_FAILED;
    }
    ACCESSTOKEN_LOG_INFO(LABEL, "tokenID=%{public}d", tokenID);

    return AccessTokenManagerClient::GetInstance().GetHapTokenInfoFromRemote(tokenID, hapSync);
}

int AccessTokenKit::GetAllNativeTokenInfo(std::vector<NativeTokenInfoForSync>& nativeTokenInfosRes)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);

    return AccessTokenManagerClient::GetInstance().GetAllNativeTokenInfo(nativeTokenInfosRes);
}

int AccessTokenKit::SetRemoteHapTokenInfo(const std::string& deviceID,
    const HapTokenInfoForSync& hapSync)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called, deviceID=%{public}s tokenID=%{public}d",
        __func__, deviceID.c_str(), hapSync.baseInfo.tokenID);
    return AccessTokenManagerClient::GetInstance().SetRemoteHapTokenInfo(deviceID, hapSync);
}

int AccessTokenKit::SetRemoteNativeTokenInfo(const std::string& deviceID,
    std::vector<NativeTokenInfoForSync>& nativeTokenInfoList)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called, deviceID=%{public}s", __func__, deviceID.c_str());
    return AccessTokenManagerClient::GetInstance()
        .SetRemoteNativeTokenInfo(deviceID, nativeTokenInfoList);
}

int AccessTokenKit::DeleteRemoteToken(const std::string& deviceID, AccessTokenID tokenID)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called, deviceID=%{public}s tokenID=%{public}d",
        __func__, deviceID.c_str(), tokenID);
    return AccessTokenManagerClient::GetInstance().DeleteRemoteToken(deviceID, tokenID);
}

int AccessTokenKit::DeleteRemoteDeviceTokens(const std::string& deviceID)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called, deviceID=%{public}s", __func__, deviceID.c_str());
    return AccessTokenManagerClient::GetInstance().DeleteRemoteDeviceTokens(deviceID);
}

AccessTokenID AccessTokenKit::GetRemoteNativeTokenID(const std::string& deviceID, AccessTokenID tokenID)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called, deviceID=%{public}s tokenID=%{public}d",
        __func__, deviceID.c_str(), tokenID);
    return AccessTokenManagerClient::GetInstance().GetRemoteNativeTokenID(deviceID, tokenID);
}
#endif

void AccessTokenKit::DumpTokenInfo(std::string& dumpInfo)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "%{public}s called", __func__);
    AccessTokenManagerClient::GetInstance().DumpTokenInfo(dumpInfo);
}
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
