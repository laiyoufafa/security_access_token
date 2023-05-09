/*
 * Copyright (c) 2023 Huawei Device Co., Ltd.
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

#include "setremotenativetokeninfostub_fuzzer.h"

#include <string>
#include <thread>
#include <vector>
#undef private
#include "accesstoken_manager_service.h"
#include "i_accesstoken_manager.h"

using namespace std;
using namespace OHOS::Security::AccessToken;

namespace OHOS {
    bool SetRemoteNativeTokenInfoStubFuzzTest(const uint8_t* data, size_t size)
    {
    #ifdef TOKEN_SYNC_ENABLE
        if ((data == nullptr) || (size == 0)) {
            return false;
        }

        std::string testName(reinterpret_cast<const char*>(data), size);
        AccessTokenID TOKENID = static_cast<AccessTokenID>(size);
        NativeTokenInfoForSync native = {
            .baseInfo.apl = APL_NORMAL,
            .baseInfo.ver = 1,
            .baseInfo.processName = testName,
            .baseInfo.dcap = {testName, testName},
            .baseInfo.tokenID = TOKENID,
            .baseInfo.tokenAttr = 0,
            .baseInfo.nativeAcls = {testName},
        };
        NativeTokenInfoForSyncParcel nativeTokenInfoForSyncParcel;
        nativeTokenInfoForSyncParcel.nativeTokenInfoForSyncParams = native;

        MessageParcel datas;
        datas.WriteInterfaceToken(IAccessTokenManager::GetDescriptor());
        if (!datas.WriteString(testName)) {
            return false;
        }
        if (!datas.WriteUint32(1)) {
            return false;
        }
        if (!datas.WriteParcelable(&nativeTokenInfoForSyncParcel)) {
            return false;
        }

        uint32_t code = static_cast<uint32_t>(
            IAccessTokenManager::InterfaceCode::SET_REMOTE_NATIVE_TOKEN_INFO);

        MessageParcel reply;
        MessageOption option;
        DelayedSingleton<AccessTokenManagerService>::GetInstance()->OnRemoteRequest(code, datas, reply, option);

        return true;
    #else
        return true;
    #endif
    }
} // namespace OHOS

/* Fuzzer entry point */
extern "C" int LLVMFuzzerTestOneInput(const uint8_t* data, size_t size)
{
    /* Run your code on data */
    OHOS::SetRemoteNativeTokenInfoStubFuzzTest(data, size);
    return 0;
}
