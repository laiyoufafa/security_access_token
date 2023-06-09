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

#include "accesstoken_manager_proxy.h"

#include "accesstoken_log.h"

#include "parcel.h"
#include "string_ex.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_ACCESSTOKEN, "AccessTokenManagerProxy"};
}

AccessTokenManagerProxy::AccessTokenManagerProxy(const sptr<IRemoteObject>& impl)
    : IRemoteProxy<IAccessTokenManager>(impl) {
}

AccessTokenManagerProxy::~AccessTokenManagerProxy()
{}

int AccessTokenManagerProxy::VerifyAccessToken(AccessTokenID tokenID, const std::string& permissionName)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return PERMISSION_DENIED;
    }
    if (!data.WriteString(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write permissionName");
        return PERMISSION_DENIED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return PERMISSION_DENIED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::VERIFY_ACCESSTOKEN), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return PERMISSION_DENIED;
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::VerifyNativeToken(AccessTokenID tokenID, const std::string& permissionName)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return PERMISSION_DENIED;
    }
    if (!data.WriteString(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write permissionName");
        return PERMISSION_DENIED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return PERMISSION_DENIED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::VERIFY_NATIVETOKEN), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return PERMISSION_DENIED;
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::GetDefPermission(
    const std::string& permissionName, PermissionDefParcel& permissionDefResult)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteString(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write permissionName");
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_DEF_PERMISSION), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    sptr<PermissionDefParcel> resultSptr = reply.ReadParcelable<PermissionDefParcel>();
    if (resultSptr == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "read permission def parcel fail");
        return RET_FAILED;
    }
    permissionDefResult = *resultSptr;
    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::GetDefPermissions(AccessTokenID tokenID,
    std::vector<PermissionDefParcel>& permList)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_DEF_PERMISSIONS), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int32_t size = reply.ReadInt32();
    if (size > MAX_PERMISSION_SIZE) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "size = %{public}d get from request is invalid", size);
        return RET_FAILED;
    }
    for (int i = 0; i < size; i++) {
        sptr<PermissionDefParcel> permissionDef = reply.ReadParcelable<PermissionDefParcel>();
        if (permissionDef != nullptr) {
            permList.emplace_back(*permissionDef);
        }
    }
    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::GetReqPermissions(
    AccessTokenID tokenID, std::vector<PermissionStateFullParcel>& reqPermList, bool isSystemGrant)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return RET_FAILED;
    }
    if (!data.WriteInt32(isSystemGrant)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write isSystemGrant");
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_REQ_PERMISSIONS), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int32_t size = reply.ReadInt32();
    if (size > MAX_PERMISSION_SIZE) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "size = %{public}d get from request is invalid", size);
        return RET_FAILED;
    }
    for (int i = 0; i < size; i++) {
        sptr<PermissionStateFullParcel> permissionReq = reply.ReadParcelable<PermissionStateFullParcel>();
        if (permissionReq != nullptr) {
            reqPermList.emplace_back(*permissionReq);
        }
    }
    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::GetPermissionFlag(AccessTokenID tokenID, const std::string& permissionName)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return PERMISSION_DEFAULT_FLAG;
    }
    if (!data.WriteString(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write permissionName");
        return PERMISSION_DEFAULT_FLAG;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return PERMISSION_DEFAULT_FLAG;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_PERMISSION_FLAG), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return PERMISSION_DEFAULT_FLAG;
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

PermissionOper AccessTokenManagerProxy::GetSelfPermissionsState(
    std::vector<PermissionListStateParcel>& permListParcel)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(permListParcel.size())) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write permListParcel size.");
        return INVALID_OPER;
    }
    for (auto permission : permListParcel) {
        if (!data.WriteParcelable(&permission)) {
            ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write permListParcel.");
            return INVALID_OPER;
        }
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return INVALID_OPER;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_PERMISSION_OPER_STATE), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d.", requestResult);
        return INVALID_OPER;
    }

    PermissionOper result = static_cast<PermissionOper>(reply.ReadInt32());
    if (result == INVALID_OPER) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "result from server is invalid!");
        return result;
    }
    size_t size = reply.ReadUint32();
    if (size != permListParcel.size()) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "permListParcel size from server is invalid!");
        return INVALID_OPER;
    }
    for (uint32_t i = 0; i < size; i++) {
        sptr<PermissionListStateParcel> permissionReq = reply.ReadParcelable<PermissionListStateParcel>();
        if (permissionReq != nullptr) {
            permListParcel[i].permsState.state = permissionReq->permsState.state;
        }
    }

    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::GrantPermission(AccessTokenID tokenID, const std::string& permissionName, int flag)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return RET_FAILED;
    }
    if (!data.WriteString(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write permissionName");
        return RET_FAILED;
    }
    if (!data.WriteInt32(flag)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write flag");
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GRANT_PERMISSION), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::RevokePermission(AccessTokenID tokenID, const std::string& permissionName, int flag)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return RET_FAILED;
    }
    if (!data.WriteString(permissionName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write permissionName");
        return RET_FAILED;
    }
    if (!data.WriteInt32(flag)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write flag");
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::REVOKE_PERMISSION), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::ClearUserGrantedPermissionState(AccessTokenID tokenID)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::CLEAR_USER_GRANT_PERMISSION), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

