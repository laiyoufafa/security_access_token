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
#include "soft_bus_channel.h"

#include <securec.h>

#include "device_info_manager.h"
#include "token_sync_event_handler.h"
#include "token_sync_manager_service.h"
#include "singleton.h"
#include "soft_bus_manager.h"

namespace OHOS {
namespace Security {
namespace AccessToken {
namespace {
static constexpr OHOS::HiviewDFX::HiLogLabel LABEL = {LOG_CORE, SECURITY_DOMAIN_ACCESSTOKEN, "SoftBusChannel"};
}
namespace {
static const std::string REQUEST_TYPE = "request";
static const std::string RESPONSE_TYPE = "response";
static const std::string TASK_NAME_CLOSE_SESSION = "atm_soft_bus_channel_close_session";
static const int32_t EXECUTE_COMMAND_TIME_OUT = 3000;
static const int32_t WAIT_SESSION_CLOSE_MILLISECONDS = 5 * 1000;
// send buf size for header
static const int RPC_TRANSFER_HEAD_BYTES_LENGTH = 1024 * 256;
// decompress buf size
static const int RPC_TRANSFER_BYTES_MAX_LENGTH = 1024 * 1024;
} // namespace
SoftBusChannel::SoftBusChannel(const std::string &deviceId)
    : deviceId_(deviceId), mutex_(), callbacks_(), responseResult_(""), loadedCond_()
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "SoftBusChannel(deviceId)");
    isDelayClosing_ = false;
    session_ = Constant::INVALID_SESSION;
    isSessionUsing_ = false;
}

SoftBusChannel::~SoftBusChannel()
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "~SoftBusChannel()");
}

int SoftBusChannel::BuildConnection()
{
    CancelCloseConnectionIfNeeded();
    if (session_ != Constant::INVALID_SESSION) {
        ACCESSTOKEN_LOG_INFO(LABEL, "session is exist, no need open again.");
        return Constant::SUCCESS;
    }

    std::unique_lock<std::mutex> lock(sessionMutex_);
    if (session_ == Constant::INVALID_SESSION) {
        ACCESSTOKEN_LOG_INFO(LABEL, "open session with device: %{public}s", (deviceId_.c_str()));
        int session = SoftBusManager::GetInstance().OpenSession(deviceId_);
        if (session == Constant::INVALID_SESSION) {
            ACCESSTOKEN_LOG_ERROR(LABEL, "open session failed.");
            return Constant::FAILURE;
        }
        session_ = session;
    }
    return Constant::SUCCESS;
}

void SoftBusChannel::CloseConnection()
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "close connection");
    std::unique_lock<std::mutex> lock(mutex_);
    if (isDelayClosing_) {
        return;
    }

    std::shared_ptr<TokenSyncEventHandler> handler =
        DelayedSingleton<TokenSyncManagerService>::GetInstance()->GetSendEventHandler();
    if (handler == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "fail to get EventHandler");
        return;
    }
    auto thisPtr = shared_from_this();
    std::function<void()> delayed = ([thisPtr]() {
        std::unique_lock<std::mutex> lock(thisPtr->sessionMutex_);
        if (thisPtr->isSessionUsing_) {
            ACCESSTOKEN_LOG_DEBUG(LABEL, "session is in using, cancel close session");
        } else {
            SoftBusManager::GetInstance().CloseSession(thisPtr->session_);
            thisPtr->session_ = Constant::INVALID_SESSION;
            ACCESSTOKEN_LOG_INFO(LABEL, "close session for device: %{public}s", thisPtr->deviceId_.c_str());
        }
        thisPtr->isDelayClosing_ = false;
    });

    ACCESSTOKEN_LOG_DEBUG(LABEL, "close session after %{public}d ms", WAIT_SESSION_CLOSE_MILLISECONDS);
    handler->ProxyPostTask(delayed, TASK_NAME_CLOSE_SESSION, WAIT_SESSION_CLOSE_MILLISECONDS);

    isDelayClosing_ = true;
}

void SoftBusChannel::Release()
{
    std::shared_ptr<TokenSyncEventHandler> handler =
        DelayedSingleton<TokenSyncManagerService>::GetInstance()->GetSendEventHandler();
    if (handler == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "fail to get EventHandler");
        return;
    }
    handler->ProxyRemoveTask(TASK_NAME_CLOSE_SESSION);
}

