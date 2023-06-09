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
#include "napi_atmanager.h"

#include <cstdio>
#include <cstring>
#include <pthread.h>
#include <unistd.h>

#include "accesstoken_kit.h"
#include "accesstoken_log.h"
#include "napi/native_api.h"
#include "napi/native_node_api.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {
    LOG_CORE, SECURITY_DOMAIN_ACCESSTOKEN, "AccessTokenAbilityAccessCtrl"
};
} // namespace

void NapiAtManager::SetNamedProperty(napi_env env, napi_value dstObj, const int32_t objValue, const char *propName)
{
    napi_value prop = nullptr;
    napi_create_int32(env, objValue, &prop);
    napi_set_named_property(env, dstObj, propName, prop);
}

napi_value NapiAtManager::Init(napi_env env, napi_value exports)
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "enter init.");

    napi_property_descriptor descriptor[] = { DECLARE_NAPI_FUNCTION("createAtManager", CreateAtManager) };

    NAPI_CALL(env, napi_define_properties(env,
        exports, sizeof(descriptor) / sizeof(napi_property_descriptor), descriptor));

    napi_property_descriptor properties[] = {
        DECLARE_NAPI_FUNCTION("verifyAccessToken", VerifyAccessToken),
        DECLARE_NAPI_FUNCTION("grantUserGrantedPermission", GrantUserGrantedPermission),
        DECLARE_NAPI_FUNCTION("revokeUserGrantedPermission", RevokeUserGrantedPermission),
        DECLARE_NAPI_FUNCTION("getPermissionFlags", GetPermissionFlags)
    };

    napi_value cons = nullptr;
    NAPI_CALL(env, napi_define_class(env, ATMANAGER_CLASS_NAME.c_str(), ATMANAGER_CLASS_NAME.size(),
        JsConstructor, nullptr, sizeof(properties) / sizeof(napi_property_descriptor), properties, &cons));

    NAPI_CALL(env, napi_create_reference(env, cons, 1, &atManagerRef_));
    NAPI_CALL(env, napi_set_named_property(env, exports, ATMANAGER_CLASS_NAME.c_str(), cons));

    napi_value GrantStatus = nullptr;
    napi_create_object(env, &GrantStatus);

    SetNamedProperty(env, GrantStatus, PERMISSION_DENIED, "PERMISSION_DENIED");
    SetNamedProperty(env, GrantStatus, PERMISSION_GRANTED, "PERMISSION_GRANTED");

    napi_property_descriptor exportFuncs[] = {
        DECLARE_NAPI_PROPERTY("GrantStatus", GrantStatus),
    };
    napi_define_properties(env, exports, sizeof(exportFuncs) / sizeof(*exportFuncs), exportFuncs);

    return exports;
}

napi_value NapiAtManager::JsConstructor(napi_env env, napi_callback_info cbinfo)
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "enter JsConstructor");

    napi_value thisVar = nullptr;

    NAPI_CALL(env, napi_get_cb_info(env, cbinfo, nullptr, nullptr, &thisVar, nullptr));

    return thisVar;
}

napi_value NapiAtManager::CreateAtManager(napi_env env, napi_callback_info cbInfo)
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "enter CreateAtManager");

    napi_value instance = nullptr;
    napi_value cons = nullptr;

    if (napi_get_reference_value(env, atManagerRef_, &cons) != napi_ok) {
        return nullptr;
    }

    ACCESSTOKEN_LOG_DEBUG(LABEL, "Get a reference to the global variable atManagerRef_ complete");

    if (napi_new_instance(env, cons, 0, nullptr, &instance) != napi_ok) {
        return nullptr;
    }

    ACCESSTOKEN_LOG_DEBUG(LABEL, "New the js instance complete");

    return instance;
}

