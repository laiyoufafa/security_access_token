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

#include "privacy_kit_test.h"

#include "accesstoken_kit.h"
#include "nativetoken_kit.h"
#include "parameter.h"
#include "privacy_kit.h"
#include "token_setproc.h"

using namespace testing::ext;
using namespace OHOS::Security::AccessToken;

const static int32_t RET_NO_ERROR = 0;
const static int32_t RET_ERROR = -1;

static HapPolicyParams g_PolicyPramsA = {
    .apl = APL_NORMAL,
    .domain = "test.domain.A",
};

static HapInfoParams g_InfoParmsA = {
    .userID = 1,
    .bundleName = "ohos.privacy_test.bundleA",
    .instIndex = 0,
    .appIDDesc = "privacy_test.bundleA"
};

static HapPolicyParams g_PolicyPramsB = {
    .apl = APL_NORMAL,
    .domain = "test.domain.B",
};

static HapInfoParams g_InfoParmsB = {
    .userID = 1,
    .bundleName = "ohos.privacy_test.bundleB",
    .instIndex = 0,
    .appIDDesc = "privacy_test.bundleB"
};

static AccessTokenID g_selfTokenId = 0;
static AccessTokenID g_TokenId_A = 0;
static AccessTokenID g_TokenId_B = 0;

void PrivacyKitTest::SetUpTestCase()
{
    g_selfTokenId = GetSelfTokenID();
}

void PrivacyKitTest::TearDownTestCase()
{
}

void PrivacyKitTest::SetUp()
{
    AccessTokenKit::AllocHapToken(g_InfoParmsA, g_PolicyPramsA);
    AccessTokenKit::AllocHapToken(g_InfoParmsB, g_PolicyPramsB);

    g_TokenId_A = AccessTokenKit::GetHapTokenID(g_InfoParmsA.userID,
                                                g_InfoParmsA.bundleName,
                                                g_InfoParmsA.instIndex);
    g_TokenId_B = AccessTokenKit::GetHapTokenID(g_InfoParmsB.userID,
                                                g_InfoParmsB.bundleName,
                                                g_InfoParmsB.instIndex);

    AccessTokenID tokenId = AccessTokenKit::GetHapTokenID(100, "com.ohos.permissionmanager", 0);
    SetSelfTokenID(tokenId);
}

void PrivacyKitTest::TearDown()
{
    SetSelfTokenID(g_selfTokenId);
    AccessTokenID tokenId = AccessTokenKit::GetHapTokenID(g_InfoParmsA.userID,
                                                          g_InfoParmsA.bundleName,
                                                          g_InfoParmsA.instIndex);
    AccessTokenKit::DeleteToken(tokenId);

    tokenId = AccessTokenKit::GetHapTokenID(g_InfoParmsB.userID,
                                            g_InfoParmsB.bundleName,
                                            g_InfoParmsB.instIndex);
    AccessTokenKit::DeleteToken(tokenId);
}

std::string PrivacyKitTest::GetLocalDeviceUdid()
{
    const int32_t DEVICE_UUID_LENGTH = 65;
    char udid[DEVICE_UUID_LENGTH] = {0};
    GetDevUdid(udid, DEVICE_UUID_LENGTH);
    return udid;
}

void PrivacyKitTest::BuildQueryRequest(AccessTokenID tokenId, const std::string deviceId, const std::string& bundleName,
    const std::vector<std::string> permissionList, PermissionUsedRequest& request)
{
    request.tokenId = tokenId;
    request.isRemote = false;
    request.deviceId = deviceId;
    request.bundleName = bundleName;
    request.permissionList = permissionList;
    request.beginTimeMillis = 0;
    request.endTimeMillis = 0;
    request.flag = FLAG_PERMISSION_USAGE_SUMMARY;
}

