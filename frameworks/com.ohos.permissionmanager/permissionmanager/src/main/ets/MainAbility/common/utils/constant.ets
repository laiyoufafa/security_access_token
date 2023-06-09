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

export default class Constants {
  // alphabetIndexer
  static ALPHABETINDEXER_SELECTEDFONT_SIZE = 12;
  static ALPHABETINDEXER_POPUPFONT_SIZE = 30;
  static ALPHABETINDEXER_ITEMSIZE = 16;
  static ALPHABETINDEXER_HEIGHT = 448;
  static ALPHABETINDEXER_WIDTH = 36;

  // backBar
  static BACKBAR_HEIGHT = 56;
  static BACKBAR_MINHEIGHT = 56;

  // image of backBar
  static BACKBAR_IMAGE_HEIGHT = 24;
  static BACKBAR_IMAGE_WIDTH = 24;
  static BACKBAR_IMAGE_MARGIN_TOP = 17;
  static BACKBAR_IMAGE_MARGIN_LEFT = 12;
  static BACKBAR_IMAGE_MARGIN_RIGHT = 15;

  // text of backBar
  static BACKBAR_TEXT_FONT_SIZE = 20;
  static BACKBAR_TEXT_FLEX_GROW = 1;
  static BACKBAR_TEXT_MARGIN_TOP = 17;

  // dialog
  static DIALOG_BORDER_RADIUS = 24;
  static DIALOG_HEIGHT = 70;
  static DIALOG_WIDTH = 336;

  // text of dialog
  static DIALOG_TEXT_FONT_SIZE = 20;
  static DIALOG_TEXT_MARGIN_TOP = 25;

  // search textInput
  static TEXTINPUT_PADDING_LEFT = 36;
  static TEXTINPUT_BORDER_RADIUS = 20;
  static TEXTINPUT_PLACEHOLDER_Font_SIZE = 16;
  static TEXTINPUT_HEIGHT = 40;
  static TEXTINPUT_IMAGE_WIDTH = 16;
  static TEXTINPUT_IMAGE_HEIGHT = 16;
  static TEXTINPUT_IMAGE_MARGIN_LEFT = -300;
  static TEXTINPUT_IMAGE_MARGIN_TOP = 12;

  // grid useSizeType
  static GRID_MARGIN = '0vp';
  static GUTTER = 0;
  static LEFT_XS_SPAN = 0;
  static LEFT_XS_OFFSET = 0;
  static LEFT_SM_SPAN = 0;
  static LEFT_SM_OFFSET = 0;
  static LEFT_MD_SPAN = 0;
  static LEFT_MD_OFFSET = 0;
  static LEFT_LG_SPAN = 2;
  static LEFT_LG_OFFSET = 0;
  static MIDDLE_XS_SPAN = 2;
  static MIDDLE_XS_OFFSET = 0;
  static MIDDLE_SM_SPAN = 4;
  static MIDDLE_SM_OFFSET = 0;
  static MIDDLE_MD_SPAN = 8;
  static MIDDLE_MD_OFFSET = 0;
  static MIDDLE_LG_SPAN = 8;
  static MIDDLE_LG_OFFSET = 2;
  static RIGHT_XS_SPAN = 0;
  static RIGHT_XS_OFFSET = 2;
  static RIGHT_SM_SPAN = 0;
  static RIGHT_SM_OFFSET = 4;
  static RIGHT_MD_SPAN = 0;
  static RIGHT_MD_OFFSET = 8;
  static RIGHT_LG_SPAN = 2;
  static RIGHT_LG_OFFSET = 10;

  // 100% width,height
  static FULL_WIDTH = '100%';
  static FULL_HEIGHT = '100%';

  // public property style
  static LAYOUT_WEIGHT = 1;
  static FLEX_GROW = 1;
  static TEXT_DECORATION_HEIGHT = 2;
  static TEXT_MIDDLE_FONT_SIZE = 16;
  static TEXT_SMAL_FONT_SIZE = 14;
  static TEXT_SMALLER_FONT_SIZE = 12;
  static CONSTRAINTSIZE_MINHEIGHT = 48;
  static LISTITEM_ROW_HEIGHT = 48;
  static LISTITEM_PADDING_LEFT = 24;
  static LIST_PADDING_LEFT = 12
  static LISTITEM_PADDING_RIGHT = 12;
  static IMAGE_HEIGHT = 24;
  static IMAGE_WIDTH = 12;
  static BORDER_RADIUS = 24;

  // application-secondary, authority-secondary
  static FLEX_MARGIN_TOP = 8;
  static FLEX_MARGIN_BOTTOM = 8;
  static LIST_PADDING_TOP = 4;
  static LIST_PADDING_BOTTOM = 4;
  static ROW_MARGIN_TOP = 9;
  static SECONDARY_TEXT_MARGIN_TOP = 19.5;
  static SECONDARY_TEXT_MARGIN_LEFT = 48;
  static SECONDARY_LIST_PADDING_LEFT = 12;
  static SECONDARY_LIST_PADDING_RIGHT = 12;