void NapiAtManager::ParseInputVerifyPermissionOrGetFlag(const napi_env env, const napi_callback_info info,
    AtManagerAsyncContext& asyncContext)
{
    size_t argc = 2;

    napi_value argv[2] = { 0 };
    napi_value thisVar = nullptr;

    void *data = nullptr;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    asyncContext.env = env;

    // parse input tokenId and permissionName
    for (size_t i = 0; i < argc; i++) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);

        if (valueType == napi_number) {
            napi_get_value_uint32(env, argv[i], &(asyncContext.tokenId)); // get tokenId
        } else if (valueType == napi_string) {
            napi_get_value_string_utf8(env, argv[i], asyncContext.permissionName,
                VALUE_BUFFER_SIZE, &(asyncContext.pNameLen)); // get permissionName
        } else {
            ACCESSTOKEN_LOG_ERROR(LABEL, "Type matching failed");
            asyncContext.result = -1;
        }
    }

    ACCESSTOKEN_LOG_DEBUG(LABEL, "tokenID = %{public}d", asyncContext.tokenId);
    ACCESSTOKEN_LOG_DEBUG(LABEL, "permissionName = %{public}s", asyncContext.permissionName);
}

void NapiAtManager::VerifyAccessTokenExecute(napi_env env, void *data)
{
    AtManagerAsyncContext* asyncContext = reinterpret_cast<AtManagerAsyncContext *>(data);

    // use innerkit class method to verify permission
    asyncContext->result = AccessTokenKit::VerifyAccessToken(asyncContext->tokenId,
        asyncContext->permissionName);

    // set status according to the innerkit class method return
    if ((asyncContext->result == PERMISSION_GRANTED) || (asyncContext->result == PERMISSION_DENIED)) {
        asyncContext->status = ASYN_THREAD_EXEC_SUCC; // granted and denied regard as function exec success
    } else {
        asyncContext->status = ASYN_THREAD_EXEC_FAIL; // other regard as function exec failure
    }
}

void NapiAtManager::VerifyAccessTokenComplete(napi_env env, napi_status status, void *data)
{
    AtManagerAsyncContext* asyncContext = reinterpret_cast<AtManagerAsyncContext *>(data);
    napi_value result;

    ACCESSTOKEN_LOG_DEBUG(LABEL, "tokenId = %{public}d, permissionName = %{public}s, verify result = %{public}d.",
        asyncContext->tokenId, asyncContext->permissionName, asyncContext->result);

    if (asyncContext->status == ASYN_THREAD_EXEC_SUCC) {
        // execute succ, use resolve to return result by the deferred create before
        napi_create_int32(env, asyncContext->result, &result); // verify result
        napi_resolve_deferred(env, asyncContext->deferred, result);
    } else {
        // execute fail, use reject to return default PERMISSION_DENIED by the deferred create before
        napi_create_int32(env, PERMISSION_DENIED, &result); // verify result
        napi_reject_deferred(env, asyncContext->deferred, result);
    }

    // after return the result, free resources
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
}

napi_value NapiAtManager::VerifyAccessToken(napi_env env, napi_callback_info info)
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "VerifyAccessToken begin.");

    auto *asyncContext = new AtManagerAsyncContext(); // for async work deliver data
    if (asyncContext == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "new struct fail.");
        return nullptr;
    }

    ParseInputVerifyPermissionOrGetFlag(env, info, *asyncContext);
    if (asyncContext->result == -1) {
        delete asyncContext;
        return nullptr;
    }

    napi_value result = nullptr;

    napi_create_promise(env, &(asyncContext->deferred), &result); // create delay promise object

    napi_value resource = nullptr; // resource name
    napi_create_string_utf8(env, "VerifyAccessToken", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work( // define work
        env, nullptr, resource, VerifyAccessTokenExecute, VerifyAccessTokenComplete,
        reinterpret_cast<void *>(asyncContext), &(asyncContext->work));
    napi_queue_async_work(env, asyncContext->work); // add async work handle to the napi queue and wait for result

    ACCESSTOKEN_LOG_DEBUG(LABEL, "VerifyAccessToken end.");

    return result;
}

