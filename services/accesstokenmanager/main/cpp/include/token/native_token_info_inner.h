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

#ifndef ACCESSTOKEN_NATIVE_TOKEN_INFO_INNER_H
#define ACCESSTOKEN_NATIVE_TOKEN_INFO_INNER_H

#include <string>
#include <vector>
#include "access_token.h"
#include "generic_values.h"
#include "native_token_info.h"
#include "permission_policy_set.h"
#include "permission_state_full.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
static const std::string JSON_PROCESS_NAME = "processName";
static const std::string JSON_APL = "APL";
static const std::string JSON_VERSION = "version";
static const std::string JSON_TOKEN_ID = "tokenId";
static const std::string JSON_TOKEN_ATTR = "tokenAttr";
static const std::string JSON_DCAPS = "dcaps";
static const std::string JSON_PERMS = "permissions";
static const int MAX_DCAPS_NUM = 32;
static const int MAX_REQ_PERM_NUM = 32;

class NativeTokenInfoInner final {
public:
    NativeTokenInfoInner();
    NativeTokenInfoInner(NativeTokenInfo& info,
        const std::vector<PermissionStateFull> &permStateList);
    virtual ~NativeTokenInfoInner();

    int Init(AccessTokenID id, const std::string& processName, int apl,
        const std::vector<std::string>& dcap,
        const std::vector<PermissionStateFull> &permStateList);
    void StoreNativeInfo(std::vector<GenericValues>& valueList,
        std::vector<GenericValues>& permStateValues) const;
    void TranslateToNativeTokenInfo(NativeTokenInfo& InfoParcel) const;
    void SetDcaps(const std::string& dcapStr);
    void ToString(std::string& info) const;
    int RestoreNativeTokenInfo(AccessTokenID tokenId, const GenericValues& inGenericValues,
        const std::vector<GenericValues>& permStateRes);
    void Update(AccessTokenID tokenId, const std::string& processName,
        int apl, const std::vector<std::string>& dcap);

    std::vector<std::string> GetDcap() const;
    AccessTokenID GetTokenID() const;
    std::string GetProcessName() const;
    NativeTokenInfo GetNativeTokenInfo() const;
    std::shared_ptr<PermissionPolicySet> GetNativeInfoPermissionPolicySet() const;
    bool IsRemote() const;
    void SetRemote(bool isRemote);

private:
    int TranslationIntoGenericValues(GenericValues& outGenericValues) const;
    std::string DcapToString(const std::vector<std::string>& dcap) const;

    // true means sync from remote.
    bool isRemote_;
    NativeTokenInfo tokenInfoBasic_;
    std::shared_ptr<PermissionPolicySet> permPolicySet_;
};
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
#endif // ACCESSTOKEN_NATIVE_TOKEN_INFO_INNER_H
