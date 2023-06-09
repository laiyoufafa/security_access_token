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

#include "native_token_receptor_test.h"

#include <fcntl.h>
#include <memory>
#include <string>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/un.h>
#include <thread>
#include <unistd.h>
#include <vector>

#include "accesstoken_info_manager.h"
#include "accesstoken_kit.h"
#include "permission_manager.h"
#include "data_storage.h"
#include "field_const.h"
#include "permission_state_full.h"
#define private public
#include "nativetoken_kit.h"
#include "native_token_receptor.h"
#undef private
#include "securec.h"

using namespace testing::ext;
using namespace OHOS::Security::AccessToken;

namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_ACCESSTOKEN, "NativeTokenReceptorTest"};
}

void NativeTokenReceptorTest::SetUpTestCase()
{
    // delete all test 0x28100000 - 0x28100007
    for (unsigned int i = 0x28100000; i <= 0x28100007; i++) {
        AccessTokenInfoManager::GetInstance().RemoveNativeTokenInfo(i);
    }
}

void NativeTokenReceptorTest::TearDownTestCase()
{}

void NativeTokenReceptorTest::SetUp()
{}

void NativeTokenReceptorTest::TearDown()
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test down!");
}

/**
 * @tc.name: ParserNativeRawData001
 * @tc.desc: Verify processing right native token json.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, ParserNativeRawData001, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test ParserNativeRawData001!");
    std::string testStr = R"([)"\
        R"({"processName":"process6","APL":3,"version":1,"tokenId":685266937,"tokenAttr":0,)"\
        R"("dcaps":["AT_CAP","ST_CAP"], "permissions":[]},)"\
        R"({"processName":"process5","APL":3,"version":1,"tokenId":678065606,"tokenAttr":0,)"\
        R"("dcaps":["AT_CAP","ST_CAP"], "permissions":[]}])";

    NativeTokenReceptor& receptor = NativeTokenReceptor::GetInstance();
    std::vector<std::shared_ptr<NativeTokenInfoInner>> tokenInfos;
    receptor.ParserNativeRawData(testStr, tokenInfos);
    int size = tokenInfos.size();
    ASSERT_EQ(2, size);
    ASSERT_NE(nullptr, tokenInfos[0]);
    ASSERT_NE(nullptr, tokenInfos[1]);

    ASSERT_EQ("process6", tokenInfos[0]->GetProcessName());
    ASSERT_EQ(685266937, tokenInfos[0]->GetTokenID());
    ASSERT_EQ(2, tokenInfos[0]->GetDcap().size());
    ASSERT_EQ("AT_CAP", (tokenInfos[0]->GetDcap())[0]);
    ASSERT_EQ("ST_CAP", (tokenInfos[0]->GetDcap())[1]);

    ASSERT_EQ("process5", tokenInfos[1]->GetProcessName());
    ASSERT_EQ(678065606, tokenInfos[1]->GetTokenID());
    ASSERT_EQ(2, tokenInfos[1]->GetDcap().size());
    ASSERT_EQ("AT_CAP", (tokenInfos[1]->GetDcap())[0]);
    ASSERT_EQ("ST_CAP", (tokenInfos[1]->GetDcap())[1]);
}

/**
 * @tc.name: ParserNativeRawData002
 * @tc.desc: Verify processing wrong native token json.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, ParserNativeRawData002, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test ParserNativeRawData002!");
    std::string testStr = R"([{"processName":""}])";
    std::vector<std::shared_ptr<NativeTokenInfoInner>> tokenInfos;

    NativeTokenReceptor& receptor = NativeTokenReceptor::GetInstance();

    receptor.ParserNativeRawData(testStr, tokenInfos);
    ASSERT_EQ(0, tokenInfos.size());

    testStr = R"([{"processName":"", }])";
    receptor.ParserNativeRawData(testStr, tokenInfos);
    ASSERT_EQ(0, tokenInfos.size());

    testStr = R"([{"processName":"process6"}, {}])";
    receptor.ParserNativeRawData(testStr, tokenInfos);
    ASSERT_EQ(0, tokenInfos.size());

    testStr = R"([{"processName":""}, {"":"", ""}])";
    receptor.ParserNativeRawData(testStr, tokenInfos);
    ASSERT_EQ(0, tokenInfos.size());

    testStr = R"([{"processName":"process6", "tokenId":685266937, "APL":3, "version":new}])";
    receptor.ParserNativeRawData(testStr, tokenInfos);
    ASSERT_EQ(0, tokenInfos.size());

    testStr = R"([{"processName":"process6", "tokenId":685266937, "APL":7, "version":1}])";
    receptor.ParserNativeRawData(testStr, tokenInfos);
    ASSERT_EQ(0, tokenInfos.size());

    testStr = R"({"NativeToken":[{"processName":"process6", "tokenId":685266937, "APL":7, "version":1}]})";
    receptor.ParserNativeRawData(testStr, tokenInfos);
    ASSERT_EQ(0, tokenInfos.size());

    testStr = R"({"NativeToken":[{"processName":"process6", "tokenId":685266937, "APL":7, "version":1}])";
    receptor.ParserNativeRawData(testStr, tokenInfos);
    ASSERT_EQ(0, tokenInfos.size());

    testStr = R"(["NativeToken":])";
    receptor.ParserNativeRawData(testStr, tokenInfos);
    ASSERT_EQ(0, tokenInfos.size());

    testStr = R"([)";
    receptor.ParserNativeRawData(testStr, tokenInfos);
    ASSERT_EQ(0, tokenInfos.size());
}

namespace OHOS {
namespace Security {
namespace AccessToken {
    extern void from_json(const nlohmann::json& j, std::shared_ptr<NativeTokenInfoInner>& p);
}
}
}

/**
 * @tc.name: from_json001
 * @tc.desc: Verify from json right case.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, from_json001, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test from_json001!");
    nlohmann::json j = nlohmann::json{
        {"processName", "process6"},
        {"APL", APL_SYSTEM_CORE},
        {"version", 1},
        {"tokenId", 685266937},
        {"tokenAttr", 0},
        {"dcaps", {"AT_CAP", "ST_CAP"}},
        {"permissions", {"ohos.permission.PLACE_CALL"}}};
    std::shared_ptr<NativeTokenInfoInner> p;
    from_json(j, p);
    ASSERT_NE((p == nullptr), true);
}

/**
 * @tc.name: from_json002
 * @tc.desc: Verify from json wrong case.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, from_json002, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test from_json002!");
    // version wrong
    nlohmann::json j = nlohmann::json{
        {"processName", "process6"}, {"APL", APL_SYSTEM_CORE},
        {"version", 2}, {"tokenId", 685266937},
        {"tokenAttr", 0},
        {"dcaps", {"AT_CAP", "ST_CAP"}}};
    std::shared_ptr<NativeTokenInfoInner> p;
    from_json(j, p);
    ASSERT_EQ((p == nullptr), true);

    // APL wrong
    j = nlohmann::json{
        {"processName", "process6"},
        {"APL", -1}, {"version", 1},
        {"tokenId", 685266937}, {"tokenAttr", 0},
        {"dcaps", {"AT_CAP", "ST_CAP"}}};
    from_json(j, p);
    ASSERT_EQ((p == nullptr), true);

    // tokenId wrong
    j = nlohmann::json{
        {"processName", "process6"},
        {"APL", APL_SYSTEM_BASIC}, {"version", 1},
        {"tokenId", 0}, {"tokenAttr", 0},
        {"dcaps", {"AT_CAP", "ST_CAP"}}};
    from_json(j, p);
    ASSERT_EQ((p == nullptr), true);

    // process name empty
    j = nlohmann::json{
        {"processName", ""},
        {"APL", APL_SYSTEM_BASIC}, {"version", 1},
        {"tokenId", 685266937}, {"tokenAttr", 0},
        {"dcaps", {"AT_CAP", "ST_CAP"}}};
    from_json(j, p);
    ASSERT_EQ((p == nullptr), true);

    // process name too long
    std::string name(512, 'c');
    j = nlohmann::json{
        {"processName", name},
        {"APL", APL_SYSTEM_BASIC}, {"version", 1},
        {"tokenId", 685266937}, {"tokenAttr", 0},
        {"dcaps", {"AT_CAP", "ST_CAP"}}};
    from_json(j, p);
    ASSERT_EQ((p == nullptr), true);

    // lose process name
    j = nlohmann::json{
        {"APL", APL_SYSTEM_BASIC},
        {"version", 1}, {"tokenId", 685266937},
        {"tokenAttr", 0}, {"dcaps", {"AT_CAP", "ST_CAP"}}};
    from_json(j, p);
    ASSERT_EQ((p == nullptr), true);
}

/**
 * @tc.name: ProcessNativeTokenInfos001
 * @tc.desc: test add one native token
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, ProcessNativeTokenInfos001, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test ProcessNativeTokenInfos001!");
    std::vector<std::shared_ptr<NativeTokenInfoInner>> tokenInfos;

    // test process one
    NativeTokenInfo info = {
        .apl = APL_NORMAL,
        .ver = 1,
        .processName = "native_token_test0",
        .tokenID = 0x28100000,
        .tokenAttr = 0,
        .dcap =  {"AT_CAP", "ST_CAP"}
    };

    std::vector<PermissionStateFull> permStateList = {};
    std::shared_ptr<NativeTokenInfoInner> nativeToken = std::make_shared<NativeTokenInfoInner>(info, permStateList);
    tokenInfos.emplace_back(nativeToken);
    AccessTokenInfoManager::GetInstance().ProcessNativeTokenInfos(tokenInfos);
    NativeTokenInfo findInfo;
    int ret = AccessTokenInfoManager::GetInstance().GetNativeTokenInfo(info.tokenID, findInfo);
    ASSERT_EQ(ret, RET_SUCCESS);
    ASSERT_EQ(findInfo.apl, info.apl);
    ASSERT_EQ(findInfo.ver, info.ver);
    ASSERT_EQ(findInfo.processName, info.processName);
    ASSERT_EQ(findInfo.tokenID, info.tokenID);
    ASSERT_EQ(findInfo.tokenAttr, info.tokenAttr);
    ASSERT_EQ(findInfo.dcap, info.dcap);

    // wait fresh tokens to sql.
    sleep(3);

    // get sql data
    std::vector<GenericValues> nativeTokenResults;
    DataStorage::GetRealDataStorage().Find(DataStorage::ACCESSTOKEN_NATIVE_INFO, nativeTokenResults);
    std::vector<GenericValues> permStateRes;
    DataStorage::GetRealDataStorage().Find(DataStorage::ACCESSTOKEN_PERMISSION_STATE, permStateRes);
    for (GenericValues nativeTokenValue : nativeTokenResults) {
        AccessTokenID tokenId = (AccessTokenID)nativeTokenValue.GetInt(FIELD_TOKEN_ID);
        if (tokenId != info.tokenID) {
            continue;
        }
        GTEST_LOG_(INFO) <<"apl " << nativeTokenValue.GetInt(FIELD_APL);
        std::shared_ptr<NativeTokenInfoInner> native = std::make_shared<NativeTokenInfoInner>();
        ASSERT_NE(native, nullptr);
        ret = native->RestoreNativeTokenInfo(tokenId, nativeTokenValue, permStateRes);
        ASSERT_EQ(ret, RET_SUCCESS);
        ASSERT_EQ(native->GetTokenID(), info.tokenID);
        ASSERT_EQ(native->GetProcessName(), info.processName);
        ASSERT_EQ(native->GetDcap(), info.dcap);
    }

    ret = AccessTokenInfoManager::GetInstance().RemoveNativeTokenInfo(info.tokenID);
    ASSERT_EQ(ret, RET_SUCCESS);
}

/**
 * @tc.name: ProcessNativeTokenInfos002
 * @tc.desc: test add two native tokens.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, ProcessNativeTokenInfos002, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test ProcessNativeTokenInfos002!");
    std::vector<std::shared_ptr<NativeTokenInfoInner>> tokenInfos;

    NativeTokenInfo info1 = {
        .apl = APL_NORMAL,
        .ver = 1,
        .processName = "native_token_test1",
        .tokenID = 0x28100001,
        .tokenAttr = 0,
        .dcap =  {"AT_CAP", "ST_CAP"}
    };

    NativeTokenInfo info2 = {
        .apl = APL_NORMAL,
        .ver = 1,
        .processName = "native_token_test2",
        .tokenID = 0x28100002,
        .tokenAttr = 0,
        .dcap =  {"AT_CAP", "ST_CAP"}
    };

    PermissionStateFull infoManagerTestState1 = {
        .grantFlags = {0},
        .grantStatus = {0},
        .isGeneral = true,
        .permissionName = "ohos.permission.ACCELEROMETER",
        .resDeviceID = {"local"}
    };

    PermissionStateFull infoManagerTestState2 = {
        .permissionName = "ohos.permission.MANAGE_USER_IDM",
        .isGeneral = true,
        .grantFlags = {0, 2},
        .grantStatus = {0, 0},
        .resDeviceID = {"device 1", "device 2"}
    };

    PermissionStateFull infoManagerTestState3 = {
        .permissionName = "ohos.permission.USER_TEAT",
        .isGeneral = true,
        .grantFlags = {0, 2},
        .grantStatus = {0, 0},
        .resDeviceID = {"device 1", "device 2"}
    };

    std::vector<PermissionStateFull> permStateList = {
        infoManagerTestState1, infoManagerTestState2, infoManagerTestState3};
    std::shared_ptr<NativeTokenInfoInner> nativeToken1 = std::make_shared<NativeTokenInfoInner>(info1, permStateList);

    std::shared_ptr<PermissionPolicySet> permPolicySet =
        nativeToken1->GetNativeInfoPermissionPolicySet();
    GTEST_LOG_(INFO) <<"permPolicySet: " << permPolicySet;

    std::vector<PermissionStateFull> permList;
    permPolicySet->GetPermissionStateFulls(permList);
    for (auto& perm : permList) {
        GTEST_LOG_(INFO) <<"perm.permissionName: " << perm.permissionName;
    }

    tokenInfos.emplace_back(nativeToken1);

    std::shared_ptr<NativeTokenInfoInner> nativeToken2 = std::make_shared<NativeTokenInfoInner>(info2, permStateList);
    tokenInfos.emplace_back(nativeToken2);

    AccessTokenInfoManager::GetInstance().ProcessNativeTokenInfos(tokenInfos);
    NativeTokenInfo findInfo;

    int ret = AccessTokenInfoManager::GetInstance().GetNativeTokenInfo(info1.tokenID, findInfo);
    ASSERT_EQ(ret, RET_SUCCESS);
    ASSERT_EQ(findInfo.apl, info1.apl);
    ASSERT_EQ(findInfo.ver, info1.ver);
    ASSERT_EQ(findInfo.processName, info1.processName);
    ASSERT_EQ(findInfo.tokenID, info1.tokenID);
    ASSERT_EQ(findInfo.tokenAttr, info1.tokenAttr);
    ASSERT_EQ(findInfo.dcap, info1.dcap);

    ret = PermissionManager::GetInstance().VerifyAccessToken(info1.tokenID, "ohos.permission.MANAGE_USER_IDM");
    ASSERT_EQ(ret, PERMISSION_GRANTED);
    ret = PermissionManager::GetInstance().VerifyAccessToken(info1.tokenID, "ohos.permission.ACCELEROMETER");
    ASSERT_EQ(ret, PERMISSION_GRANTED);
    ret = PermissionManager::GetInstance().VerifyAccessToken(info1.tokenID, "ohos.permission.DISCOVER_BLUETOOTH");
    ASSERT_EQ(ret, PERMISSION_DENIED);

    ret = AccessTokenInfoManager::GetInstance().GetNativeTokenInfo(info2.tokenID, findInfo);
    ASSERT_EQ(ret, RET_SUCCESS);
    ASSERT_EQ(findInfo.apl, info2.apl);
    ASSERT_EQ(findInfo.ver, info2.ver);
    ASSERT_EQ(findInfo.processName, info2.processName);
    ASSERT_EQ(findInfo.tokenID, info2.tokenID);
    ASSERT_EQ(findInfo.tokenAttr, info2.tokenAttr);
    ASSERT_EQ(findInfo.dcap, info2.dcap);

    ret = AccessTokenInfoManager::GetInstance().RemoveNativeTokenInfo(info1.tokenID);
    ASSERT_EQ(ret, RET_SUCCESS);

    ret = PermissionManager::GetInstance().VerifyAccessToken(info2.tokenID, "ohos.permission.MANAGE_USER_IDM");
    ASSERT_EQ(ret, PERMISSION_GRANTED);
    ret = PermissionManager::GetInstance().VerifyAccessToken(info2.tokenID, "ohos.permission.ACCELEROMETER");
    ASSERT_EQ(ret, PERMISSION_GRANTED);

    ret = AccessTokenInfoManager::GetInstance().RemoveNativeTokenInfo(info2.tokenID);
    ASSERT_EQ(ret, RET_SUCCESS);
}

/**
 * @tc.name: ProcessNativeTokenInfos003
 * @tc.desc: test add nullptr tokenInfo.
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, ProcessNativeTokenInfos003, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test ProcessNativeTokenInfos003!");
    std::vector<std::shared_ptr<NativeTokenInfoInner>> tokenInfos;

    std::shared_ptr<NativeTokenInfoInner> nativeToken1 = std::make_shared<NativeTokenInfoInner>();
    tokenInfos.emplace_back(nativeToken1);
    AccessTokenInfoManager::GetInstance().ProcessNativeTokenInfos(tokenInfos);
    ASSERT_EQ(RET_SUCCESS, RET_SUCCESS);
}

/**
 * @tc.name: ProcessNativeTokenInfos004
 * @tc.desc: test add repeat id, but process doesn't
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, ProcessNativeTokenInfos004, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test ProcessNativeTokenInfos004!");
    std::vector<std::shared_ptr<NativeTokenInfoInner>> tokenInfos;

    NativeTokenInfo info3 = {
        .apl = APL_NORMAL,
        .ver = 1,
        .processName = "native_token_test3",
        .tokenID = 0x28100003,
        .tokenAttr = 0,
        .dcap =  {"AT_CAP", "ST_CAP"}
    };

    NativeTokenInfo info4 = {
        .apl = APL_NORMAL,
        .ver = 1,
        .processName = "native_token_test4",
        .tokenID = 0x28100003,
        .tokenAttr = 0,
        .dcap =  {"AT_CAP", "ST_CAP"}
    };
    std::vector<PermissionStateFull> permStateList = {};
    std::shared_ptr<NativeTokenInfoInner> nativeToken3 = std::make_shared<NativeTokenInfoInner>(info3, permStateList);
    tokenInfos.emplace_back(nativeToken3);

    std::shared_ptr<NativeTokenInfoInner> nativeToken4 = std::make_shared<NativeTokenInfoInner>(info4, permStateList);
    tokenInfos.emplace_back(nativeToken4);

    AccessTokenInfoManager::GetInstance().ProcessNativeTokenInfos(tokenInfos);

    NativeTokenInfo findInfo;
    int ret = AccessTokenInfoManager::GetInstance().GetNativeTokenInfo(info3.tokenID, findInfo);
    ASSERT_EQ(ret, RET_SUCCESS);
    ASSERT_EQ(findInfo.apl, info3.apl);
    ASSERT_EQ(findInfo.ver, info3.ver);
    ASSERT_EQ(findInfo.processName, info3.processName);
    ASSERT_EQ(findInfo.tokenID, info3.tokenID);
    ASSERT_EQ(findInfo.tokenAttr, info3.tokenAttr);
    ASSERT_EQ(findInfo.dcap, info3.dcap);

    ret = AccessTokenInfoManager::GetInstance().RemoveNativeTokenInfo(info3.tokenID);
    ASSERT_EQ(ret, RET_SUCCESS);
}

/**
 * @tc.name: ProcessNativeTokenInfos005
 * @tc.desc: test add repeat process, but id doesn't
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, ProcessNativeTokenInfos005, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test ProcessNativeTokenInfos005!");
    std::vector<std::shared_ptr<NativeTokenInfoInner>> tokenInfos;

    NativeTokenInfo info5 = {
        .apl = APL_NORMAL,
        .ver = 1,
        .processName = "native_token_test5",
        .tokenID = 0x28100005,
        .tokenAttr = 0,
        .dcap =  {"AT_CAP", "ST_CAP"}
    };

    NativeTokenInfo info6 = {
        .apl = APL_NORMAL,
        .ver = 1,
        .processName = "native_token_test5",
        .tokenID = 0x28100006,
        .tokenAttr = 0,
        .dcap =  {"AT_CAP", "ST_CAP"}
    };
    std::vector<PermissionStateFull> permStateList = {};
    std::shared_ptr<NativeTokenInfoInner> nativeToken5 = std::make_shared<NativeTokenInfoInner>(info5, permStateList);
    tokenInfos.emplace_back(nativeToken5);

    std::shared_ptr<NativeTokenInfoInner> nativeToken6 = std::make_shared<NativeTokenInfoInner>(info6, permStateList);
    tokenInfos.emplace_back(nativeToken6);

    AccessTokenInfoManager::GetInstance().ProcessNativeTokenInfos(tokenInfos);

    NativeTokenInfo findInfo;
    int ret = AccessTokenInfoManager::GetInstance().GetNativeTokenInfo(info5.tokenID, findInfo);
    ASSERT_EQ(ret, RET_FAILED);

    ret = AccessTokenInfoManager::GetInstance().GetNativeTokenInfo(info6.tokenID, findInfo);
    ASSERT_EQ(ret, RET_SUCCESS);
    ASSERT_EQ(findInfo.apl, info6.apl);
    ASSERT_EQ(findInfo.ver, info6.ver);
    ASSERT_EQ(findInfo.processName, info6.processName);
    ASSERT_EQ(findInfo.tokenID, info6.tokenID);
    ASSERT_EQ(findInfo.tokenAttr, info6.tokenAttr);
    ASSERT_EQ(findInfo.dcap, info6.dcap);

    ret = AccessTokenInfoManager::GetInstance().RemoveNativeTokenInfo(info6.tokenID);
    ASSERT_EQ(ret, RET_SUCCESS);
}

/**
 * @tc.name: ProcessNativeTokenInfos006
 * @tc.desc: test add repeat process and id
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, ProcessNativeTokenInfos006, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test ProcessNativeTokenInfos006!");
    std::vector<std::shared_ptr<NativeTokenInfoInner>> tokenInfos;

    NativeTokenInfo info7 = {
        .apl = APL_NORMAL,
        .ver = 1,
        .processName = "native_token_test7",
        .tokenID = 0x28100007,
        .tokenAttr = 0,
        .dcap =  {"AT_CAP", "ST_CAP"}
    };

    NativeTokenInfo info8 = {
        .apl = APL_SYSTEM_BASIC,
        .ver = 1,
        .processName = "native_token_test7",
        .tokenID = 0x28100007,
        .tokenAttr = 0,
        .dcap =  {"AT_CAP"}
    };
    std::vector<PermissionStateFull> permStateList = {};
    std::shared_ptr<NativeTokenInfoInner> nativeToken7 = std::make_shared<NativeTokenInfoInner>(info7, permStateList);
    tokenInfos.emplace_back(nativeToken7);

    std::shared_ptr<NativeTokenInfoInner> nativeToken8 = std::make_shared<NativeTokenInfoInner>(info8, permStateList);
    tokenInfos.emplace_back(nativeToken8);

    AccessTokenInfoManager::GetInstance().ProcessNativeTokenInfos(tokenInfos);

    NativeTokenInfo findInfo;
    int ret = AccessTokenInfoManager::GetInstance().GetNativeTokenInfo(info7.tokenID, findInfo);
    ASSERT_EQ(ret, RET_SUCCESS);
    ASSERT_EQ(findInfo.apl, info8.apl);
    ASSERT_EQ(findInfo.ver, info8.ver);
    ASSERT_EQ(findInfo.processName, info8.processName);
    ASSERT_EQ(findInfo.tokenID, info8.tokenID);
    ASSERT_EQ(findInfo.tokenAttr, info8.tokenAttr);
    ASSERT_EQ(findInfo.dcap, info8.dcap);

    ret = AccessTokenInfoManager::GetInstance().RemoveNativeTokenInfo(info8.tokenID);
    ASSERT_EQ(ret, RET_SUCCESS);
}

/**
 * @tc.name: init001
 * @tc.desc: test get native cfg
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, init001, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test init001!");
    std::vector<std::shared_ptr<NativeTokenInfoInner>> tokenInfos;

    const char *dcaps[1];
    dcaps[0] = "AT_CAP_01";
    int dcapNum = 1;
    const char *perms[2];
    perms[0] = "ohos.permission.test1";
    perms[1] = "ohos.permission.test2";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = dcapNum,
        .permsNum = 2,
        .dcaps = dcaps,
        .perms = perms,
        .processName = "native_token_test7",
        .aplStr = "system_core",
    };
    uint64_t tokenId = ::GetAccessTokenId(&infoInstance);
    ASSERT_NE(tokenId, 0);

    NativeTokenReceptor::GetInstance().Init();
    NativeTokenInfo findInfo;
    int ret = AccessTokenInfoManager::GetInstance().GetNativeTokenInfo(tokenId, findInfo);
    ASSERT_EQ(ret, RET_SUCCESS);
    ASSERT_EQ(findInfo.processName, infoInstance.processName);

    ret = AccessTokenInfoManager::GetInstance().RemoveNativeTokenInfo(tokenId);
    ASSERT_EQ(ret, RET_SUCCESS);
}

/**
 * @tc.name: ProcessNativeTokenInfos007
 * @tc.desc: test get native cfg
 * @tc.type: FUNC
 * @tc.require: Issue Number
 */
