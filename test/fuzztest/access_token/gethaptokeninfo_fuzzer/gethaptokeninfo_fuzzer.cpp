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

#include "gethaptokeninfo_fuzzer.h"

#include <stddef.h>
#include <stdint.h>
#include "accesstoken_kit.h"
#include "hap_token_info.h"

using namespace OHOS::Security::AccessToken;

namespace OHOS {
    bool GetHapTokenInfoFuzzTest(const uint8_t* data, size_t size)
    {
        bool result = false;
        if ((data == nullptr) || (size <= 0)) {
            return result;
        }
        if (size > 0) {
            int num = static_cast<int>(size);
            char ver = static_cast<char>(size);
            AccessTokenID TOKENID = static_cast<AccessTokenID>(size);
            std::string testName(reinterpret_cast<const char*>(data), size);
            HapTokenInfo HapTokenInfotest = {
                .ver = ver,
                .userID = num,
                .bundleName = testName,
                .instIndex = num,
                .dlpType = num,
                .appID = testName,
                .deviceID = testName,
                .tokenID = TOKENID,
                .tokenAttr = TOKENID,
            };
            result = AccessTokenKit::GetHapTokenInfo(TOKENID, HapTokenInfotest);
        }
        return result;
    }
}

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::GetHapTokenInfoFuzzTest(data, size);
    return 0;
}
