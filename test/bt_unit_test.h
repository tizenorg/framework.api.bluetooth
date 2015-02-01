/*
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *		http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */

#ifndef __TIZEN_NETWORK_BLUETOOTH_UNIT_TEST_H__
#define __TIZEN_NETWORK_BLUETOOTH_UNIT_TEST_H__


#ifdef __cplusplus
extern "C" {
#endif

#define BUFFER_LEN 19
#define MAX_SERVICES 10
#define PRT(format, args...) printf("%s:%d() "format, __FUNCTION__, __LINE__, ##args)
#define TC_PRT(format, args...) PRT(format"\n", ##args)

typedef enum
{
	BT_UNIT_TEST_TABLE_MAIN = 1,
	BT_UNIT_TEST_TABLE_SET_ADDRESS,
	BT_UNIT_TEST_TABLE_ADAPTER,
	BT_UNIT_TEST_TABLE_ADAPTER_LE,
	BT_UNIT_TEST_TABLE_DEVICE,
	BT_UNIT_TEST_TABLE_SOCKET,
	BT_UNIT_TEST_TABLE_OPP,
	BT_UNIT_TEST_TABLE_AUDIO,
	BT_UNIT_TEST_TABLE_PAN,
	BT_UNIT_TEST_TABLE_GATT,
	BT_UNIT_TEST_TABLE_AVRCP,
	BT_UNIT_TEST_TABLE_HID,
	BT_UNIT_TEST_TABLE_AUTOMATED_TEST,
	BT_UNIT_TEST_TABLE_END,
} bt_unit_test_table_e;

typedef enum
{
	BT_UNIT_TEST_FUNCTION_BACK = 0,
	BT_UNIT_TEST_FUNCTION_SET_ADDRESS = 0,
	BT_UNIT_TEST_FUNCTION_INITIALIZE = 1,
	BT_UNIT_TEST_FUNCTION_DEINITIALIZE,
	BT_UNIT_TEST_FUNCTION_ADAPTER_GET_STATE = 1,
	BT_UNIT_TEST_FUNCTION_ADAPTER_ENABLE,
	BT_UNIT_TEST_FUNCTION_ADAPTER_DISABLE,
	BT_UNIT_TEST_FUNCTION_ADAPTER_RECOVER,
	BT_UNIT_TEST_FUNCTION_ADAPTER_START_DEVICE_DISCOVERY,
	BT_UNIT_TEST_FUNCTION_ADAPTER_STOP_DEVICE_DISCOVERY,
	BT_UNIT_TEST_FUNCTION_ADAPTER_IS_DISCOVERING,
	BT_UNIT_TEST_FUNCTION_ADAPTER_FOREACH_BONDED_DEVICE,
	BT_UNIT_TEST_FUNCTION_ADAPTER_GET_BONDED_DEVICE_INFO,
	BT_UNIT_TEST_FUNCTION_ADAPTER_IS_SERVICE_USED,
	BT_UNIT_TEST_FUNCTION_ADAPTER_GET_VISIBILITY,
	BT_UNIT_TEST_FUNCTION_ADAPTER_SET_DEVICE_DISCOVERY_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_DEVICE_DISCOVERY_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_ADAPTER_GET_LOCAL_OOB_DATA,
	BT_UNIT_TEST_FUNCTION_ADAPTER_SET_LOCAL_OOB_DATA,
	BT_UNIT_TEST_FUNCTION_ADAPTER_REMOVE_REMOTE_OOB_DATA,
	BT_UNIT_TEST_FUNCTION_ADAPTER_SET_VISIBILITY_MODE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_VISIBILITY_MODE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_ADAPTER_SET_VISIBILITY_DURATION_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_VISIBILITY_DURATION_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_ADAPTER_SET_CONNECTABLE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_CONNECTABLE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_ADAPTER_GET_CONNECTABLE,
	BT_UNIT_TEST_FUNCTION_ADAPTER_SET_CONNECTABLE,
	BT_UNIT_TEST_FUNCTION_ADAPTER_GET_VERSION,
	BT_UNIT_TEST_FUNCTION_ADAPTER_GET_LOCAL_INFO,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_GET_STATE = 1,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ENABLE,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_DISABLE,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_DEVICE_DISCOVERY,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_DEVICE_DISCOVERY,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_ADVERTISING,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_ADVERTISING,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ADD_WHITE_LIST,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_REMOVE_WHITE_LIST,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_CLEAR_WHITE_LIST,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ENABLE_PRIVACY,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_DEVICE_DISCOVERY_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_ADAPTER_LE_UNSET_DEVICE_DISCOVERY_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_DEVICE_SET_AUTHORIZATION_TRUE = 1,
	BT_UNIT_TEST_FUNCTION_DEVICE_SET_AUTHORIZATION_FALSE,
	BT_UNIT_TEST_FUNCTION_DEVICE_SET_AUTHORIZATION_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_DEVICE_UNSET_AUTHORIZATION_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_DEVICE_SET_CONNECTION_STAET_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_DEVICE_UNSET_CONNECTION_STAET_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_DEVICE_FOREACH_CONNECTED_PROFILES,
	BT_UNIT_TEST_FUNCTION_DEVICE_IS_PROFILE_CONNECTED,
	BT_UNIT_TEST_FUNCTION_DEVICE_SET_BOND_CREATED_CB,
	BT_UNIT_TEST_FUNCTION_DEVICE_CREATE_BOND,
	BT_UNIT_TEST_FUNCTION_DEVICE_CREATE_BOND_BY_TYPE,
	BT_UNIT_TEST_FUNCTION_SOCKET_CREATE_RFCOMM = 1,
	BT_UNIT_TEST_FUNCTION_SOCKET_DESTROY_RFCOMM,
	BT_UNIT_TEST_FUNCTION_SOCKET_LISTEN_AND_ACCEPT_RFCOMM,
	BT_UNIT_TEST_FUNCTION_SOCKET_LISTEN,
	BT_UNIT_TEST_FUNCTION_SOCKET_ACCEPT,
	BT_UNIT_TEST_FUNCTION_SOCKET_REJECT,
	BT_UNIT_TEST_FUNCTION_SOCKET_CONNECT_RFCOMM,
	BT_UNIT_TEST_FUNCTION_SOCKET_DISCONNECT_RFCOMM,
	BT_UNIT_TEST_FUNCTION_SOCKET_SEND_DATA,
	BT_UNIT_TEST_FUNCTION_SOCKET_SET_DATA_RECEIVED_CB,
	BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_DATA_RECEIVED_CB,
	BT_UNIT_TEST_FUNCTION_SOCKET_SET_CONNECTION_REQUESTED_CB,
	BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_CONNECTION_REQUESTED_CB,
	BT_UNIT_TEST_FUNCTION_SOCKET_SET_CONNECTION_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_CONNECTION_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_OPP_CLIENT_INITIALIZE = 1,
	BT_UNIT_TEST_FUNCTION_OPP_CLIENT_DEINITIALIZE,
	BT_UNIT_TEST_FUNCTION_OPP_CLIENT_ADD_FILE,
	BT_UNIT_TEST_FUNCTION_OPP_CLIENT_CLEAR_FILES,
	BT_UNIT_TEST_FUNCTION_OPP_CLIENT_PUSH_FILES,
	BT_UNIT_TEST_FUNCTION_OPP_CLIENT_CANCEL_PUSH,
	BT_UNIT_TEST_FUNCTION_AUDIO_INITIALIZE = 1,
	BT_UNIT_TEST_FUNCTION_AUDIO_DEINITIALIZE,
	BT_UNIT_TEST_FUNCTION_AUDIO_CONNECT,
	BT_UNIT_TEST_FUNCTION_AUDIO_DISCONNECT,
	BT_UNIT_TEST_FUNCTION_AUDIO_SET_CONNECTION_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AUDIO_UNSET_CONNECTION_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AG_OPEN_SCO,
	BT_UNIT_TEST_FUNCTION_AG_CLOSE_SCO,
	BT_UNIT_TEST_FUNCTION_AG_IS_SCO_OPENED,
	BT_UNIT_TEST_FUNCTION_AG_SET_SCO_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AG_UNSET_SCO_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_IDLE,
	BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_ANSWERED,
	BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_DIALING,
	BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_ALERTING,
	BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_INCOMING,
	BT_UNIT_TEST_FUNCTION_CALL_LIST_CREATE,
	BT_UNIT_TEST_FUNCTION_CALL_LIST_DESTROY,
	BT_UNIT_TEST_FUNCTION_CALL_LIST_RESET,
	BT_UNIT_TEST_FUNCTION_CALL_LIST_ADD,
	BT_UNIT_TEST_FUNCTION_AG_NOTIFY_CALL_LIST,
	BT_UNIT_TEST_FUNCTION_AG_SET_CALL_HANDLING_EVENT_CB,
	BT_UNIT_TEST_FUNCTION_AG_IS_NREC_ENABLED,
	BT_UNIT_TEST_FUNCTION_NAP_ACTIVATE = 1,
	BT_UNIT_TEST_FUNCTION_NAP_DEACTIVATE,
	BT_UNIT_TEST_FUNCTION_NAP_DISCONNECT_ALL,
	BT_UNIT_TEST_FUNCTION_NAP_DISCONNECT,
	BT_UNIT_TEST_FUNCTION_PANU_SET_CONNECTION_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_PANU_CONNECT,
	BT_UNIT_TEST_FUNCTION_GATT_FOREACH_PRIMARY_SERVICES = 1,
	BT_UNIT_TEST_FUNCTION_GATT_DISCOVER_CHARACTERISTICS,
	BT_UNIT_TEST_FUNCTION_GATT_GET_SERVICE_UUID,
	BT_UNIT_TEST_FUNCTION_GATT_FOREACH_INCLUDED_SERVICES,
	BT_UNIT_TEST_FUNCTION_GATT_SET_CHARACTERISTIC_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_GATT_UNSET_CHARACTERISTIC_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_GATT_GET_CHARACTERISTIC_DECLARATION,
	BT_UNIT_TEST_FUNCTION_GATT_SET_CHARACTERISTIC_VALUE,
	BT_UNIT_TEST_FUNCTION_GATT_READ_CHARACTERISTIC_VALUE,
	BT_UNIT_TEST_FUNCTION_GATT_WATCH_CHARACTERISTIC_CHANGES,
	BT_UNIT_TEST_FUNCTION_GATT_UNWATCH_CHARACTERISTIC_CHANGES,
	BT_UNIT_TEST_FUNCTION_GATT_CONNECT,
	BT_UNIT_TEST_FUNCTION_GATT_DISCONNECT,
	BT_UNIT_TEST_FUNCTION_SET_GATT_CONNECTION_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_UNSET_GATT_CONNECTION_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AVRCP_TARGET_INITIALIZE = 1,
	BT_UNIT_TEST_FUNCTION_AVRCP_TARGET_DEINITIALIZE,
	BT_UNIT_TEST_FUNCTION_AVRCP_SET_EQUALIZER_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AVRCP_SET_REPEAT_MODE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AVRCP_SET_SHUFFLE_MODE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AVRCP_SET_SCAN_MODE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_EQUALIZER_STATE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_REPEAT_MODE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_SHUFFLE_MODE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_SCAN_MODE_CHANGED_CB,
	BT_UNIT_TEST_FUNCTION_HID_HOST_INITIALIZE = 1,
	BT_UNIT_TEST_FUNCTION_HID_HOST_DEINITIALIZE,
	BT_UNIT_TEST_FUNCTION_HID_HOST_CONNECT,
	BT_UNIT_TEST_FUNCTION_HID_HOST_DISCONNECT,
	BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST = 1,
	BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST_1_SEC_DELAY,
	BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST_N_SEC_DELAY
} bt_unit_test_function_e;

typedef struct {
	const char *tc_name;
	int tc_code;
} tc_table_t;

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_NETWORK_BLUETOOTH_UNIT_TEST_H__ */
