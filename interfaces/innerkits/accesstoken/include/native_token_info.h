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

#ifndef ACCESSTOKEN_NATIVE_TOKEN_INFO_H
#define ACCESSTOKEN_NATIVE_TOKEN_INFO_H

#include "access_token.h"
#include <string>
#include <vector>
#include "permission_state_full.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
class NativeTokenInfo final {
public:
    ATokenAplEnum apl;
    unsigned char ver;
    std::string processName;
    std::vector<std::string> dcap;
    AccessTokenID tokenID;
    AccessTokenAttr tokenAttr;
};

class NativeTokenInfoForSync final {
public:
    NativeTokenInfo baseInfo;
    std::vector<PermissionStateFull> permStateList;
};
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
#endif // ACCESSTOKEN_NATIVE_TOKEN_INFO_H