std::string SoftBusChannel::ExecuteCommand(const std::string &commandName, const std::string &jsonPayload)
{
    if (commandName.empty() || jsonPayload.empty()) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "invalid params, commandName: %{public}s, jsonPayload: %{public}s",
            commandName.c_str(), jsonPayload.c_str());
        return "";
    }

    // to use a lib like libuuid
    int uuidStrLen = 37; // 32+4+1
    char uuidbuf[uuidStrLen];
    random_uuid(uuidbuf, uuidStrLen);
    std::string uuid(uuidbuf);
    ACCESSTOKEN_LOG_DEBUG(LABEL, "generated message uuid: %{public}s", uuid.c_str());

    int len = (signed)(RPC_TRANSFER_HEAD_BYTES_LENGTH + jsonPayload.length());
    unsigned char *buf = new unsigned char[len + 1];
    if (buf == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "no enough memory: %{public}d", len);
        return "";
    }
    (void)memset_s(buf, len + 1, 0, len + 1);
    int result = PrepareBytes(REQUEST_TYPE, uuid, commandName, jsonPayload, buf, len);
    if (result != Constant::SUCCESS) {
        delete[] buf;
        return "";
    }

    std::unique_lock<std::mutex> lock(sessionMutex_);
    std::function<void(const std::string &)> callback = [&](const std::string &result) {
        ACCESSTOKEN_LOG_INFO(LABEL, "onResponse called, data: %{public}s", result.c_str());
        responseResult_ = std::string(result);
        loadedCond_.notify_all();
        ACCESSTOKEN_LOG_DEBUG(LABEL, "onResponse called end");
    };
    callbacks_.insert(std::pair<std::string, std::function<void(std::string)>>(uuid, callback));

    isSessionUsing_ = true;
    lock.unlock();

    int retCode = SendRequestBytes(buf, len);
    delete[] buf;

    std::unique_lock<std::mutex> lock2(sessionMutex_);
    if (retCode != Constant::SUCCESS) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "send request data failed: %{public}d ", retCode);
        callbacks_.erase(uuid);
        isSessionUsing_ = false;
        return "";
    }

    ACCESSTOKEN_LOG_DEBUG(LABEL, "wait command response");
    if (loadedCond_.wait_for(lock2, std::chrono::milliseconds(EXECUTE_COMMAND_TIME_OUT)) == std::cv_status::timeout) {
        ACCESSTOKEN_LOG_WARN(LABEL, "time out to wait response.");
        callbacks_.erase(uuid);
        isSessionUsing_ = false;
        return "";
    }

    isSessionUsing_ = false;
    return responseResult_;
}

void SoftBusChannel::HandleDataReceived(int session, const unsigned char *bytes, int length)
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "HandleDataReceived");
#ifdef DEBUG_API_PERFORMANCE
    ACCESSTOKEN_LOG_INFO(LABEL, "api_performance:recieve message from softbus");
#endif
    if (session <= 0 || length <= 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "invalid params: session: %{public}d, data length: %{public}d", session, length);
        return;
    }
    std::string receiveData = Decompress(bytes, length);
    if (receiveData.empty()) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "invalid parameter bytes");
        return;
    }
    std::shared_ptr<SoftBusMessage> message = SoftBusMessage::FromJson(receiveData);
    if (message == nullptr) {
        ACCESSTOKEN_LOG_DEBUG(LABEL, "invalid json string: %{public}s", receiveData.c_str());
        return;
    }
    if (!message->IsValid()) {
        ACCESSTOKEN_LOG_DEBUG(LABEL, "invalid data, has empty field: %{public}s", receiveData.c_str());
        return;
    }

    std::string type = message->GetType();
    if (REQUEST_TYPE == (type)) {
        std::function<void()> delayed = ([=]() {
            HandleRequest(session, message->GetId(), message->GetCommandName(), message->GetJsonPayload());
        });

        std::shared_ptr<TokenSyncEventHandler> handler =
            DelayedSingleton<TokenSyncManagerService>::GetInstance()->GetRecvEventHandler();
        if (handler == nullptr) {
            ACCESSTOKEN_LOG_ERROR(LABEL, "fail to get EventHandler");
            return;
        }
        handler->ProxyPostTask(delayed, "HandleDataReceived_HandleRequest");
    } else if (RESPONSE_TYPE == (type)) {
        HandleResponse(message->GetId(), message->GetJsonPayload());
    } else {
        ACCESSTOKEN_LOG_ERROR(LABEL, "invalid type: %{public}s ", type.c_str());
    }
}