void NapiAtManager::ParseInputGrantOrRevokePermission(const napi_env env, const napi_callback_info info,
    AtManagerAsyncContext& asyncContext)
{
    size_t argc = 4;

    napi_value argv[4] = { 0 };
    napi_value thisVar = nullptr;

    void *data = nullptr;

    napi_get_cb_info(env, info, &argc, argv, &thisVar, &data);

    asyncContext.env = env;

    // parse input tokenId and permissionName
    for (size_t i = 0; i < argc; i++) {
        napi_valuetype valueType = napi_undefined;
        napi_typeof(env, argv[i], &valueType);

        if ((i == 0) && (valueType == napi_number)) {
            napi_get_value_uint32(env, argv[i], &(asyncContext.tokenId)); // get tokenId
        } else if (valueType == napi_string) {
            napi_get_value_string_utf8(env, argv[i], asyncContext.permissionName,
                VALUE_BUFFER_SIZE, &(asyncContext.pNameLen)); // get permissionName
        } else if (valueType == napi_number) {
            napi_get_value_int32(env, argv[i], &(asyncContext.flag)); // get flag
        } else if (valueType == napi_function) {
            napi_create_reference(env, argv[i], 1, &asyncContext.callbackRef); // get probably callback
        } else {
            ACCESSTOKEN_LOG_ERROR(LABEL, "Type matching failed");
            asyncContext.result = -1;
        }
    }

    ACCESSTOKEN_LOG_DEBUG(LABEL, "tokenID = %{public}d", asyncContext.tokenId);
    ACCESSTOKEN_LOG_DEBUG(LABEL, "permissionName = %{public}s", asyncContext.permissionName);
    ACCESSTOKEN_LOG_DEBUG(LABEL, "flag = %{public}d", asyncContext.flag);
}

void NapiAtManager::GrantUserGrantedPermissionExcute(napi_env env, void *data)
{
    AtManagerAsyncContext* asyncContext = reinterpret_cast<AtManagerAsyncContext *>(data);
    PermissionDef permissionDef;

    // struct init, can not use = { 0 } or memset otherwise program crashdump
    permissionDef.grantMode = 0;
    permissionDef.availableLevel = APL_NORMAL;
    permissionDef.provisionEnable = false;
    permissionDef.distributedSceneEnable = false;
    permissionDef.labelId = 0;
    permissionDef.descriptionId = 0;

    // use innerkit class method to check if the permission grantmode is USER_GRANT-0
    AccessTokenKit::GetDefPermission(asyncContext->permissionName, permissionDef);

    ACCESSTOKEN_LOG_DEBUG(LABEL, "permissionName = %{public}s, grantmode = %{public}d.", asyncContext->permissionName,
        permissionDef.grantMode);

    if (permissionDef.grantMode != USER_GRANT) {
        // system_grant permission, return fail directly
        asyncContext->result = ACCESSTOKEN_PERMISSION_GRANT_FAIL;
        asyncContext->status = ASYN_THREAD_EXEC_SUCC;
    } else {
        // user_grant permission, use innerkit class method to grant permission
        asyncContext->result = AccessTokenKit::GrantPermission(asyncContext->tokenId,
                                                               asyncContext->permissionName,
                                                               asyncContext->flag);

        ACCESSTOKEN_LOG_DEBUG(LABEL,
            "tokenId = %{public}d, permissionName = %{public}s, flag = %{public}d, grant result = %{public}d.",
            asyncContext->tokenId, asyncContext->permissionName, asyncContext->flag, asyncContext->result);

        // set status according to the innerkit class method return
        if ((asyncContext->result == ACCESSTOKEN_PERMISSION_GRANT_SUCC) ||
            (asyncContext->result == ACCESSTOKEN_PERMISSION_GRANT_FAIL)) {
            asyncContext->status = ASYN_THREAD_EXEC_SUCC; // success or failure regard as function exec success
        } else {
            asyncContext->status = ASYN_THREAD_EXEC_FAIL; // other regard as function exec failure
        }
    }
}

