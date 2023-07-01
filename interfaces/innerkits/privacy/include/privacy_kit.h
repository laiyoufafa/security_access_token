/*
 * Copyright (c) 2022-2023 Huawei Device Co., Ltd.
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

/**
 * @addtogroup Privacy
 * @{
 *
 * @brief Provides sensitive permissions access management.
 *
 * @since 8.0
 * @version 8.0
 */

/**
 * @file privacy_kit.h
 *
 * @brief Declares PrivacyKit interfaces.
 *
 * @since 8.0
 * @version 8.0
 */

#ifndef INTERFACES_INNER_KITS_PRIVACY_KIT_H
#define INTERFACES_INNER_KITS_PRIVACY_KIT_H

#include <string>

#include "access_token.h"
#include "on_permission_used_record_callback.h"
#include "permission_used_request.h"
#include "permission_used_result.h"
#include "perm_active_status_customized_cbk.h"
#ifdef SECURITY_COMPONENT_ENHANCE_ENABLE
#include "sec_comp_enhance_data.h"
#endif
#include "state_customized_cbk.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
/**
 * @brief Declares PrivacyKit class
 */
class PrivacyKit {
public:
    /**
     * @brief Add input tokenID access input permission record.
     * @param tokenID token id
     * @param permissionName permission nanme
     * @param successCount access success count
     * @param failCount fail success count
     * @return error code, see privacy_error.h
     */
    static int32_t AddPermissionUsedRecord(
        AccessTokenID tokenID, const std::string& permissionName, int32_t successCount, int32_t failCount);
    /**
     * @brief Input tokenID start using input permission.
     * @param tokenID token id
     * @param permissionName permission nanme
     * @return error code, see privacy_error.h
     */
    static int32_t StartUsingPermission(AccessTokenID tokenID, const std::string& permissionName);
    /**
     * @brief Input tokenID start using input permission and return by callback,
     *        only those services which has float window such as camera or
     *        microphone can use this interface.
     * @param tokenID token id
     * @param permissionName permission nanme
     * @param callback StateCustomizedCbk nanme
     * @return error code, see privacy_error.h
     */
    static int32_t StartUsingPermission(AccessTokenID tokenID, const std::string& permissionName,
        const std::shared_ptr<StateCustomizedCbk>& callback);
    /**
     * @brief Input tokenID stop using input permission.
     * @param tokenID token id
     * @param permissionName permission nanme
     * @return error code, see privacy_error.h
     */
    static int32_t StopUsingPermission(AccessTokenID tokenID, const std::string& permissionName);
    /**
     * @brief Remove input tokenID sensitive permission used records.
     * @param tokenID token id
     * @param deviceID device id
     * @return error code, see privacy_error.h
     */
    static int32_t RemovePermissionUsedRecords(AccessTokenID tokenID, const std::string& deviceID);
    /**
     * @brief Get sensitive permission used records.
     * @param request PermissionUsedRequest quote
     * @param result PermissionUsedResult quote, as query result
     * @return error code, see privacy_error.h
     */
    static int32_t GetPermissionUsedRecords(const PermissionUsedRequest& request, PermissionUsedResult& result);
    /**
     * @brief Get sensitive permission used records.
     * @param request PermissionUsedRequest quote
     * @param callback OnPermissionUsedRecordCallback smart pointer quote
     * @return error code, see privacy_error.h
     */
    static int32_t GetPermissionUsedRecords(
        const PermissionUsedRequest& request, const sptr<OnPermissionUsedRecordCallback>& callback);
    /**
     * @brief Register sensitive permission active status change callback.
     * @param callback PermActiveStatusCustomizedCbk smark pointer quote
     * @return error code, see privacy_error.h
     */
    static int32_t RegisterPermActiveStatusCallback(const std::shared_ptr<PermActiveStatusCustomizedCbk>& callback);
    /**
     * @brief Unregister sensitive permission active status change callback.
     * @param callback PermActiveStatusCustomizedCbk smark pointer quote
     * @return error code, see privacy_error.h
     */
    static int32_t UnRegisterPermActiveStatusCallback(const std::shared_ptr<PermActiveStatusCustomizedCbk>& callback);
    /**
     * @brief Judge whether the input tokenID can use the input permission or not.
     * @param tokenID token id
     * @param permissionName permission nanme
     * @return true means allow to user the permission, false means not allow
     */
    static bool IsAllowedUsingPermission(AccessTokenID tokenID, const std::string& permissionName);

#ifdef SECURITY_COMPONENT_ENHANCE_ENABLE
    /**
     * @brief Register security component enhance data when security component service did not start
     * @param enhance enhance data
     * @return error code, see privacy_error.h
     */
    static int32_t RegisterSecCompEnhance(const SecCompEnhanceData& enhance);
    /**
     * @brief deposit security component enhance data when security component service exit
     * @param enhanceList enhance data
     * @return error code, see privacy_error.h
     */
    static int32_t DepositSecCompEnhance(const std::vector<SecCompEnhanceData>& enhanceList);
    /**
     * @brief recover security component enhance data to security component service
     * @param enhanceList enhance data
     * @return error code, see privacy_error.h
     */
    static int32_t RecoverSecCompEnhance(std::vector<SecCompEnhanceData>& enhanceList);
#endif
};
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
#endif
