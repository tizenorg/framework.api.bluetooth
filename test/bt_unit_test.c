/*
 * capi-network-bluetooth
 *
 * Copyright (c) 2000 - 2011 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Contact:  Hocheol Seo <hocheol.seo@samsung.com>
 *		 Girishashok Joshi <girish.joshi@samsung.com>
 *		 Chanyeol Park <chanyeol.park@samsung.com>
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

/**
 * @file       bt_unit_test.c
 * @brief      This is the source file for capi unit test.
 */

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>
#include <dbus/dbus-glib.h>

#include "bluetooth.h"
#include "bluetooth-telephony-api.h"
#include "bt_unit_test.h"

const char *spp_uuid = "00001101-0000-1000-8000-00805F9B34FB";
const char *opp_uuid = "00001105-0000-1000-8000-00805f9b34fb";

static bt_unit_test_table_e current_tc_table;
static char remote_addr[18] = "F6:FB:8F:D8:C8:7C";
static bool input_automated_test_delay = false;

static int server_fd;
static int client_fd;

static gboolean is_sub_index = FALSE;
int service_index;
int char_service_index;
static int selected_service_index = -1;
static int selected_char_index = 0;

bt_gatt_attribute_h service_clone[MAX_SERVICES];
bt_gatt_attribute_h characteristics[MAX_SERVICES];
bt_gatt_attribute_h characteristics_services[MAX_SERVICES];

static unsigned char *hash = NULL;
static unsigned char *randomizer = NULL;
int hash_len;
int rand_len;

bt_call_list_h call_list;
GMainLoop *main_loop = NULL;

tc_table_t tc_main[] = {
	/* Categories */
	{"SET ADDRESS"				, BT_UNIT_TEST_FUNCTION_SET_ADDRESS},
	{"bt_initialize"			, BT_UNIT_TEST_FUNCTION_INITIALIZE},
	{"bt_deinitialize"			, BT_UNIT_TEST_FUNCTION_DEINITIALIZE},
	{"Adapter"				, BT_UNIT_TEST_TABLE_ADAPTER},
	{"Adapter LE"				, BT_UNIT_TEST_TABLE_ADAPTER_LE},
	{"Device"				, BT_UNIT_TEST_TABLE_DEVICE},
	{"Socket"				, BT_UNIT_TEST_TABLE_SOCKET},
	{"Opp"					, BT_UNIT_TEST_TABLE_OPP},
	{"Audio(ag, a2dp, call)"		, BT_UNIT_TEST_TABLE_AUDIO},
	{"Pan(nap, panu)"			, BT_UNIT_TEST_TABLE_PAN},
	{"Gatt"					, BT_UNIT_TEST_TABLE_GATT},
	{"Avrcp"				, BT_UNIT_TEST_TABLE_AVRCP},
	{"Hid"					, BT_UNIT_TEST_TABLE_HID},
	{"Automated test"			, BT_UNIT_TEST_TABLE_AUTOMATED_TEST},
	{"FINISH"				, 0x00ff},
	{NULL					, 0x0000},
};

