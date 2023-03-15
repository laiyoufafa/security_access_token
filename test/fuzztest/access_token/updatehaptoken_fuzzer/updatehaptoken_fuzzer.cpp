/*
 * Copyright (c) 2022 Huawei Device Co., Ltd.
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

#include "updatehaptoken_fuzzer.h"

#include <string>
#include <vector>
#include <thread>
#undef private
#include "accesstoken_kit.h"

using namespace std;
using namespace OHOS::Security::AccessToken;

namespace OHOS {
    bool UpdateHapTokenFuzzTest(const uint8_t* data, size_t size)
    {
        if ((data == nullptr) || (size == 0)) {
            return false;
        }

        AccessTokenIDEx TOKENIDEX = {
            .tokenIdExStruct.tokenID = static_cast<AccessTokenID>(size),
            .tokenIdExStruct.tokenAttr = static_cast<AccessTokenAttr>(size),
        };

        std::string testName(reinterpret_cast<const char*>(data), size);
        PermissionDef TestPermDef = {
            .permissionName = testName,
            .bundleName = testName,
            .grantMode = 1,
            .availableLevel = APL_NORMAL,
            .label = testName,
            .labelId = 1,
            .description = testName,
            .descriptionId = 1
        };
        PermissionStateFull TestState = {
            .permissionName = testName,
            .isGeneral = true,
            .resDeviceID = {testName},
            .grantStatus = {PermissionState::PERMISSION_GRANTED},
            .grantFlags = {1},
        };
        HapPolicyParams TestPolicyPrams = {
            .apl = APL_NORMAL,
            .domain = testName,
            .permList = {TestPermDef},
            .permStateList = {TestState}
        };
        constexpr int32_t DEFAULT_API_VERSION = 8;
        int32_t result = AccessTokenKit::UpdateHapToken(TOKENIDEX, false, testName, DEFAULT_API_VERSION, TestPolicyPrams);

        return result == RET_SUCCESS;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::UpdateHapTokenFuzzTest(data, size);
    return 0;
}