void PrivacyKitTest::CheckPermissionUsedResult(const PermissionUsedRequest& request, const PermissionUsedResult& result,
    int32_t permRecordSize, int32_t totalSuccessCount, int32_t totalFailCount)
{
    int32_t successCount = 0;
    int32_t failCount = 0;
    ASSERT_EQ(request.tokenId, result.bundleRecords[0].tokenId);
    ASSERT_EQ(request.isRemote, result.bundleRecords[0].isRemote);
    ASSERT_EQ(request.deviceId, result.bundleRecords[0].deviceId);
    ASSERT_EQ(request.bundleName, result.bundleRecords[0].bundleName);
    ASSERT_EQ(permRecordSize, result.bundleRecords[0].permissionRecords.size());
    for (int32_t i = 0; i < permRecordSize; i++) {
        successCount += result.bundleRecords[0].permissionRecords[i].accessCount;
        failCount += result.bundleRecords[0].permissionRecords[i].rejectCount;
    }
    ASSERT_EQ(totalSuccessCount, successCount);
    ASSERT_EQ(totalFailCount, failCount);
}

/**
 * @tc.name: AddPermissionUsedRecord001
 * @tc.desc: cannot AddPermissionUsedRecord with illegal tokenId and permission.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, AddPermissionUsedRecord001, TestSize.Level1)
{
    ASSERT_EQ(RET_ERROR, PrivacyKit::AddPermissionUsedRecord(
        0, "ohos.permission.READ_CONTACTS", 1, 0));
    ASSERT_EQ(RET_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "", 1, 0));
    ASSERT_EQ(RET_ERROR, PrivacyKit::AddPermissionUsedRecord(
        g_TokenId_A, "ohos.permission.READ_CONTACTS", -1, 0));

    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(0, result.bundleRecords.size());
}

/**
 * @tc.name: AddPermissionUsedRecord002
 * @tc.desc: cannot AddPermissionUsedRecord with invalid tokenId and permission.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, AddPermissionUsedRecord002, TestSize.Level1)
{
    ASSERT_EQ(RET_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.test", 1, 0));
    ASSERT_EQ(RET_ERROR, PrivacyKit::AddPermissionUsedRecord(123, "ohos.permission.CAMERA", 1, 0));
    ASSERT_EQ(RET_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.READ_CONTACTS", 0, 0));

    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    BuildQueryRequest(123, "", "", permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(0, result.bundleRecords.size());

    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(0, result.bundleRecords.size());
}

/**
 * @tc.name: AddPermissionUsedRecord003
 * @tc.desc: cannot AddPermissionUsedRecord with native tokenId.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, AddPermissionUsedRecord003, TestSize.Level1)
{
    const char **dcaps = new const char *[2];
    dcaps[0] = "AT_CAP";
    dcaps[1] = "ST_CAP";
    uint64_t tokenId;
    const char **acls = new const char *[2];
    acls[0] = "ohos.permission.test1";
    acls[1] = "ohos.permission.test2";
    const char **perms = new const char *[2];
    perms[0] = "ohos.permission.test1";
    perms[1] = "ohos.permission.test2";
    NativeTokenInfoParams infoInstance = {
        .dcapsNum = 2,
        .permsNum = 2,
        .aclsNum = 2,
        .dcaps = dcaps,
        .perms = perms,
        .acls = acls,
        .processName = "GetAccessTokenId008",
        .aplStr = "system_core",
    };
    tokenId = GetAccessTokenId(&infoInstance);
    ASSERT_NE(tokenId, 0);

    delete[] perms;
    delete[] dcaps;
    delete[] acls;

    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(
        tokenId, "ohos.permission.READ_CONTACTS", 1, 0));

    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    BuildQueryRequest(tokenId, "", "", permissionList, request);

    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(0, result.bundleRecords.size());
}

/**
 * @tc.name: AddPermissionUsedRecord004
 * @tc.desc: AddPermissionUsedRecord user_grant permission.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, AddPermissionUsedRecord004, TestSize.Level1)
{
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.WRITE_CONTACTS", 0, 1));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.LOCATION", 1, 1));

    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));

    ASSERT_EQ(1, result.bundleRecords.size());
    CheckPermissionUsedResult(request, result, 3, 2, 2);
}

/**
 * @tc.name: AddPermissionUsedRecord005
 * @tc.desc: AddPermissionUsedRecord user_grant permission.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, AddPermissionUsedRecord005, TestSize.Level1)
{
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.LOCATION", 0, 1));

    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_B,  "ohos.permission.CAMERA", 0, 1));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_B,  "ohos.permission.LOCATION", 1, 0));


    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));

    ASSERT_EQ(1, result.bundleRecords.size());
    CheckPermissionUsedResult(request, result, 2, 1, 1);

    BuildQueryRequest(g_TokenId_B, GetLocalDeviceUdid(), g_InfoParmsB.bundleName, permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));

    ASSERT_EQ(1, result.bundleRecords.size());
    CheckPermissionUsedResult(request, result, 2, 1, 1);
}

/**
 * @tc.name: AddPermissionUsedRecord006
 * @tc.desc: AddPermissionUsedRecord permission combine records.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, AddPermissionUsedRecord006, TestSize.Level1)
{
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));

    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);
    request.flag = FLAG_PERMISSION_USAGE_DETAIL;
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));

    ASSERT_EQ(1, result.bundleRecords.size());
    ASSERT_EQ(1, result.bundleRecords[0].permissionRecords.size());
    ASSERT_EQ(1, result.bundleRecords[0].permissionRecords[0].accessRecords.size());
    CheckPermissionUsedResult(request, result, 1, 4, 0);

    sleep(61);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));
   
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));

    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(1, result.bundleRecords.size());
    ASSERT_EQ(1, result.bundleRecords[0].permissionRecords.size());
    ASSERT_EQ(2, result.bundleRecords[0].permissionRecords[0].accessRecords.size());
    CheckPermissionUsedResult(request, result, 1, 5, 0);
}

/**
 * @tc.name: RemovePermissionUsedRecords001
 * @tc.desc: cannot RemovePermissionUsedRecords with illegal tokenId and deviceID.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, RemovePermissionUsedRecords001, TestSize.Level1)
{
    ASSERT_EQ(RET_ERROR, PrivacyKit::RemovePermissionUsedRecords(0, ""));
}

/**
 * @tc.name: RemovePermissionUsedRecords002
 * @tc.desc: RemovePermissionUsedRecords with invalid tokenId and deviceID.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, RemovePermissionUsedRecords002, TestSize.Level1)
{
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));
    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);

    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::RemovePermissionUsedRecords(g_TokenId_A, "invalid_device"));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(1, result.bundleRecords.size());

    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::RemovePermissionUsedRecords(123, GetLocalDeviceUdid()));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(1, result.bundleRecords.size());
}

/**
 * @tc.name: RemovePermissionUsedRecords003
 * @tc.desc: RemovePermissionUsedRecords with valid tokenId and deviceID.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, RemovePermissionUsedRecords003, TestSize.Level1)
{
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));
    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);

    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::RemovePermissionUsedRecords(g_TokenId_A, ""));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(0, result.bundleRecords.size());
}

/**
 * @tc.name: GetPermissionUsedRecords001
 * @tc.desc: cannot GetPermissionUsedRecords with invalid query request.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, GetPermissionUsedRecords001, TestSize.Level1)
{
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.MICROPHONE", 1, 0));
    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);
    request.beginTimeMillis = -1;
    request.endTimeMillis = -1;
    ASSERT_EQ(RET_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));

    request.beginTimeMillis = 3;
    request.endTimeMillis = 1;
    ASSERT_EQ(RET_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));

    request.flag = -1;
    ASSERT_EQ(RET_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));

    request.flag = 3;
    ASSERT_EQ(RET_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
}

/**
 * @tc.name: GetPermissionUsedRecords002
 * @tc.desc: cannot GetPermissionUsedRecords with valid query time.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, GetPermissionUsedRecords002, TestSize.Level1)
{
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.MICROPHONE", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.READ_CALENDAR", 1, 0));

    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    // query by tokenId
    BuildQueryRequest(g_TokenId_A, "", "", permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(1, result.bundleRecords.size());
    request.deviceId = GetLocalDeviceUdid();
    request.bundleName = g_InfoParmsA.bundleName;
    CheckPermissionUsedResult(request, result, 3, 3, 0);

    // query by deviceId and bundle Name
    BuildQueryRequest(0, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(1, result.bundleRecords.size());
    request.tokenId = g_TokenId_A;
    CheckPermissionUsedResult(request, result, 3, 3, 0);

    // query by unmatched tokenId, deviceId and bundle Name
    BuildQueryRequest(123, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(0, result.bundleRecords.size());

    // query by unmatched tokenId, deviceId and bundle Name
    BuildQueryRequest(g_TokenId_A, "local device", g_InfoParmsA.bundleName, permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(0, result.bundleRecords.size());

    // query by unmatched tokenId, deviceId and bundle Name
    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), "bundleA", permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(0, result.bundleRecords.size());
}

/**
 * @tc.name: GetPermissionUsedRecords003
 * @tc.desc: cannot GetPermissionUsedRecords with valid query time.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, GetPermissionUsedRecords003, TestSize.Level1)
{
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.MICROPHONE", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.MICROPHONE", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.MICROPHONE", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.MICROPHONE", 1, 0));

    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(1, result.bundleRecords.size());
    CheckPermissionUsedResult(request, result, 1, 4, 0);

    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.READ_CALENDAR", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.WRITE_CALENDAR", 1, 0));

    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), g_InfoParmsA.bundleName, permissionList, request);
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    ASSERT_EQ(1, result.bundleRecords.size());
    CheckPermissionUsedResult(request, result, 4, 7, 0);
}

/**
 * @tc.name: GetPermissionUsedRecords004
 * @tc.desc: cannot GetPermissionUsedRecords with valid query time.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, GetPermissionUsedRecords004, TestSize.Level1)
{
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.CAMERA", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, "ohos.permission.READ_CALENDAR", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_B, "ohos.permission.CAMERA", 1, 0));
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_B, "ohos.permission.READ_CALENDAR", 1, 0));

    PermissionUsedRequest request;
    PermissionUsedResult result;
    std::vector<std::string> permissionList;
    BuildQueryRequest(0, GetLocalDeviceUdid(), "", permissionList, request);

    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, result));
    if (result.bundleRecords.size() < 2) {
        ASSERT_EQ(RET_NO_ERROR, RET_ERROR);
    }
}

/**
 * @tc.name: GetPermissionUsedRecordsAsync001
 * @tc.desc: cannot GetPermissionUsedRecordsAsync with invalid query time.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, GetPermissionUsedRecordsAsync001, TestSize.Level1)
{
    std::string permission = "ohos.permission.CAMERA";
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, permission, 1, 0));
    PermissionUsedRequest request;
    std::vector<std::string> permissionList;
    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), "", permissionList, request);
    request.beginTimeMillis = -1;
    request.endTimeMillis = -1;
    OHOS::sptr<TestCallBack> callback(new TestCallBack());
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, callback));
}

/**
 * @tc.name: GetPermissionUsedRecordsAsync002
 * @tc.desc: cannot GetPermissionUsedRecordsAsync with valid query time.
 * @tc.type: FUNC
 * @tc.require:Issue Number
 */
HWTEST_F(PrivacyKitTest, GetPermissionUsedRecordsAsync002, TestSize.Level1)
{
    std::string permission = "ohos.permission.CAMERA";
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::AddPermissionUsedRecord(g_TokenId_A, permission, 1, 0));
    PermissionUsedRequest request;
    std::vector<std::string> permissionList;
    BuildQueryRequest(g_TokenId_A, GetLocalDeviceUdid(), "", permissionList, request);
    OHOS::sptr<TestCallBack> callback(new TestCallBack());
    ASSERT_EQ(RET_NO_ERROR, PrivacyKit::GetPermissionUsedRecords(request, callback));
}