int SoftBusChannel::PrepareBytes(const std::string &type, const std::string &id, const std::string &commandName,
    const std::string &jsonPayload, const unsigned char *bytes, int &bytesLength)
{
    SoftBusMessage messageEntity(type, id, commandName, jsonPayload);
    std::string json = messageEntity.ToJson();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "softbus message json: %{public}s", json.c_str());
    return Compress(json, bytes, bytesLength);
}

int SoftBusChannel::Compress(const std::string &json, const unsigned char *compressedBytes, int &compressedLength)
{
    uLong len = compressBound(json.size());
    // length will not so that long
    if (compressedLength > 0 && (int) len > compressedLength) {
        ACCESSTOKEN_LOG_ERROR(LABEL,
            "compress error. data length overflow, bound length: %{public}d, buffer length: %{public}d", (int) len,
            compressedLength);
        return Constant::FAILURE;
    }

    int result = compress((Byte *) compressedBytes, &len, (unsigned char *) json.c_str(), json.size() + 1);
    if (result != Z_OK) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "compress failed! error code: %{public}d", result);
        return result;
    }
    ACCESSTOKEN_LOG_DEBUG(LABEL, "compress complete. compress %{public}d bytes to %{public}d", compressedLength,
        (int)len);
    compressedLength = (signed)len;
    return Constant::SUCCESS;
}

std::string SoftBusChannel::Decompress(const unsigned char *bytes, const int length)
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "input length: %{public}d", length);
    uLong len = RPC_TRANSFER_BYTES_MAX_LENGTH;
    unsigned char *buf = new unsigned char[len + 1];
    if (buf == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "no enough memory!");
        return "";
    }
    (void)memset_s(buf, len + 1, 0, len + 1);
    int result = uncompress(buf, &len, (unsigned char *) bytes, length);
    if (result != Z_OK) {
        ACCESSTOKEN_LOG_ERROR(LABEL,
            "uncompress failed, error code: %{public}d, bound length: %{public}d, buffer length: %{public}d", result,
            (int)len, length);
        delete[] buf;
        return "";
    }
    buf[len] = '\0';
    std::string str(reinterpret_cast<char *>(buf));
    delete[] buf;
    ACCESSTOKEN_LOG_DEBUG(LABEL, "done, output: %{public}s", str.c_str());
    return str;
}

int SoftBusChannel::SendRequestBytes(const unsigned char *bytes, const int bytesLength)
{
    if (bytesLength == 0) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "bytes data is invalid.");
        return Constant::FAILURE;
    }

    std::unique_lock<std::mutex> lock(sessionMutex_);
    if (CheckSessionMayReopenLocked() != Constant::SUCCESS) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "session invalid and reopen failed!");
        return Constant::FAILURE;
    }

    ACCESSTOKEN_LOG_DEBUG(LABEL, "send len (after compress len)= %{public}d", bytesLength);
#ifdef DEBUG_API_PERFORMANCE
    ACCESSTOKEN_LOG_INFO(LABEL, "api_performance:send command to softbus");
#endif
    int result = ::SendBytes(session_, bytes, bytesLength);
    if (result != Constant::SUCCESS) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "fail to send! result= %{public}d", result);
        return Constant::FAILURE;
    }
    ACCESSTOKEN_LOG_DEBUG(LABEL, "send successfully.");
    return Constant::SUCCESS;
}

int SoftBusChannel::CheckSessionMayReopenLocked()
{
    // when session is opened, we got a valid sessionid, when session closed, we will reset sessionid.
    if (IsSessionAvailable()) {
        return Constant::SUCCESS;
    }
    int session = SoftBusManager::GetInstance().OpenSession(deviceId_);
    if (session != Constant::INVALID_SESSION) {
        session_ = session;
        return Constant::SUCCESS;
    }
    return Constant::FAILURE;
}

