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

export const permissionGroups: any[] = [
  {
    "permissionName": "ohos.permission.LOCATION_IN_BACKGROUND",
    "groupName": "LOCATION",
    "label": "后台定位权限",
    "description": "允许应用在后台运行时获取位置信息。",
    "groupId": 0
  },
  {
    "permissionName": "ohos.permission.LOCATION",
    "groupName": "LOCATION",
    "label": "前台定位权限",
    "description": "允许应用在前台运行时获取位置信息。",
    "groupId": 0
  },
  {
    "permissionName": "ohos.permission.CAMERA",
    "groupName": "CAMERA",
    "label": "拍摄照片和录制视频",
    "description": "允许应用拍摄照片和视频。",
    "groupId": 1
  },
  {
    "permissionName": "ohos.permission.MICROPHONE",
    "groupName": "MICROPHONE",
    "label": "录制音频",
    "description": "允许应用打开或关闭录音通路。",
    "groupId": 2
  },
  {
    "permissionName": "ohos.permission.ANSWER_CALL",
    "groupName": "PHONE",
    "label": "接听电话",
    "description": "允许应用接听电话。",
    "groupId": 3
  },
  {
    "permissionName": "ohos.permission.MANAGE_VOICEMAIL",
    "groupName": "PHONE",
    "label": "语音信箱",
    "description": "允许应用管理语音信箱。",
    "groupId": 3
  },
  {
    "permissionName": "ohos.permission.READ_CELL_MESSAGES",
    "groupName": "SMS",
    "label": "读取小区广播",
    "description": "允许应用读取设备接收的小区广播信息。",
    "groupId": 4
  },
  {
    "permissionName": "ohos.permission.READ_MESSAGES",
    "groupName": "SMS",
    "label": "读取短彩信",
    "description": "允许应用读取设备接收的短彩信信息。",
    "groupId": 4
  },
  {
    "permissionName": "ohos.permission.RECEIVE_MMS",
    "groupName": "SMS",
    "label": "接收彩信",
    "description": "允许应用接收彩信。",
    "groupId": 4
  },
  {
    "permissionName": "ohos.permission.RECEIVE_SMS",
    "groupName": "SMS",
    "label": "接收短信",
    "description": "允许应用接收短信。",
    "groupId": 4
  },
  {
    "permissionName": "ohos.permission.RECEIVE_WAP_MESSAGES",
    "groupName": "SMS",
    "label": "接收WAP消息",
    "description": "允许应用接收和处理WAP消息。",
    "groupId": 4
  },
  {
    "permissionName": "ohos.permission.SEND_MESSAGES",
    "groupName": "SMS",
    "label": "发送短彩信",
    "description": "允许应用发送短彩信",
    "groupId": 4
  },
  {
    "permissionName": "ohos.permission.READ_CONTACTS",
    "groupName": "CONTACTS",
    "label": "读取通讯录",
    "description": "允许应用读取设备上存储的联系人信息。",
    "groupId": 5
  },
  {
    "permissionName": "ohos.permission.WRITE_CONTACTS",
    "groupName": "CONTACTS",
    "label": "新建/修改/删除通讯录",
    "description": "允许应用新建/修改/删除设备上存储的联系人信息。",
    "groupId": 5
  },
  {
    "permissionName": "ohos.permission.READ_CALL_LOG",
    "groupName": "CALL_LOG",
    "label": "读取通话记录",
    "description": "允许应用读取设备上的通话记录信息。",
    "groupId": 6
  },
  {
    "permissionName": "ohos.permission.WRITE_CALL_LOG",
    "groupName": "CALL_LOG",
    "label": "新建/修改/删除通话记录",
    "description": "允许新建/修改/删除设备上的通话记录信息。",
    "groupId": 6
  },
  {
    "permissionName": "ohos.permission.MEDIA_LOCATION",
    "groupName": "MEDIA",
    "label": "允许应用访问拍摄位置",
    "description": "应用访问用户媒体文件中的拍摄位置信息如经纬度信息。",
    "groupId": 7
  },
  {
    "permissionName": "ohos.permission.READ_MEDIA",
    "groupName": "MEDIA",
    "label": "允许应用读取媒体文件",
    "description": "允许应用访问户媒体文件，如视频、音频、图片等。",
    "groupId": 7
  },
  {
    "permissionName": "ohos.permission.WRITE_MEDIA",
    "groupName": "MEDIA",
    "label": "允许应用读写媒体文件",
    "description": "允许应用读写户媒体文件，如视频、音频、图片等。",
    "groupId": 7
  },
  {
    "permissionName": "ohos.permission.READ_CALENDAR",
    "groupName": "CALENDAR",
    "label": "读取日历",
    "description": "允许应用读取日历。",
    "groupId": 8
  },
  {
    "permissionName": "ohos.permission.WRITE_CALENDAR",
    "groupName": "CALENDAR",
    "label": "修建/修改/删除日历",
    "description": "允许应用修建/修改/删除日历。",
    "groupId": 8
  },
  {
    "permissionName": "ohos.permission.ACTIVITY_MOTION",
    "groupName": "SPORT",
    "label": "读取用户的运动状态",
    "description": "允许应用程序读取用户的运动状态。",
    "groupId": 9
  },
  {
    "permissionName": "ohos.permission.READ_HEALTH_DATA",
    "groupName": "HEALTH",
    "label": "读取用户的健康数据",
    "description": "允许应用程序读取用户的健康数据。",
    "groupId": 10
  },
  {
    "permissionName": "ohos.permission.DISTRIBUTED_DATASYNC",
    "groupName": "OTHER",
    "icon": $r("app.media.ic_multi_device_vector"),
    "label": "使用多设备协同",
    "description": "发现并访问其他设备。",
    "groupId": 11
  }
]