tc_table_t tc_adapter[] = {
	/* Adapter functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_adapter_get_state"						, BT_UNIT_TEST_FUNCTION_ADAPTER_GET_STATE},
	{"bt_adapter_enable"						, BT_UNIT_TEST_FUNCTION_ADAPTER_ENABLE},
	{"bt_adapter_disable"						, BT_UNIT_TEST_FUNCTION_ADAPTER_DISABLE},
	{"bt_adapter_recover"						, BT_UNIT_TEST_FUNCTION_ADAPTER_RECOVER},
	{"bt_adapter_start_device_discovery"				, BT_UNIT_TEST_FUNCTION_ADAPTER_START_DEVICE_DISCOVERY},
	{"bt_adapter_stop_device_discovery"				, BT_UNIT_TEST_FUNCTION_ADAPTER_STOP_DEVICE_DISCOVERY},
	{"bt_adapter_is_discovering"					, BT_UNIT_TEST_FUNCTION_ADAPTER_IS_DISCOVERING},
	{"bt_adapter_foreach_bonded_device"				, BT_UNIT_TEST_FUNCTION_ADAPTER_FOREACH_BONDED_DEVICE},
	{"bt_adapter_get_bonded_device_info"				, BT_UNIT_TEST_FUNCTION_ADAPTER_GET_BONDED_DEVICE_INFO},
	{"bt_adapter_is_service_used"					, BT_UNIT_TEST_FUNCTION_ADAPTER_IS_SERVICE_USED},
	{"bt_adapter_get_visibility"					, BT_UNIT_TEST_FUNCTION_ADAPTER_GET_VISIBILITY},
	{"bt_adapter_set_device_discovery_state_changed_cb"		, BT_UNIT_TEST_FUNCTION_ADAPTER_SET_DEVICE_DISCOVERY_STATE_CHANGED_CB},
	{"bt_adapter_unset_device_discovery_state_changed_cb"		, BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_DEVICE_DISCOVERY_STATE_CHANGED_CB},
	{"bt_adapter_get_local_oob_data"				, BT_UNIT_TEST_FUNCTION_ADAPTER_GET_LOCAL_OOB_DATA},
	{"bt_adapter_set_remote_oob_data" 				, BT_UNIT_TEST_FUNCTION_ADAPTER_SET_LOCAL_OOB_DATA},
	{"bt_adapter_remove_remote_oob_data"				, BT_UNIT_TEST_FUNCTION_ADAPTER_REMOVE_REMOTE_OOB_DATA},
	{"bt_adapter_set_visibility_mode_changed_cb"			, BT_UNIT_TEST_FUNCTION_ADAPTER_SET_VISIBILITY_MODE_CHANGED_CB},
	{"bt_adapter_unset_visibility_mode_changed_cb"			, BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_VISIBILITY_MODE_CHANGED_CB},
	{"bt_adapter_set_visibility_duration_changed_cb"		, BT_UNIT_TEST_FUNCTION_ADAPTER_SET_VISIBILITY_DURATION_CHANGED_CB},
	{"bt_adapter_unset_visibility_duration_changed_cb"		, BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_VISIBILITY_DURATION_CHANGED_CB},
	{"bt_adapter_set_connectable_changed_cb"			, BT_UNIT_TEST_FUNCTION_ADAPTER_SET_CONNECTABLE_CHANGED_CB},
	{"bt_adapter_unset_connectable_changed_cb"			, BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_CONNECTABLE_CHANGED_CB},
	{"bt_adapter_get_connectable"					, BT_UNIT_TEST_FUNCTION_ADAPTER_GET_CONNECTABLE},
	{"bt_adapter_set_connectable"					, BT_UNIT_TEST_FUNCTION_ADAPTER_SET_CONNECTABLE},
	{"bt_adapter_get_version"					, BT_UNIT_TEST_FUNCTION_ADAPTER_GET_VERSION},
	{"bt_adapter_get_local_info"					, BT_UNIT_TEST_FUNCTION_ADAPTER_GET_LOCAL_INFO},
	{NULL					, 0x0000},
};

tc_table_t tc_adapter_le[] = {
	/* Adapter functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_adapter_le_get_state"					, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_GET_STATE},
	{"bt_adapter_le_enable" 					, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ENABLE},
	{"bt_adapter_le_disable"					, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_DISABLE},
	{"bt_adapter_le_start_device_discovery"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_DEVICE_DISCOVERY},
	{"bt_adapter_le_stop_device_discovery"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_DEVICE_DISCOVERY},
	{"bt_adapter_le_start_advertising"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_ADVERTISING},
	{"bt_adapter_le_stop_advertising"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_ADVERTISING},
	{"bt_adapter_le_add_white_list"					, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ADD_WHITE_LIST},
	{"bt_adapter_le_remove_white_list"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_REMOVE_WHITE_LIST},
	{"bt_adapter_le_clear_white_list"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_CLEAR_WHITE_LIST},
	{"bt_adapter_le_enable_privacy"					, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ENABLE_PRIVACY},
	{"bt_adapter_le_set_device_discovery_state_changed_cb"		, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_DEVICE_DISCOVERY_STATE_CHANGED_CB},
	{"bt_adapter_le_unset_device_discovery_state_changed_cb"	, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_UNSET_DEVICE_DISCOVERY_STATE_CHANGED_CB},
	{NULL					, 0x0000},
};

tc_table_t tc_device[] = {
	/* Device functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_device_set_authorization (true)"				, BT_UNIT_TEST_FUNCTION_DEVICE_SET_AUTHORIZATION_TRUE},
	{"bt_device_set_authorization (false)"				, BT_UNIT_TEST_FUNCTION_DEVICE_SET_AUTHORIZATION_FALSE},
	{"bt_device_set_authorization_changed_cb"			, BT_UNIT_TEST_FUNCTION_DEVICE_SET_AUTHORIZATION_CHANGED_CB},
	{"bt_device_unset_authorization_changed_cb"			, BT_UNIT_TEST_FUNCTION_DEVICE_UNSET_AUTHORIZATION_CHANGED_CB},
	{"bt_device_set_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_DEVICE_SET_CONNECTION_STAET_CHANGED_CB},
	{"bt_device_unset_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_DEVICE_UNSET_CONNECTION_STAET_CHANGED_CB},
	{"bt_device_foreach_connected_profiles"				, BT_UNIT_TEST_FUNCTION_DEVICE_FOREACH_CONNECTED_PROFILES},
	{"bt_device_is_profile_connected"				, BT_UNIT_TEST_FUNCTION_DEVICE_IS_PROFILE_CONNECTED},
	{"bt_device_set_bond_created_cb"				, BT_UNIT_TEST_FUNCTION_DEVICE_SET_BOND_CREATED_CB},
	{"bt_device_create_bond" 					, BT_UNIT_TEST_FUNCTION_DEVICE_CREATE_BOND},
	{"bt_device_create_bond_by_type"				, BT_UNIT_TEST_FUNCTION_DEVICE_CREATE_BOND_BY_TYPE},
	{NULL					, 0x0000},
};

tc_table_t tc_socket[] = {
	/* Socket functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_socket_create_rfcomm"					, BT_UNIT_TEST_FUNCTION_SOCKET_CREATE_RFCOMM},
	{"bt_socket_destroy_rfcomm"					, BT_UNIT_TEST_FUNCTION_SOCKET_DESTROY_RFCOMM},
	{"bt_socket_listen_and_accept_rfcomm"				, BT_UNIT_TEST_FUNCTION_SOCKET_LISTEN_AND_ACCEPT_RFCOMM},
	{"bt_socket_listen"						, BT_UNIT_TEST_FUNCTION_SOCKET_LISTEN},
	{"bt_socket_accept"						, BT_UNIT_TEST_FUNCTION_SOCKET_ACCEPT},
	{"bt_socket_reject"						, BT_UNIT_TEST_FUNCTION_SOCKET_REJECT},
	{"bt_socket_connect_rfcomm"					, BT_UNIT_TEST_FUNCTION_SOCKET_CONNECT_RFCOMM},
	{"bt_socket_disconnect_rfcomm"					, BT_UNIT_TEST_FUNCTION_SOCKET_DISCONNECT_RFCOMM},
	{"bt_socket_send_data"						, BT_UNIT_TEST_FUNCTION_SOCKET_SEND_DATA},
	{"bt_socket_set_data_received_cb"				, BT_UNIT_TEST_FUNCTION_SOCKET_SET_DATA_RECEIVED_CB},
	{"bt_socket_unset_data_received_cb"				, BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_DATA_RECEIVED_CB},
	{"bt_socket_set_connection_requested_cb"			, BT_UNIT_TEST_FUNCTION_SOCKET_SET_CONNECTION_REQUESTED_CB},
	{"bt_socket_unset_connection_requested_cb"			, BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_CONNECTION_REQUESTED_CB},
	{"bt_socket_set_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_SOCKET_SET_CONNECTION_STATE_CHANGED_CB},
	{"bt_socket_unset_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_CONNECTION_STATE_CHANGED_CB},
	{NULL					, 0x0000},
};

tc_table_t tc_opp[] = {
	/* OPP functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_opp_client_initialize"					, BT_UNIT_TEST_FUNCTION_OPP_CLIENT_INITIALIZE},
	{"bt_opp_client_deinitialize"					, BT_UNIT_TEST_FUNCTION_OPP_CLIENT_DEINITIALIZE},
	{"bt_opp_client_add_file"					, BT_UNIT_TEST_FUNCTION_OPP_CLIENT_ADD_FILE},
	{"bt_opp_client_clear_files"					, BT_UNIT_TEST_FUNCTION_OPP_CLIENT_CLEAR_FILES},
	{"bt_opp_client_push_files"					, BT_UNIT_TEST_FUNCTION_OPP_CLIENT_PUSH_FILES},
	{"bt_opp_client_cancel_push"					, BT_UNIT_TEST_FUNCTION_OPP_CLIENT_CANCEL_PUSH},
	{NULL					, 0x0000},
};

tc_table_t tc_audio[] = {
	/* AG functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_audio_initialize"						, BT_UNIT_TEST_FUNCTION_AUDIO_INITIALIZE},
	{"bt_audio_deinitialize"					, BT_UNIT_TEST_FUNCTION_AUDIO_DEINITIALIZE},
	{"bt_audio_connect"						, BT_UNIT_TEST_FUNCTION_AUDIO_CONNECT},
	{"bt_audio_disconnect"						, BT_UNIT_TEST_FUNCTION_AUDIO_DISCONNECT},
	{"bt_audio_set_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_AUDIO_SET_CONNECTION_STATE_CHANGED_CB},
	{"bt_audio_unset_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_AUDIO_UNSET_CONNECTION_STATE_CHANGED_CB},
	{"bt_ag_open_sco" 						, BT_UNIT_TEST_FUNCTION_AG_OPEN_SCO},
	{"bt_ag_close_sco" 						, BT_UNIT_TEST_FUNCTION_AG_CLOSE_SCO},
	{"bt_ag_is_sco_opened"						, BT_UNIT_TEST_FUNCTION_AG_IS_SCO_OPENED},
	{"bt_ag_set_sco_state_changed_cb"				, BT_UNIT_TEST_FUNCTION_AG_SET_SCO_STATE_CHANGED_CB},
	{"bt_ag_unset_sco_state_changed_cb"				, BT_UNIT_TEST_FUNCTION_AG_UNSET_SCO_STATE_CHANGED_CB},
	{"BT_AG_CALL_EVENT_IDLE"					, BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_IDLE},
	{"BT_AG_CALL_EVENT_ANSWERED"					, BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_ANSWERED},
	{"BT_AG_CALL_EVENT_DIALING"					, BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_DIALING},
	{"BT_AG_CALL_EVENT_ALERTING"					, BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_ALERTING},
	{"BT_AG_CALL_EVENT_INCOMING"					, BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_INCOMING},
	{"bt_call_list_create" 						, BT_UNIT_TEST_FUNCTION_CALL_LIST_CREATE},
	{"bt_call_list_destroy" 					, BT_UNIT_TEST_FUNCTION_CALL_LIST_DESTROY},
	{"bt_call_list_reset" 						, BT_UNIT_TEST_FUNCTION_CALL_LIST_RESET},
	{"bt_call_list_add"	 					, BT_UNIT_TEST_FUNCTION_CALL_LIST_ADD},
	{"bt_ag_notify_call_list"					, BT_UNIT_TEST_FUNCTION_AG_NOTIFY_CALL_LIST},
	{"bt_ag_set_call_handling_event_cb"				, BT_UNIT_TEST_FUNCTION_AG_SET_CALL_HANDLING_EVENT_CB},
	{"bt_ag_is_nrec_enabled"					, BT_UNIT_TEST_FUNCTION_AG_IS_NREC_ENABLED},
	{NULL					, 0x0000},
};

tc_table_t tc_pan[] = {
	/* Network functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_nap_activate"						, BT_UNIT_TEST_FUNCTION_NAP_ACTIVATE},
	{"bt_nap_deactivate"						, BT_UNIT_TEST_FUNCTION_NAP_DEACTIVATE},
	{"bt_nap_disconnect_all"					, BT_UNIT_TEST_FUNCTION_NAP_DISCONNECT_ALL},
	{"bt_nap_disconnect"						, BT_UNIT_TEST_FUNCTION_NAP_DISCONNECT},
	{"bt_panu_set_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_PANU_SET_CONNECTION_STATE_CHANGED_CB},
	{"bt_panu_connect"						, BT_UNIT_TEST_FUNCTION_PANU_CONNECT},
	{NULL					, 0x0000},
};

tc_table_t tc_gatt[] = {
	/* Gatt functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_gatt_foreach_primary_services"				, BT_UNIT_TEST_FUNCTION_GATT_FOREACH_PRIMARY_SERVICES},
	{"bt_gatt_discover_characteristics"				, BT_UNIT_TEST_FUNCTION_GATT_DISCOVER_CHARACTERISTICS},
	{"bt_gatt_get_service_uuid"					, BT_UNIT_TEST_FUNCTION_GATT_GET_SERVICE_UUID},
	{"bt_gatt_foreach_included_services"				, BT_UNIT_TEST_FUNCTION_GATT_FOREACH_INCLUDED_SERVICES},
	{"bt_gatt_set_characteristic_changed_cb"			, BT_UNIT_TEST_FUNCTION_GATT_SET_CHARACTERISTIC_CHANGED_CB},
	{"bt_gatt_unset_characteristic_changed_cb"			, BT_UNIT_TEST_FUNCTION_GATT_UNSET_CHARACTERISTIC_CHANGED_CB},
	{"bt_gatt_get_characteristic_declaration"			, BT_UNIT_TEST_FUNCTION_GATT_GET_CHARACTERISTIC_DECLARATION},
	{"bt_gatt_set_characteristic_value"				, BT_UNIT_TEST_FUNCTION_GATT_SET_CHARACTERISTIC_VALUE},
	{"bt_gatt_read_characteristic_value"				, BT_UNIT_TEST_FUNCTION_GATT_READ_CHARACTERISTIC_VALUE},
	{"bt_gatt_watch_characteristic_changes"				, BT_UNIT_TEST_FUNCTION_GATT_WATCH_CHARACTERISTIC_CHANGES},
	{"bt_gatt_unwatch_characteristic_changes"			, BT_UNIT_TEST_FUNCTION_GATT_UNWATCH_CHARACTERISTIC_CHANGES},
	{"bt_gatt_connect"						, BT_UNIT_TEST_FUNCTION_GATT_CONNECT},
	{"bt_gatt_disconnect"						, BT_UNIT_TEST_FUNCTION_GATT_DISCONNECT},
	{"bt_gatt_set_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_SET_GATT_CONNECTION_STATE_CHANGED_CB},
	{"bt_gatt_unset_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_UNSET_GATT_CONNECTION_STATE_CHANGED_CB},
	{NULL					, 0x0000},
};

tc_table_t tc_avrcp[] = {
	/* AVRCP functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_avrcp_target_initialize"					, BT_UNIT_TEST_FUNCTION_AVRCP_TARGET_INITIALIZE},
	{"bt_avrcp_target_deinitialize"					, BT_UNIT_TEST_FUNCTION_AVRCP_TARGET_DEINITIALIZE},
	{"bt_avrcp_set_equalizer_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_AVRCP_SET_EQUALIZER_STATE_CHANGED_CB},
	{"bt_avrcp_set_repeat_mode_changed_cb"				, BT_UNIT_TEST_FUNCTION_AVRCP_SET_REPEAT_MODE_CHANGED_CB},
	{"bt_avrcp_set_shuffle_mode_changed_cb"				, BT_UNIT_TEST_FUNCTION_AVRCP_SET_SHUFFLE_MODE_CHANGED_CB},
	{"bt_avrcp_set_scan_mode_changed_cb"				, BT_UNIT_TEST_FUNCTION_AVRCP_SET_SCAN_MODE_CHANGED_CB},
	{"bt_avrcp_unset_equalizer_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_EQUALIZER_STATE_CHANGED_CB},
	{"bt_avrcp_unset_repeat_mode_changed_cb"			, BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_REPEAT_MODE_CHANGED_CB},
	{"bt_avrcp_unset_shuffle_mode_changed_cb"			, BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_SHUFFLE_MODE_CHANGED_CB},
	{"bt_avrcp_unset_scan_mode_changed_cb"				, BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_SCAN_MODE_CHANGED_CB},
	{NULL					, 0x0000},
};

tc_table_t tc_hid[] = {
	/* HID functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_hid_host_initialize"					, BT_UNIT_TEST_FUNCTION_HID_HOST_INITIALIZE},
	{"bt_hid_host_deinitialize"					, BT_UNIT_TEST_FUNCTION_HID_HOST_DEINITIALIZE},
	{"bt_hid_host_connect"						, BT_UNIT_TEST_FUNCTION_HID_HOST_CONNECT},
	{"bt_hid_host_disconnect"					, BT_UNIT_TEST_FUNCTION_HID_HOST_DISCONNECT},
	{NULL					, 0x0000},
};

tc_table_t tc_automated_test[] = {
	/* Automated test Functions*/
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"On/Off repeat test()"				, BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST},
	{"On/Off repeat(1sec delay) test()" , BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST_1_SEC_DELAY},
	{"On/Off repeat(Nsec delay) test()" , BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST_N_SEC_DELAY},
	{NULL					, 0x0000},
};