void NapiAtManager::GrantUserGrantedPermissionComplete(napi_env env, napi_status status, void *data)
{
    AtManagerAsyncContext* asyncContext = reinterpret_cast<AtManagerAsyncContext*>(data);
    napi_value result = nullptr;

    if (asyncContext->status == ASYN_THREAD_EXEC_SUCC) {
        // execute succ, consider asyncContext->result as return result
        napi_create_int32(env, asyncContext->result, &result);
    } else {
        // execute fail, set default failure result
        napi_create_int32(env, ACCESSTOKEN_PERMISSION_GRANT_FAIL, &result);
    }

    if (asyncContext->deferred) {
        // promise type
        if (asyncContext->status == ASYN_THREAD_EXEC_SUCC) {
            // innerkit class methon exec success, use resolve to return result
            napi_resolve_deferred(env, asyncContext->deferred, result);
        } else {
            // innerkit class methon exec failure, use reject to return result
            napi_reject_deferred(env, asyncContext->deferred, result);
        }
    } else {
        // callback type
        napi_value callback = nullptr;
        napi_value thisValue = nullptr; // recv napi value
        napi_value thatValue = nullptr; // result napi value

        // set call function params->napi_call_function(env, recv, func, argc, argv, result)
        napi_get_undefined(env, &thisValue); // can not null otherwise js code can not get return
        napi_create_int32(env, 0, &thatValue); // can not null otherwise js code can not get return
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, thisValue, callback, 1, &result, &thatValue);
        napi_delete_reference(env, asyncContext->callbackRef); // release callback handle
    }

    // after return the result, free resources
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
}

napi_value NapiAtManager::GrantUserGrantedPermission(napi_env env, napi_callback_info info)
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "GrantUserGrantedPermission begin.");

    auto *asyncContext = new (std::nothrow) AtManagerAsyncContext(); // for async work deliver data
    if (asyncContext == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "new struct fail.");
        return nullptr;
    }

    ParseInputGrantOrRevokePermission(env, info, *asyncContext);
    if (asyncContext->result == -1) {
        delete asyncContext;
        return nullptr;
    }

    napi_value result = nullptr;

    if (asyncContext->callbackRef == nullptr) {
        // when callback null, create delay promise object for returning result in async work complete function
        napi_create_promise(env, &(asyncContext->deferred), &result);
    } else {
        // callback not null, use callback type to return result
        napi_get_undefined(env, &result);
    }

    napi_value resource = nullptr; // resource name
    napi_create_string_utf8(env, "GrantUserGrantedPermission", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work( // define work
        env, nullptr, resource, GrantUserGrantedPermissionExcute, GrantUserGrantedPermissionComplete,
        reinterpret_cast<void *>(asyncContext), &(asyncContext->work));

    napi_queue_async_work(env, asyncContext->work); // add async work handle to the napi queue and wait for result

    ACCESSTOKEN_LOG_DEBUG(LABEL, "GrantUserGrantedPermission end.");

    return result;
}