export const groups: any[] = [
  {
    "name": "LOCATION",
    "groupName": "位置信息",
    "icon": $r('app.media.ic_public_gps'),
    "label": "访问位置信息",
    "description": "",
    "permissions": [
      "ohos.permission.LOCATION_IN_BACKGROUND",
      "ohos.permission.LOCATION"
    ]
  },
  {
    "name": "CAMERA",
    "groupName": "相机",
    "icon": $r('app.media.ic_public_camera'),
    "label": "使用相机",
    "description": "",
    "permissions": [
      "ohos.permission.CAMERA"
    ]
  },
  {
    "name": "MICROPHONE",
    "groupName": "麦克风",
    "icon": $r('app.media.ic_public_voice'),
    "label": "使用麦克风",
    "description": "",
    "permissions": [
      "ohos.permission.MICROPHONE"
    ]
  },
  {
    "name": "PHONE",
    "groupName": "电话",
    "icon": $r('app.media.ic_public_phone'),
    "label": "使用拨打电话",
    "description": "包括拨打电话等权限。",
    "permissions": [
      "ohos.permission.ANSWER_CALL",
      "ohos.permission.MANAGE_VOICEMAIL"
    ]
  },
  {
    "name": "SMS",
    "groupName": "信息",
    "icon": $r('app.media.ic_public_message'),
    "label": "访问信息",
    "description": "包括读取短信/彩信、接受短信、发送短信等权限。",
    "permissions": [
      "ohos.permission.READ_CELL_MESSAGES",
      "ohos.permission.READ_MESSAGES",
      "ohos.permission.RECEIVE_MMS",
      "ohos.permission.RECEIVE_SMS",
      "ohos.permission.RECEIVE_WAP_MESSAGES",
      "ohos.permission.SEND_MESSAGES"
    ]
  },
  {
    "name": "CONTACTS",
    "groupName": "通讯录",
    "icon": $r('app.media.ic_public_contacts_group'),
    "label": "访问通讯录",
    "description": "包括新建/修改/删除联系人、读取联系人等权限。",
    "permissions": [
      "ohos.permission.READ_CONTACTS",
      "ohos.permission.WRITE_CONTACTS"
    ]
  },
  {
    "name": "CALL_LOG",
    "groupName": "通话记录",
    "icon": $r('app.media.ic_call_logs'),
    "label": "访问通话记录",
    "description": "包括读取通话记录、新建/修改/删除通话记录等权限。",
    "permissions": [
      "ohos.permission.READ_CALL_LOG",
      "ohos.permission.WRITE_CALL_LOG"
    ]
  },
  {
    "name": "MEDIA",
    "groupName": "媒体和文件",
    "icon": $r('app.media.ic_public_folder'),
    "label": "访问媒体和文件",
    "description": "",
    "permissions": [
      "ohos.permission.MEDIA_LOCATION",
      "ohos.permission.READ_MEDIA",
      "ohos.permission.WRITE_MEDIA"
    ]
  },
  {
    "name": "CALENDAR",
    "groupName": "日历",
    "icon": $r('app.media.ic_public_calendar'),
    "label": "访问日历",
    "description": "包括读取日历和新建/修改/删除日历。",
    "permissions": [
      "ohos.permission.READ_CALENDAR",
      "ohos.permission.WRITE_CALENDAR"
    ]
  },
  {
    "name": "SPORT",
    "groupName": "健身运动",
    "icon": $r('app.media.ic_sport'),
    "label": "访问健身运动",
    "description": "",
    "permissions": [
      "ohos.permission.ACTIVITY_MOTION"
    ]
  },
  {
    "name": "HEALTH",
    "groupName": "身体传感器",
    "icon": $r('app.media.ic_ssensor'),
    "label": "使用身体传感器",
    "description": "",
    "permissions": [
      "ohos.permission.READ_HEALTH_DATA"
    ]
  },
  {
    "name": "OTHER",
    "groupName": "其他权限",
    "icon": $r('app.media.ic_more'),
    "description": "",
    "permissions": [
      "ohos.permission.DISTRIBUTED_DATASYNC"
    ]
  }
]

export const userGrantPermissions: string[] = [
  "ohos.permission.LOCATION_IN_BACKGROUND",
  "ohos.permission.LOCATION",
  "ohos.permission.CAMERA",
  "ohos.permission.MICROPHONE",
  "ohos.permission.ANSWER_CALL",
  "ohos.permission.MANAGE_VOICEMAIL",
  "ohos.permission.READ_CELL_MESSAGES",
  "ohos.permission.READ_MESSAGES",
  "ohos.permission.RECEIVE_MMS",
  "ohos.permission.RECEIVE_SMS",
  "ohos.permission.RECEIVE_WAP_MESSAGES",
  "ohos.permission.SEND_MESSAGES",
  "ohos.permission.READ_CONTACTS",
  "ohos.permission.WRITE_CONTACTS",
  "ohos.permission.READ_CALL_LOG",
  "ohos.permission.WRITE_CALL_LOG",
  "ohos.permission.MEDIA_LOCATION",
  "ohos.permission.READ_MEDIA",
  "ohos.permission.WRITE_MEDIA",
  "ohos.permission.READ_CALENDAR",
  "ohos.permission.WRITE_CALENDAR",
  "ohos.permission.ACTIVITY_MOTION",
  "ohos.permission.READ_HEALTH_DATA",
  "ohos.permission.DISTRIBUTED_DATASYNC"
]