void tc_usage_print(void)
{
	int i = 0;
	tc_table_t* tc_table = NULL;

	switch (current_tc_table) {
	case BT_UNIT_TEST_TABLE_SET_ADDRESS:
		TC_PRT("Key 0 : usage BACK");
		TC_PRT("Input the address of remote device.(e.g., F6:FB:8F:D8:C8:7C)\n\n");
		return;
	case BT_UNIT_TEST_TABLE_ADAPTER:
		tc_table = tc_adapter;
		break;
	case BT_UNIT_TEST_TABLE_ADAPTER_LE:
		tc_table = tc_adapter_le;
		break;
	case BT_UNIT_TEST_TABLE_DEVICE:
		tc_table = tc_device;
		break;
	case BT_UNIT_TEST_TABLE_SOCKET:
		tc_table = tc_socket;
		break;
	case BT_UNIT_TEST_TABLE_OPP:
		tc_table = tc_opp;
		break;
	case BT_UNIT_TEST_TABLE_AUDIO:
		tc_table = tc_audio;
		break;
	case BT_UNIT_TEST_TABLE_PAN:
		tc_table = tc_pan;
		break;
	case BT_UNIT_TEST_TABLE_GATT:
		tc_table = tc_gatt;
		break;
	case BT_UNIT_TEST_TABLE_AVRCP:
		tc_table = tc_avrcp;
		break;
	case BT_UNIT_TEST_TABLE_HID:
		tc_table = tc_hid;
		break;
	case BT_UNIT_TEST_TABLE_AUTOMATED_TEST:
		if (input_automated_test_delay == true) {
			TC_PRT("Input the delay for automated test.(e.g., 10)\n\n");
			return;
		}
		tc_table = tc_automated_test;
		break;
	case BT_UNIT_TEST_TABLE_MAIN:
	default:
		tc_table = tc_main;
		break;
	}

	while (tc_table[i].tc_name) {
		TC_PRT("Key %d : usage %s", tc_table[i].tc_code,
						tc_table[i].tc_name);
		i++;
	}
	printf("\n\n");
}

static const char* __bt_get_error_message(bt_error_e err)
{
	const char* err_str = NULL;

	switch (err) {
	case BT_ERROR_NONE:
		err_str = "BT_ERROR_NONE";
		break;
	case BT_ERROR_CANCELLED:
		err_str = "BT_ERROR_CANCELLED";
		break;
	case BT_ERROR_INVALID_PARAMETER:
		err_str = "BT_ERROR_INVALID_PARAMETER";
		break;
	case BT_ERROR_OUT_OF_MEMORY:
		err_str = "BT_ERROR_OUT_OF_MEMORY";
		break;
	case BT_ERROR_RESOURCE_BUSY:
		err_str = "BT_ERROR_RESOURCE_BUSY";
		break;
	case BT_ERROR_TIMED_OUT:
		err_str = "BT_ERROR_TIMED_OUT";
		break;
	case BT_ERROR_NOW_IN_PROGRESS:
		err_str = "BT_ERROR_NOW_IN_PROGRESS";
		break;
	case BT_ERROR_NOT_INITIALIZED:
		err_str = "BT_ERROR_NOT_INITIALIZED";
		break;
	case BT_ERROR_NOT_ENABLED:
		err_str = "BT_ERROR_NOT_ENABLED";
		break;
	case BT_ERROR_ALREADY_DONE:
		err_str = "BT_ERROR_ALREADY_DONE";
		break;
	case BT_ERROR_OPERATION_FAILED:
		err_str = "BT_ERROR_OPERATION_FAILED";
		break;
	case BT_ERROR_NOT_IN_PROGRESS:
		err_str = "BT_ERROR_NOT_IN_PROGRESS";
		break;
	case BT_ERROR_REMOTE_DEVICE_NOT_BONDED:
		err_str = "BT_ERROR_REMOTE_DEVICE_NOT_BONDED";
		break;
	case BT_ERROR_AUTH_REJECTED:
		err_str = "BT_ERROR_AUTH_REJECTED";
		break;
	case BT_ERROR_AUTH_FAILED:
		err_str = "BT_ERROR_AUTH_FAILED";
		break;
	case BT_ERROR_REMOTE_DEVICE_NOT_FOUND:
		err_str = "BT_ERROR_REMOTE_DEVICE_NOT_FOUND";
		break;
	case BT_ERROR_SERVICE_SEARCH_FAILED:
		err_str = "BT_ERROR_SERVICE_SEARCH_FAILED";
		break;
	case BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED:
		err_str = "BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED";
		break;
	case BT_ERROR_PERMISSION_DENIED:
		err_str = "BT_ERROR_PERMISSION_DENIED";
		break;
	case BT_ERROR_SERVICE_NOT_FOUND:
		err_str = "BT_ERROR_SERVICE_NOT_FOUND";
		break;
	case BT_ERROR_NOT_SUPPORTED:
		err_str = "BT_ERROR_NOT_SUPPORTED";
		break;
	default:
		err_str = "NOT Defined";
		break;
	}

	return err_str;
}