HWTEST_F(NativeTokenReceptorTest, ProcessNativeTokenInfos007, TestSize.Level1)
{
    ACCESSTOKEN_LOG_INFO(LABEL, "test ProcessNativeTokenInfos007!");

    const char *dcaps[1];
    dcaps[0] = "AT_CAP_01";
    int dcapNum = 1;

    char apl3[32];
    (void)strcpy_s(apl3, sizeof(apl3), "system_core");
    char apl2[32];
    (void)strcpy_s(apl2, sizeof(apl2), "system_basic");
    char apl1[32];
    (void)strcpy_s(apl1, sizeof(apl1), "normal");

    NativeTokenInfoParams infoInstance = {
        .dcapsNum = dcapNum,
        .permsNum = 0,
        .dcaps = dcaps,
        .perms = nullptr,
    };
    infoInstance.aplStr = apl3;
    infoInstance.processName = "ProcessNativeTokenInfos007_003";
    uint64_t tokenIdApl3 = ::GetAccessTokenId(&infoInstance);
    ASSERT_NE(tokenIdApl3, 0);

    infoInstance.aplStr = apl2;
    infoInstance.processName = "ProcessNativeTokenInfos007_002";
    uint64_t tokenIdApl2 = ::GetAccessTokenId(&infoInstance);
    ASSERT_NE(tokenIdApl2, 0);

    infoInstance.aplStr = apl1;
    infoInstance.processName = "ProcessNativeTokenInfos007_001";
    uint64_t tokenIdApl1 = ::GetAccessTokenId(&infoInstance);
    ASSERT_NE(tokenIdApl1, 0);

    NativeTokenReceptor& receptor = NativeTokenReceptor::GetInstance();
    receptor.ready_ = false;

    receptor.Init();
    // wait fresh tokens to sql.
    sleep(3);

    const std::string permission = "ohos.permission.SEND_MESSAGES";
    int ret = PermissionManager::GetInstance().VerifyNativeToken(tokenIdApl3, permission);
    ASSERT_EQ(ret, PERMISSION_GRANTED);

    ret = PermissionManager::GetInstance().VerifyNativeToken(tokenIdApl2, permission);
    ASSERT_EQ(ret, PERMISSION_GRANTED);

    ret = PermissionManager::GetInstance().VerifyNativeToken(tokenIdApl1, permission);
    ASSERT_EQ(ret, PERMISSION_DENIED);

    ret = AccessTokenInfoManager::GetInstance().RemoveNativeTokenInfo(tokenIdApl3);
    ASSERT_EQ(ret, RET_SUCCESS);
    ret = AccessTokenInfoManager::GetInstance().RemoveNativeTokenInfo(tokenIdApl2);
    ASSERT_EQ(ret, RET_SUCCESS);
    ret = AccessTokenInfoManager::GetInstance().RemoveNativeTokenInfo(tokenIdApl1);
    ASSERT_EQ(ret, RET_SUCCESS);
}