  // application-tertiary
  static TERTIARY_IMAGE_WIDTH = 64;
  static TERTIARY_IMAGE_HEIGHT = 64;
  static TERTIARY_IMAGE_MARGIN_LEFT = 12;
  static TERTIARY_IMAGE_MARGIN_RIGHT = 12;
  static TERTIARY_HALF_WIDTH = '50%';
  static TERTIARY_ROW_MARGIN_TOP = 24;
  static TERTIARY_PERMISSION_ROW_MARGIN_TOP = 12;
  static TERTIARY_RADIO_IMAGE_WIDTH = 24;
  static TERTIARY_RADIO_IMAGE_HEIGHT = 24;
  static TERTIARY_RADIO_IMAGE_MARGIN_TOP = 4;
  static TERTIARY_LISTITEM_PADDING_LEFT = 24;
  static TERTIARY_LISTITEM_PADDING_RIGHT = 24;
  static TERTIARY_LISTITEM_MARGIN_TOP = 1;
  static TERTIARY_LIST_PADDING_LEFT = 12;
  static TERTIARY_LIST_PADDING_RIGHT = 12;
  static TERTIARY_LIST_PADDING_TOP = 3;
  static TERTIARY_LIST_PADDING_BOTTOM = 4;

  // authority-tertiary
  static AUTHORITY_IMAGE_WIDTH = 40;
  static AUTHORITY_IMAGE_HEIGHT = 40;
  static AUTHORITY_IMAGE_MARGIN_RIGHT = 16;
  static AUTHORITY_TOGGLE_WIDTH = 36;
  static AUTHORITY_TOGGLE_HEIGHT = 20;
  static AUTHORITY_ROW_HEIGHT = 72;
  static AUTHORITY_CONSTRAINTSIZE_MINHEIGHT = 72;
  static AUTHORITY_LISTITEM_PADDING_LEFT = 12;
  static AUTHORITY_LISTITEM_PADDING_RIGHT = 12;
  static AUTHORITY_TEXTINPUT_PADDING_LEFT = 12;
  static AUTHORITY_TEXTINPUT_PADDING_RIGHT = 4;
  static AUTHORITY_TEXT_MARGIN_TOP = 24;
  static AUTHORITY_TEXT_MARGIN_LEFT = 0;
  static AUTHORITY_ROW_MARGIN_TOP = 24;
  static SEARCHNORESULT_IMAGE_WIDTH = 200;
  static SEARCHNORESULT_IMAGE_HEIGHT = 200;
  static AUTHORITY_COLUMN_HEIGHT = '95%';
  static AUTHORITY_ALPHABETINDEX_WIDTH = 24;
  static AUTHORITY_ALPHABETINDEX_PADDING_TOP = 16;
  static AUTHORITY_TEXT_DECORATION_WIDTH = '86%';
  static AUTHORITY_TEXT_DECORATION_MARGIN_LEFT = 53;

  // authority-management
  static MANAGEMENT_IMAGE_WIDTH = 24;
  static MANAGEMENT_IMAGE_HEIGHT = 24;
  static MANAGEMENT_IMAGE_MARGIN_RIGHT = 16;
  static MANAGEMENT_ROW_HEIGHT = 56;
  static MANAGEMENT_TEXT_DECORATION_WIDTH = '90%';
  static MANAGEMENT_TEXT_DECORATION_MARGIN_LEFT = 40;
  static MANAGEMENT_LODING_IMAGE_WIDTH = 48;
  static MANAGEMENT_LODING_IMAGE_HEIGHT = 48;
  static MANAGEMENT_LODING_ROW_HEIGHT = '90%';
  static MANAGEMENT_ROW_PADDING_LEFT = 12;
  static MANAGEMENT_ROW_PADDING_RIGHT = 12;
  static MANAGEMENT_ROW_PADDING_TOP = 8;
  static MANAGEMENT_ROW_MARGIN_TOP = 12;
  static MANAGEMENT_TRANSPARENCY = 0.8;
  static APPLICATION_IMAGE_WIDTH = 40;
  static APPLICATION_IMAGE_HEIGHT = 40;
  static APPLICATION_IMAGE_MARGIN_RIGHT = 16;
  static APPLICATION_TEXT_MARGIN_RIGHT = 4;
  static APPLICATION_LIST_PADDING_LEFT = 12;
  static APPLICATION_LISTITEM_PADDING_LEFT = 12;
  static APPLICATION_LISTITEM_PADDING_RIGHT = 12;
  static APPLICATION_TEXTINPUT_PADDING_LEFT = 12;
  static APPLICATION_TEXTINPUT_PADDING_TOP = 8;
  static APPLICATION_TEXTINPUT_PADDING_RIGHT = 4;
  static APPLICATION_COLUMN_HEIGHT = '93%';
  static APPLICATION_ALPHABETINDEX_MARGIN_TOP = 12;
  static APPLICATION_ALPHABETINDEX_WIDTH = 24;
  static APPLICATION_TEXT_DECORATION_WIDTH = '83%';
  static APPLICATION_TEXT_DECORATION_MARGIN_LEFT = 68;

  // slice
  static SLICE_START = 0;
  static SLICE_END = -1;
  static SLICE_START_INDEX = 0;
  static SLICE_END_INDEX = 1;

  // tabBar
  static BAR_WIDTH = 250;

  // setTimeout
  static DELAY_TIME = 1000;
  static DELAY_TIME_MAX = 2000;

  // radio
  static RADIO_ALLOW_NAME = '允许';
  static RADIO_ALLOW_INDEX = 0;
  static RADIO_ALLOW_GROUP_NAME = 'radio';
  static RADIO_BAN_NAME = '禁止';
  static RADIO_BAN_INDEX = 1;
  static RADIO_BAN_GROUP_NAME = 'radio';

  // utils
  static CHAR_CODE = 0;
  static UNI_MAX = 40869;
  static UNI_MIN = 19968;

  static PARMETER_BUNDLE_FLAG = 16;
  static USERID = 100;

  static PERMISSION_INDEX = 0;

  static PERMISSION_NUM = 0;

  static PERMISSION_FLAG = 2;
  static API_VERSION_SUPPORT_STAGE = 9;

  static PRE_AUTHORIZATION_NOT_MODIFIED = 4;
}