void NapiAtManager::RevokeUserGrantedPermissionExcute(napi_env env, void *data)
{
    AtManagerAsyncContext* asyncContext = reinterpret_cast<AtManagerAsyncContext *>(data);
    PermissionDef permissionDef;

    // struct init, can not use = { 0 } or memset otherwise program crashdump
    permissionDef.grantMode = 0;
    permissionDef.availableLevel = APL_NORMAL;
    permissionDef.provisionEnable = false;
    permissionDef.distributedSceneEnable = false;
    permissionDef.labelId = 0;
    permissionDef.descriptionId = 0;

    // use innerkit class method to check if the permission grantmode is USER_GRANT-0
    AccessTokenKit::GetDefPermission(asyncContext->permissionName, permissionDef);

    ACCESSTOKEN_LOG_DEBUG(LABEL, "permissionName = %{public}s, grantmode = %{public}d.", asyncContext->permissionName,
        permissionDef.grantMode);

    if (permissionDef.grantMode != USER_GRANT) {
        // system_grant permission, return fail directly
        asyncContext->result = ACCESSTOKEN_PERMISSION_REVOKE_FAIL;
        asyncContext->status = ASYN_THREAD_EXEC_SUCC;
    } else {
        // user_grant permission, use innerkit class method to grant permission
        asyncContext->result = AccessTokenKit::RevokePermission(asyncContext->tokenId,
            asyncContext->permissionName, asyncContext->flag);

        ACCESSTOKEN_LOG_DEBUG(LABEL,
            "tokenId = %{public}d, permissionName = %{public}s, flag = %{public}d, revoke result = %{public}d.",
            asyncContext->tokenId, asyncContext->permissionName, asyncContext->flag, asyncContext->result);

        // set status according to the innerkit class method return
        if ((asyncContext->result == ACCESSTOKEN_PERMISSION_REVOKE_SUCC) ||
            (asyncContext->result == ACCESSTOKEN_PERMISSION_REVOKE_FAIL)) {
            asyncContext->status = ASYN_THREAD_EXEC_SUCC; // success or failure regard as function exec success
        } else {
            asyncContext->status = ASYN_THREAD_EXEC_FAIL; // other regard as function exec failure
        }
    }
}

void NapiAtManager::RevokeUserGrantedPermissionComplete(napi_env env, napi_status status, void *data)
{
    AtManagerAsyncContext* asyncContext = reinterpret_cast<AtManagerAsyncContext*>(data);
    napi_value result = nullptr;

    if (asyncContext->status == ASYN_THREAD_EXEC_SUCC) {
        // execute succ, consider asyncContext->result as return result
        napi_create_int32(env, asyncContext->result, &result);
    } else {
        // execute fail, set default failure result
        napi_create_int32(env, ACCESSTOKEN_PERMISSION_GRANT_FAIL, &result);
    }

    if (asyncContext->deferred) {
        // promise type
        if (asyncContext->status == ASYN_THREAD_EXEC_SUCC) {
            // innerkit class methon exec success, use resolve to return result
            napi_resolve_deferred(env, asyncContext->deferred, result);
        } else {
            // innerkit class methon exec failure, use reject to return result
            napi_reject_deferred(env, asyncContext->deferred, result);
        }
    } else {
        // callback type
        napi_value callback = nullptr;
        napi_value thisValue = nullptr; // recv napi value
        napi_value thatValue = nullptr; // result napi value

        // set call function params->napi_call_function(env, recv, func, argc, argv, result)
        napi_get_undefined(env, &thisValue); // can not null otherwise js code can not get return
        napi_create_int32(env, 0, &thatValue); // can not null otherwise js code can not get return
        napi_get_reference_value(env, asyncContext->callbackRef, &callback);
        napi_call_function(env, thisValue, callback, 1, &result, &thatValue);
        napi_delete_reference(env, asyncContext->callbackRef); // release callback handle
    }

    // after return the result, free resources
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
}

napi_value NapiAtManager::RevokeUserGrantedPermission(napi_env env, napi_callback_info info)
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "RevokeUserGrantedPermission begin.");

    auto *asyncContext = new AtManagerAsyncContext(); // for async work deliver data
    if (asyncContext == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "new struct fail.");
        return nullptr;
    }

    ParseInputGrantOrRevokePermission(env, info, *asyncContext);
    if (asyncContext->result == -1) {
        delete asyncContext;
        return nullptr;
    }

    napi_value result = nullptr;

    if (asyncContext->callbackRef == nullptr) {
        // when callback null, create delay promise object for returning result in async work complete function
        napi_create_promise(env, &(asyncContext->deferred), &result);
    } else {
        // callback not null, use callback type to return result
        napi_get_undefined(env, &result);
    }

    napi_value resource = nullptr; // resource name
    napi_create_string_utf8(env, "RevokeUserGrantedPermission", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work( // define work
        env, nullptr, resource, RevokeUserGrantedPermissionExcute, RevokeUserGrantedPermissionComplete,
        reinterpret_cast<void *>(asyncContext), &(asyncContext->work));

    napi_queue_async_work(env, asyncContext->work); // add async work handle to the napi queue and wait for result

    ACCESSTOKEN_LOG_DEBUG(LABEL, "RevokeUserGrantedPermission end.");

    return result;
}