static gboolean __select_index_cb(GIOChannel *chan,
				GIOCondition cond,
				gpointer data)
{
	char buf[BUFFER_LEN] = { 0 };
	unsigned int len = 0;
	int *index = data;

	memset(buf, 0, sizeof(buf));

	if (g_io_channel_read_chars(chan, buf, sizeof(buf),
				&len, NULL) == G_IO_STATUS_ERROR) {
		TC_PRT("IO Channel read error");
		is_sub_index = FALSE;
		return FALSE;
	}

	*index = atoi(buf);
	is_sub_index = FALSE;

	return FALSE;
}

static void __select_index(void *data)
{
	GIOChannel *key_io;

	is_sub_index = TRUE;

	TC_PRT("Select Index : ");

	key_io = g_io_channel_unix_new(fileno(stdin));

	g_io_channel_set_encoding(key_io, NULL, NULL);
	g_io_channel_set_flags(key_io, G_IO_FLAG_NONBLOCK, NULL);

	g_io_add_watch(key_io, G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL,
			__select_index_cb, data);

	g_io_channel_unref(key_io);
}

static void __bt_free_gatt_services(void)
{
	int i;

	for (i = 0; i < service_index; i++) {
		bt_gatt_destroy_attribute_handle(service_clone[i]);
		service_clone[i] = NULL;
	}

	service_index = 0;
}

static void __bt_free_gatt_characteristics(void)
{
	int i;

	for (i = 0; i < MAX_SERVICES; i++) {
		if (characteristics[i] == NULL)
			return;

		bt_gatt_destroy_attribute_handle(characteristics[i]);
		characteristics[i] = NULL;
	}
}

static void __bt_free_gatt_characteristics_services(void)
{
	int i;

	for (i = 0; i < char_service_index; i++) {
		bt_gatt_destroy_attribute_handle(characteristics_services[i]);
		characteristics_services[i] = NULL;
	}
	char_service_index = 0;
}

static void __bt_adapter_device_visibility_mode_changed_cb(int result,
				bt_adapter_visibility_mode_e visibility_mode,
				void *user_data)
{
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("visibility_mode: %d", visibility_mode);
}

static void __bt_adapter_device_visibility_duration_changed_cb(int duration,
				void *user_data)
{
	TC_PRT("visibility_duration: %d", duration);
}

static void __bt_adapter_connectable_changed_cb(int result,
				bool connectable, void *user_data)
{
	if (result != BT_ERROR_NONE) {
		TC_PRT("connectable change is failed : [0x%04x]", result);
		return;
	}

	TC_PRT("%s", connectable ? "Connectable" : "Non-connectable");
}

static bool __bt_adapter_bonded_device_cb(bt_device_info_s *device_info, void *user_data)
{
	int i;

	TC_PRT("remote_address: %s", device_info->remote_address);
	TC_PRT("remote_name: %s", device_info->remote_name);
	TC_PRT("service_count: %d", device_info->service_count);

	if (device_info->service_uuid == NULL) {
		TC_PRT("No uuids");
	} else {
		for (i = 0; i < device_info->service_count; i++) {
			TC_PRT("uuid: %s", device_info->service_uuid[i]);
		}
	}

	if (device_info->manufacturer_data_len > 0) {
		TC_PRT("manufacturer specific data(len:%d)", device_info->manufacturer_data_len);
		printf("data [");
		for (i = 0; i < device_info->manufacturer_data_len; i++)
			printf("%02x ", device_info->manufacturer_data[i]);
		printf("]\n");
	}

	return true;
}

static void __bt_adapter_device_discovery_state_changed_cb(int result,
				bt_adapter_device_discovery_state_e discovery_state,
				bt_adapter_device_discovery_info_s *discovery_info,
				void *user_data)
{
	int i;

	TC_PRT("discovery_state: %d", discovery_state);

	if (discovery_info == NULL) {
		TC_PRT("No discovery_info!");
		return;
	}

	TC_PRT("remote_address: %s", discovery_info->remote_address);
	TC_PRT("remote_name: %s", discovery_info->remote_name);
	TC_PRT("rssi: %d", discovery_info->rssi);
	TC_PRT("is_bonded: %d", discovery_info->is_bonded);
	TC_PRT("service_count: %d", discovery_info->service_count);

	if (discovery_info->service_uuid == NULL) {
		TC_PRT("No uuids");
	} else {
		for (i = 0; i < discovery_info->service_count; i++) {
			TC_PRT("uuid: %s", discovery_info->service_uuid[i]);
		}
	}

	if (discovery_info->manufacturer_data_len > 0) {
		TC_PRT("manufacturer specific data(len:%d)", discovery_info->manufacturer_data_len);
		printf("data [");
		for (i = 0; i < discovery_info->manufacturer_data_len; i++)
			printf("%02x ", discovery_info->manufacturer_data[i]);
		printf("]\n");
	}
}

static void __bt_adapter_le_device_discovery_state_changed_cb(int result,
				bt_adapter_le_device_discovery_state_e discovery_state,
				bt_adapter_le_device_discovery_info_s *discovery_info,
				void *user_data)
{
//	int i;

	if (discovery_info == NULL && discovery_state == BT_ADAPTER_LE_DEVICE_DISCOVERY_FOUND) {
		TC_PRT("No discovery_info!");
		return;
	}

	if (discovery_state != BT_ADAPTER_LE_DEVICE_DISCOVERY_FOUND) {
		TC_PRT("LE discovery %s", discovery_state == BT_ADAPTER_LE_DEVICE_DISCOVERY_STARTED?
				"Started" : "Finished");
	} else {
		TC_PRT("%s Adv %d Scan resp %d RSSI %d Addr_type %d", discovery_info->remote_address,
				discovery_info->adv_data_len, discovery_info->scan_data_len, discovery_info->rssi,
				discovery_info->address_type);

		if (discovery_info->adv_data_len > 31 || discovery_info->scan_data_len > 31) {
			TC_PRT("###################");
			bt_adapter_le_stop_device_discovery();
			TC_PRT("###################");
			return;
		}
#if 0
		/* LE ADV information */
		TC_PRT("appearance: %d", discovery_info->appearance);
		if (discovery_info->adv_data_len > 0)
			for (i = 0; i < discovery_info->adv_data_len; i++)
				TC_PRT("data[%d] : %x", i, discovery_info->adv_data[i]);
		if (discovery_info->scan_data_len > 0)
			for (i = 0; i < discovery_info->scan_data_len; i++)
				TC_PRT("data[%d] : %x", i, discovery_info->scan_data[i]);

		if (discovery_info->service_uuid == NULL) {
			TC_PRT("No uuids");
			return;
		}

		for (i = 0; i < discovery_info->service_count; i++) {
			TC_PRT("uuid: %s", discovery_info->service_uuid[i]);
		}
#endif
	}
}

static void __bt_socket_data_received_cb(bt_socket_received_data_s *data, void *user_data)
{
	TC_PRT("+");

	if (data == NULL) {
		TC_PRT("No recieved data!");
		return;
	}

	TC_PRT("Socket fd: %d", data->socket_fd);
	TC_PRT("Data: %s", data->data);
	TC_PRT("Size: %d", data->data_size);
}

static void __bt_socket_connection_requested_cb(int socket_fd, const char *remote_address, void *user_data)
{
	TC_PRT("Socket fd: %d", socket_fd);
	TC_PRT("remote_address: %s", remote_address);
}

static void __bt_socket_connection_state_changed_cb(int result,
				bt_socket_connection_state_e connection_state,
				bt_socket_connection_s *connection,
				void *user_data)
{
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("connection_state: %d", connection_state);

	if (connection == NULL) {
		TC_PRT("No connection data!");
		return;
	}

	TC_PRT("socket fd: %d", connection->socket_fd);
	TC_PRT("server socket fd: %d", connection->server_fd);
	TC_PRT("role: %d", connection->local_role);
	TC_PRT("remote address: %s", connection->remote_address);
	TC_PRT("service_uuid: %s", connection->service_uuid);
	client_fd = connection->socket_fd;
}

void __bt_opp_client_push_responded_cb(int result,
					const char *remote_address,
					void *user_data)
{
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("remote_address: %s", remote_address);
}