bool SoftBusChannel::IsSessionAvailable()
{
    return session_ > Constant::INVALID_SESSION;
}

void SoftBusChannel::CancelCloseConnectionIfNeeded()
{
    std::unique_lock<std::mutex> lock(mutex_);
    if (!isDelayClosing_) {
        return;
    }
    ACCESSTOKEN_LOG_DEBUG(LABEL, "cancel close connection");

    Release();
    isDelayClosing_ = false;
}

void SoftBusChannel::HandleRequest(int session, const std::string &id, const std::string &commandName,
    const std::string &jsonPayload)
{
    std::shared_ptr<BaseRemoteCommand> command =
        RemoteCommandFactory::GetInstance().NewRemoteCommandFromJson(commandName, jsonPayload);
    if (command == nullptr) {
        // send result back directly
        ACCESSTOKEN_LOG_WARN(LABEL, "command %{public}s cannot get from json %{public}s", commandName.c_str(),
            jsonPayload.c_str());

        int sendlen = (signed)(RPC_TRANSFER_HEAD_BYTES_LENGTH + jsonPayload.length());
        unsigned char *sendbuf = new unsigned char[sendlen + 1];
        if (sendbuf == nullptr) {
            ACCESSTOKEN_LOG_ERROR(LABEL, "no enough memory: %{public}d", sendlen);
            return;
        }
        (void)memset_s(sendbuf, sendlen + 1, 0, sendlen + 1);
        int sendResult = PrepareBytes(RESPONSE_TYPE, id, commandName, jsonPayload, sendbuf, sendlen);
        if (sendResult != Constant::SUCCESS) {
            delete[] sendbuf;
            return;
        }
        int sendResultCode = SendResponseBytes(session, sendbuf, sendlen);
        delete[] sendbuf;
        ACCESSTOKEN_LOG_DEBUG(LABEL, "send response result= %{public}d ", sendResultCode);
        return;
    }

    // execute command
    command->Execute();
    ACCESSTOKEN_LOG_DEBUG(LABEL, "command uniqueId: %{public}s, finish with status: %{public}d, message: %{public}s",
        command->remoteProtocol_.uniqueId.c_str(), command->remoteProtocol_.statusCode,
        command->remoteProtocol_.message.c_str());

    // send result back
    std::string resultJsonPayload = command->ToJsonPayload();
    int len = (signed)(RPC_TRANSFER_HEAD_BYTES_LENGTH + resultJsonPayload.length());
    unsigned char *buf = new unsigned char[len + 1];
    if (buf == nullptr) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "no enough memory: %{public}d", len);
        return;
    }
    (void)memset_s(buf, len + 1, 0, len + 1);
    int result = PrepareBytes(RESPONSE_TYPE, id, commandName, resultJsonPayload, buf, len);
    if (result != Constant::SUCCESS) {
        delete[] buf;
        return;
    }
    int retCode = SendResponseBytes(session, buf, len);
    delete[] buf;
    ACCESSTOKEN_LOG_DEBUG(LABEL, "send response result= %{public}d", retCode);
}

void SoftBusChannel::HandleResponse(const std::string &id, const std::string &jsonPayload)
{
    std::unique_lock<std::mutex> lock(sessionMutex_);
    auto callback = callbacks_.find(id);
    if (callback != callbacks_.end()) {
        (callback->second)(jsonPayload);
        callbacks_.erase(callback);
    }
}

int SoftBusChannel::SendResponseBytes(int session, const unsigned char *bytes, const int bytesLength)
{
    ACCESSTOKEN_LOG_DEBUG(LABEL, "send len (after compress len)= %{public}d", bytesLength);
    int result = ::SendBytes(session, bytes, bytesLength);
    if (result != Constant::SUCCESS) {
        ACCESSTOKEN_LOG_ERROR(LABEL, "fail to send! result= %{public}d", result);
        return Constant::FAILURE;
    }
    ACCESSTOKEN_LOG_DEBUG(LABEL, "send successfully.");
    return Constant::SUCCESS;
}
} // namespace AccessToken
} // namespace Security
} // namespace OHOS