AccessTokenIDEx AccessTokenManagerProxy::AllocHapToken(
    const HapInfoParcel& hapInfo, const HapPolicyParcel& policyParcel)
{
    MessageParcel data;
    AccessTokenIDEx res;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());

    if (!data.WriteParcelable(&hapInfo)) {
        res.tokenIDEx = 0;
        return res;
    }
    if (!data.WriteParcelable(&policyParcel)) {
        res.tokenIDEx = 0;
        return res;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        res.tokenIDEx = 0;
        return res;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::ALLOC_TOKEN_HAP), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        res.tokenIDEx = 0;
        return res;
    }

    unsigned long long result = reply.ReadUint64();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}llu", result);
    res.tokenIDEx = result;
    return res;
}

int AccessTokenManagerProxy::DeleteToken(AccessTokenID tokenID)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());

    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::TOKEN_DELETE), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::GetTokenType(AccessTokenID tokenID)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());

    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_TOKEN_TYPE), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::CheckNativeDCap(AccessTokenID tokenID, const std::string& dcap)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());

    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return RET_FAILED;
    }
    if (!data.WriteString(dcap)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write dcap");
        return RET_FAILED;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::CHECK_NATIVE_DCAP), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

AccessTokenID AccessTokenManagerProxy::GetHapTokenID(int userID, const std::string& bundleName, int instIndex)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());

    if (!data.WriteInt32(userID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write tokenID");
        return 0;
    }
    if (!data.WriteString(bundleName)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write dcap");
        return 0;
    }
    if (!data.WriteInt32(instIndex)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write dcap");
        return 0;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return 0;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_HAP_TOKEN_ID), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return 0;
    }

    int result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "result from server data = %{public}d", result);
    return result;
}

AccessTokenID AccessTokenManagerProxy::AllocLocalTokenID(
    const std::string& remoteDeviceID, AccessTokenID remoteTokenID)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());

    if (!data.WriteString(remoteDeviceID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write dcap");
        return 0;
    }
    if (!data.WriteUint32(remoteTokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write dcap");
        return 0;
    }
    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return 0;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::ALLOC_LOCAL_TOKEN_ID), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "request fail, result: %{public}d", requestResult);
        return 0;
    }

    AccessTokenID result = reply.ReadUint32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "get result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::GetNativeTokenInfo(AccessTokenID tokenID, NativeTokenInfoParcel& nativeTokenInfoRes)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write permissionName");
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_NATIVE_TOKENINFO), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "send request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    sptr<NativeTokenInfoParcel> resultSptr = reply.ReadParcelable<NativeTokenInfoParcel>();
    if (resultSptr == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "ReadParcelable fail");
        return RET_FAILED;
    }
    nativeTokenInfoRes = *resultSptr;
    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "get result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::GetHapTokenInfo(AccessTokenID tokenID, HapTokenInfoParcel& hapTokenInfoRes)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "Failed to write permissionName");
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_HAP_TOKENINFO), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "send request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    sptr<HapTokenInfoParcel> resultSptr = reply.ReadParcelable<HapTokenInfoParcel>();
    if (resultSptr == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "ReadParcelable fail");
        return RET_FAILED;
    }
    hapTokenInfoRes = *resultSptr;
    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "get result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::UpdateHapToken(AccessTokenID tokenID,
                                            const std::string& appIDDesc, const HapPolicyParcel& policyParcel)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        return RET_FAILED;
    }
    if (!data.WriteString(appIDDesc)) {
        return RET_FAILED;
    }
    if (!data.WriteParcelable(&policyParcel)) {
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::UPDATE_HAP_TOKEN), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "send request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "get result from server data = %{public}d", result);
    return result;
}