void __bt_opp_client_push_progress_cb(const char *file,
					long long size,
					int percent,
					void *user_data)
{
	TC_PRT("size: %ld", (long)size);
	TC_PRT("percent: %d", percent);
	TC_PRT("file: %s", file);
}

void __bt_opp_client_push_finished_cb(int result,
				const char *remote_address,
				void *user_data)
{
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("remote_address: %s", remote_address);
}

void __bt_audio_connection_state_changed_cb(int result,
					bool connected, const char *remote_address,
					bt_audio_profile_type_e type, void *user_data)
{
	TC_PRT("result [%s]", __bt_get_error_message(result));
	TC_PRT("connected [%d]", connected);
	TC_PRT("address [%s]", remote_address);
	TC_PRT("type [%d]", type);
}

void __bt_ag_sco_state_changed_cb(int result, bool opened, void *user_data)
{
	TC_PRT("result [%s]", __bt_get_error_message(result));
	TC_PRT("opened [%s]", opened ? "YES" : "NO");
}

void __bt_ag_set_call_handling_event_cb(bt_ag_call_handling_event_e event,
					unsigned int call_id, void *user_data)
{
	TC_PRT("event[%d] call_id[%d]", event, call_id);
}

void __bt_ag_set_vendor_cmd_event_cb(char *cmd, void *user_data)
{
	TC_PRT("Vendor command[%s]", cmd);
}

void __bt_device_authorization_changed_cb(bt_device_authorization_e authorization,
						char *remote_address, void *user_data)
{
	TC_PRT("__bt_device_authorization_changed_cb [%d]", authorization);
	TC_PRT("address: %s", remote_address);
}

void __bt_device_connection_state_changed_cb(bool connected,
						bt_device_connection_info_s *conn_info,
						void *user_data)
{
	TC_PRT("__bt_device_connection_state_changed_cb [%d]", connected);
	TC_PRT("address [%s]", conn_info->remote_address);
	TC_PRT("link type [%d]", conn_info->link);
	TC_PRT("disconnection reason [%d]", conn_info->disconn_reason);
}

bool __bt_device_connected_profile(bt_profile_e profile, void *user_data)
{
	TC_PRT("__bt_device_connected_profile [%d]", profile);
	return true;
}

void __bt_device_bond_created_cb(int result, bt_device_info_s *device_info, void *user_data)
{
	if(result == BT_ERROR_NONE)
	{
		TC_PRT("Callback: A bond with chat_server is created.");
		TC_PRT("Callback: The number of service - %d.", device_info->service_count);

		int i = 0;
		for(i=0; i<device_info->service_count; i++) {
			TC_PRT("Callback: service[%d] - %s", i+1, device_info->service_uuid[i]);
		}
		TC_PRT("Callback: is_bonded - %d.", device_info->is_bonded);
		TC_PRT("Callback: is_connected - %d.", device_info->is_connected);

		if (device_info->manufacturer_data_len > 0) {
			TC_PRT("manufacturer specific data(len:%d)", device_info->manufacturer_data_len);
			printf("data [");
			for (i = 0; i < device_info->manufacturer_data_len; i++)
				printf("%02x ", device_info->manufacturer_data[i]);
			printf("]\n");
		}
	} else {
		TC_PRT("result: %s", __bt_get_error_message(result));
		TC_PRT("Callback: Creating a bond is failed.");
	}
}

void __bt_gatt_connection_state_changed_cb(int result, bool connected, const char *remote_address, void *user_data)
{
	TC_PRT("result: %s", __bt_get_error_message(result));
	if (connected) {
		TC_PRT("LE connected");
	} else {
		TC_PRT("LE DISconnected");
	}
}

bool __bt_gatt_primary_service_cb(bt_gatt_attribute_h service, void *user_data)
{
	TC_PRT("__bt_gatt_primary_service_cb");
	TC_PRT("service %s", (char *)service);
	if (service_index >= MAX_SERVICES)
		return true;

	bt_gatt_clone_attribute_handle(&service_clone[service_index], service);
	service_index++;
	return true;
}

bool __bt_gatt_characteristics_discovered_cb(int result, int index, int total, bt_gatt_attribute_h characteristic, void *user_data)
{
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("index %d", index);
	TC_PRT("total %d", total);
	TC_PRT("characteristic %s", (char *)characteristic);

	if (index >= MAX_SERVICES)
		return true;

	bt_gatt_clone_attribute_handle(&characteristics[index], characteristic);
	return true;
}

bool __bt_gatt_included_service_cb(bt_gatt_attribute_h service, void *user_data)
{
	char *uuid = NULL;
	unsigned char *value = NULL;
	int value_length = 0;

	TC_PRT("__bt_gatt_included_service_cb");
	TC_PRT("service %s", (char *)service);

	if (char_service_index >= MAX_SERVICES)
		return true;

	bt_gatt_clone_attribute_handle(&characteristics_services[char_service_index], service);
	char_service_index++;

	bt_gatt_get_characteristic_declaration(service, &uuid, &value, &value_length);
	TC_PRT("uuid %s", uuid);
	return true;
}

void __bt_gatt_characteristic_changed_cb(bt_gatt_attribute_h characteristic, unsigned char *value, int value_length, void *user_data)
{
	int i;

	TC_PRT("__bt_gatt_characteristic_changed_cb");
	TC_PRT("service %s", (char *)characteristic);
	TC_PRT("value_length %d", value_length);

	for (i = 0; i < value_length; i++) {
		TC_PRT("value %c", value[i]);
	}

	return;
}

void __bt_gatt_char_write_cb(void *user_data)
{
	TC_PRT("__bt_gatt_char_write_cb");
	return;
}

void __bt_gatt_char_read_cb(unsigned char *value, int value_length, void *user_data)
{
	int i;

	TC_PRT("__bt_gatt_char_read_cb");

        TC_PRT("value_length %d", value_length);

        for (i = 0; i < value_length; i++) {
                TC_PRT("value %u", value[i]);
        }

	return;
}

void __bt_avrcp_target_connection_state_changed_cb(bool connected, const char *remote_address, void *user_data)
{
	TC_PRT("__bt_avrcp_target_connection_state_changed_cb");
	TC_PRT("connected %d", connected);
	TC_PRT("remote_address %s", remote_address);
}

void __bt_avrcp_equalizer_state_changed_cb(bt_avrcp_equalizer_state_e equalizer, void *user_data)
{
	TC_PRT("__bt_avrcp_equalizer_state_changed_cb");
	TC_PRT("equalizer %d", equalizer);
}

void __bt_avrcp_repeat_mode_changed_cb(bt_avrcp_repeat_mode_e repeat, void *user_data)
{
	TC_PRT("__bt_avrcp_repeat_mode_changed_cb");
	TC_PRT("repeat %d", repeat);
}

void __bt_avrcp_shuffle_mode_changed_cb(bt_avrcp_shuffle_mode_e shuffle, void *user_data)
{
	TC_PRT("__bt_avrcp_shuffle_mode_changed_cb");
	TC_PRT("shuffle %d", shuffle);
}

void __bt_avrcp_scan_mode_changed_cb(bt_avrcp_scan_mode_e scan, void *user_data)
{
	TC_PRT("__bt_avrcp_scan_mode_changed_cb");
	TC_PRT("equalizer %d", scan);
}

void panu_conn_state_changed(int result, bool connected,
					const char *remote_address,
					bt_panu_service_type_e type,
					void *user_data)
{
	TC_PRT("result [%s]", __bt_get_error_message(result));
	TC_PRT("connected [%d]", connected);
	TC_PRT("address [%s]", remote_address);
	TC_PRT("type [%d]", type);

}

void __bt_hid_host_connection_state_changed_cb(int result,
		bool connected, const char *remote_address, void *user_data)
{
	TC_PRT("__bt_hid_host_connection_state_changed_cb: called");
	TC_PRT("result: %s", __bt_get_error_message(result));
}

char * get_bd_from_file(char *filename)
{
	int fd;
	char *buf;

	if ((fd = open(filename, O_RDONLY)) < 0) {
		perror("Can't open file");
		return NULL;
	}

	buf = g_malloc0(20);

	if (read(fd, buf, 17) < 17) {
		perror("read failed");
		g_free(buf);
		close(fd);
		return NULL;
	}

	close(fd);

	return buf;
}

void __bt_repeat_test_adapter_state_changed_cb(int result, bt_adapter_state_e adapter_state, void *user_data)
{
	unsigned int delay = *(unsigned int*)user_data;

	TC_PRT("__bt_repeat_test_adapter_state_changed_cb");
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("state: %s, delay(%dsec)\n", (adapter_state == BT_ADAPTER_ENABLED)? "ENABLED" : "DISABLED", delay);

	if (result != BT_ERROR_NONE) {
		char *argv[] = {NULL};

		execv("all_log_dump.sh", argv);
		return;
	} else {
		if (delay)
			sleep(delay);

		if (adapter_state == BT_ADAPTER_DISABLED) {
			bt_adapter_enable();
		} else {
			bt_adapter_disable();
		}
	}
}