void NapiAtManager::GetPermissionFlagsExcute(napi_env env, void *data)
{
    AtManagerAsyncContext* asyncContext = reinterpret_cast<AtManagerAsyncContext*>(data);

    // use innerkit class method to get permission flag
    asyncContext->flag = AccessTokenKit::GetPermissionFlag(asyncContext->tokenId,
        asyncContext->permissionName);
    asyncContext->status = ASYN_THREAD_EXEC_SUCC; // status default failure
}

void NapiAtManager::GetPermissionFlagsComplete(napi_env env, napi_status status, void *data)
{
    AtManagerAsyncContext* asyncContext = reinterpret_cast<AtManagerAsyncContext*>(data);
    napi_value result;

    ACCESSTOKEN_LOG_DEBUG(LABEL, "permissionName = %{public}s, tokenId = %{public}d, flag = %{public}d.",
        asyncContext->permissionName, asyncContext->tokenId, asyncContext->flag);

    if (asyncContext->status == ASYN_THREAD_EXEC_SUCC) {
        // execute succ, use resolve to return result by the deferred create before
        napi_create_int32(env, asyncContext->flag, &result);
        napi_resolve_deferred(env, asyncContext->deferred, result);
    } else {
        // execute fail, this way may not match, but for code strict, still keep
        napi_create_int32(env, asyncContext->flag, &result);
        napi_reject_deferred(env, asyncContext->deferred, result);
    }

    // after return the result, free resources
    napi_delete_async_work(env, asyncContext->work);
    delete asyncContext;
}

napi_value NapiAtManager::GetPermissionFlags(napi_env env, napi_callback_info info)
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "GetPermissionFlags begin.");

    auto *asyncContext = new AtManagerAsyncContext(); // for async work deliver data
    if (asyncContext == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "new struct fail.");
        return nullptr;
    }

    ParseInputVerifyPermissionOrGetFlag(env, info, *asyncContext);
    if (asyncContext->result == -1) {
        delete asyncContext;
        return nullptr;
    }

    napi_value result = nullptr;

    napi_create_promise(env, &(asyncContext->deferred), &result); // create delay promise object

    napi_value resource = nullptr; // resource name
    napi_create_string_utf8(env, "VerifyAccessToken", NAPI_AUTO_LENGTH, &resource);

    napi_create_async_work( // define work
        env, nullptr, resource, GetPermissionFlagsExcute, GetPermissionFlagsComplete,
        reinterpret_cast<void *>(asyncContext), &(asyncContext->work));
    napi_queue_async_work(env, asyncContext->work); // add async work handle to the napi queue and wait for result

    ACCESSTOKEN_LOG_DEBUG(LABEL, "GetPermissionFlags end.");

    return result;
}
}  // namespace AccessToken
}  // namespace Security
}  // namespace OHOS

EXTERN_C_START
/*
 * function for module exports
 */
static napi_value Init(napi_env env, napi_value exports)
{
    ACCESSTOKEN_LOG_DEBUG(OHOS::Security::AccessToken::LABEL, "Register end, start init.");

    return OHOS::Security::AccessToken::NapiAtManager::Init(env, exports);
}
EXTERN_C_END

/*
 * Module define
 */
static napi_module _module = {
    .nm_version = 1,
    .nm_flags = 0,
    .nm_filename = nullptr,
    .nm_register_func = Init,
    .nm_modname = "abilityAccessCtrl",
    .nm_priv = ((void *)0),
    .reserved = {0}
};

/*
 * Module register function
 */
extern "C" __attribute__((constructor)) void AbilityAccessCtrlmoduleRegister(void)
{
    napi_module_register(&_module);
}