#ifdef TOKEN_SYNC_ENABLE
int AccessTokenManagerProxy::GetHapTokenInfoFromRemote(AccessTokenID tokenID,
    HapTokenInfoForSyncParcel& hapSyncParcel)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteUint32(tokenID)) {
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_HAP_TOKEN_FROM_REMOTE), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "send request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    sptr<HapTokenInfoForSyncParcel> hapResult = reply.ReadParcelable<HapTokenInfoForSyncParcel>();
    if (hapResult == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "ReadParcelable fail");
        return RET_FAILED;
    }
    hapSyncParcel = *hapResult;

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "get result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::GetAllNativeTokenInfo(std::vector<NativeTokenInfoForSyncParcel>& nativeTokenInfoRes)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_ALL_NATIVE_TOKEN_FROM_REMOTE),
        data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "send request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int32_t size = reply.ReadInt32();
    if (size > MAX_NATIVE_TOKEN_INFO_SIZE) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "size = %{public}d get from request is invalid", size);
        return RET_FAILED;
    }
    for (int i = 0; i < size; i++) {
        sptr<NativeTokenInfoForSyncParcel> nativeResult = reply.ReadParcelable<NativeTokenInfoForSyncParcel>();
        if (nativeResult != nullptr) {
            nativeTokenInfoRes.emplace_back(*nativeResult);
        }
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "get result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::SetRemoteHapTokenInfo(const std::string& deviceID,
    HapTokenInfoForSyncParcel& hapSyncParcel)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteString(deviceID)) {
        return RET_FAILED;
    }
    if (!data.WriteParcelable(&hapSyncParcel)) {
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::SET_REMOTE_HAP_TOKEN_INFO), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "send request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "get result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::SetRemoteNativeTokenInfo(const std::string& deviceID,
    std::vector<NativeTokenInfoForSyncParcel>& nativeTokenInfoForSyncParcel)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteString(deviceID)) {
        return RET_FAILED;
    }
    if (!data.WriteUint32(nativeTokenInfoForSyncParcel.size())) {
        return RET_FAILED;
    }
    for (NativeTokenInfoForSyncParcel& parcel : nativeTokenInfoForSyncParcel) {
        if (!data.WriteParcelable(&parcel)) {
            return RET_FAILED;
        }
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::SET_REMOTE_NATIVE_TOKEN_INFO), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "send request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "get result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::DeleteRemoteToken(const std::string& deviceID, AccessTokenID tokenID)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteString(deviceID)) {
        return RET_FAILED;
    }

    if (!data.WriteUint32(tokenID)) {
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::DELETE_REMOTE_TOKEN_INFO), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "send request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "get result from server data = %{public}d", result);
    return result;
}

AccessTokenID AccessTokenManagerProxy::GetRemoteNativeTokenID(const std::string& deviceID, AccessTokenID tokenID)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteString(deviceID)) {
        return 0;
    }

    if (!data.WriteUint32(tokenID)) {
        return 0;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return 0;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::GET_NATIVE_REMOTE_TOKEN), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "send request fail, result: %{public}d", requestResult);
        return 0;
    }

    AccessTokenID result = reply.ReadUint32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "get result from server data = %{public}d", result);
    return result;
}

int AccessTokenManagerProxy::DeleteRemoteDeviceTokens(const std::string& deviceID)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
    if (!data.WriteString(deviceID)) {
        return RET_FAILED;
    }

    MessageParcel reply;
    MessageOption option(MessageOption::TF_SYNC);
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "remote service null.");
        return RET_FAILED;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::DELETE_REMOTE_DEVICE_TOKEN), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "send request fail, result: %{public}d", requestResult);
        return RET_FAILED;
    }

    int32_t result = reply.ReadInt32();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "get result from server data = %{public}d", result);
    return result;
}
#endif

void AccessTokenManagerProxy::DumpTokenInfo(std::string& dumpInfo)
{
    MessageParcel data;
    data.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());

    MessageParcel reply;
    MessageOption option;
    sptr<IRemoteObject> remote = Remote();
    if (remote == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "%{public}s: remote service null.", __func__);
        return;
    }
    int32_t requestResult = remote->SendRequest(
        static_cast<uint32_t>(IAccessTokenManager::InterfaceCode::DUMP_TOKENINFO), data, reply, option);
    if (requestResult != NO_ERROR) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "%{public}s send request fail, result: %{public}d", __func__, requestResult);
        return;
    }

    dumpInfo = reply.ReadString();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "%{public}s get result from server dumpInfo = %{public}s", __func__, dumpInfo.c_str());
}
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