int test_input_callback(void *data)
{
	int ret = 0;
	int test_id = (int)data;

	switch (current_tc_table) {
	case BT_UNIT_TEST_TABLE_MAIN: {
		switch (test_id) {
		case 0x00ff:
			TC_PRT("Finished");
			g_main_loop_quit(main_loop);
			break;

		case BT_UNIT_TEST_FUNCTION_INITIALIZE:
			ret = bt_initialize();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_DEINITIALIZE:
			ret = bt_deinitialize();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_ADAPTER: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_ADAPTER_GET_STATE: {
			bt_adapter_state_e state = BT_ADAPTER_DISABLED;

			ret = bt_adapter_get_state(&state);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			TC_PRT("state: %d", state);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_ENABLE:
			ret = bt_adapter_enable();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_ADAPTER_DISABLE:
			ret = bt_adapter_disable();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_ADAPTER_RECOVER:
			ret = bt_adapter_recover();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_ADAPTER_START_DEVICE_DISCOVERY:
			ret = bt_adapter_start_device_discovery();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_ADAPTER_STOP_DEVICE_DISCOVERY:
			ret = bt_adapter_stop_device_discovery();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_ADAPTER_IS_DISCOVERING: {
			bool is_discovering = FALSE;
			ret = bt_adapter_is_discovering(&is_discovering);
			if (ret < BT_ERROR_NONE)
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			else
				TC_PRT("is_discovering: %d", is_discovering);

			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_FOREACH_BONDED_DEVICE: {
			ret = bt_adapter_foreach_bonded_device(__bt_adapter_bonded_device_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_GET_BONDED_DEVICE_INFO: {
			bt_device_info_s *device_info = NULL;

			ret = bt_adapter_get_bonded_device_info(remote_addr,
								&device_info);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));

			if (device_info) {
				TC_PRT("address: %s", device_info->remote_address);
				TC_PRT("name: %s", device_info->remote_name);
			}

			bt_adapter_free_device_info(device_info);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_IS_SERVICE_USED: {
			bool used = FALSE;

			ret = bt_adapter_is_service_used(opp_uuid, &used);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			TC_PRT("used: %d", used);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_GET_VISIBILITY: {
			bt_adapter_visibility_mode_e visibility_mode = BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE;
			int time = 0;

			ret = bt_adapter_get_visibility(&visibility_mode, &time);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			TC_PRT("mode: %d", visibility_mode);
			TC_PRT("time: %d", time);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_SET_DEVICE_DISCOVERY_STATE_CHANGED_CB:
			ret = bt_adapter_set_device_discovery_state_changed_cb(__bt_adapter_device_discovery_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_DEVICE_DISCOVERY_STATE_CHANGED_CB:
			ret = bt_adapter_unset_device_discovery_state_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_ADAPTER_GET_LOCAL_OOB_DATA: {
			ret = bt_adapter_get_local_oob_data(&hash, &randomizer, &hash_len, &rand_len);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			} else {
				TC_PRT("hash = [%s]", hash);
				TC_PRT("randomizer = [%s]", randomizer);
			}
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_SET_LOCAL_OOB_DATA: {
			ret = bt_adapter_set_remote_oob_data(remote_addr, hash, randomizer, hash_len, rand_len);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_REMOVE_REMOTE_OOB_DATA: {
			ret = bt_adapter_remove_remote_oob_data(remote_addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_SET_VISIBILITY_MODE_CHANGED_CB:
			ret = bt_adapter_set_visibility_mode_changed_cb(__bt_adapter_device_visibility_mode_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_VISIBILITY_MODE_CHANGED_CB:
			ret = bt_adapter_unset_visibility_mode_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_ADAPTER_SET_VISIBILITY_DURATION_CHANGED_CB:
			ret = bt_adapter_set_visibility_duration_changed_cb(
					__bt_adapter_device_visibility_duration_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_VISIBILITY_DURATION_CHANGED_CB:
			ret = bt_adapter_unset_visibility_duration_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_ADAPTER_SET_CONNECTABLE_CHANGED_CB: {
			ret = bt_adapter_set_connectable_changed_cb(__bt_adapter_connectable_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_CONNECTABLE_CHANGED_CB: {
			ret = bt_adapter_unset_connectable_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_GET_CONNECTABLE: {
			bool connectable = false;

			ret = bt_adapter_get_connectable(&connectable);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
				break;
			}

			TC_PRT("%s", connectable ? "Connectable" : "Non-connectable");
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_SET_CONNECTABLE: {
			bool connectable = false;

			bt_adapter_get_connectable(&connectable);

			ret = bt_adapter_set_connectable(!connectable);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_GET_VERSION: {
			char *version = NULL;

			ret = bt_adapter_get_version(&version);
			if (ret < BT_ERROR_NONE)
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			else {
				TC_PRT("Version: [%s]",version);
				g_free(version);
			}
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_GET_LOCAL_INFO: {
			char *chipset = NULL;
			char *firmware = NULL;
			char *stack_version = NULL;
			char *profiles = NULL;

			ret = bt_adapter_get_local_info(&chipset, &firmware, &stack_version, &profiles);
			if (ret < BT_ERROR_NONE)
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			else {
				TC_PRT("Local info: [%s, %s, %s]", chipset, firmware, stack_version);
				TC_PRT("Local info(Profiles):\n%s", profiles);
				g_free(chipset);
				g_free(firmware);
				g_free(stack_version);
				g_free(profiles);
			}
			break;
		}

		default:
			break;
		}

		break;
	}

	case BT_UNIT_TEST_TABLE_ADAPTER_LE: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_GET_STATE: {
			bt_adapter_le_state_e le_state = BT_ADAPTER_LE_DISABLED;
			
			ret = bt_adapter_le_get_state(&le_state);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			TC_PRT("le_state: %d", le_state);
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ENABLE: {
			ret = bt_adapter_le_enable();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_DISABLE: {
			ret = bt_adapter_le_disable();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_DEVICE_DISCOVERY:
			ret = bt_adapter_le_start_device_discovery();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_DEVICE_DISCOVERY:
			ret = bt_adapter_le_stop_device_discovery();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ADD_WHITE_LIST: {
			ret = bt_adapter_le_add_white_list(remote_addr, BT_DEVICE_PUBLIC_ADDRESS);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_REMOVE_WHITE_LIST: {
			ret = bt_adapter_le_remove_white_list(remote_addr, BT_DEVICE_PUBLIC_ADDRESS);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_CLEAR_WHITE_LIST: {
			ret = bt_adapter_le_clear_white_list();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ENABLE_PRIVACY: {
			static bool enable_privacy = false;

			if (enable_privacy == false)
				enable_privacy = true;
			else
				enable_privacy = false;

			ret = bt_adapter_le_enable_privacy(enable_privacy);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_DEVICE_DISCOVERY_STATE_CHANGED_CB:
			ret = bt_adapter_le_set_device_discovery_state_changed_cb(__bt_adapter_le_device_discovery_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_UNSET_DEVICE_DISCOVERY_STATE_CHANGED_CB:
			ret = bt_adapter_le_unset_device_discovery_state_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		default:
			break;
		}

		break;
	}

	case BT_UNIT_TEST_TABLE_DEVICE: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_DEVICE_SET_AUTHORIZATION_TRUE: {
			ret = bt_device_set_authorization(remote_addr, BT_DEVICE_AUTHORIZED);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_DEVICE_SET_AUTHORIZATION_FALSE: {
			ret = bt_device_set_authorization(remote_addr, BT_DEVICE_UNAUTHORIZED);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_DEVICE_SET_AUTHORIZATION_CHANGED_CB:
			ret = bt_device_set_authorization_changed_cb(__bt_device_authorization_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_DEVICE_UNSET_AUTHORIZATION_CHANGED_CB:
			ret = bt_device_unset_authorization_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_DEVICE_SET_CONNECTION_STAET_CHANGED_CB:
			ret = bt_device_set_connection_state_changed_cb(__bt_device_connection_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_DEVICE_UNSET_CONNECTION_STAET_CHANGED_CB:
			ret = bt_device_unset_connection_state_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_DEVICE_FOREACH_CONNECTED_PROFILES: {
			ret = bt_device_foreach_connected_profiles(remote_addr, __bt_device_connected_profile, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_DEVICE_IS_PROFILE_CONNECTED: {
			bool connected_status = false;

			ret = bt_device_is_profile_connected(remote_addr, BT_PROFILE_HSP, &connected_status);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			}
			TC_PRT("Profile [%s]", connected_status ? "Connected" : "Disconnected");
			break;
		}

		case BT_UNIT_TEST_FUNCTION_DEVICE_SET_BOND_CREATED_CB : {
			ret = bt_device_set_bond_created_cb(__bt_device_bond_created_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_DEVICE_CREATE_BOND : {
			ret = bt_device_create_bond(remote_addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_DEVICE_CREATE_BOND_BY_TYPE : {
			ret = bt_device_create_bond_by_type(remote_addr, BT_DEVICE_CONNECTION_LINK_LE);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_SOCKET: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_SOCKET_CREATE_RFCOMM: {
			int socket_fd = 0;

			ret = bt_socket_create_rfcomm(spp_uuid, &socket_fd);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			} else {
				TC_PRT("socket_fd: %d", socket_fd);
				server_fd = socket_fd;
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_SOCKET_DESTROY_RFCOMM:
			ret = bt_socket_destroy_rfcomm(server_fd);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_LISTEN_AND_ACCEPT_RFCOMM:
			ret = bt_socket_listen_and_accept_rfcomm(server_fd, 1);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_LISTEN:
			ret = bt_socket_listen(server_fd, 1);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_ACCEPT: {
			int socket_fd = 0;

			ret = bt_socket_accept(server_fd);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			} else {
				TC_PRT("socket_fd: %d", socket_fd);
				client_fd = socket_fd;
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_SOCKET_REJECT:
			ret = bt_socket_reject(server_fd);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_CONNECT_RFCOMM: {
			ret = bt_socket_connect_rfcomm(remote_addr, spp_uuid);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_SOCKET_DISCONNECT_RFCOMM:
			ret = bt_socket_disconnect_rfcomm(client_fd);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_SEND_DATA: {
			char data[] = "Sending test";
			ret = bt_socket_send_data(client_fd, data, sizeof(data));
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_SOCKET_SET_DATA_RECEIVED_CB:
			ret = bt_socket_set_data_received_cb(__bt_socket_data_received_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_DATA_RECEIVED_CB:
			ret = bt_socket_unset_data_received_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_SET_CONNECTION_REQUESTED_CB:
			ret = bt_socket_set_connection_requested_cb(__bt_socket_connection_requested_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_CONNECTION_REQUESTED_CB:
			ret = bt_socket_unset_connection_requested_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_SET_CONNECTION_STATE_CHANGED_CB:
			ret = bt_socket_set_connection_state_changed_cb(__bt_socket_connection_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_CONNECTION_STATE_CHANGED_CB:
			ret = bt_socket_unset_connection_state_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_OPP: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_OPP_CLIENT_INITIALIZE:
			ret = bt_opp_client_initialize();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_OPP_CLIENT_DEINITIALIZE:
			ret = bt_opp_client_deinitialize();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_OPP_CLIENT_ADD_FILE:
			ret = bt_opp_client_add_file("/opt/media/Images/image1.jpg");
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_OPP_CLIENT_CLEAR_FILES:
			ret = bt_opp_client_clear_files();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_OPP_CLIENT_PUSH_FILES: {
			ret = bt_opp_client_push_files(remote_addr,__bt_opp_client_push_responded_cb,
						__bt_opp_client_push_progress_cb,
						__bt_opp_client_push_finished_cb,
						NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_OPP_CLIENT_CANCEL_PUSH:
			ret = bt_opp_client_cancel_push();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_AUDIO: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_AUDIO_INITIALIZE:
			ret = bt_audio_initialize();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_AUDIO_DEINITIALIZE:
			ret = bt_audio_deinitialize();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_AUDIO_CONNECT: {
			ret = bt_audio_connect(remote_addr,  BT_AUDIO_PROFILE_TYPE_HSP_HFP);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_AUDIO_DISCONNECT: {
			ret = bt_audio_disconnect(remote_addr,  BT_AUDIO_PROFILE_TYPE_HSP_HFP);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_AUDIO_SET_CONNECTION_STATE_CHANGED_CB:
			ret = bt_audio_set_connection_state_changed_cb(
						__bt_audio_connection_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_AUDIO_UNSET_CONNECTION_STATE_CHANGED_CB:
			ret = bt_audio_unset_connection_state_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_AG_OPEN_SCO:
			ret = bt_ag_open_sco();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_AG_CLOSE_SCO:
			ret = bt_ag_close_sco();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_AG_IS_SCO_OPENED: {
			bool opened = false;
			ret = bt_ag_is_sco_opened(&opened);
			TC_PRT("opend[%s]", opened ? "YES" : "NO");
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_AG_SET_SCO_STATE_CHANGED_CB:
			ret = bt_ag_set_sco_state_changed_cb(__bt_ag_sco_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_AG_UNSET_SCO_STATE_CHANGED_CB:
			ret = bt_ag_unset_sco_state_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_IDLE: {
			unsigned int call_id = 1;

			ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_IDLE, call_id, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_ANSWERED: {
			unsigned int call_id = 1;
			bool sco_required = true;

			ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_ANSWERED, call_id, NULL);
			if (sco_required)
				bt_ag_open_sco();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_DIALING: {
			unsigned int call_id = 1;
			bool sco_required = TRUE;
			char *phone_number;

			phone_number = g_strdup("01012345678");
			ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_DIALING,
						call_id, (const char*)phone_number);
			if (sco_required)
				bt_ag_open_sco();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			g_free(phone_number);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_ALERTING: {
			unsigned int call_id = 1;

			ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_ALERTING,
						call_id, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_INCOMING: {
			unsigned int call_id = 1;
			char *phone_number;

			phone_number = g_strdup("01012345678");
			ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_INCOMING,
						call_id, (const char*)phone_number);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			g_free(phone_number);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_CALL_LIST_CREATE:
			ret = bt_call_list_create(&call_list);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_CALL_LIST_DESTROY:
			ret = bt_call_list_destroy(call_list);
			call_list = NULL;
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_CALL_LIST_RESET:
			ret = bt_call_list_reset(call_list);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_CALL_LIST_ADD: {
			char *phone_number;

			phone_number = g_strdup("01012345678");
			ret = bt_call_list_add(call_list, 1, BT_AG_CALL_STATE_ACTIVE,
						(const char*)phone_number);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			g_free(phone_number);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_AG_NOTIFY_CALL_LIST:
			ret = bt_ag_notify_call_list(call_list);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_AG_SET_CALL_HANDLING_EVENT_CB:
			ret = bt_ag_set_call_handling_event_cb(
						__bt_ag_set_call_handling_event_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_AG_IS_NREC_ENABLED: {
			bool enabled = false;
			ret = bt_ag_is_nrec_enabled(&enabled);
			TC_PRT("opend[%s]", enabled ? "YES" : "NO");
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			}
			break;
		}
		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_PAN: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_NAP_ACTIVATE:
			ret = bt_nap_activate();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_NAP_DEACTIVATE:
			ret = bt_nap_deactivate();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_NAP_DISCONNECT_ALL:
			ret = bt_nap_disconnect_all();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_NAP_DISCONNECT: {
			ret = bt_nap_disconnect(remote_addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_PANU_SET_CONNECTION_STATE_CHANGED_CB: {
			ret = bt_panu_set_connection_state_changed_cb(panu_conn_state_changed,
												   NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_PANU_CONNECT: {
			ret = bt_panu_connect(remote_addr, BT_PANU_SERVICE_TYPE_NAP);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_GATT: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_GATT_FOREACH_PRIMARY_SERVICES: {
			__bt_free_gatt_services();

			ret = bt_gatt_foreach_primary_services(remote_addr, __bt_gatt_primary_service_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			if (ret == BT_ERROR_NONE)
				__select_index(&selected_service_index);
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_DISCOVER_CHARACTERISTICS: {
			__bt_free_gatt_characteristics();

			if (selected_service_index < 0) {
				TC_PRT("Need to select primary service \n");
				break;
			}

			ret = bt_gatt_discover_characteristics(service_clone[selected_service_index], __bt_gatt_characteristics_discovered_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			if (ret == BT_ERROR_NONE)
				__select_index(&selected_char_index);
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_GET_SERVICE_UUID: {
			char *uuid = NULL;

			if (selected_service_index < 0) {
				TC_PRT("Need to select primary service \n");
				break;
			}

			ret = bt_gatt_get_service_uuid(service_clone[selected_service_index], &uuid);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			} else {
				TC_PRT("uuid: %s", uuid);
				g_free(uuid);
			}
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_FOREACH_INCLUDED_SERVICES: {
			__bt_free_gatt_characteristics_services();

			if (selected_service_index < 0) {
				TC_PRT("Need to select primary service \n");
				break;
			}

			ret = bt_gatt_foreach_included_services(service_clone[selected_service_index], __bt_gatt_included_service_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_SET_CHARACTERISTIC_CHANGED_CB: {
			ret = bt_gatt_set_characteristic_changed_cb(__bt_gatt_characteristic_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_UNSET_CHARACTERISTIC_CHANGED_CB: {
			ret = bt_gatt_unset_characteristic_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_GET_CHARACTERISTIC_DECLARATION: {
			int i;
			char *uuid = NULL;
			unsigned char *value = NULL;
			int value_length = 0;

			ret = bt_gatt_get_characteristic_declaration(characteristics_services[0], &uuid, &value, &value_length);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			} else {
				TC_PRT("uuid: %s", uuid);

				for (i = 0; i < value_length; i++) {
					TC_PRT("value %c", value[i]);
				}

				g_free(uuid);
				g_free(value);
			}
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_SET_CHARACTERISTIC_VALUE: {
			unsigned char value[] = { 1 };
			if (selected_char_index == 0) {
				TC_PRT("Need to select charateristic value \n");
				break;
			}

			ret = bt_gatt_set_characteristic_value_request(characteristics[selected_char_index], value, 1,
					__bt_gatt_char_write_cb);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_READ_CHARACTERISTIC_VALUE : {
			if (selected_char_index == 0) {
				TC_PRT("Need to select charateristic value \n");
				break;
			}

			ret = bt_gatt_read_characteristic_value(characteristics[selected_char_index], __bt_gatt_char_read_cb);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_WATCH_CHARACTERISTIC_CHANGES: {
			if (selected_service_index < 0) {
				TC_PRT("Need to select primary service \n");
				break;
			}

			ret = bt_gatt_watch_characteristic_changes(service_clone[selected_service_index]);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_UNWATCH_CHARACTERISTIC_CHANGES: {
			ret = bt_gatt_unwatch_characteristic_changes(service_clone[selected_service_index]);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CONNECT : {
			ret = bt_gatt_connect(remote_addr, false);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_DISCONNECT : {
			ret = bt_gatt_disconnect(remote_addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_SET_GATT_CONNECTION_STATE_CHANGED_CB : {
			ret = bt_gatt_set_connection_state_changed_cb(__bt_gatt_connection_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_UNSET_GATT_CONNECTION_STATE_CHANGED_CB : {
			ret = bt_gatt_unset_connection_state_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_AVRCP: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_AVRCP_TARGET_INITIALIZE:
			ret = bt_avrcp_target_initialize(__bt_avrcp_target_connection_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_TARGET_DEINITIALIZE:
			ret = bt_avrcp_target_deinitialize();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_SET_EQUALIZER_STATE_CHANGED_CB:
			ret = bt_avrcp_set_equalizer_state_changed_cb(__bt_avrcp_equalizer_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_SET_REPEAT_MODE_CHANGED_CB:
			ret = bt_avrcp_set_repeat_mode_changed_cb(__bt_avrcp_repeat_mode_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_SET_SHUFFLE_MODE_CHANGED_CB:
			ret = bt_avrcp_set_shuffle_mode_changed_cb(__bt_avrcp_shuffle_mode_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_SET_SCAN_MODE_CHANGED_CB:
			ret = bt_avrcp_set_scan_mode_changed_cb(__bt_avrcp_scan_mode_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_EQUALIZER_STATE_CHANGED_CB:
			ret = bt_avrcp_unset_equalizer_state_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_REPEAT_MODE_CHANGED_CB:
			ret = bt_avrcp_unset_repeat_mode_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_SHUFFLE_MODE_CHANGED_CB:
			ret = bt_avrcp_unset_shuffle_mode_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_SCAN_MODE_CHANGED_CB:
			ret = bt_avrcp_unset_scan_mode_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_HID: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_HID_HOST_INITIALIZE:
			ret = bt_hid_host_initialize(__bt_hid_host_connection_state_changed_cb,
											NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_HID_HOST_DEINITIALIZE:
			ret = bt_hid_host_deinitialize();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_HID_HOST_CONNECT: {
			ret = bt_hid_host_connect(remote_addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HID_HOST_DISCONNECT: {
			ret = bt_hid_host_disconnect(remote_addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_AUTOMATED_TEST: {
		static unsigned int delay = 0;

		if (input_automated_test_delay == true) {
			delay = test_id;
			test_id = BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST_N_SEC_DELAY;
		}

		switch (test_id) {
		/*Automated Test Cases */
		case BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST: {
			bt_adapter_state_e state = BT_ADAPTER_DISABLED;
			delay = 0;

			ret = bt_adapter_set_state_changed_cb(__bt_repeat_test_adapter_state_changed_cb, &delay);
			TC_PRT("set_cb returns %s\n", __bt_get_error_message(ret));

			ret = bt_adapter_get_state(&state);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			TC_PRT("Current state: %d", state);

			if (state == BT_ADAPTER_DISABLED)
				bt_adapter_enable();
			else
				bt_adapter_disable();
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST_1_SEC_DELAY: {
			bt_adapter_state_e state = BT_ADAPTER_DISABLED;
			delay = 1;

			ret = bt_adapter_set_state_changed_cb(__bt_repeat_test_adapter_state_changed_cb, &delay);
			TC_PRT("set_cb returns %s\n", __bt_get_error_message(ret));

			ret = bt_adapter_get_state(&state);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			TC_PRT("Current state: %d", state);

			if (state == BT_ADAPTER_DISABLED)
				bt_adapter_enable();
			else
				bt_adapter_disable();
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST_N_SEC_DELAY: {
			bt_adapter_state_e state = BT_ADAPTER_DISABLED;

			input_automated_test_delay = false;

			ret = bt_adapter_set_state_changed_cb(__bt_repeat_test_adapter_state_changed_cb, &delay);
			TC_PRT("set_cb returns %s\n", __bt_get_error_message(ret));

			ret = bt_adapter_get_state(&state);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			TC_PRT("Current state: %d", state);

			if (state == BT_ADAPTER_DISABLED)
				bt_adapter_enable();
			else
				bt_adapter_disable();
			break;
		}
		default:
			break;
		}

		break;
	}
	default:
		break;
	}

	return 0;
}

static gboolean key_event_cb(GIOChannel *chan,
				GIOCondition cond,
				gpointer data)
{
	char buf[BUFFER_LEN] = { 0 };

	unsigned int len = 0;
	int test_id;
	bool is_call_api = false;

	if (is_sub_index)
		return TRUE;

	memset(buf, 0, sizeof(buf));

	if (g_io_channel_read_chars(chan, buf, sizeof(buf),
				&len, NULL) == G_IO_STATUS_ERROR) {
		TC_PRT("IO Channel read error");
		return FALSE;
	}

	TC_PRT("%s", buf);

	test_id = atoi(buf);

	if (current_tc_table == BT_UNIT_TEST_TABLE_MAIN) {
		if (buf[0] == '0' && buf[2] == 0)
			current_tc_table = BT_UNIT_TEST_TABLE_SET_ADDRESS;
		else if(test_id >= BT_UNIT_TEST_TABLE_ADAPTER && test_id < BT_UNIT_TEST_TABLE_END)
			current_tc_table = test_id;
		else
			is_call_api = true;
	}  else {
		if (buf[0] == '0' && buf[2] == 0) {
			current_tc_table = BT_UNIT_TEST_TABLE_MAIN;
		} else if (current_tc_table == BT_UNIT_TEST_TABLE_AUTOMATED_TEST
					&& test_id == BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST_N_SEC_DELAY
					&& input_automated_test_delay == false) {
				input_automated_test_delay = true;
		} else {
			is_call_api = true;
		}
	}

	tc_usage_print();

	if (current_tc_table == BT_UNIT_TEST_TABLE_SET_ADDRESS && is_call_api == true) {
		memcpy (remote_addr, buf, 17);
		remote_addr[17] = 0;
	} else if (test_id && is_call_api)
		g_idle_add(test_input_callback, (void *)test_id);

	return TRUE;
}

int main()
{
	GIOChannel *key_io;

	g_type_init();
	current_tc_table = BT_UNIT_TEST_TABLE_MAIN;

	key_io = g_io_channel_unix_new(fileno(stdin));

	g_io_channel_set_encoding(key_io, NULL, NULL);
	g_io_channel_set_flags(key_io, G_IO_FLAG_NONBLOCK, NULL);

	g_io_add_watch(key_io, G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL,
			key_event_cb, NULL);

	g_io_channel_unref(key_io);

	main_loop = g_main_loop_new(NULL, FALSE);

	g_main_loop_run(main_loop);

	bt_deinitialize();

	return 0;
}
