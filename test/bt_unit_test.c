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

#include <sys/time.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>
#include <ctype.h>

#include "bluetooth.h"
#include "bluetooth_internal.h"
#include "bluetooth_extension.h"
#include "bluetooth_private.h"
#include "bluetooth-telephony-api.h"
#include "bt_unit_test.h"

const char *spp_uuid = "00001101-0000-1000-8000-00805F9B7777";
//const char *spp_uuid = "00001101-0000-1000-8000-00805F9B34FB";
const char *hid_uuid = "00001124-0000-1000-8000-00805f9b34fb";
const char *opp_uuid = "00001105-0000-1000-8000-00805f9b34fb";
const char *custom_uuid = "fa87c0d0-afac-11de-8a39-0800200c9a66";

// Temporary HPS UUIDs. SIG has to define the UUIDs yet.
#define HPS_UUID "00001900-0000-1000-8000-00805f9b34fb"
#define HTTP_URI_UUID "00001901-0000-1000-8000-00805f9b34fb"
#define HTTP_HDR_UUID "00001902-0000-1000-8000-00805f9b34fb"
#define HTTP_ENTITY_UUID "00001903-0000-1000-8000-00805f9b34fb"
#define HTTP_CP_UUID "00001904-0000-1000-8000-00805f9b34fb"
#define HTTP_STATUS_UUID "00001905-0000-1000-8000-00805f9b34fb"
#define HTTP_SECURITY_UUID "00001906-0000-1000-8000-00805f9b34fb"
#define HTTP_STATUS_DESC_UUID "2902"

static bt_unit_test_table_e current_tc_table;
static char remote_addr[18] = "F6:FB:8F:D8:C8:7C";
static bool input_automated_test_delay = false;

/* For HDP profile TEST */
static char *appid = "/org/bluez/health_app_2";
static char* data_hdp = "aaa";
static unsigned channel_hdp = 0;

static bool need_to_set_params = false;
static int g_test_id = -1;
static tc_params_t g_test_param = {0,};

static int server_fd;
static int client_fd;
static int custom_server_fd;
static int custom_client_fd;

static int bt_onoff_cnt = 0;
static int bt_onoff_cnt_success = 0;
static int bt_onoff_cnt_fail = 0;
static int total_time = 0;
static struct timeval check_time = {0,};

static gboolean is_sub_index = FALSE;
#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
int service_index;
int char_service_index;
static int selected_service_index = -1;
static int selected_char_index = 0;
#endif

static bt_advertiser_h advertiser = NULL;
static bt_advertiser_h advertiser_list[3] = {NULL, };
static int advertiser_index = 0;

bt_gatt_client_h client = NULL;
bt_gatt_client_h hps_client = NULL;

bt_gatt_server_h server = NULL;
typedef struct {
	bt_gatt_h svc;
	bt_gatt_h chr;
	bt_gatt_h desc;
} gatt_handle_t;
gatt_handle_t battery_h;
gatt_handle_t heart_rate_h;
gatt_handle_t thermometer_h;
gatt_handle_t custom_h;
gatt_handle_t ipsp_h;

#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
bt_gatt_attribute_h service_clone[MAX_SERVICES];
bt_gatt_attribute_h characteristics[MAX_SERVICES];
bt_gatt_attribute_h characteristics_services[MAX_SERVICES];
#endif

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
	{"HPS" 				, BT_UNIT_TEST_TABLE_HPS},
	{"Avrcp"				, BT_UNIT_TEST_TABLE_AVRCP},
	{"Hid"					, BT_UNIT_TEST_TABLE_HID},
	{"IPSP"				, BT_UNIT_TEST_TABLE_IPSP},
	{"HDP"				, BT_UNIT_TEST_TABLE_HDP},
#ifdef TIZEN_WEARABLE
	{"HF Role"				, BT_UNIT_TEST_TABLE_HF},
	{"PBAP Client"				, BT_UNIT_TEST_TABLE_PBAP_CLIENT},
#endif
	{"Automated test"			, BT_UNIT_TEST_TABLE_AUTOMATED_TEST},
	{"Initialize All"			, BT_UNIT_TEST_FUNCTION_INITIALIZE_ALL},
	{"FINISH"				, BT_UNIT_TEST_TABLE_FINISH},
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
	{"bt_adapter_set_manufacturer_data_changed_cb"			, BT_UNIT_TEST_FUNCTION_ADAPTER_SET_MANUFACTURER_DATA_CHANGED_CB},
	{"bt_adapter_unset_manufacturer_data_changed_cb"		, BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_MANUFACTURER_DATA_CHANGED_CB},
	{"bt_adapter_set_manufacturer_data"				, BT_UNIT_TEST_FUNCTION_ADAPTER_SET_MANUFACTURER_DATA},
	{"bt_adapter_set_authentication_req_cb"				, BT_UNIT_TEST_FUNCTION_ADAPTER_SET_AUTHENTICATION_REQUSET_CB},
	{"bt_adapter_unset_authentication_req_cb"			, BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_AUTHENTICATION_REQUSET_CB},
	{"bt_passkey_reply(passkey, TRUE)"				, BT_UNIT_TEST_FUNCTION_ADAPTER_PASSKEY_REPLY_ACCEPT},
	{"bt_passkey_reply(passkey, FALSE)"				, BT_UNIT_TEST_FUNCTION_ADAPTER_PASSKEY_REPLY_CANCEL},
	{"bt_passkey_confirmation_reply(TRUE)"				, BT_UNIT_TEST_FUNCTION_ADAPTER_PASSKEY_CONFIRMATION_REPLY_ACCEPT},
	{"bt_passkey_confirmation_reply(FALSE)"				, BT_UNIT_TEST_FUNCTION_ADAPTER_PASSKEY_CONFIRMATION_REPLY_REJECT},
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
	{NULL					, 0x0000},
};

tc_table_t tc_adapter_le[] = {
	/* Adapter functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_adapter_le_get_state"					, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_GET_STATE},
	{"bt_adapter_le_enable" 					, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ENABLE},
	{"bt_adapter_le_disable"					, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_DISABLE},
	{"bt_adapter_le_set_scan_mode"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_SCAN_MODE},
	{"bt_adapter_le_start_scan"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_SCAN},
	{"bt_adapter_le_stop_scan"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_SCAN},
#ifndef TIZEN_WEARABLE
	{"bt_adapter_le_start_device_discovery"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_DEVICE_DISCOVERY},
	{"bt_adapter_le_stop_device_discovery"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_DEVICE_DISCOVERY},
#endif
	{"bt_adapter_le_add_advertising_data"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ADD_ADVERTISING_DATA},
	{"bt_adapter_le_set_advertising_mode"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_MODE},
	{"bt_adapter_le_set_advertising_filter_policy"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_FILTER_POLICY},
	{"bt_adapter_le_set_advertising_connectable"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_CONNECTABLE},
	{"bt_adapter_le_start_advertising_new"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_ADVERTISING_NEW},	
#ifndef TIZEN_WEARABLE
	{"bt_adapter_le_start_advertising"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_ADVERTISING},
#endif
	{"bt_adapter_le_stop_advertising"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_ADVERTISING},
	{"bt_adapter_le_add_white_list"					, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ADD_WHITE_LIST},
	{"bt_adapter_le_remove_white_list"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_REMOVE_WHITE_LIST},
#ifndef TIZEN_WEARABLE
	{"bt_adapter_le_clear_white_list"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_CLEAR_WHITE_LIST},
#endif
	{"bt_adapter_le_enable_privacy"					, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ENABLE_PRIVACY},
	{"Register scan filter (Device Address)"			, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_REGISTER_SCAN_FILTER_DEVICE_ADDRESS},
	{"Register scan filter (Service UUID)"				, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_REGISTER_SCAN_FILTER_SERVICE_UUID},
	{"Register scan filter (Service Solicitation UUID)"		, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_REGISTER_SCAN_FILTER_SERVICE_SOLICITATION_UUID},
	{"Unregister all scan filters"					, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_UNREGISTER_ALL_SCAN_FILTERS},
#ifndef TIZEN_WEARABLE
	{"bt_adapter_le_set_device_discovery_state_changed_cb"		, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_DEVICE_DISCOVERY_STATE_CHANGED_CB},
	{"bt_adapter_le_unset_device_discovery_state_changed_cb"	, BT_UNIT_TEST_FUNCTION_ADAPTER_LE_UNSET_DEVICE_DISCOVERY_STATE_CHANGED_CB},
#endif
	{"bt_adater_le_read_maximum_data_length",		BT_UNIT_TEST_FUNCTION_ADAPTER_LE_READ_MAXIMUM_DATA_LENGTH},
	{"bt_adater_le_wite_host_suggested_def_data_length",		BT_UNIT_TEST_FUNCTION_ADAPTER_LE_WRITE_HOST_SUGGESTED_DEFAULT_DATA_LENGTH},
	{"bt_adater_le_read_host_suggested_def_data_length",		BT_UNIT_TEST_FUNCTION_ADAPTER_LE_READ_HOST_SUGGESTED_DEFAULT_DATA_LENGTH},
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
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
	{"bt_device_set_bond_destroyed_cb"				, BT_UNIT_TEST_FUNCTION_DEVICE_SET_BOND_DESTROYED_CB},
	{"bt_device_create_bond" 					, BT_UNIT_TEST_FUNCTION_DEVICE_CREATE_BOND},
	{"bt_device_create_bond_by_type"				, BT_UNIT_TEST_FUNCTION_DEVICE_CREATE_BOND_BY_TYPE},
	{"bt_device_destroy_bond"					, BT_UNIT_TEST_FUNCTION_DEVICE_DESTROY_BOND},
	{"bt_device_le_set_data_length"                       		, BT_UNIT_TEST_FUNCTION_LE_DEVICE_SET_DATA_LENGTH},
	{"bt_device_le_data_length_changed_cb",		BT_UNIT_TEST_FUNCTION_LE_DEVICE_DATA_LENGTH_CHANGED_CB},
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
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
	{"bt_socket_create_rfcomm with custom uuid" 			, BT_UNIT_TEST_FUNCTION_SOCKET_CREATE_RFCOMM_CUSTOM_UUID},
	{"bt_socket_destroy_rfcomm with custom uuid"			, BT_UNIT_TEST_FUNCTION_SOCKET_DESTROY_RFCOMM_CUSTOM_UUID},
	{"bt_socket_listen_and_accept_rfcomm with custom uuid"		, BT_UNIT_TEST_FUNCTION_SOCKET_LISTEN_AND_ACCEPT_RFCOMM_CUSTOM_UUID},
	{"bt_socket_connect_rfcomm with custom uuid" 			, BT_UNIT_TEST_FUNCTION_SOCKET_CONNECT_RFCOMM_CUSTOM_UUID},
	{"bt_socket_disconnect_rfcomm with custom uuid"			, BT_UNIT_TEST_FUNCTION_SOCKET_DISCONNECT_RFCOMM_CUSTOM_UUID},
	{"bt_socket_send_data with custom uuid"				, BT_UNIT_TEST_FUNCTION_SOCKET_SEND_DATA_CUSTOM_UUID},
	{"bt_socket_set_data_received_cb"				, BT_UNIT_TEST_FUNCTION_SOCKET_SET_DATA_RECEIVED_CB},
	{"bt_socket_unset_data_received_cb"				, BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_DATA_RECEIVED_CB},
	{"bt_socket_set_connection_requested_cb"			, BT_UNIT_TEST_FUNCTION_SOCKET_SET_CONNECTION_REQUESTED_CB},
	{"bt_socket_unset_connection_requested_cb"			, BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_CONNECTION_REQUESTED_CB},
	{"bt_socket_set_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_SOCKET_SET_CONNECTION_STATE_CHANGED_CB},
	{"bt_socket_unset_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_SOCKET_UNSET_CONNECTION_STATE_CHANGED_CB},
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
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
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
	{NULL					, 0x0000},
};

tc_table_t tc_audio[] = {
	/* AG functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_audio_initialize"						, BT_UNIT_TEST_FUNCTION_AUDIO_INITIALIZE},
	{"bt_audio_deinitialize"					, BT_UNIT_TEST_FUNCTION_AUDIO_DEINITIALIZE},
	{"bt_audio_connect"						, BT_UNIT_TEST_FUNCTION_AUDIO_CONNECT},
	{"bt_audio_disconnect"						, BT_UNIT_TEST_FUNCTION_AUDIO_DISCONNECT},
	{"bt_audio_sink_connect"					, BT_UNIT_TEST_FUNCTION_AUDIO_SINK_CONNECT},
	{"bt_audio_sink_disconnect"					, BT_UNIT_TEST_FUNCTION_AUDIO_SINK_DISCONNECT},
	{"bt_audio_set_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_AUDIO_SET_CONNECTION_STATE_CHANGED_CB},
	{"bt_audio_unset_connection_state_changed_cb"			, BT_UNIT_TEST_FUNCTION_AUDIO_UNSET_CONNECTION_STATE_CHANGED_CB},
	{"bt_ag_open_sco" 						, BT_UNIT_TEST_FUNCTION_AG_OPEN_SCO},
	{"bt_ag_close_sco" 						, BT_UNIT_TEST_FUNCTION_AG_CLOSE_SCO},
	{"bt_ag_is_sco_opened"						, BT_UNIT_TEST_FUNCTION_AG_IS_SCO_OPENED},
	{"bt_ag_set_sco_state_changed_cb"				, BT_UNIT_TEST_FUNCTION_AG_SET_SCO_STATE_CHANGED_CB},
	{"bt_ag_unset_sco_state_changed_cb"				, BT_UNIT_TEST_FUNCTION_AG_UNSET_SCO_STATE_CHANGED_CB},
	{"bt_ag_is_connected"						, BT_UNIT_TEST_FUNCTION_AG_IS_CONNECTED},
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

	/* A2DP SCMS-T functions */
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
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
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
	{NULL					, 0x0000},
};

tc_table_t tc_gatt[] = {
	/* Gatt functions */
	{"BACK"							, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_gatt_connect"					, BT_UNIT_TEST_FUNCTION_GATT_CONNECT},
	{"bt_gatt_disconnect"					, BT_UNIT_TEST_FUNCTION_GATT_DISCONNECT},
	{"bt_gatt_set_connection_state_changed_cb"		, BT_UNIT_TEST_FUNCTION_SET_GATT_CONNECTION_STATE_CHANGED_CB},
	{"bt_gatt_unset_connection_state_changed_cb"		, BT_UNIT_TEST_FUNCTION_UNSET_GATT_CONNECTION_STATE_CHANGED_CB},
	{"bt_gatt_client_create"				, BT_UNIT_TEST_FUNCTION_GATT_CLIENT_CREATE},
	{"bt_gatt_client_destroy"				, BT_UNIT_TEST_FUNCTION_GATT_CLIENT_DESTROY},
	{"bt_gatt_client_get_remote_address"			, BT_UNIT_TEST_FUNCTION_GATT_CLIENT_GET_REMOTE_ADDRESS},
	{"bt_gatt_client_print_all"				, BT_UNIT_TEST_FUNCTION_GATT_CLIENT_PRINT_ALL},
	{"bt_gatt_client_read_value"				, BT_UNIT_TEST_FUNCTION_GATT_CLIENT_READ_VALUE},
	{"bt_gatt_client_write_value"				, BT_UNIT_TEST_FUNCTION_GATT_CLIENT_WRITE_VALUE},
	{"bt_gatt_client_set_char_value_changed_cb"		, BT_UNIT_TEST_FUNCTION_GATT_CLIENT_SET_CHAR_VALUE_CHANGED_CB},
	{"bt_gatt_client_unset_char_value_changed_cb"		, BT_UNIT_TEST_FUNCTION_GATT_CLIENT_UNSET_CHAR_VALUE_CHANGED_CB},
	{"bt_gatt_client_foreach_services"			, BT_UNIT_TEST_FUNCTION_GATT_CLIENT_FOREACH_SERVICES},
	{"bt_gatt_server_initialize"				, BT_UNIT_TEST_FUNCTION_GATT_SERVER_INITIALIZE},
	{"bt_gatt_server_deinitilaize"				, BT_UNIT_TEST_FUNCTION_GATT_SERVER_DEINITIALIZE},
	{"bt_gatt_server_unregister_all_services"		, BT_UNIT_TEST_FUNCTION_GATT_SERVER_UNREGISTER_ALL_SERVICES},
	{"Register Battery Service"				, BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_BATTERY_SVC},
	{"Change Battery Level"					, BT_UNIT_TEST_FUNCTION_GATT_SERVER_CHANGE_BATTERY_LEVEL},
	{"Register Heart Rate Service"				, BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_HEART_RATE_SVC},
	{"Notify Heart Rate Measurement"			, BT_UNIT_TEST_FUNCTION_GATT_SERVER_NOTIFY_HEART_RATE_MEASUREMENT},
	{"Register Thermometer Service"				, BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_THERMOMETER_SVC},
	{"Change Thermometer Measurement"			, BT_UNIT_TEST_FUNCTION_GATT_SERVER_CHANGE_THERMOMETER_MEASUREMENT},
	{"Register Device Information Service"			, BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_DEVICE_INFORMATION_SVC},
	{"Register Link Loss Service"				, BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_LINK_LOSS_SVC},
	{"Register Custom Service"				, BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_CUSTOM_SVC},
	{"Change Custom Value"					, BT_UNIT_TEST_FUNCTION_GATT_SERVER_CHANGE_CUSTOM_VAL},
	{"ANCS (Pair)"						, BT_UNIT_TEST_FUNCTION_ANCS_PAIR},
	{"ANCS (Watch notification)"				, BT_UNIT_TEST_FUNCTION_ANCS_WATCH},
	{"ANCS (Positive action)"				, BT_UNIT_TEST_FUNCTION_ANCS_POSITIVE_ACTION},
	{"ANCS (Negative action)"				, BT_UNIT_TEST_FUNCTION_ANCS_NEGATIVE_ACTION},
	{"ANCS (Get Noti. Attr.)"				, BT_UNIT_TEST_FUNCTION_ANCS_GET_NOTI_ATTR},
#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
	{"bt_gatt_foreach_primary_services"			, BT_UNIT_TEST_FUNCTION_GATT_FOREACH_PRIMARY_SERVICES},
	{"bt_gatt_discover_characteristics"			, BT_UNIT_TEST_FUNCTION_GATT_DISCOVER_CHARACTERISTICS},
	{"bt_gatt_discover_characteristic_descriptor"		, BT_UNIT_TEST_FUNCTION_GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS},
	{"bt_gatt_get_service_uuid"				, BT_UNIT_TEST_FUNCTION_GATT_GET_SERVICE_UUID},
	{"bt_gatt_foreach_included_services"			, BT_UNIT_TEST_FUNCTION_GATT_FOREACH_INCLUDED_SERVICES},
	{"bt_gatt_set_characteristic_changed_cb"		, BT_UNIT_TEST_FUNCTION_GATT_SET_CHARACTERISTIC_CHANGED_CB},
	{"bt_gatt_unset_characteristic_changed_cb"		, BT_UNIT_TEST_FUNCTION_GATT_UNSET_CHARACTERISTIC_CHANGED_CB},
	{"bt_gatt_get_characteristic_declaration"		, BT_UNIT_TEST_FUNCTION_GATT_GET_CHARACTERISTIC_DECLARATION},
	{"bt_gatt_set_characteristic_value"			, BT_UNIT_TEST_FUNCTION_GATT_SET_CHARACTERISTIC_VALUE},
	{"bt_gatt_set_characteristic_value_request"		, BT_UNIT_TEST_FUNCTION_GATT_SET_CHARACTERISTIC_VALUE_REQUEST},
	{"bt_gatt_read_characteristic_value"			, BT_UNIT_TEST_FUNCTION_GATT_READ_CHARACTERISTIC_VALUE},
	{"bt_gatt_watch_characteristic_changes"			, BT_UNIT_TEST_FUNCTION_GATT_WATCH_CHARACTERISTIC_CHANGES},
	{"bt_gatt_unwatch_characteristic_changes"		, BT_UNIT_TEST_FUNCTION_GATT_UNWATCH_CHARACTERISTIC_CHANGES},
#endif
	{"Select this menu to set parameters and then select the function again."	, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
	{NULL					, 0x0000},
};

tc_table_t tc_hps[] = {
	/* HPS functions */
	{"BACK" 						, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_hps_client_create"				, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_CREATE},
	{"bt_hps_client_destroy"				, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_DESTROY},
	{"bt_hps_client_print_all" 			, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_PRINT_ALL},
	{"bt_hps_client_read_value"				, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_READ_VALUE},
	{"bt_hps_client_set_uri"				, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_URI},
	{"bt_hps_client_set_hdr"				, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_HDR},
	{"bt_hps_client_set_entity"				, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_ENTITY},
	{"bt_hps_client_set_cp" 			, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_CP},
	{"bt_hps_client_get_uri"				, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_GET_URI},
	{"bt_hps_client_get_hdr"				, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_GET_HDR},
	{"bt_hps_client_get_entity" 			, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_GET_ENTITY},
	{"bt_hps_client_get_security" 			, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_GET_SECURITY},
	{"bt_hps_client_set_status_notification"			, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_STATUS_NOTIFICATION},
	{"bt_hps_client_unset_status_notification"			, BT_UNIT_TEST_FUNCTION_HPS_CLIENT_UNSET_STATUS_NOTIFICATION},
	{"Select this menu to set parameters and then select the function again."	, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
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
	{"bt_avrcp_control_initialize"					, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_INITIALIZE},
	{"bt_avrcp_control_deinitialize"				, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_DEINITIALIZE},
	{"bt_avrcp_control_connect"				, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_CONNECT},
	{"bt_avrcp_control_disconnect"				, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_DISCONNECT},
	{"bt_avrcp_control_player_PLAY"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_PLAY},
	{"bt_avrcp_control_player_STOP"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_STOP},
	{"bt_avrcp_control_player_PAUSE"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_PAUSE},
	{"bt_avrcp_control_player_FF"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_FF},
	{"bt_avrcp_control_player_RW"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_RW},
	{"bt_avrcp_control_player_NEXT"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_NEXT},
	{"bt_avrcp_control_player_PREV"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_PREV},
	{"bt_avrcp_control_player_GET_REPEAT"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_GET_REPEAT},
	{"bt_avrcp_control_player_SET_REPEAT"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_SET_REPEAT},
	{"bt_avrcp_control_player_GET_SHUFFLE"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_GET_SHUFFLE},
	{"bt_avrcp_control_player_SET_SHUFFLE"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_SET_SHUFFLE},
	{"bt_avrcp_control_player_GET_POSITION"			, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_GET_POSITION},
	{"bt_avrcp_control_player_GET_PLAY STATUS"		, BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_GET_PLAY_STATUS},
	{"bt_avrcp_control_player_GET_TRACK_INFO"		,BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_GET_TRACK_INFO},
	{"bt_avrcp_set_song_position_changed_cb"			, BT_UNIT_TEST_FUNCTION_AVRCP_SET_SONG_POSITION_CHANGED_CB},
	{"bt_avrcp_set_play_status_changed_cb"				, BT_UNIT_TEST_FUNCTION_AVRCP_SET_PLAY_STATE_CHANGED_CB},
	{"bt_avrcp_set_track_info_changed_cb"				, BT_UNIT_TEST_FUNCTION_AVRCP_SET_TRACK_INFO_CHANGED_CB},
	{"bt_avrcp_unset_song_position_changed_cb"			, BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_SONG_POSITION_CHANGED_CB},
	{"bt_avrcp_unset_play_status_changed_cb"			, BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_PLAY_STATE_CHANGED_CB},
	{"bt_avrcp_unset_track_info_changed_cb"			, BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_TRACK_INFO_CHANGED_CB},
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
	{NULL					, 0x0000},
};

tc_table_t tc_hid[] = {
	/* HID functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_hid_host_initialize"					, BT_UNIT_TEST_FUNCTION_HID_HOST_INITIALIZE},
	{"bt_hid_host_deinitialize"					, BT_UNIT_TEST_FUNCTION_HID_HOST_DEINITIALIZE},
	{"bt_hid_host_connect"						, BT_UNIT_TEST_FUNCTION_HID_HOST_CONNECT},
	{"bt_hid_host_disconnect"					, BT_UNIT_TEST_FUNCTION_HID_HOST_DISCONNECT},
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
	{NULL					, 0x0000},
};

tc_table_t tc_ipsp[] = {
	/* IPSP functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_le_ipsp_register"					, BT_UNIT_TEST_FUNCTION_IPSP_REGISTER},
	{"bt_le_ipsp_unregister"				, BT_UNIT_TEST_FUNCTION_IPSP_UNREGISTER},
	{"bt_le_ipsp_initialize"					, BT_UNIT_TEST_FUNCTION_IPSP_INITIALIZE},
	{"bt_le_ipsp_deinitialize"				, BT_UNIT_TEST_FUNCTION_IPSP_DEINITIALIZE},
	{"bt_le_ipsp_connect"					, BT_UNIT_TEST_FUNCTION_IPSP_CONNECT},
	{"bt_le_ipsp_disconnect"				, BT_UNIT_TEST_FUNCTION_IPSP_DISCONNECT},
	{"bt_le_ipsp_start_advertising"					, BT_UNIT_TEST_FUNCTION_IPSP_START_ADVERTISING},
	{"bt_ipsp_set_connection_state_changed_cb"		, BT_UNIT_TEST_FUNCTION_IPSP_SET_CONNECTION_STATE_CHANGED_CB},
	{"bt_ipsp_nsset_connection_state_changed_cb"		, BT_UNIT_TEST_FUNCTION_IPSP_UNSET_CONNECTION_STATE_CHANGED_CB},
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
	{NULL					, 0x0000},
};

tc_table_t tc_HDP[] = {
	/* HDP functions */
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_hdp_register_sink_app"					, BT_UNIT_TEST_FUNCTION_HDP_REGISTER_SINK_APP},
	{"bt_hdp_unregister_sink_app"					, BT_UNIT_TEST_FUNCTION_HDP_UNREGISTER_SINK_APP},
	{"bt_hdp_connect_to_source"						, BT_UNIT_TEST_FUNCTION_HDP_CONNECT_TO_SOURCE},
	{"bt_hdp_disconnect"					, BT_UNIT_TEST_FUNCTION_HDP_DISCONNECT},
	{"bt_hdp_send_data"					, BT_UNIT_TEST_FUNCTION_HDP_SEND_DATA},
	{"bt_hdp_set_connection_state_changed_cb"					, BT_UNIT_TEST_FUNCTION_HDP_SET_CONNECTION_CB},
	{"bt_hdp_unset_connection_state_changed_cb"					, BT_UNIT_TEST_FUNCTION_HDP_UNSET_CONNECTION_CB},
	{"bt_hdp_set_data_received_cb"					, BT_UNIT_TEST_FUNCTION_HDP_SET_DATA_RECEIVED_CB},
	{"bt_hdp_unset_data_received_cb"					, BT_UNIT_TEST_FUNCTION_HDP_UNSET_DATA_RECEIVED_CB},
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
	{NULL					, 0x0000},
};

#ifdef TIZEN_WEARABLE
tc_table_t tc_hf[] = {
	/* Handsfree role functions */
	{"BACK"						, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_hf_initialize"				, BT_UNIT_TEST_FUNCTION_HF_INITIALIZE},
	{"bt_hf_deinitialize"				, BT_UNIT_TEST_FUNCTION_HF_DEINITIALIZE},
	{"(CALL Handling) bt_hf_notify_call_event(answer)"		, BT_UNIT_TEST_FUNCTION_HF_NOTIFY_CALL_ANSWER},
	{"(CALL Handling) bt_hf_notify_call_event(initiate)"		, BT_UNIT_TEST_FUNCTION_HF_NOTIFY_CALL_INIT},
	{"(CALL Handling) bt_hf_notify_call_event(terminate)"		, BT_UNIT_TEST_FUNCTION_HF_NOTIFY_CALL_TERM},
	{"(CALL Status) bt_hf_get_call_status_info_list"		, BT_UNIT_TEST_FUNCTION_HF_GET_CALL_STATUS_INFO_LIST},
	{"(CALL Status) bt_hf_set_call_handling_event_cb()"		, BT_UNIT_TEST_FUNCTION_HF_SET_CALL_EVENT_CB},
	{"(CALL Status) bt_hf_unset_call_handling_event_cb"		, BT_UNIT_TEST_FUNCTION_HF_UNSET_CALL_EVENT_CB},
	{"(CALL Status) bt_hf_set_call_status_updated_event_cb"	, BT_UNIT_TEST_FUNCTION_HF_SET_CALL_STATUS_UPDATED_EVENT_CB},
	{"(CALL Status) bt_hf_unset_call_status_updated_event_cb"	, BT_UNIT_TEST_FUNCTION_HF_UNSET_CALL_STATUS_UPDATED_EVENT_CB},
	{"(Volume) bt_hf_notify_speaker_gain"			, BT_UNIT_TEST_FUNCTION_HF_NOTIFY_SPEAKER_GAIN},
	{"(Volume) bt_hf_set_speaker_gain_changed_cb"		, BT_UNIT_TEST_FUNCTION_HF_SET_SPEAKER_GAIN_CB},
	{"(Volume) bt_hf_unset_speaker_gain_changed_cb"		, BT_UNIT_TEST_FUNCTION_HF_UNSET_SPEAKER_GAIN_CB},
	{"(Voice-recog) bt_hf_notify_voice_recognition_state(true)"	, BT_UNIT_TEST_FUNCTION_HF_NOTIFY_VOICE_REC_TRUE},
	{"(Voice-recog) bt_hf_notify_voice_recognition_state(false)"	, BT_UNIT_TEST_FUNCTION_HF_NOTIFY_VOICE_REC_FALSE},
	{"(SCO) bt_hf_is_sco_opened"				, BT_UNIT_TEST_FUNCTION_HF_IS_SCO_OPENED},
	{"(SCO) bt_hf_get_codec_id"				, BT_UNIT_TEST_FUNCTION_HF_GET_CODEC_ID},
	{"(SCO) bt_hf_set_sco_state_changed_cb()" 		, BT_UNIT_TEST_FUNCTION_HF_SET_SCO_CHANGED_CB},
	{"(SCO) bt_hf_unset_sco_state_changed_cb()"		, BT_UNIT_TEST_FUNCTION_HF_UNSET_SCO_CHANGED_CB},
	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
	{NULL						, 0x0000},
};

tc_table_t tc_pbap_client[] = {
	/* PBAP Functions*/
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"bt_pbap_init()",			BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_INITIALIZE},
	{"bt_pbap_deinit()",			BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_DEINITIALIZE},
	{"bt_pbap_connect()",			BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_CONNECT},
	{"bt_pbap_disconnect()",		BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_DISCONNECT},
	{"bt_pbap_get_phonebook_size()",	BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_GETSIZE},
	{"bt_pbap_get_phonebook()",		BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_PHONEBOOKPULL},
	{"bt_pbap_get_list()",		BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_VCARDLIST},
	{"bt_pbap_pull_vcard()",		BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_VCARDPULL},
	{"bt_pbap_phonebook_search()",		BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_PHONEBOOKSEARCH},
	{"bt_pbap_get_list() Maxlist=0",		BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_VCARDLIST_MAXLIST_ZERO},

	{"Select this menu to set parameters and then select the function again."			, BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS},
	{NULL					, 0x0000},
};
#endif

tc_table_t tc_automated_test[] = {
	/* Automated test Functions*/
	{"BACK"								, BT_UNIT_TEST_FUNCTION_BACK},
	{"On/Off repeat test()"				, BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST},
	{"On/Off repeat(1sec delay) test()" , BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST_1_SEC_DELAY},
	{"On/Off repeat(Nsec delay) test()" , BT_UNIT_TEST_FUNCTION_ON_OFF_REPEAT_TEST_N_SEC_DELAY},
	{NULL					, 0x0000},
};

static void __bt_initialize_all(void);

void __bt_gatt_client_value_changed_cb(bt_gatt_h chr, char *value, int len,
		void *user_data);

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
	case BT_UNIT_TEST_TABLE_HPS:
		tc_table = tc_hps;
		break;
	case BT_UNIT_TEST_TABLE_AVRCP:
		tc_table = tc_avrcp;
		break;
	case BT_UNIT_TEST_TABLE_HID:
		tc_table = tc_hid;
		break;
	case BT_UNIT_TEST_TABLE_IPSP:
		tc_table = tc_ipsp;
		break;
	case BT_UNIT_TEST_TABLE_HDP:
		tc_table = tc_HDP;
		break;
#ifdef TIZEN_WEARABLE
	case BT_UNIT_TEST_TABLE_HF:
		tc_table = tc_hf;
		break;
	case BT_UNIT_TEST_TABLE_PBAP_CLIENT:
		tc_table = tc_pbap_client;
		break;
#endif
	case BT_UNIT_TEST_TABLE_AUTOMATED_TEST:
		if (input_automated_test_delay == true) {
			TC_PRT("Input the delay for automated test.(e.g., 10)\n\n");
			return;
		}
		tc_table = tc_automated_test;
		break;
	case BT_UNIT_TEST_FUNCTION_INITIALIZE_ALL:
		__bt_initialize_all();
		TC_PRT("Key 0 : usage BACK");
		return;
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

static void __bt_free_test_param(tc_params_t *param)
{
	int i = 0;

	for (i = 0; i < param->param_count; i++)
		g_free(param->params[i]);
	g_free(param->params);
	param->params = NULL;
	param->param_count = 0;
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

#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
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

static void __select_index(void *data, const char *str)
{
	GIOChannel *key_io;

	is_sub_index = TRUE;

	if (str)
		TC_PRT("%s ", str);

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
#endif

void __bt_adapter_state_changed_cb(int result, bt_adapter_state_e adapter_state, void *user_data)
{
	TC_PRT("__bt_adapter_state_changed_cb");
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("state: %s",
			(adapter_state == BT_ADAPTER_ENABLED)?
					"ENABLED" : "DISABLED");
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

static void __bt_adapter_manufacturer_data_changed_cb(char *data,
						int len, void *user_data)
{
	int i;

	TC_PRT("manufacturer data is changed");
	for (i = 0; i < len; i++) {
		TC_PRT("[%d] %02x", i, data[i]);
	}
}

static void __bt_adapter_authentication_req_cb(
	int result, bt_authentication_type_info_e auth_type,
	char *device_name, char *remote_addr, char *pass_key, void *user_data)
{
	TC_PRT("__bt_adapter_authentication_req_cb: device name = %s", device_name);
	TC_PRT("__bt_adapter_authentication_req_cb: device address = %s", remote_addr);

	if (auth_type == BT_AUTH_PIN_REQUEST) {
		TC_PRT("Auth Type = BT_AUTH_PIN_REQUEST");
	} else if (auth_type == BT_AUTH_PASSKEY_CONFIRM_REQUEST) {
		TC_PRT("Auth Type = BT_AUTH_PASSKEY_CONFIRM_REQUEST");
		TC_PRT("Passkey: [%s]", pass_key);
	} else if (auth_type == BT_AUTH_KEYBOARD_PASSKEY_DISPLAY) {
		TC_PRT("Auth Type = BT_AUTH_KEYBOARD_PASSKEY_DISPLAY");
		TC_PRT("Passkey: [%s]", pass_key);
	}
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

static void __bt_adapter_le_scan_result_cb(int result,
		bt_adapter_le_device_scan_result_info_s *info, void *user_data)
{
	int i;
	bt_adapter_le_packet_type_e pkt_type = BT_ADAPTER_LE_PACKET_ADVERTISING;

	if (info == NULL) {
		TC_PRT("No discovery_info!");
		return;
	}

	TC_PRT("%s Adv %d Scan resp %d RSSI %d Addr_type %d", info->remote_address,
			info->adv_data_len, info->scan_data_len, info->rssi,
			info->address_type);

	if (info->adv_data_len > 31 || info->scan_data_len > 31) {
		TC_PRT("###################");
		bt_adapter_le_stop_scan();
		TC_PRT("###################");
		return;
	}

	for (i = 0; i < 2; i++) {
		char **uuids;
		char *device_name;
		int tx_power_level;
		bt_adapter_le_service_data_s *data_list;
		int appearance;
		int manufacturer_id;
		char *manufacturer_data;
		int manufacturer_data_len;
		int count;

		pkt_type += i;
		if (pkt_type == BT_ADAPTER_LE_PACKET_ADVERTISING && info->adv_data == NULL) continue;
		if (pkt_type == BT_ADAPTER_LE_PACKET_SCAN_RESPONSE && info->scan_data == NULL) break;

		if (bt_adapter_le_get_scan_result_service_uuids(info, pkt_type, &uuids, &count) == BT_ERROR_NONE) {
			int i;
			for (i = 0; i < count; i++) {
				TC_PRT("UUID[%d] = %s", i + 1, uuids[i]);
				g_free(uuids[i]);
			}
			g_free(uuids);
		}
		if (bt_adapter_le_get_scan_result_device_name(info, pkt_type, &device_name) == BT_ERROR_NONE) {
			TC_PRT("Device name = %s", device_name);
			g_free(device_name);
		}
		if (bt_adapter_le_get_scan_result_tx_power_level(info, pkt_type, &tx_power_level) == BT_ERROR_NONE) {
			TC_PRT("TX Power level = %d", tx_power_level);
		}
		if (bt_adapter_le_get_scan_result_service_solicitation_uuids(info, pkt_type, &uuids, &count) == BT_ERROR_NONE) {
			int i;
			for (i = 0; i < count; i++) {
				TC_PRT("Solicitation UUID[%d] = %s", i + 1, uuids[i]);
				g_free(uuids[i]);
			}
			g_free(uuids);
		}
		if (bt_adapter_le_get_scan_result_service_data_list(info, pkt_type, &data_list, &count) == BT_ERROR_NONE) {
			int i;
			for (i = 0; i < count; i++)
				TC_PRT("Service Data[%d] = [0x%2.2X%2.2X:0x%.2X...]", i + 1, data_list[i].service_uuid[0], data_list[i].service_uuid[1], data_list[i].service_data[0]);
			bt_adapter_le_free_service_data_list(data_list, count);
		}
		if (bt_adapter_le_get_scan_result_appearance(info, pkt_type, &appearance) == BT_ERROR_NONE) {
			TC_PRT("Appearance = %d", appearance);
		}
		if (bt_adapter_le_get_scan_result_manufacturer_data(info, pkt_type, &manufacturer_id, &manufacturer_data, &manufacturer_data_len) == BT_ERROR_NONE) {
				TC_PRT("Manufacturer data[ID:%.4X, 0x%.2X%.2X...(len:%d)]", manufacturer_id, manufacturer_data[0], manufacturer_data[1], manufacturer_data_len);
			g_free(manufacturer_data);
		}
	}
}

void __bt_gatt_server_read_value_requested_cb (char *remote_address,
				int request_id, bt_gatt_server_h server, bt_gatt_h gatt_handle,
				int offset, void *user_data)
{
	char char_value_1[3] = {0, 1, 2};

	TC_PRT("__bt_gatt_server_read_value_requested_cb");
	TC_PRT("remote_address %s", remote_address);
	TC_PRT("req_id %d", request_id);
	TC_PRT("server %s", (char *)server);
	TC_PRT("gatt_handle %s", (char *)gatt_handle);
	TC_PRT("Offset %d", offset);
	/* Get the attribute new values here */
	bt_gatt_server_send_response(request_id, offset, char_value_1, 3 - offset);
}

void __bt_gatt_server_notification_state_change_cb (bool notify,
			bt_gatt_server_h server, bt_gatt_h gatt_handle, void *user_data)
{
	TC_PRT("__bt_gatt_server_notification_state_change_cb");
	TC_PRT("notify %d", notify);
	TC_PRT("server %s", (char *)server);
	TC_PRT("gatt_handle %s", (char *)gatt_handle);
}


#ifndef TIZEN_WEARABLE
static void __bt_adapter_le_device_discovery_state_changed_cb(int result,
				bt_adapter_le_device_discovery_state_e discovery_state,
				bt_adapter_le_device_discovery_info_s *discovery_info,
				void *user_data)
{
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
		int i, ret;
		int field_len = 0;
		unsigned char *field_data;
		
		printf("Adv data [");
		for (i = 0; i < discovery_info->adv_data_len; i++)
			printf("%02x ", discovery_info->adv_data[i]);
		printf("]\n");

		printf("Scan resp data [");
		for (i = 0; i < discovery_info->scan_data_len; i++)
			printf("%02x ", discovery_info->scan_data[i]);
		printf("]\n");

		ret = bt_adapter_le_get_scan_field_data(discovery_info, BT_ADAPTER_LE_PACKET_ADVERTISING,
				BT_ADAPTER_LE_PACKET_DATA_MANUFACTURER_SPECIFIC_DATA, &field_data, &field_len);
		if (ret == BT_ERROR_NONE) {
			printf("Manufacturer specific data [");
			for (i = 0; i < field_len; i++)
				printf("%02x ", field_data[i]);
			printf("]\n");
			g_free(field_data);
		}

		ret = bt_adapter_le_get_scan_field_data(discovery_info, BT_ADAPTER_LE_PACKET_SCAN_RESPONSE,
				BT_ADAPTER_LE_PACKET_DATA_LOCAL_NAME, &field_data, &field_len);
		if (ret == BT_ERROR_NONE) {
			TC_PRT("%s", field_data);
			g_free(field_data);
		}		
#endif
	}
}
#endif

static void __bt_adapter_le_advertising_state_changed_cb(int result,
		bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e adv_state, void *user_data)
{
	TC_PRT("Result : %d", result);
	TC_PRT("Advertiser : %p", advertiser);
	TC_PRT("Advertising %s [%d]", adv_state == BT_ADAPTER_LE_ADVERTISING_STARTED ?
				"started" : "stopped", adv_state);
}

static void __bt_adapter_le_advertising_state_changed_cb_2(int result,
		bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e adv_state, void *user_data)
{
	TC_PRT("Result : %d", result);
	TC_PRT("Advertiser : %p", advertiser);
	TC_PRT("Advertising %s [%d]", adv_state == BT_ADAPTER_LE_ADVERTISING_STARTED ?
				"started" : "stopped", adv_state);
}

static void __bt_adapter_le_advertising_state_changed_cb_3(int result,
		bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e adv_state, void *user_data)
{
	TC_PRT("Result : %d", result);
	TC_PRT("Advertiser : %p", advertiser);
	TC_PRT("Advertising %s [%d]", adv_state == BT_ADAPTER_LE_ADVERTISING_STARTED ?
				"started" : "stopped", adv_state);
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
	if (strcmp(spp_uuid, connection->service_uuid) == 0) {
		TC_PRT("service_uuid: %s", spp_uuid);
		client_fd = connection->socket_fd;
	} else {
		TC_PRT("service_uuid: %s", connection->service_uuid);
		custom_client_fd = connection->socket_fd;
	}
}

static void __bt_adapter_le_state_changed_cb(int result, bt_adapter_le_state_e adapter_le_state, void *user_data)
{
	TC_PRT("__bt_adapter_le_state_changed_cb");
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("state: %s",
			(adapter_le_state == BT_ADAPTER_LE_ENABLED)?
					"ENABLED" : "DISABLED");
}

static void __bt_le_set_data_length_changed_cb(int result, const char *remote_address, int max_tx_octets,
		int max_tx_time, int max_rx_octets, int max_rx_time,void *user_data)
{
	TC_PRT("__bt_le_set_data_length_changed_cb \n");
	TC_PRT("max_tx_octets: %d  max_tx_time: %d  max_rx_octets: %d  max_rx_time: %d",
			max_tx_octets, max_tx_time, max_rx_octets, max_rx_time);
	TC_PRT("result: %s", __bt_get_error_message(result));
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
		TC_PRT("Callback: A bond is created.");
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
		TC_PRT("Callback: Creating a bond is failed.");
		TC_PRT("result: %s", __bt_get_error_message(result));
	}
}

void __bt_device_bond_destroyed_cb(int result, char *remote_address, void *user_data)
{
        if(result == BT_ERROR_NONE)
        {
                TC_PRT("Callback: A bond is destroyed.");
                TC_PRT("Callback: Address - %s.", remote_address);
        } else {
                TC_PRT("Callback: Destroying a bond is failed.");
                TC_PRT("result: %s", __bt_get_error_message(result));
        }
}

void __bt_print_repeat_test_summary(void)
{
	static struct timeval current_time = {0,};
	static struct timeval diff_time = {0,};
	int time_diff;

	gettimeofday(&current_time, NULL);

	diff_time.tv_sec = current_time.tv_sec - check_time.tv_sec;
	diff_time.tv_usec = current_time.tv_usec - check_time.tv_usec;
	if(diff_time.tv_usec < 0)
	{
		diff_time.tv_sec = diff_time.tv_sec - 1;
		diff_time.tv_usec = diff_time.tv_usec + 1000000;
	}
	time_diff = (diff_time.tv_sec * 1000);
	time_diff += diff_time.tv_usec / 1000;

	TC_PRT("try: %d, success: %d, fail: %d, time(%d msec)\n",
		bt_onoff_cnt/2, bt_onoff_cnt_success/2, bt_onoff_cnt_fail,
		time_diff);

	total_time += time_diff;
}

void __bt_print_repeat_test_final_summary()
{
	TC_PRT("try: %d, success: %d, fail: %d, Total_time(%d msec), Average_time(%d msec)\n",
		bt_onoff_cnt/2, bt_onoff_cnt_success/2, bt_onoff_cnt_fail,
		total_time, total_time / bt_onoff_cnt);
}

void __bt_gatt_connection_state_changed_cb(int result, bool connected, const char *remote_address, void *user_data)
{
	TC_PRT("result: %s", __bt_get_error_message(result));
	if (connected) {
		TC_PRT("LE connected(address = %s)", remote_address);
		/* Fix : STRING_OVERFLOW */
		strncpy(remote_addr, remote_address, strlen(remote_addr));
	} else {
		TC_PRT("LE DISconnected");
	}
}

void __bt_hps_connection_state_changed_cb(int result, bool connected, const char *remote_address, void *user_data)
{
	int ret;
	TC_PRT("result: %s", __bt_get_error_message(result));
	if (connected) {
		TC_PRT("HPS connected(address = %s)", remote_address);
		/* Fix : STRING_OVERFLOW */
		strncpy(remote_addr, remote_address, strlen(remote_addr));
		if (hps_client) {
			   bt_gatt_client_destroy(hps_client);
			   hps_client = NULL;
		}
		ret = bt_gatt_client_create(remote_addr, &hps_client);
		TC_PRT("returns bt_gatt_client_create %s\n", __bt_get_error_message(ret));

	} else {
		TC_PRT("HPS DISconnected");
	}
}

int __bt_gatt_client_set_value(char *type, char *value, bt_gatt_h h)
{
	int ret;
	int s_val;
	unsigned int u_val;
	char *buf;
	int len;

	if (strcasecmp(type, "int8") == 0) {
		s_val = atoi(value);
		buf = (char *)&s_val;
		len = 1;
	} else if (strcasecmp(type, "int16") == 0) {
		s_val = atoi(value);
		buf = (char *)&s_val;
		len = 2;
	} else if (strcasecmp(type, "int32") == 0) {
		s_val = atoi(value);
		buf = (char *)&s_val;
		len = 4;
	} else if (strcasecmp(type, "uint8") == 0) {
		u_val = strtoul(value, NULL, 10);
		buf = (char *)&u_val;
		len = 1;
	} else if (strcasecmp(type, "uint16") == 0) {
		u_val = strtoul(value, NULL, 10);
		buf = (char *)&u_val;
		len = 2;
	} else if (strcasecmp(type, "uint32") == 0) {
		u_val = strtoul(value, NULL, 10);
		buf = (char *)&u_val;
		len = 4;
	} else if (strcasecmp(type, "str") == 0) {
		buf = value;
		len = strlen(buf);
	} else
		return BT_ERROR_INVALID_PARAMETER;

	ret = bt_gatt_set_value(h, buf, len);
	if (ret != BT_ERROR_NONE)
		TC_PRT("bt_gatt_set_value is failed : %d", ret);

	return ret;
}

void __bt_gatt_client_print_value(bt_gatt_h h)
{
	int ret;
	char *str = NULL;
	char *value = NULL;
	int len = 0;
	bool hex_format = false;
	int i;

	ret = bt_gatt_get_value(h, &value, &len);
	if (ret != BT_ERROR_NONE) {
		TC_PRT("bt_gatt_get_value is failed : %d", ret);
		return;
	}

	for (i = 0; i < len; i++)
		if (isalnum(value[i]) == 0) {
			hex_format = true;
			break;
		}

	if (hex_format) {
		str = g_malloc0(len * 5 + 1);
		/* Fix : NULL_RETURNS */
		if (!str) {
			TC_PRT("BT_ERROR_OUT_OF_MEMORY");
			goto fail;
		}
		for (i = 0; i < len; i++) {
			if (i > 0)
				str[i * 5 - 1] = ' ';

			snprintf(&str[i * 5], 5, "0x%02X", value[i]);
		}
	} else {
		str = g_malloc0(len + 1);
		/* Fix : NULL_RETURNS */
		if (!str) {
			TC_PRT("BT_ERROR_OUT_OF_MEMORY");
			goto fail;
		}
		memcpy(str, value, len);
	}

	TC_PRT("Value : %s", str);

	g_free(str);
fail:
	g_free(value);
}

bool __bt_gatt_print_info_cb(int total, int index, bt_gatt_h gatt_handle, void *user_data)
{
	char *uuid = NULL;
	char *str = NULL;

	bt_gatt_get_uuid(gatt_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	TC_PRT("[%d / %d] %s (%s)", index, total, str ? str : "Unknown", uuid);

	g_free(str);
	g_free(uuid);

	return true;
}

void __bt_gatt_client_write_complete_cb(int result, bt_gatt_h gatt_handle, void *data)
{
	char *uuid = NULL;
	char *str = NULL;

	bt_gatt_get_uuid(gatt_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	TC_PRT("Write %s for %s (%s)", result == BT_ERROR_NONE ? "Success" : "Fail", str ? str : "Unknown", uuid);

	g_free(str);
	g_free(uuid);

	return;
}

void __bt_gatt_client_read_complete_cb(int result, bt_gatt_h gatt_handle, void *data)
{
	char *uuid = NULL;
	char *str = NULL;

	bt_gatt_get_uuid(gatt_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	TC_PRT("Read %s for %s (%s)", result == BT_ERROR_NONE ? "Success" : "Fail", str ? str : "Unknown", uuid);

	g_free(str);
	g_free(uuid);

	if (result != BT_ERROR_NONE)
		return;

	__bt_gatt_client_print_value(gatt_handle);

	return;
}

bool __bt_gatt_client_foreach_desc_cb(int total, int index, bt_gatt_h desc_handle, void *data)
{
	char *uuid = NULL;
	char *str = NULL;

	bt_gatt_get_uuid(desc_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	TC_PRT("\t\t[%d / %d] %s (%s)", index, total, str ? str : "Unknown", uuid);
	__bt_gatt_client_print_value(desc_handle);

	g_free(str);
	g_free(uuid);

	return true;
}

bool __bt_gatt_client_foreach_chr_cb(int total, int index, bt_gatt_h chr_handle, void *data)
{
	int ret;
	char *uuid = NULL;
	char *str = NULL;

	bt_gatt_get_uuid(chr_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	TC_PRT("\t[%d / %d] %s (%s)", index, total, str ? str : "Unknown", uuid);
	__bt_gatt_client_print_value(chr_handle);

	g_free(str);
	g_free(uuid);

	ret = bt_gatt_characteristic_foreach_descriptors(chr_handle,
			__bt_gatt_client_foreach_desc_cb, NULL);
	if (ret != BT_ERROR_NONE)
		TC_PRT("bt_gatt_characteristic_foreach_descriptors is failed : %d", ret);

	return true;
}

bool __bt_gatt_client_foreach_svc_cb(int total, int index, bt_gatt_h svc_handle, void *data)
{
	int test_id = (int)data;
	int ret;
	char *uuid = NULL;
	char *str = NULL;

	bt_gatt_get_uuid(svc_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	TC_PRT("[%d / %d] %s (%s)", index, total, str ? str : "Unknown", uuid);

	g_free(str);
	g_free(uuid);

	if (test_id == BT_UNIT_TEST_FUNCTION_GATT_CLIENT_PRINT_ALL) {
		ret = bt_gatt_service_foreach_characteristics(svc_handle,
				__bt_gatt_client_foreach_chr_cb, (void *)test_id);
		if (ret != BT_ERROR_NONE)
			TC_PRT("bt_gatt_service_foreach_characteristics is failed : %d", ret);
	}

	return true;
}

bool __bt_hps_client_svc_cb(int total, int index, bt_gatt_h svc_handle, void *data)
{
	int test_id = (int)data;
	int ret;
	char *uuid = NULL;
	char *str = NULL;

	bt_gatt_get_uuid(svc_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	if (!g_strcmp0(uuid, HPS_UUID)) {

		TC_PRT("[%d / %d] %s (%s)", index, total, str ? str : "Unknown", uuid);

		g_free(str);
		g_free(uuid);

		if (test_id == BT_UNIT_TEST_FUNCTION_HPS_CLIENT_PRINT_ALL) {
			ret = bt_gatt_service_foreach_characteristics(svc_handle,
					__bt_gatt_client_foreach_chr_cb, (void *)test_id);
			if (ret != BT_ERROR_NONE)
				TC_PRT("bt_gatt_service_foreach_characteristics is failed : %d", ret);
		}
	}
	return true;
}

static void __ancs_write_completed_cb(int result, bt_gatt_h request_handle,
		void *user_data)
{
	if (result != BT_ERROR_NONE) {
		TC_PRT("write request is failed : %d", result);
		return;
	}

	TC_PRT("write request is done");

	return;
}

static int __ancs_write_value_to_control_point(bt_gatt_client_h client, char *value, int len)
{
	bt_gatt_h ancs_svc = NULL;
	bt_gatt_h control_point = NULL;
	char *svc_uuid = "7905F431-B5CE-4E99-A40F-4B1E122D00D0";
	char *chr_uuid = "69D1D8F3-45E1-49A8-9821-9BBDFDAAD9D9";
	int ret;

	ret = bt_gatt_client_get_service(client, svc_uuid, &ancs_svc);
	if (ret != BT_ERROR_NONE) {
		TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
		return ret;
	}

	ret = bt_gatt_service_get_characteristic(ancs_svc, chr_uuid, &control_point);
	if (ret != BT_ERROR_NONE) {
		TC_PRT("bt_gatt_service_get_characteristic is failed");
		return ret;
	}

	ret = bt_gatt_characteristic_set_write_type(control_point,
			BT_GATT_WRITE_TYPE_WRITE);
	if (ret != BT_ERROR_NONE) {
		TC_PRT("bt_gatt_characteristic_set_write_type is failed : %d", ret);
		return ret;
	}

	ret = bt_gatt_set_value(control_point, value, len);
	if (ret != BT_ERROR_NONE) {
		TC_PRT("bt_gatt_set_value is failed : %d", ret);
		return ret;
	}

	ret = bt_gatt_client_write_value(control_point,
			__ancs_write_completed_cb, NULL);
	if (ret != BT_ERROR_NONE) {
		TC_PRT("bt_gatt_client_write_value is failed : %d", ret);
		return ret;
	}

	return ret;
}

void __bt_gatt_client_value_changed_cb(bt_gatt_h chr, char *value, int len,
		void *user_data)
{
	char *uuid = NULL;
	int i;

	bt_gatt_get_uuid(chr, &uuid);

	TC_PRT("Value changed for [%s]", uuid);
	TC_PRT("len [%d]", len);
        for (i = 0; i < len; i++) {
                TC_PRT("value %u", value[i]);
        }

	g_free(uuid);

	return;
}

void __bt_HP_client_cp_req_status_changed_cb(bt_gatt_h chr, char *value, int len,
		void *user_data)
{
	char *uuid = NULL;
	int i;

	bt_gatt_get_uuid(chr, &uuid);

	TC_PRT("Value changed for [%s]", uuid);
	TC_PRT("len [%d]", len);
		for (i = 0; i < len; i++) {
				TC_PRT("value %u", value[i]);
		}

	g_free(uuid);

	return;
}

void __bt_gatt_server_notification_sent_cb(int result,
		char *remote_address, bt_gatt_server_h server,
		bt_gatt_h characteristic, bool completed, void *user_data)
{
	TC_PRT("result [%s]", __bt_get_error_message(result));
	TC_PRT("remote_address : %s", remote_address);
	TC_PRT("completed : %d", completed);
	TC_PRT("characteristic : %p", characteristic);
}

void __bt_gatt_server_value_changed_cb(char *remote_address,
				bt_gatt_server_h server, bt_gatt_h gatt_handle,
				int offset, char *value, int len,
				void *user_data)
{
	int i;
	TC_PRT("remote_address : %s", remote_address);
	TC_PRT("offset : %d", offset);
	TC_PRT("len [%d] : ", len);
	for (i = 0; i < len; i++) {
		printf("%d ", value[i]);
	}
	printf("\n");
}

#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
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

void __bt_gatt_characteristic_descriptor_discovered_cb(int result,
		unsigned char characteristic_format, int total,
		bt_gatt_attribute_h characteristic_descriptor, void *user_data) {
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("total %d", total);
	TC_PRT("characteristic_format 0x%02X", characteristic_format);
	TC_PRT("characteristic_descriptor %s", (char *)characteristic_descriptor);
}

bool __bt_gatt_included_service_cb(bt_gatt_attribute_h service, void *user_data)
{
	char *uuid = NULL;
	char *name = NULL;
	unsigned char *value = NULL;
	int value_length = 0;

	TC_PRT("__bt_gatt_included_service_cb");
	TC_PRT("service %s", (char *)service);

	if (char_service_index >= MAX_SERVICES)
		return true;

	bt_gatt_clone_attribute_handle(&characteristics_services[char_service_index], service);
	char_service_index++;

	bt_gatt_get_characteristic_declaration(service, &uuid, &value, &value_length);
	bt_gatt_get_uuid_specification_name(uuid, &name);
	TC_PRT("uuid: %s [%s]", uuid, name);

	g_free(uuid);
	g_free(name);
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

void __bt_gatt_remote_characteristic_write_cb(bt_gatt_attribute_h characteristic,
			unsigned char *char_value,
			int char_length, void *user_data)
{
	TC_PRT("remote device updated the characteristics in db");

	TC_PRT("updated characteristic is %s", (char *)characteristic);
	TC_PRT("new char value is %s", char_value);
}
#endif

void __bt_avrcp_target_connection_state_changed_cb(bool connected, const char *remote_address, void *user_data)
{
	TC_PRT("__bt_avrcp_target_connection_state_changed_cb");
	TC_PRT("connected %d", connected);
	TC_PRT("remote_address %s", remote_address);
}

void __bt_avrcp_equalizer_state_changed_cb(bt_avrcp_equalizer_state_e equalizer, void *user_data)
{
	TC_PRT("__bt_avrcp_equalizer_state_changed_cb");
	TC_PRT("equalizer %d (1: OFF, 2: ON)", equalizer);
}

void __bt_avrcp_repeat_mode_changed_cb(bt_avrcp_repeat_mode_e repeat, void *user_data)
{
	TC_PRT("__bt_avrcp_repeat_mode_changed_cb");
	TC_PRT("repeat %d (1: OFF, 2: SINGLE, 3: All, 4:GROUP)", repeat);
}

void __bt_avrcp_shuffle_mode_changed_cb(bt_avrcp_shuffle_mode_e shuffle, void *user_data)
{
	TC_PRT("__bt_avrcp_shuffle_mode_changed_cb");
	TC_PRT("shuffle -> %d (1: OFF, 2: All TRACK, 3:GROUP)", shuffle);
}

void __bt_avrcp_scan_mode_changed_cb(bt_avrcp_scan_mode_e scan, void *user_data)
{
	TC_PRT("__bt_avrcp_scan_mode_changed_cb");
	TC_PRT("scan -> %d", scan);
}

void __bt_avrcp_control_connection_state_changed_cb(bool connected, const char *remote_address, void *user_data)
{
	TC_PRT("__bt_avrcp_target_connection_state_changed_cb");
	TC_PRT("connected %d", connected);
	TC_PRT("remote_address %s", remote_address);
}

void __bt_avrcp_song_position_changed_cb(unsigned int postion, void *user_data)
{
	TC_PRT("__bt_avrcp_song_position_changed_cb");
	TC_PRT("Position %d", postion);
}

void __bt_avrcp_player_state_changed_cb(bt_avrcp_player_state_e state, void *user_data)
{
	TC_PRT("__bt_avrcp_player_state_changed_cb");
	TC_PRT("Player state -> %d (0: stopped, 1: Playing, 2:Paused, 3:FF, 4:REW)", state);
}

void __bt_avrcp_track_info_changed_cb(bt_avrcp_metadata_attributes_info_s *metadata, void *user_data)
{
	TC_PRT("__bt_avrcp_track_info_changed_cb");
	TC_PRT("title %s", metadata->title);
	TC_PRT("artist %s", metadata->artist);
	TC_PRT("album %s", metadata->album);
	TC_PRT("genre %s", metadata->genre);
	TC_PRT("total_tracks %d", metadata->total_tracks);
	TC_PRT("number %d", metadata->number);
	TC_PRT("duration %d", metadata->duration);
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

void __bt_hdp_connected_cb(int result, const char *remote_address, const char *app_id,
    bt_hdp_channel_type_e type, unsigned int channel, void *user_data)
{
	TC_PRT("__bt_hdp_connected_cb: called");
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("remote_address: %s", remote_address);
	TC_PRT("app_id: %s", app_id);
	TC_PRT("type: %x", type);
	TC_PRT("channel: %d", channel);

	channel_hdp = channel;
}

void __bt_hdp_disconnected_cb(int result, const char *remote_address, unsigned int channel, void *user_data)
{
	TC_PRT("__bt_hdp_connected_cb: called");
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("remote_address: %s", remote_address);
	TC_PRT("channel: %d", channel);

	channel_hdp = channel;
}

void __bt_hdp_data_received_cb(unsigned int channel, const char *data, unsigned int size, void *user_data)
{
	TC_PRT("__bt_hdp_data_received_cb: called");
	TC_PRT("data: %s", data);
	TC_PRT("size: %d", size);
	TC_PRT("channel: %d", channel);
}

void __bt_le_ipsp_init_state_changed_cb(int result,
		bool ipsp_initialized, void *user_data)
{
	TC_PRT("result: %s", __bt_get_error_message(result));
	if (ipsp_initialized) {
		TC_PRT("IPSP Init state: INITIALIZED");
	} else {
		TC_PRT("IPSP Init state: UN-INITIALIZED");
		_bt_unset_cb(BT_EVENT_IPSP_INIT_STATE_CHANGED);
	}
}

void __bt_le_ipsp_connection_state_changed_cb(int result,
		bool connected, const char *remote_address, void *user_data)
{
	TC_PRT("__bt_le_ipsp_connection_state_changed_cb: called");
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("Connected: %d", connected);
}

#ifdef TIZEN_WEARABLE
void __bt_hf_sco_state_changed_cb(int result, bool opened, void *user_data)
{
	TC_PRT("opened [%s]", opened ? "YES" : "NO");
}

void __bt_hf_set_call_handling_event_cb(bt_hf_call_handling_event_e event,
		char *phone_number, void *user_data)
{
	switch(event) {
	case BT_HF_CALL_HANDLING_EVENT_ANSWER:
		TC_PRT("[call_handling_event_cb] event [ANSWER]");
		break;
	case BT_HF_CALL_HANDLING_EVENT_RELEASE:
		TC_PRT("[call_handling_event_cb] event [RELEASE]");
		break;
	case BT_HF_CALL_HANDLING_EVENT_REJECT:
		TC_PRT("[call_handling_event_cb] event [REJECT]");
		break;
	case BT_HF_CALL_HANDLING_EVENT_RING:
		TC_PRT("[call_handling_event_cb] event [RING]");
		break;
	case BT_HF_CALL_HANDLING_EVENT_CALL_STARTED:
		TC_PRT("[call_handling_event_cb] event [CALL_STARTED]");
		break;
	case BT_HF_CALL_HANDLING_EVENT_CALL_ENDED:
		TC_PRT("[call_handling_event_cb] event [CALL_ENDED]");
		break;
	case BT_HF_CALL_HANDLING_EVENT_VOICE_RECOGNITION_ENABLED:
		TC_PRT("[call_handling_event_cb] event [VOICE_RECOGNITION_ENABLED]");
		break;
	case BT_HF_CALL_HANDLING_EVENT_VOICE_RECOGNITION_DISABLED:
		TC_PRT("[call_handling_event_cb] event [VOICE_RECOGNITION_DISABLED]");
		break;
	case BT_HF_CALL_HANDLING_EVENT_WAITING:
		TC_PRT("[call_handling_event_cb] event [WAITING]");
		break;
	case BT_HF_CALL_HANDLING_EVENT_HELD:
		TC_PRT("[call_handling_event_cb] event [HELD]");
		break;
	case BT_HF_CALL_HANDLING_EVENT_UNHELD:
		TC_PRT("[call_handling_event_cb] event [UNHELD]");
		break;
	case BT_HF_CALL_HANDLING_EVENT_SWAPPED:
		TC_PRT("[call_handling_event_cb] event [SWAPPED]");
		break;
	default:
		TC_PRT("[call_handling_event_cb] event [UNKNOWN]");
	}

	TC_PRT("[call_handling_event_cb] phone_number [%s]", phone_number);
}

void __bt_hf_call_status_updated_event_cb(GSList *call_info_list, void *user_data)
{
	TC_PRT("[call_status_updated_event_cb]");
	for (; call_info_list; call_info_list = g_slist_next(call_info_list)) {
		bt_hf_call_status_info_s *call_info = call_info_list->data;
		TC_PRT("    > Call info [No:%s, Dir:%d, Stat:%d, Mpt:%d, Idx:%d]",
			call_info->number, call_info->direction, call_info->status, call_info->multi_party, call_info->index);
	}

	bt_hf_free_call_status_info_list(call_info_list);
}

void __bt_hf_speaker_gain_changed_cb(int gain, void *user_data)
{
	TC_PRT("__bt_hf_speaker_gain_changed_cb");
	TC_PRT("Gain= %d", gain);
}

void __bt_pbap_enabled_cb(const char *remote_address,
		int pbap_enabled, void *user_data)
{
	TC_PRT("__bt_pbap_enabled_cb");
	TC_PRT("Remote Device: %s", remote_address);
	TC_PRT("Connected Status: %d", pbap_enabled);
}

void __bt_pbap_phonebook_size_cb(const char *remote_address,
		int size, void *user_data)
{
	TC_PRT("__bt_pbap_phonebook_size_cb");
	TC_PRT("Remote Device: %s", remote_address);
	TC_PRT("Phonebook Size: %d", size);
}

void __bt_pbap_phonebook_pull_cb(const char *remote_address,
		char *vcf_file, int status, void *user_data)
{
	TC_PRT("__bt_pbap_phonebook_pull_cb");
	TC_PRT("Remote Device: %s", remote_address);
	TC_PRT("Phonebook Download File: %s", vcf_file);
	TC_PRT("Phonebook Download Status: %s", status? "Successful" : "Unsuccessful");
}

void __bt_pbap_vcard_list_cb(const char *remote_address,
		char **vcards, int count, int status, void *user_data)
{
	TC_PRT("__bt_pbap_vcard_list_cb");
	TC_PRT("Remote Device: %s", remote_address);
	TC_PRT("vCard List: %s", vcards[0]);
	TC_PRT("vCard Count: %d", count);
	TC_PRT("Listing Status: %s", status? "Successful" : "Unsuccessful");
}
#endif

void __bt_repeat_test_adapter_state_changed_cb(int result, bt_adapter_state_e adapter_state, void *user_data)
{
	unsigned int delay = *(unsigned int*)user_data;

	TC_PRT("__bt_repeat_test_adapter_state_changed_cb");
	TC_PRT("result: %s", __bt_get_error_message(result));
	TC_PRT("state: %s, delay(%dsec)",
			(adapter_state == BT_ADAPTER_ENABLED)?
					"ENABLED" : "DISABLED", delay);

	__bt_print_repeat_test_summary();

	if (result != BT_ERROR_NONE) {
		char *argv[] = {NULL};

		TC_PRT("make dump\n");
		execv("all_log_dump.sh", argv);

		bt_onoff_cnt_fail++;
	} else {
		bt_onoff_cnt_success++;

		if (delay)
			sleep(delay);
	}

	gettimeofday(&check_time, NULL);
	if (adapter_state == BT_ADAPTER_DISABLED)
		bt_adapter_enable();
	else
		bt_adapter_disable();

	bt_onoff_cnt++;
}

static void __bt_initialize_all(void)
{
	int ret;
	TC_PRT("Initialize and set callback functions");

	ret = bt_initialize();
	if (ret != BT_ERROR_NONE)
		TC_PRT("returns %s\n", __bt_get_error_message(ret));
	ret = bt_adapter_set_state_changed_cb(__bt_adapter_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		TC_PRT("returns %s\n", __bt_get_error_message(ret));
	ret = bt_adapter_set_connectable_changed_cb(__bt_adapter_connectable_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		TC_PRT("returns %s\n", __bt_get_error_message(ret));
	ret = bt_device_set_bond_created_cb(__bt_device_bond_created_cb, NULL);
	if (ret != BT_ERROR_NONE)
		TC_PRT("returns %s\n", __bt_get_error_message(ret));
	ret = bt_device_set_bond_destroyed_cb(__bt_device_bond_destroyed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		TC_PRT("returns %s\n", __bt_get_error_message(ret));
	ret = bt_gatt_set_connection_state_changed_cb(__bt_gatt_connection_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		TC_PRT("returns %s\n", __bt_get_error_message(ret));
	ret = bt_adapter_set_device_discovery_state_changed_cb(__bt_adapter_device_discovery_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		TC_PRT("returns %s\n", __bt_get_error_message(ret));
#ifndef TIZEN_WEARABLE
	ret = bt_adapter_le_set_device_discovery_state_changed_cb(__bt_adapter_le_device_discovery_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		TC_PRT("returns %s\n", __bt_get_error_message(ret));
#endif
	ret = bt_socket_set_connection_state_changed_cb(__bt_socket_connection_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		TC_PRT("returns %s\n", __bt_get_error_message(ret));

	ret = bt_adapter_le_set_state_changed_cb(__bt_adapter_le_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		TC_PRT("returns %s\n", __bt_get_error_message(ret));
	return;
}

int test_set_params(int test_id, char *param)
{
	static int param_index = 0;
	const char *param_type = NULL;

	switch (current_tc_table) {
	case BT_UNIT_TEST_TABLE_ADAPTER: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_ADAPTER_SET_CONNECTABLE: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_BOOL;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_SET_MANUFACTURER_DATA:
		{
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_BYTE_ARRAY;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}

		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}

	case BT_UNIT_TEST_TABLE_ADAPTER_LE: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_SCAN_MODE:
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ADD_ADVERTISING_DATA:
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_MODE:
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_FILTER_POLICY:
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_CONNECTABLE:
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_ADVERTISING: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			switch (test_id) {
			case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_SCAN_MODE:
				TC_PRT("Scan mode\n (0 : Balanced (5000/2000 ms), 1: Low Latency (5000/5000 ms), 2 : Low Energy (5000/500 ms)");
				break;
			case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ADD_ADVERTISING_DATA:
				TC_PRT("Select advertising data \n (0 : Service uuid, 1: Service solicitation 2 : Appearance & Tx power, 3 : All, 4 : ANCS");
				TC_PRT("\t (Default scan response : Service Data &Local name & Manufacturer data)");
				break;
			case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_MODE:
				TC_PRT("Input mode \n (0 : Balanced(500 ms), 1 : Low Latency(150 ms), 2 : Low Energy (1000 ms))");
				break;
			case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_FILTER_POLICY:
				TC_PRT("Input filter_policy \n (0 : No use White List, 1 : Allow scan, 2 : Allow conneciton, 3 : Allow scan & conneciton)");
				break;
			case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_CONNECTABLE:
				TC_PRT("Input adv type \n (0 : Non-connectable (ADV_SCAN_IND), 1 : Connectable (ADV_IND) ");
				break;
			case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_ADVERTISING:
				TC_PRT("Input adv slot id \n (Default is 0, Maximum is 2) ");
				break;
			}
			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ENABLE_PRIVACY: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_BOOL;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}

		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}

	case BT_UNIT_TEST_TABLE_DEVICE: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_DEVICE_IS_PROFILE_CONNECTED: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_INT;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}

		case BT_UNIT_TEST_FUNCTION_DEVICE_CREATE_BOND_BY_TYPE : {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_INT;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}

		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_SOCKET: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_SOCKET_SEND_DATA: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_STRING;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}
		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_OPP: {
		switch (test_id) {
		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_AUDIO: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_AUDIO_CONNECT: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_INT;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_AUDIO_DISCONNECT: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_INT;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}

		case BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_DIALING: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_STRING;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_AG_CALL_EVENT_INCOMING: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_STRING;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_CALL_LIST_ADD: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_STRING;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}

		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_PAN: {
		switch (test_id) {
		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_GATT: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_GATT_CONNECT: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_BOOL;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_READ_VALUE: {
			if (param_index == 0) {
				g_test_param.param_count = 3;
				g_test_param.params = g_malloc0(sizeof(char *) * g_test_param.param_count);
			}

			if (param_index > 0) {
				int len = strlen(param);
				g_test_param.params[param_index - 1] = g_malloc0(len + 1);
				/* Remove new line character */
				param[len - 1] = '\0';
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			switch (param_index) {
			case 0:
				TC_PRT("Input Service UUID");
				break;
			case 1:
				TC_PRT("Input Characteristic UUID");
				break;
			case 2:
				TC_PRT("Input Descriptor UUID or nothing");
				break;
			}

			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_WRITE_VALUE: {
			if (param_index == 0) {
				g_test_param.param_count = 5;
				g_test_param.params = g_malloc0(sizeof(char *) * g_test_param.param_count);
			}

			if (param_index > 0) {
				int len = strlen(param);
				g_test_param.params[param_index - 1] = g_malloc0(len + 1);
				/* Remove new line character */
				param[len - 1] = '\0';
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			switch (param_index) {
			case 0:
				TC_PRT("Input Service UUID");
				break;
			case 1:
				TC_PRT("Input Characteristic UUID");
				break;
			case 2:
				TC_PRT("Input Descriptor UUID or nothing");
				break;
			case 3:
				TC_PRT("Input Value Type (avail. : uint8, uint16, uint32, int8, int16, int32, str)");
				break;
			case 4:
				TC_PRT("Input Value");
				break;
			}

			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_SET_CHAR_VALUE_CHANGED_CB: {
			if (param_index == 0) {
				g_test_param.param_count = 2;
				g_test_param.params = g_malloc0(sizeof(char *) * g_test_param.param_count);
			}

			if (param_index > 0) {
				int len = strlen(param);
				g_test_param.params[param_index - 1] = g_malloc0(len + 1);
				/* Remove new line character */
				param[len - 1] = '\0';
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			switch (param_index) {
			case 0:
				TC_PRT("Input Service UUID");
				break;
			case 1:
				TC_PRT("Input Characteristic UUID");
				break;
			}

			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_UNSET_CHAR_VALUE_CHANGED_CB: {
			if (param_index == 0) {
				g_test_param.param_count = 2;
				g_test_param.params = g_malloc0(sizeof(char *) * g_test_param.param_count);
			}

			if (param_index > 0) {
				int len = strlen(param);
				g_test_param.params[param_index - 1] = g_malloc0(len + 1);
				/* Remove new line character */
				param[len - 1] = '\0';
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			switch (param_index) {
			case 0:
				TC_PRT("Input Service UUID");
				break;
			case 1:
				TC_PRT("Input Characteristic UUID");
				break;
			}

			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_ANCS_POSITIVE_ACTION:
		case BT_UNIT_TEST_FUNCTION_ANCS_NEGATIVE_ACTION:
		case BT_UNIT_TEST_FUNCTION_ANCS_GET_NOTI_ATTR: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char *) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_STRING;
			}

			if (param_index > 0) {
				int len = strlen(param);
				g_test_param.params[param_index - 1] = g_malloc0(len + 1);
				/* Remove new line character */
				param[len - 1] = '\0';
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;
			break;
		}

		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_HPS: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_CREATE: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char*) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_BOOL;
			}

			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_HDR:
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_ENTITY:
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_URI: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char *) * g_test_param.param_count);
			}

			if (param_index > 0) {
				int len = strlen(param);
				g_test_param.params[param_index - 1] = g_malloc0(len + 1);
				/* Remove new line character */
				param[len - 1] = '\0';
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			switch (param_index) {
			case 0:
				TC_PRT("Input Value in string");
				break;
			}

			param_index++;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_CP: {
			if (param_index == 0) {
				g_test_param.param_count = 1;
				g_test_param.params = g_malloc0(sizeof(char *) * g_test_param.param_count);
				param_type = BT_UNIT_TEST_PARAM_TYPE_INT;
			}
			if (param_index > 0) {
				g_test_param.params[param_index - 1] = g_malloc0(strlen(param) + 1);
				strcpy(g_test_param.params[param_index - 1], param);
			}

			if (param_index == g_test_param.param_count) {
				need_to_set_params = false;
				test_input_callback((void *)test_id);
				param_index = 0;
				return 0;
			}

			switch (param_index) {
			case 0:
				TC_PRT("Input Value Type (avail. : \n1.HTTP_GET, \n2.HTTP_HEAD, \n3.HTTP_POST, \n4.HTTP_PUT, \n5.HTTP_DELETE, \n6.HTTPS_GET, \n7.HTTPS_HEAD, \n8.HTTPS_POST, \n9.HTTPS_PUT, \n10.HTTPS_DELETE, \n11.HTTP_CANCEL");
				break;
			}
			TC_PRT("Input param(%d) type:%s", param_index + 1, param_type);
			param_index++;

			break;
		}
		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}
		break;
	}

	case BT_UNIT_TEST_TABLE_AVRCP: {
		switch (test_id) {
		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_HID: {
		switch (test_id) {
		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_IPSP: {
		switch (test_id) {
		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}

	case BT_UNIT_TEST_TABLE_HDP: {
		switch (test_id) {
		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}
#ifdef TIZEN_WEARABLE
	case BT_UNIT_TEST_TABLE_HF: {
		switch (test_id) {
		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_PBAP_CLIENT: {
		switch (test_id) {
		default:
			TC_PRT("There is no param to set\n");
			need_to_set_params = false;
			break;
		}

		break;
	}
#endif
	default:
		TC_PRT("There is no param to set\n");
		need_to_set_params = false;
		break;
	}

	return 0;
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

			if (g_test_param.param_count > 0) {
				if (g_strrstr(g_test_param.params[0], "true"))
					connectable = true;
				else
					connectable = false;

				__bt_free_test_param(&g_test_param);
			}

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

		case BT_UNIT_TEST_FUNCTION_ADAPTER_SET_MANUFACTURER_DATA_CHANGED_CB:
		{
			ret = bt_adapter_set_manufacturer_data_changed_cb(__bt_adapter_manufacturer_data_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_MANUFACTURER_DATA_CHANGED_CB:
			ret = bt_adapter_unset_manufacturer_data_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_ADAPTER_SET_MANUFACTURER_DATA:
		{
			char data[20] = {0x03, 0xFF, 0x01, 0x02};
			int len = 4;
			int i = 0;

			if (g_test_param.param_count > 0) {
				len = 0;
				for (i = 0; i < (strlen(g_test_param.params[0]) - 1); i += 2) {
					if (g_test_param.params[0][i] >= 'A' && g_test_param.params[0][i] <= 'F')
						data[i/2] = (g_test_param.params[0][i] - 'A' + 0X0A) << 4;
					else
						data[i/2] = (g_test_param.params[0][i] - '0') << 4;

					if (g_test_param.params[0][i+1] >= 'A' && g_test_param.params[0][i+1] <= 'F')
						data[i/2] += (g_test_param.params[0][i+1] - 'A' + 0X0A);
					else
						data[i/2] += (g_test_param.params[0][i+1] - '0');
					len++;
				}

				__bt_free_test_param(&g_test_param);
			}

			ret = bt_adapter_set_manufacturer_data(data, len);

			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_SET_AUTHENTICATION_REQUSET_CB:
		{
			ret = bt_adapter_set_authentication_req_cb(__bt_adapter_authentication_req_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_UNSET_AUTHENTICATION_REQUSET_CB:
		{
			ret = bt_adapter_unset_authentication_req_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_PASSKEY_REPLY_ACCEPT:
		{
			char *passkey = NULL;
			TC_PRT("bt_passkey_reply: Authenticate Logitech Mouse: reply = Accept");
			passkey = g_strdup("0000"); //Passkey - "0000" for mouse
			ret = bt_passkey_reply(passkey, TRUE);
			if (ret < BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);
			else
				TC_PRT("bt_passkey_reply: accept authentication result = %d", ret);
			g_free(passkey);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_PASSKEY_REPLY_CANCEL:
		{
			char *passkey = NULL;
			TC_PRT("bt_passkey_reply: cancel authentication reply");
			passkey = g_strdup("0000"); //Passkey - "0000"
			ret = bt_passkey_reply(passkey, FALSE);
			if (ret < BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);
			else
				TC_PRT("bt_passkey_reply: Logitech Mouse cancel authentication result = %d", ret);
			g_free(passkey);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_PASSKEY_CONFIRMATION_REPLY_ACCEPT:
		{
			ret = bt_passkey_confirmation_reply(TRUE);
			if (ret < BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);
			else
				TC_PRT("bt_passkey_confirmation_reply accept, result = %d", ret);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_PASSKEY_CONFIRMATION_REPLY_REJECT:
		{
			ret = bt_passkey_confirmation_reply(FALSE);
			if (ret < BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);
			else
				TC_PRT("bt_passkey_confirmation_reply reject, result = %d", ret);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
			break;

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

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_SCAN_MODE: {
			int mode = BT_ADAPTER_LE_SCAN_MODE_BALANCED;

			if (g_test_param.param_count > 0) {
				mode = atoi(g_test_param.params[0]);
				__bt_free_test_param(&g_test_param);
			}

			ret = bt_adapter_le_set_scan_mode(mode);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_SCAN:
			ret = bt_adapter_le_start_scan(__bt_adapter_le_scan_result_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_SCAN:
			ret = bt_adapter_le_stop_scan();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

#ifndef TIZEN_WEARABLE
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_DEVICE_DISCOVERY:
			ret = bt_adapter_le_start_device_discovery();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_DEVICE_DISCOVERY:
			ret = bt_adapter_le_stop_device_discovery();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
#endif

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ADD_ADVERTISING_DATA: {
			int adv_data_type = 3; // default all
			int manufacturer_id = 117;
			char *manufacture = NULL;
			char manufacture_0[] = {0x0, 0x0, 0x0, 0x0};
			char manufacture_1[] = {0x01, 0x01, 0x01, 0x01};
			char manufacture_2[] = {0x02, 0x02, 0x02, 0x02};
			char manufacture_3[] = {0x03, 0x03, 0x03, 0x03};
			char service_data[] = {0x01, 0x02, 0x03};
			const char *time_svc_uuid_16 = "1805";
			const char *battery_svc_uuid_16 = "180f";
			const char *heart_rate_svc_uuid_16 = "180d";
			const char *lmmediate_alert_svc_uuid_16 = "1802";
			const char *ancs_uuid_128 = "7905F431-B5CE-4E99-A40F-4B1E122D00D0";
			int appearance = 192;  // 192 is generic watch

			advertiser = advertiser_list[advertiser_index];

			if (advertiser == NULL) {
				ret = bt_adapter_le_create_advertiser(&advertiser);
				TC_PRT("created le advertiser(%d)", ret);
				advertiser_list[advertiser_index] = advertiser;
			} else {
				ret = bt_adapter_le_clear_advertising_data(advertiser, BT_ADAPTER_LE_PACKET_ADVERTISING);
				if (ret != BT_ERROR_NONE)
					TC_PRT("clear advertising data [0x%04x]", ret);
				ret = bt_adapter_le_clear_advertising_data(advertiser, BT_ADAPTER_LE_PACKET_SCAN_RESPONSE);
				if (ret != BT_ERROR_NONE)
					TC_PRT("clear scan response data [0x%04x]", ret);
			}

			if (g_test_param.param_count > 0) {
				adv_data_type = atoi(g_test_param.params[0]);
				__bt_free_test_param(&g_test_param);
			}

			switch (adv_data_type) {
			case 0: // service uuid
				ret = bt_adapter_le_add_advertising_service_uuid(advertiser,
						BT_ADAPTER_LE_PACKET_ADVERTISING, time_svc_uuid_16);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add service_uuid [0x%04x]", ret);

				ret = bt_adapter_le_add_advertising_service_uuid(advertiser,
						BT_ADAPTER_LE_PACKET_ADVERTISING, battery_svc_uuid_16);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add service_uuid [0x%04x]", ret);

				manufacture = manufacture_0;
				break;

			case 1: // service solicitation
				ret = bt_adapter_le_add_advertising_service_solicitation_uuid(advertiser,
						BT_ADAPTER_LE_PACKET_ADVERTISING, heart_rate_svc_uuid_16);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add service_solicitation_uuid [0x%04x]", ret);

				ret = bt_adapter_le_add_advertising_service_solicitation_uuid(advertiser,
						BT_ADAPTER_LE_PACKET_ADVERTISING, lmmediate_alert_svc_uuid_16);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add service_solicitation_uuid [0x%04x]", ret);

				manufacture = manufacture_1;
				break;

			case 2: // appearance & tx power level
				ret = bt_adapter_le_set_advertising_appearance(advertiser,
					BT_ADAPTER_LE_PACKET_ADVERTISING, appearance);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add appearance data [0x%04x]", ret);

				ret = bt_adapter_le_set_advertising_tx_power_level(advertiser,
					BT_ADAPTER_LE_PACKET_ADVERTISING, true);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add appearance data [0x%04x]", ret);
				manufacture = manufacture_2;		
				break;

			case 3: // All
				ret = bt_adapter_le_add_advertising_service_uuid(advertiser,
						BT_ADAPTER_LE_PACKET_ADVERTISING, time_svc_uuid_16);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add service_uuid [0x%04x]", ret);

				ret = bt_adapter_le_add_advertising_service_uuid(advertiser,
						BT_ADAPTER_LE_PACKET_ADVERTISING, battery_svc_uuid_16);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add service_uuid [0x%04x]", ret);

				ret = bt_adapter_le_add_advertising_service_solicitation_uuid(advertiser,
						BT_ADAPTER_LE_PACKET_ADVERTISING, heart_rate_svc_uuid_16);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add service_solicitation_uuid [0x%04x]", ret);

				ret = bt_adapter_le_add_advertising_service_solicitation_uuid(advertiser,
						BT_ADAPTER_LE_PACKET_ADVERTISING, lmmediate_alert_svc_uuid_16);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add service_solicitation_uuid [0x%04x]", ret);

				ret = bt_adapter_le_set_advertising_appearance(advertiser,
					BT_ADAPTER_LE_PACKET_ADVERTISING, appearance);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add appearance data [0x%04x]", ret);

				ret = bt_adapter_le_set_advertising_tx_power_level(advertiser,
					BT_ADAPTER_LE_PACKET_ADVERTISING, true);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add tx_power_level [0x%04x]", ret);

				manufacture = manufacture_3;
				break;

			case 4: // ANCS
				ret = bt_adapter_le_add_advertising_service_solicitation_uuid(advertiser,
						BT_ADAPTER_LE_PACKET_ADVERTISING, time_svc_uuid_16);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add service_solicitation_uuid [0x%04x]", ret);

				ret = bt_adapter_le_add_advertising_service_solicitation_uuid(advertiser,
						BT_ADAPTER_LE_PACKET_ADVERTISING, ancs_uuid_128);
				if (ret != BT_ERROR_NONE)
					TC_PRT("add service_solicitation_uuid [0x%04x]", ret);

				ret = bt_adapter_le_set_advertising_device_name(advertiser,
					BT_ADAPTER_LE_PACKET_SCAN_RESPONSE, true);
				if (ret != BT_ERROR_NONE)
					TC_PRT("set device name [0x%04x]", ret);

				return 0;

			default:
				TC_PRT("No adv data");
				break;
			}

			/* Default scsn response data */
			ret = bt_adapter_le_add_advertising_service_data(advertiser,
				BT_ADAPTER_LE_PACKET_SCAN_RESPONSE, time_svc_uuid_16,
				service_data, sizeof(service_data));
			if (ret != BT_ERROR_NONE)
				TC_PRT("add service_data [0x%04x]", ret);

			ret = bt_adapter_le_set_advertising_device_name(advertiser,
				BT_ADAPTER_LE_PACKET_SCAN_RESPONSE, true);
			if (ret != BT_ERROR_NONE)
				TC_PRT("set device name [0x%04x]", ret);

			ret = bt_adapter_le_add_advertising_manufacturer_data(advertiser,
				BT_ADAPTER_LE_PACKET_SCAN_RESPONSE,
				manufacturer_id,
				manufacture, sizeof(manufacture_0));
			if (ret != BT_ERROR_NONE)
				TC_PRT("add manufacturer data [0x%04x]", ret);

			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_MODE: {
			int mode = BT_ADAPTER_LE_ADVERTISING_MODE_BALANCED;

			advertiser = advertiser_list[advertiser_index];

			if (advertiser == NULL) {
				ret = bt_adapter_le_create_advertiser(&advertiser);
				TC_PRT("created le advertiser(%d)", ret);
				advertiser_list[advertiser_index] = advertiser;
			}

			if (g_test_param.param_count > 0) {
				mode = atoi(g_test_param.params[0]);
				__bt_free_test_param(&g_test_param);
			}

			ret = bt_adapter_le_set_advertising_mode(advertiser, mode);
			if (ret != BT_ERROR_NONE)
				TC_PRT("add scan response data [0x%04x]", ret);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_FILTER_POLICY: {
			int filter_policy = BT_ADAPTER_LE_ADVERTISING_FILTER_ALLOW_CONN_WL;

			advertiser = advertiser_list[advertiser_index];

			if (advertiser == NULL) {
				ret = bt_adapter_le_create_advertiser(&advertiser);
				TC_PRT("created le advertiser(%d)", ret);
				advertiser_list[advertiser_index] = advertiser;
			}

			if (g_test_param.param_count > 0) {
				filter_policy = atoi(g_test_param.params[0]);
				__bt_free_test_param(&g_test_param);
			}

			ret = bt_adapter_le_set_advertising_filter_policy(advertiser, filter_policy);
			if (ret != BT_ERROR_NONE)
				TC_PRT("add scan response data [0x%04x]", ret);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_ADVERTISING_CONNECTABLE: {
			int type = BT_ADAPTER_LE_ADVERTISING_CONNECTABLE;

			advertiser = advertiser_list[advertiser_index];

			if (advertiser == NULL) {
				ret = bt_adapter_le_create_advertiser(&advertiser);
				TC_PRT("created le advertiser(%d)", ret);
				advertiser_list[advertiser_index] = advertiser;
			}

			if (g_test_param.param_count > 0) {
				type = atoi(g_test_param.params[0]);
				__bt_free_test_param(&g_test_param);
			}

			ret = bt_adapter_le_set_advertising_connectable(advertiser, type);
			if (ret != BT_ERROR_NONE)
				TC_PRT("add scan response data [0x%04x]", ret);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_ADVERTISING_NEW: {
			bt_adapter_le_advertising_state_changed_cb cb;

			if (advertiser_index == 0) cb = __bt_adapter_le_advertising_state_changed_cb;
			else if (advertiser_index == 1) cb = __bt_adapter_le_advertising_state_changed_cb_2;
			else cb = __bt_adapter_le_advertising_state_changed_cb_3;

			advertiser = advertiser_list[advertiser_index];

			if (advertiser == NULL) {
				ret = bt_adapter_le_create_advertiser(&advertiser);
				TC_PRT("created le advertiser(%d)", ret);
				advertiser_list[advertiser_index] = advertiser;
			}
			advertiser_index++;
			advertiser_index %= 3;

			TC_PRT("advertiser: %p", advertiser);

			ret = bt_adapter_le_start_advertising_new(advertiser, cb, NULL);
			if (ret < BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);
			break;
		}
#ifndef TIZEN_WEARABLE
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_START_ADVERTISING: {
			bt_adapter_le_advertising_state_changed_cb cb;
			bt_adapter_le_advertising_params_s adv_params = { 0, };

			if (advertiser_index == 0) cb = __bt_adapter_le_advertising_state_changed_cb;
			else if (advertiser_index == 1) cb = __bt_adapter_le_advertising_state_changed_cb_2;
			else cb = __bt_adapter_le_advertising_state_changed_cb_3;

			advertiser = advertiser_list[advertiser_index];

			adv_params.interval_min= 500;
			adv_params.interval_max= 500;

			if (advertiser == NULL) {
				ret = bt_adapter_le_create_advertiser(&advertiser);
				TC_PRT("created le advertiser(%d)", ret);
				advertiser_list[advertiser_index] = advertiser;
			}
			advertiser_index++;
			advertiser_index %= 3;

			ret = bt_adapter_le_start_advertising(advertiser, &adv_params, cb, NULL);
			if (ret < BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);
			break;
		}
#endif
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_STOP_ADVERTISING: {
			int slot_id = 0;

			if (g_test_param.param_count > 0) {
				slot_id = atoi(g_test_param.params[0]);
				__bt_free_test_param(&g_test_param);
			}

			advertiser = advertiser_list[slot_id];

			TC_PRT("advertiser: %p", advertiser);

			ret = bt_adapter_le_stop_advertising(advertiser);

			if(advertiser_index > 0)
				advertiser_index--;
			else
				advertiser_index = 2;

			advertiser_list[slot_id] = NULL;

			if (ret < BT_ERROR_NONE)
					TC_PRT("failed with [0x%04x]", ret);
			break;
		}
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

#ifndef TIZEN_WEARABLE
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_CLEAR_WHITE_LIST: {
			ret = bt_adapter_le_clear_white_list();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
#endif

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_ENABLE_PRIVACY: {
			static bool enable_privacy = false;

			if (enable_privacy == false)
				enable_privacy = true;
			else
				enable_privacy = false;

			if (g_test_param.param_count > 0) {
				if (g_strrstr(g_test_param.params[0], "true"))
					enable_privacy = true;
				else
					enable_privacy = false;

				__bt_free_test_param(&g_test_param);
			}

			ret = bt_adapter_le_enable_privacy(enable_privacy);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_REGISTER_SCAN_FILTER_DEVICE_ADDRESS: {
			bt_scan_filter_h scan_filter;

			ret = bt_adapter_le_create_scan_filter(&scan_filter);
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			ret = bt_adapter_le_scan_filter_set_device_address(scan_filter, remote_addr);
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			ret = bt_adapter_le_register_scan_filter(scan_filter);
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			ret = bt_adapter_le_destroy_scan_filter(scan_filter);
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_REGISTER_SCAN_FILTER_SERVICE_UUID: {
			bt_scan_filter_h scan_filter;

			ret = bt_adapter_le_create_scan_filter(&scan_filter);
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			ret = bt_adapter_le_scan_filter_set_service_uuid(scan_filter, "1805");
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			ret = bt_adapter_le_register_scan_filter(scan_filter);
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			ret = bt_adapter_le_destroy_scan_filter(scan_filter);
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_REGISTER_SCAN_FILTER_SERVICE_SOLICITATION_UUID: {
			bt_scan_filter_h scan_filter;

			ret = bt_adapter_le_create_scan_filter(&scan_filter);
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			ret = bt_adapter_le_scan_filter_set_service_solicitation_uuid(scan_filter, "1805");
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			ret = bt_adapter_le_register_scan_filter(scan_filter);
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			ret = bt_adapter_le_destroy_scan_filter(scan_filter);
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			break;
		}

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_UNREGISTER_ALL_SCAN_FILTERS: {
			ret = bt_adapter_le_unregister_all_scan_filters();
			if (ret != BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			break;
		}

#ifndef TIZEN_WEARABLE
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_SET_DEVICE_DISCOVERY_STATE_CHANGED_CB:
			ret = bt_adapter_le_set_device_discovery_state_changed_cb(__bt_adapter_le_device_discovery_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_UNSET_DEVICE_DISCOVERY_STATE_CHANGED_CB:
			ret = bt_adapter_le_unset_device_discovery_state_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
#endif
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_READ_MAXIMUM_DATA_LENGTH: {

			TC_PRT("Read Maximum LE Data length");

			int max_tx_octects = 0;
			int max_rx_octects = 0;
			int max_tx_time = 0;
			int max_rx_time = 0;
			ret = bt_adapter_le_read_maximum_data_length(
				&max_tx_octects, &max_tx_time,
				&max_rx_octects, &max_rx_time);
			TC_PRT("max data length values are  %d %d %d %d",
				max_tx_octects, max_tx_time,
				max_rx_octects, max_rx_time);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));

			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_WRITE_HOST_SUGGESTED_DEFAULT_DATA_LENGTH: {

			TC_PRT("Testing: Write Host suggested default LE Data length");

			unsigned int def_tx_octects = 30;
			unsigned int def_tx_time = 330;
			ret = bt_adapter_le_write_host_suggested_default_data_length(
				def_tx_octects, def_tx_time);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));

			break;
		}
		case BT_UNIT_TEST_FUNCTION_ADAPTER_LE_READ_HOST_SUGGESTED_DEFAULT_DATA_LENGTH: {

			TC_PRT("Read host suggested default LE Data length");

			unsigned int def_tx_octets = 0;
			unsigned 	int def_tx_time = 0;
			ret = bt_adapter_le_read_suggested_default_data_length(
				&def_tx_octets, &def_tx_time);
			TC_PRT("host suggested default le data length values are  %d %d",
					def_tx_octets, def_tx_time);
			break;
		}

		case BT_UNIT_TEST_FUNCTION_LE_DEVICE_SET_DATA_LENGTH: {

			TC_PRT("Set LE Data length paramters cmd");

			unsigned int tx_octets = 50;
			unsigned 	int tx_time = 500;

			TC_PRT("settting le data length values  tx octects: %d  tx time: %d",
			tx_octets, tx_time);
			ret = bt_device_le_set_data_length(remote_addr,
				tx_octets, tx_time);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));

			break;
		}
		case BT_UNIT_TEST_FUNCTION_LE_DEVICE_DATA_LENGTH_CHANGED_CB: {

			TC_PRT("Setting LE Data length change callback");

			ret = bt_device_le_set_data_length_change_cb(__bt_le_set_data_length_changed_cb,
				NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));

			break;
		}
		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
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
			bt_profile_e profile = BT_PROFILE_HSP;

			if (g_test_param.param_count > 0) {
				profile = atoi(g_test_param.params[0]);

				__bt_free_test_param(&g_test_param);
			}

			ret = bt_device_is_profile_connected(remote_addr, profile, &connected_status);
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

		case BT_UNIT_TEST_FUNCTION_DEVICE_SET_BOND_DESTROYED_CB : {
			ret = bt_device_set_bond_destroyed_cb(__bt_device_bond_destroyed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_DEVICE_CREATE_BOND : {
			ret = bt_device_create_bond(remote_addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_DEVICE_CREATE_BOND_BY_TYPE : {
			bt_device_connection_link_type_e link_type = BT_DEVICE_CONNECTION_LINK_LE;

			if (g_test_param.param_count > 0) {
				link_type = atoi(g_test_param.params[0]);

				__bt_free_test_param(&g_test_param);
			}

			ret = bt_device_create_bond_by_type(remote_addr, link_type);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_DEVICE_DESTROY_BOND : {
			ret = bt_device_destroy_bond(remote_addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_DEVICE_GET_CONNECTION_STATE: {
			bool bredr_connected = false;
			bool le_connected = false;

			ret = bt_device_get_connection_state(remote_addr,
					BT_DEVICE_CONNECTION_LINK_BREDR,
					&bredr_connected);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			}

			ret = bt_device_get_connection_state(remote_addr,
					BT_DEVICE_CONNECTION_LINK_LE,
					&le_connected);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			}

			TC_PRT("BR/EDR [%s], LE [%s]",
					bredr_connected ? "Connected" : "Disconnected",
					le_connected ? "Connected" : "Disconnected");

			break;
		}

		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
			break;

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

			if (g_test_param.param_count > 0) {
				ret = bt_socket_send_data(client_fd, g_test_param.params[0], strlen(g_test_param.params[0]) + 1);

				__bt_free_test_param(&g_test_param);
			} else {
				ret = bt_socket_send_data(client_fd, data, sizeof(data));
			}

			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_SOCKET_CREATE_RFCOMM_CUSTOM_UUID: {
			int socket_fd = 0;

			ret = bt_socket_create_rfcomm(custom_uuid, &socket_fd);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			} else {
				TC_PRT("socket_fd: %d", socket_fd);
				custom_server_fd = socket_fd;
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_SOCKET_DESTROY_RFCOMM_CUSTOM_UUID:
			ret = bt_socket_destroy_rfcomm(custom_server_fd);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_LISTEN_AND_ACCEPT_RFCOMM_CUSTOM_UUID:
			ret = bt_socket_listen_and_accept_rfcomm(custom_server_fd, 1);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_CONNECT_RFCOMM_CUSTOM_UUID: {
			ret = bt_socket_connect_rfcomm(remote_addr, custom_uuid);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_SOCKET_DISCONNECT_RFCOMM_CUSTOM_UUID:
			ret = bt_socket_disconnect_rfcomm(custom_client_fd);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_SOCKET_SEND_DATA_CUSTOM_UUID: {
			char data[] = "Sending test";

			if (g_test_param.param_count > 0) {
				ret = bt_socket_send_data(custom_client_fd, g_test_param.params[0], strlen(g_test_param.params[0]) + 1);

				__bt_free_test_param(&g_test_param);
			} else {
				ret = bt_socket_send_data(custom_client_fd, data, sizeof(data));
			}

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

		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
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

		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
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
			bt_audio_profile_type_e audio_profile = BT_AUDIO_PROFILE_TYPE_ALL;

			if (g_test_param.param_count > 0) {
				audio_profile = atoi(g_test_param.params[0]);

				__bt_free_test_param(&g_test_param);
			}

			ret = bt_audio_connect(remote_addr,  audio_profile);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_AUDIO_DISCONNECT: {
			bt_audio_profile_type_e audio_profile = BT_AUDIO_PROFILE_TYPE_ALL;

			if (g_test_param.param_count > 0) {
				audio_profile = atoi(g_test_param.params[0]);

				__bt_free_test_param(&g_test_param);
			}

			ret = bt_audio_disconnect(remote_addr,  audio_profile);
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
		case BT_UNIT_TEST_FUNCTION_AG_IS_CONNECTED: {
			bool connected = false;
			ret = bt_ag_is_connected(&connected);
			TC_PRT("returns %s\n", connected ? "Connected" : "Disconnected");
			break;
		}
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
			const char *phone_number;

			if (g_test_param.param_count > 0) {
				ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_DIALING,
						call_id, g_test_param.params[0]);

				__bt_free_test_param(&g_test_param);
			} else {
				phone_number = g_strdup("01012345678");

				ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_DIALING,
						call_id, phone_number);
			}

			if (sco_required)
				bt_ag_open_sco();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
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
			const char *phone_number;

			if (g_test_param.param_count > 0) {
				ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_INCOMING,
						call_id, g_test_param.params[0]);

				__bt_free_test_param(&g_test_param);
			} else {
				phone_number = g_strdup("01012345678");

				ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_INCOMING,
						call_id, phone_number);
			}

			TC_PRT("returns %s\n", __bt_get_error_message(ret));
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
			const char *phone_number;

			if (g_test_param.param_count > 0) {
				ret = bt_call_list_add(call_list, 1, BT_AG_CALL_STATE_ACTIVE, g_test_param.params[0]);

				__bt_free_test_param(&g_test_param);
			} else {
				phone_number = g_strdup("01012345678");

				ret = bt_call_list_add(call_list, 1, BT_AG_CALL_STATE_ACTIVE, phone_number);
			}

			TC_PRT("returns %s\n", __bt_get_error_message(ret));
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
		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
			break;

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

		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
			break;

		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_GATT: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_GATT_CONNECT : {
			bool auto_connect = false;

			if (g_test_param.param_count > 0) {
				if (g_strrstr(g_test_param.params[0], "true"))
					auto_connect = true;
				else
					auto_connect = false;

				__bt_free_test_param(&g_test_param);
			}

			ret = bt_gatt_connect(remote_addr, auto_connect);
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
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_CREATE: {
			if (client) {
			       bt_gatt_client_destroy(client);
			       client = NULL;
			}
			ret = bt_gatt_client_create(remote_addr, &client);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_DESTROY: {
			if (!client)
				break;

			ret = bt_gatt_client_destroy(client);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			client = NULL;
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_GET_REMOTE_ADDRESS: {
			char *addr = NULL;

			ret = bt_gatt_client_get_remote_address(client, &addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));

			if (addr) {
				TC_PRT("remote addr : %s\n", addr);
				g_free(addr);
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_PRINT_ALL: {
			ret = bt_gatt_client_foreach_services(client,
					__bt_gatt_client_foreach_svc_cb, (void *)test_id);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_READ_VALUE: {
			char *svc_uuid = NULL;
			char *chr_uuid = NULL;
			char *desc_uuid = NULL;
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;
			bt_gatt_h desc = NULL;

			if (g_test_param.param_count < 3) {
				TC_PRT("Input parameters first");
				break;
			}

			svc_uuid = g_test_param.params[0];
			chr_uuid = g_test_param.params[1];

			ret = bt_gatt_client_get_service(client, svc_uuid, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, chr_uuid, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			if (strlen(g_test_param.params[2]) > 0) {
				desc_uuid = g_test_param.params[2];
				ret = bt_gatt_characteristic_get_descriptor(chr, desc_uuid, &desc);
				if (ret != BT_ERROR_NONE) {
					TC_PRT("bt_gatt_characteristic_get_descriptor is failed : %d", ret);
					__bt_free_test_param(&g_test_param);
					break;
				}
				ret = bt_gatt_client_read_value(desc,
						__bt_gatt_client_read_complete_cb, NULL);
			} else {
				ret = bt_gatt_client_read_value(chr,
						__bt_gatt_client_read_complete_cb, NULL);
			}

			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_read_value is failed : %d", ret);
			}

			__bt_free_test_param(&g_test_param);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_WRITE_VALUE: {
			char *svc_uuid = NULL;
			char *chr_uuid = NULL;
			char *desc_uuid = NULL;
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;
			bt_gatt_h desc = NULL;

			if (g_test_param.param_count < 5) {
				TC_PRT("Input parameters first");
				break;
			}

			svc_uuid = g_test_param.params[0];
			chr_uuid = g_test_param.params[1];

			ret = bt_gatt_client_get_service(client, svc_uuid, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, chr_uuid, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			if (strlen(g_test_param.params[2]) > 0) {
				desc_uuid = g_test_param.params[2];
				ret = bt_gatt_characteristic_get_descriptor(chr, desc_uuid, &desc);
				if (ret != BT_ERROR_NONE) {
					TC_PRT("bt_gatt_characteristic_get_descriptor is failed : %d", ret);
					__bt_free_test_param(&g_test_param);
					break;
				}

				ret = __bt_gatt_client_set_value(g_test_param.params[3],
						g_test_param.params[4], desc);
				if (ret != BT_ERROR_NONE) {
					TC_PRT("bt_gatt_set_value is failed : %d", ret);
					__bt_free_test_param(&g_test_param);
					break;
				}

				ret = bt_gatt_client_write_value(desc,
						__bt_gatt_client_write_complete_cb, NULL);
			} else {
				ret = __bt_gatt_client_set_value(g_test_param.params[3],
						g_test_param.params[4], chr);
				if (ret != BT_ERROR_NONE) {
					TC_PRT("bt_gatt_set_value is failed : %d", ret);
					__bt_free_test_param(&g_test_param);
					break;
				}

				ret = bt_gatt_client_write_value(chr,
						__bt_gatt_client_write_complete_cb, NULL);
			}

			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_write_value is failed : %d", ret);
			}

			__bt_free_test_param(&g_test_param);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_SET_CHAR_VALUE_CHANGED_CB: {
			char *svc_uuid = NULL;
			char *chr_uuid = NULL;
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;

			if (g_test_param.param_count < 2) {
				TC_PRT("Input parameters first");
				break;
			}

			svc_uuid = g_test_param.params[0];
			chr_uuid = g_test_param.params[1];

			ret = bt_gatt_client_get_service(client, svc_uuid, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, chr_uuid, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_client_set_characteristic_value_changed_cb(chr,
					__bt_gatt_client_value_changed_cb, NULL);
			if (ret != BT_ERROR_NONE)
				TC_PRT("bt_gatt_client_set_characteristic_value_changed_cb is failed : %d", ret);

			__bt_free_test_param(&g_test_param);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_UNSET_CHAR_VALUE_CHANGED_CB: {
			char *svc_uuid = NULL;
			char *chr_uuid = NULL;
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;

			if (g_test_param.param_count < 2) {
				TC_PRT("Input parameters first");
				break;
			}

			svc_uuid = g_test_param.params[0];
			chr_uuid = g_test_param.params[1];

			ret = bt_gatt_client_get_service(client, svc_uuid, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, chr_uuid, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_client_unset_characteristic_value_changed_cb(chr);
			if (ret != BT_ERROR_NONE)
				TC_PRT("bt_gatt_client_unset_characteristic_value_changed_cb is failed : %d", ret);

			__bt_free_test_param(&g_test_param);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_CLIENT_FOREACH_SERVICES: {
			ret = bt_gatt_client_foreach_services(client,
					__bt_gatt_client_foreach_svc_cb, (void *)test_id);
			if (ret != BT_ERROR_NONE)
				TC_PRT("bt_gatt_client_foreach_services_by_uuid is failed");
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_INITIALIZE : {

			ret = bt_gatt_server_initialize();

			TC_PRT("returns %s \n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_DEINITIALIZE : {

			ret = bt_gatt_server_deinitialize();

			TC_PRT("returns %s \n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_UNREGISTER_ALL_SERVICES: {
			ret = bt_gatt_server_unregister_all_services(server);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_BATTERY_SVC: {
			bt_gatt_h service = NULL;
			bt_gatt_h characteristic = NULL;
			bt_gatt_h descriptor = NULL;
			char *service_uuid = "180f"; // Battery Service
			char *char_uuid = "2a19"; // Battery Level
			char *desc_uuid = "2902";  // CCCD
			char char_value[1] = {80}; // 80%
			char desc_value[2] = {0, 0}; // Notification & Indication disabled
			int permissions = BT_GATT_PERMISSION_READ;
			int properties = BT_GATT_PROPERTY_READ | BT_GATT_PROPERTY_NOTIFY;

			ret = bt_gatt_server_initialize();
			TC_PRT("bt_gatt_server_initialize : %s \n", __bt_get_error_message(ret));

			if (server == NULL) {
				ret = bt_gatt_server_create(&server);
				TC_PRT("bt_gatt_server_create : %s \n", __bt_get_error_message(ret));
			}

			ret = bt_gatt_service_create(service_uuid, BT_GATT_SERVICE_TYPE_PRIMARY, &service);
			TC_PRT("bt_gatt_service_create : %s \n", __bt_get_error_message(ret));

			ret = bt_gatt_characteristic_create(char_uuid, permissions, properties,
												char_value, sizeof(char_value), &characteristic);
			TC_PRT("bt_gatt_characteristic_create : %s\n", __bt_get_error_message(ret));

			bt_gatt_server_set_read_value_requested_cb(characteristic, __bt_gatt_server_read_value_requested_cb, NULL);
			bt_gatt_server_set_notification_state_change_cb(characteristic, __bt_gatt_server_notification_state_change_cb, NULL);
			ret = bt_gatt_service_add_characteristic(service, characteristic);
			TC_PRT("bt_gatt_service_add_characteristic : %s\n", __bt_get_error_message(ret));

			permissions = BT_GATT_PERMISSION_READ | BT_GATT_PERMISSION_WRITE;
			ret = bt_gatt_descriptor_create(desc_uuid, permissions, desc_value, sizeof(desc_value), &descriptor);
			TC_PRT("bt_gatt_descriptor_create : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_characteristic_add_descriptor(characteristic, descriptor);
			TC_PRT("bt_gatt_characteristic_add_descriptor : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_server_register_service(server, service);
			TC_PRT("bt_gatt_server_register_service : %s\n", __bt_get_error_message(ret));

			battery_h.svc = service;
			battery_h.chr = characteristic;
			battery_h.desc = descriptor;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_CHANGE_BATTERY_LEVEL: {
			char char_value[1] = {1 + (rand()%100)};
			if (!server) {
				TC_PRT("bt gatt server is not created");
				break;
			}
			ret = bt_gatt_set_value(battery_h.chr, char_value, 1);
			TC_PRT("Value[%d], returns %s\n", char_value[0], __bt_get_error_message(ret));

			/* notify only client remote_addr */
			ret = bt_gatt_server_notify(battery_h.chr, true, __bt_gatt_server_notification_sent_cb, remote_addr, NULL);
			TC_PRT("bt_gatt_server_notify : %s\n", __bt_get_error_message(ret));

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_HEART_RATE_SVC: {
			bt_gatt_h service = NULL;
			bt_gatt_h characteristic = NULL;
			bt_gatt_h descriptor = NULL;
			char *service_uuid = "180d"; // Heart Rate Service
			char *char_uuid = "2a37"; // Heart Rate Measurement
			char *desc_uuid = "2902";  // CCCD
			char char_value_1[3] = {1, 0x38, 0}; // Flag + Measurement value (56)
			char desc_value[2] = {1, 0}; // Notification enabled
			int permissions = BT_GATT_PERMISSION_READ;
			int properties = BT_GATT_PROPERTY_NOTIFY | BT_GATT_PROPERTY_READ; // READ property added for testing

			ret = bt_gatt_server_initialize();
			TC_PRT("bt_gatt_server_initialize : %s \n", __bt_get_error_message(ret));

			if (server == NULL) {
				ret = bt_gatt_server_create(&server);
				TC_PRT("bt_gatt_server_create : %s \n", __bt_get_error_message(ret));
			}

			ret = bt_gatt_service_create(service_uuid, BT_GATT_SERVICE_TYPE_PRIMARY, &service);
			TC_PRT("bt_gatt_service_create : %s \n", __bt_get_error_message(ret));

			ret = bt_gatt_characteristic_create(char_uuid, permissions, properties,
												char_value_1, sizeof(char_value_1), &characteristic);
			TC_PRT("bt_gatt_characteristic_create : %s\n", __bt_get_error_message(ret));

			heart_rate_h.chr = characteristic;

			bt_gatt_server_set_read_value_requested_cb(characteristic, __bt_gatt_server_read_value_requested_cb, NULL);
			bt_gatt_server_set_notification_state_change_cb(characteristic, __bt_gatt_server_notification_state_change_cb, NULL);
			ret = bt_gatt_service_add_characteristic(service, characteristic);
			TC_PRT("bt_gatt_service_add_characteristic : %s\n", __bt_get_error_message(ret));

			permissions = BT_GATT_PERMISSION_READ | BT_GATT_PERMISSION_WRITE;
			ret = bt_gatt_descriptor_create(desc_uuid, permissions, desc_value, sizeof(desc_value), &descriptor);
			TC_PRT("bt_gatt_descriptor_create : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_characteristic_add_descriptor(characteristic, descriptor);
			TC_PRT("bt_gatt_characteristic_add_descriptor : %s\n", __bt_get_error_message(ret));

			char_uuid = "2a38"; // Body Sensor Location
			permissions = BT_GATT_PERMISSION_READ;
			properties = BT_GATT_PROPERTY_READ;
			char char_value_2 = 4; // Hand
			ret = bt_gatt_characteristic_create(char_uuid, permissions, properties,
												&char_value_2, sizeof(char_value_2), &characteristic);
			TC_PRT("bt_gatt_characteristic_create : %s\n", __bt_get_error_message(ret));

			bt_gatt_server_set_read_value_requested_cb(characteristic, __bt_gatt_server_read_value_requested_cb, NULL);
			ret = bt_gatt_service_add_characteristic(service, characteristic);
			TC_PRT("bt_gatt_service_add_characteristic : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_server_register_service(server, service);
			TC_PRT("bt_gatt_server_register_service : %s\n", __bt_get_error_message(ret));

			heart_rate_h.svc = service;
			heart_rate_h.desc = descriptor;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_NOTIFY_HEART_RATE_MEASUREMENT: {
			int char_value = 60 + (rand()%60);
			if (!server) {
				TC_PRT("bt gatt server is not created");
				break;
			}
			ret = bt_gatt_set_int_value(heart_rate_h.chr, BT_DATA_TYPE_UINT16, char_value, 1);
			TC_PRT("bt_gatt_set_value(value : %d) : %s\n", char_value, __bt_get_error_message(ret));

			/* Notify all client devices */
			ret = bt_gatt_server_notify(heart_rate_h.chr, true, __bt_gatt_server_notification_sent_cb, NULL, NULL);
			TC_PRT("bt_gatt_server_notify : %s\n", __bt_get_error_message(ret));

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_THERMOMETER_SVC: {
			bt_gatt_h service = NULL;
			bt_gatt_h characteristic = NULL;
			bt_gatt_h descriptor = NULL;
			char *service_uuid = "1809"; // Health Thermometer Service
			char *char_uuid = "2a1c"; // Temperature Measurement
			char *desc_uuid = "2902";  // CCCD
			char char_value[5] = {0, 0x85, 0xff, 0xff, 0xfe}; // Flag + Temperature Measurement Value (Celsius, -1.23)
			char desc_value[2] = {2, 0}; // Indication enabled
			int permissions = BT_GATT_PERMISSION_READ;
			int properties = BT_GATT_PROPERTY_INDICATE | BT_GATT_PROPERTY_READ; // READ property added for testing

			ret = bt_gatt_server_initialize();
			TC_PRT("bt_gatt_server_initialize : %s \n", __bt_get_error_message(ret));

			if (server == NULL) {
				ret = bt_gatt_server_create(&server);
				TC_PRT("bt_gatt_server_create : %s \n", __bt_get_error_message(ret));
			}

			ret = bt_gatt_service_create(service_uuid, BT_GATT_SERVICE_TYPE_PRIMARY, &service);
			TC_PRT("bt_gatt_service_create : %s \n", __bt_get_error_message(ret));

			ret = bt_gatt_characteristic_create(char_uuid, permissions, properties,
												char_value, sizeof(char_value), &characteristic);
			TC_PRT("bt_gatt_characteristic_create : %s\n", __bt_get_error_message(ret));

			bt_gatt_server_set_read_value_requested_cb(characteristic, __bt_gatt_server_read_value_requested_cb, NULL);
			bt_gatt_server_set_notification_state_change_cb(characteristic, __bt_gatt_server_notification_state_change_cb, NULL);
			ret = bt_gatt_service_add_characteristic(service, characteristic);
			TC_PRT("bt_gatt_service_add_characteristic : %s\n", __bt_get_error_message(ret));

			permissions = BT_GATT_PERMISSION_READ | BT_GATT_PERMISSION_WRITE;
			ret = bt_gatt_descriptor_create(desc_uuid, permissions, desc_value, sizeof(desc_value), &descriptor);
			TC_PRT("bt_gatt_descriptor_create : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_characteristic_add_descriptor(characteristic, descriptor);
			TC_PRT("bt_gatt_characteristic_add_descriptor : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_server_register_service(server, service);
			TC_PRT("bt_gatt_server_register_service : %s\n", __bt_get_error_message(ret));

			thermometer_h.chr = characteristic;
			thermometer_h.svc = service;
			thermometer_h.desc = descriptor;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_CHANGE_THERMOMETER_MEASUREMENT: {
			if (!server) {
				TC_PRT("bt gatt server is not created");
				break;
			}
			ret = bt_gatt_set_float_value(thermometer_h.chr, BT_DATA_TYPE_FLOAT, 123, -2, 1);
			TC_PRT("bt_gatt_set_value(value : 1.23) : %s\n", __bt_get_error_message(ret));

			/* notify only client remote_addr */
			ret = bt_gatt_server_notify(thermometer_h.chr, true, __bt_gatt_server_notification_sent_cb, remote_addr, NULL);
			TC_PRT("bt_gatt_server_notify : %s\n", __bt_get_error_message(ret));

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_DEVICE_INFORMATION_SVC: {
			bt_gatt_h service = NULL;
			bt_gatt_h characteristic = NULL;
			char *service_uuid = "180a"; // Device Infromation Service
			char *char_uuid = NULL;
			char *char_value;
			int permissions = BT_GATT_PERMISSION_READ;
			int properties = BT_GATT_PROPERTY_READ;

			ret = bt_gatt_server_initialize();
			TC_PRT("bt_gatt_server_initialize : %s \n", __bt_get_error_message(ret));

			if (server == NULL) {
				ret = bt_gatt_server_create(&server);
				TC_PRT("bt_gatt_server_create : %s \n", __bt_get_error_message(ret));
			}

			ret = bt_gatt_service_create(service_uuid, BT_GATT_SERVICE_TYPE_PRIMARY, &service);
			TC_PRT("bt_gatt_service_create : %s \n", __bt_get_error_message(ret));

			char_uuid = "2a29"; // Manufacturer Name String
			char_value = g_strdup("Samsung");
			ret = bt_gatt_characteristic_create(char_uuid, permissions, properties,
												char_value, strlen(char_value), &characteristic);
			TC_PRT("bt_gatt_characteristic_create : %s\n", __bt_get_error_message(ret));
			g_free(char_value);

			bt_gatt_server_set_read_value_requested_cb(characteristic, __bt_gatt_server_read_value_requested_cb, NULL);
			ret = bt_gatt_service_add_characteristic(service, characteristic);
			TC_PRT("bt_gatt_service_add_characteristic : %s\n", __bt_get_error_message(ret));

			char_uuid = "2a24"; // Model Number String
			char_value = g_strdup("TIZEN_HR 0408");
			ret = bt_gatt_characteristic_create(char_uuid, permissions, properties,
												char_value, strlen(char_value), &characteristic);
			TC_PRT("bt_gatt_characteristic_create : %s\n", __bt_get_error_message(ret));
			g_free(char_value);

			bt_gatt_server_set_read_value_requested_cb(characteristic, __bt_gatt_server_read_value_requested_cb, NULL);
			ret = bt_gatt_service_add_characteristic(service, characteristic);
			TC_PRT("bt_gatt_service_add_characteristic : %s\n", __bt_get_error_message(ret));

			char_uuid = "2a28"; // S/W Revision String
			char_value = g_strdup("NC2");
			ret = bt_gatt_characteristic_create(char_uuid, permissions, properties,
												char_value, strlen(char_value), &characteristic);
			TC_PRT("bt_gatt_characteristic_create : %s\n", __bt_get_error_message(ret));
			g_free(char_value);

			bt_gatt_server_set_read_value_requested_cb(characteristic, __bt_gatt_server_read_value_requested_cb, NULL);
			ret = bt_gatt_service_add_characteristic(service, characteristic);
			TC_PRT("bt_gatt_service_add_characteristic : %s\n", __bt_get_error_message(ret));

			char_uuid = "2a26"; // Firmware Revision Name String
			char_value = g_strdup("Firmware Ver 04");
			ret = bt_gatt_characteristic_create(char_uuid, permissions, properties,
												char_value, strlen(char_value), &characteristic);
			TC_PRT("bt_gatt_characteristic_create : %s\n", __bt_get_error_message(ret));
			g_free(char_value);

			bt_gatt_server_set_read_value_requested_cb(characteristic, __bt_gatt_server_read_value_requested_cb, NULL);
			ret = bt_gatt_service_add_characteristic(service, characteristic);
			TC_PRT("bt_gatt_service_add_characteristic : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_server_register_service(server, service);
			TC_PRT("bt_gatt_server_register_service : %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_LINK_LOSS_SVC: {
			bt_gatt_h service = NULL;
			bt_gatt_h characteristic = NULL;
			char *service_uuid = "1803"; // Link Loss Service
			char *char_uuid = "2a06"; // Alert Level
			char char_value[1] = {2}; // high alert
			int value_length = 1;
			int permissions = BT_GATT_PERMISSION_READ;
			int properties = BT_GATT_PROPERTY_READ | BT_GATT_PROPERTY_WRITE;

			ret = bt_gatt_server_initialize();
			TC_PRT("bt_gatt_server_initialize : %s \n", __bt_get_error_message(ret));

			if (server == NULL) {
				ret = bt_gatt_server_create(&server);
				TC_PRT("bt_gatt_server_create : %s \n", __bt_get_error_message(ret));
			}

			ret = bt_gatt_service_create(service_uuid, BT_GATT_SERVICE_TYPE_PRIMARY, &service);
			TC_PRT("bt_gatt_service_create : %s \n", __bt_get_error_message(ret));

			ret = bt_gatt_characteristic_create(char_uuid, permissions, properties,
												char_value, value_length, &characteristic);
			TC_PRT("bt_gatt_characteristic_create : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_server_set_value_changed_cb(characteristic, __bt_gatt_server_value_changed_cb, NULL);
			TC_PRT("bt_gatt_server_set_value_changed_cb : %s\n", __bt_get_error_message(ret));

			bt_gatt_server_set_read_value_requested_cb(characteristic, __bt_gatt_server_read_value_requested_cb, NULL);
			ret = bt_gatt_service_add_characteristic(service, characteristic);
			TC_PRT("bt_gatt_service_add_characteristic : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_server_register_service(server, service);
			TC_PRT("bt_gatt_server_register_service : %s\n", __bt_get_error_message(ret));

			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_REGISTER_CUSTOM_SVC: {
			bt_gatt_h service = NULL;
			bt_gatt_h characteristic = NULL;
			bt_gatt_h descriptor = NULL;
			char *service_uuid = "000018f2-0000-1000-8000-00805f9b34fb";
			char *char_uuid = "00002af6-0000-1000-8000-00805f9b34fb";
			char *desc_uuid = "00002a56-0000-1000-8000-00805f9b34fb";
			char char_value[4] = {10, 20, 30, 40};
			char desc_value[4] = {12, 34, 56, 78};
			int value_length = 4;
			int permissions = BT_GATT_PERMISSION_READ;
			int properties = BT_GATT_PROPERTY_BROADCAST | BT_GATT_PROPERTY_READ |
							BT_GATT_PROPERTY_WRITE | BT_GATT_PROPERTY_NOTIFY;

			ret = bt_gatt_server_initialize();
			TC_PRT("bt_gatt_server_initialize : %s \n", __bt_get_error_message(ret));

			if (server == NULL) {
				ret = bt_gatt_server_create(&server);
				TC_PRT("bt_gatt_server_create : %s \n", __bt_get_error_message(ret));
			}

			ret = bt_gatt_service_create(service_uuid, BT_GATT_SERVICE_TYPE_PRIMARY, &service);
			TC_PRT("bt_gatt_service_create : %s \n", __bt_get_error_message(ret));

			ret = bt_gatt_characteristic_create(char_uuid, permissions, properties,
												char_value, value_length, &characteristic);
			TC_PRT("bt_gatt_characteristic_create : %s\n", __bt_get_error_message(ret));

			bt_gatt_server_set_read_value_requested_cb(characteristic, __bt_gatt_server_read_value_requested_cb, NULL);
			ret = bt_gatt_service_add_characteristic(service, characteristic);
			TC_PRT("bt_gatt_service_add_characteristic : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_descriptor_create(desc_uuid, permissions, desc_value, value_length, &descriptor);
			TC_PRT("bt_gatt_descriptor_create : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_characteristic_add_descriptor(characteristic, descriptor);
			TC_PRT("bt_gatt_characteristic_add_descriptor : %s\n", __bt_get_error_message(ret));

			ret = bt_gatt_server_register_service(server, service);
			TC_PRT("bt_gatt_server_register_service : %s\n", __bt_get_error_message(ret));

			custom_h.svc = service;
			custom_h.chr = characteristic;
			custom_h.desc = descriptor;
			break;
		}
		case BT_UNIT_TEST_FUNCTION_GATT_SERVER_CHANGE_CUSTOM_VAL: {
			char char_value[4] = {50, 60, 70, 80};
			if (!server) {
				TC_PRT("bt gatt server is not created");
				break;
			}
			ret = bt_gatt_set_value(custom_h.chr, char_value, 4);
			TC_PRT("returns  %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ANCS_PAIR: {
			bt_device_connection_link_type_e link_type = BT_DEVICE_CONNECTION_LINK_LE;

			if (g_test_param.param_count > 0) {
				link_type = atoi(g_test_param.params[0]);

				__bt_free_test_param(&g_test_param);
			}

			ret = bt_device_create_bond_by_type(remote_addr, link_type);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ANCS_WATCH: {
			bt_gatt_h ancs_svc = NULL;
			bt_gatt_h noti_source = NULL;;
			char *svc_uuid = "7905F431-B5CE-4E99-A40F-4B1E122D00D0";
			char *chr_uuid = "9FBF120D-6301-42D9-8C58-25E699A21DBD";

			ret = bt_gatt_client_get_service(client, svc_uuid, &ancs_svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				break;
			}

			ret = bt_gatt_service_get_characteristic(ancs_svc, chr_uuid, &noti_source);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				break;
			}

			ret = bt_gatt_client_set_characteristic_value_changed_cb(noti_source,
					__bt_gatt_client_value_changed_cb, NULL);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_set_characteristic_value_changed_cb is failed : %d", ret);
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ANCS_POSITIVE_ACTION: {
			char value[6];
			char *noti_id = NULL;
			int id;

			if (g_test_param.param_count == 0) {
				TC_PRT("Input parameters first");
				break;
			}

			noti_id = g_test_param.params[0];
			id = atoi(noti_id);

			value[0] = 0x02;
			memcpy(&value[1], &id, sizeof(int));
			value[5] = 0x00;

			__ancs_write_value_to_control_point(client, value, sizeof(value));

			break;
		}
		case BT_UNIT_TEST_FUNCTION_ANCS_NEGATIVE_ACTION: {
			char value[6];
			char *noti_id = NULL;
			int id;

			if (g_test_param.param_count == 0) {
				TC_PRT("Input parameters first");
				break;
			}

			noti_id = g_test_param.params[0];
			id = atoi(noti_id);

			value[0] = 0x02;
			memcpy(&value[1], &id, sizeof(int));
			value[5] = 0x01;

			__ancs_write_value_to_control_point(client, value, sizeof(value));

			break;
		}
		case BT_UNIT_TEST_FUNCTION_ANCS_GET_NOTI_ATTR: {
			bt_gatt_h ancs_svc = NULL;
			bt_gatt_h data_source = NULL;;
			char *svc_uuid = "7905F431-B5CE-4E99-A40F-4B1E122D00D0";
			char *chr_uuid = "22EAC6E9-24D6-4BB5-BE44-B36ACE7C7BFB";

			char value[19];
			char *noti_id = NULL;
			int id;

			if (g_test_param.param_count == 0) {
				TC_PRT("Input parameters first");
				break;
			}

			ret = bt_gatt_client_get_service(client, svc_uuid, &ancs_svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				break;
			}

			ret = bt_gatt_service_get_characteristic(ancs_svc, chr_uuid, &data_source);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				break;
			}

			ret = bt_gatt_client_set_characteristic_value_changed_cb(data_source,
					__bt_gatt_client_value_changed_cb, NULL);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_set_characteristic_value_changed_cb is failed : %d", ret);
			}

			noti_id = g_test_param.params[0];
			id = atoi(noti_id);

			value[0] = 0x00;
			memcpy(&value[1], &id, sizeof(int));
			value[5] = 0x00;
			value[6] = 0x01;
			value[7] = 0xff;
			value[8] = 0x00;
			value[9] = 0x02;
			value[10] = 0xff;
			value[11] = 0x00;
			value[12] = 0x03;
			value[13] = 0xff;
			value[14] = 0x00;
			value[15] = 0x04;
			value[16] = 0x05;
			value[17] = 0x06;
			value[18] = 0x07;

			__ancs_write_value_to_control_point(client, value, sizeof(value));

			break;
		}
#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
		case BT_UNIT_TEST_FUNCTION_GATT_FOREACH_PRIMARY_SERVICES: {
			__bt_free_gatt_services();

			ret = bt_gatt_foreach_primary_services(remote_addr, __bt_gatt_primary_service_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			if (ret == BT_ERROR_NONE)
				__select_index(&selected_service_index, "Select primary service :");
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
				__select_index(&selected_char_index, "Select index :");
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_DISCOVER_CHARACTERISTIC_DESCRIPTORS: {
			ret = bt_gatt_discover_characteristic_descriptor(characteristics[selected_char_index],
					__bt_gatt_characteristic_descriptor_discovered_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_GET_SERVICE_UUID: {
			char *uuid = NULL;
			char *name = NULL;

			if (selected_service_index < 0) {
				TC_PRT("Need to select primary service \n");
				break;
			}

			ret = bt_gatt_get_service_uuid(service_clone[selected_service_index], &uuid);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			} else {
				bt_gatt_get_uuid_specification_name(uuid, &name);
				TC_PRT("uuid: %s [%s]", uuid, name);
				g_free(name);
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
			char *name = NULL;
			unsigned char *value = NULL;
			int value_length = 0;

			ret = bt_gatt_get_characteristic_declaration(characteristics_services[0], &uuid, &value, &value_length);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			} else {
				bt_gatt_get_uuid_specification_name(uuid, &name);
				TC_PRT("uuid: %s [%s]", uuid, name);

				for (i = 0; i < value_length; i++) {
					TC_PRT("value %c", value[i]);
				}

				g_free(uuid);
				g_free(name);
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

			ret = bt_gatt_set_characteristic_value(characteristics[selected_char_index], value, 1);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_GATT_SET_CHARACTERISTIC_VALUE_REQUEST: {
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
#endif
		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
			break;

		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_HPS: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_CREATE : {
			bool auto_connect = false;

			if (g_test_param.param_count > 0) {
				if (g_strrstr(g_test_param.params[0], "true"))
					auto_connect = true;
				else
					auto_connect = false;

				__bt_free_test_param(&g_test_param);
			}

			ret = bt_gatt_connect(remote_addr, auto_connect);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			ret = bt_gatt_set_connection_state_changed_cb(__bt_hps_connection_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_DESTROY: {
			if (!hps_client)
				break;

			ret = bt_gatt_client_destroy(hps_client);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			hps_client = NULL;
			ret = bt_gatt_disconnect(remote_addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_PRINT_ALL: {
			ret = bt_gatt_client_foreach_services(hps_client,
					__bt_hps_client_svc_cb, (void *)test_id);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("returns %s\n", __bt_get_error_message(ret));
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_URI: {
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;
			char *uri = NULL;

			if (g_test_param.param_count < 1) {
							TC_PRT("Input parameters first");
							break;
			}

			uri = g_test_param.params[0];
			ret = bt_gatt_client_get_service(hps_client, HPS_UUID, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, HTTP_URI_UUID, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = __bt_gatt_client_set_value("str", uri, chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_set_value is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_client_write_value(chr,
					__bt_gatt_client_write_complete_cb, NULL);

			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_write_value is failed : %d", ret);
			}

			__bt_free_test_param(&g_test_param);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_HDR: {
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;
			char *hdr = NULL;

			if (g_test_param.param_count < 1) {
							TC_PRT("Input parameters first");
							break;
			}

			hdr = g_test_param.params[0];

			ret = bt_gatt_client_get_service(hps_client, HPS_UUID, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, HTTP_HDR_UUID, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = __bt_gatt_client_set_value("str", hdr, chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_set_value is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_client_write_value(chr,
					__bt_gatt_client_write_complete_cb, NULL);

			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_write_value is failed : %d", ret);
			}

			__bt_free_test_param(&g_test_param);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_ENTITY: {
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;
			char *entity = NULL;

			if (g_test_param.param_count < 1) {
							TC_PRT("Input parameters first");
							break;
			}

			entity = g_test_param.params[0];

			ret = bt_gatt_client_get_service(hps_client, HPS_UUID, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, HTTP_ENTITY_UUID, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = __bt_gatt_client_set_value("str", entity, chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_set_value is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_client_set_characteristic_value_changed_cb(chr,
					__bt_HP_client_cp_req_status_changed_cb, NULL);

			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_write_value is failed : %d", ret);
			}

			ret = bt_gatt_client_write_value(chr,
					__bt_gatt_client_write_complete_cb, NULL);

			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_write_value is failed : %d", ret);
			}

			__bt_free_test_param(&g_test_param);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_CP: {
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;

			if (g_test_param.param_count < 1) {
					TC_PRT("Input parameters first");
					break;
			}

			ret = bt_gatt_client_get_service(hps_client, HPS_UUID, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, HTTP_CP_UUID, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = __bt_gatt_client_set_value("uint8", g_test_param.params[0], chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_set_value is failed : %d", ret);
				__bt_free_test_param(&g_test_param);
				break;
			}

			ret = bt_gatt_client_write_value(chr,
					__bt_gatt_client_write_complete_cb, NULL);

			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_write_value is failed : %d", ret);
			}
			__bt_free_test_param(&g_test_param);
			break;
		}

		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_GET_URI: {
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;

			ret = bt_gatt_client_get_service(hps_client, HPS_UUID, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, HTTP_URI_UUID, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
					break;
			}

			ret = bt_gatt_client_read_value(chr,
					__bt_gatt_client_read_complete_cb, NULL);

			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_read_value is failed : %d", ret);
			}
			break;
		}

		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_GET_HDR: {
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;

			ret = bt_gatt_client_get_service(hps_client, HPS_UUID, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, HTTP_HDR_UUID, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
					break;
			}

			ret = bt_gatt_client_read_value(chr,
					__bt_gatt_client_read_complete_cb, NULL);

			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_read_value is failed : %d", ret);
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_GET_ENTITY: {
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;

			ret = bt_gatt_client_get_service(hps_client, HPS_UUID, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, HTTP_ENTITY_UUID, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
					break;
			}

			ret = bt_gatt_client_read_value(chr,
					__bt_gatt_client_read_complete_cb, NULL);

			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_read_value is failed : %d", ret);
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_GET_SECURITY: {
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;

			ret = bt_gatt_client_get_service(hps_client, HPS_UUID, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, HTTP_SECURITY_UUID, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
					break;
			}

			ret = bt_gatt_client_read_value(chr,
					__bt_gatt_client_read_complete_cb, NULL);

			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_read_value is failed : %d", ret);
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_SET_STATUS_NOTIFICATION: {
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;

			ret = bt_gatt_client_get_service(hps_client, HPS_UUID, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, HTTP_STATUS_UUID, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				break;
			}

			ret = bt_gatt_client_set_characteristic_value_changed_cb(chr,
					__bt_gatt_client_value_changed_cb, NULL);
			if (ret != BT_ERROR_NONE)
				TC_PRT("bt_gatt_client_set_characteristic_value_changed_cb is failed : %d", ret);
				break;
		}
		case BT_UNIT_TEST_FUNCTION_HPS_CLIENT_UNSET_STATUS_NOTIFICATION: {
			bt_gatt_h svc = NULL;
			bt_gatt_h chr = NULL;

			ret = bt_gatt_client_get_service(hps_client, HPS_UUID, &svc);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_client_get_service is failed : %d", ret);
				break;
			}

			ret = bt_gatt_service_get_characteristic(svc, HTTP_STATUS_UUID, &chr);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_gatt_service_get_characteristic is failed : %d", ret);
				break;
			}

			ret = bt_gatt_client_unset_characteristic_value_changed_cb(chr);
			if (ret != BT_ERROR_NONE)
				TC_PRT("bt_gatt_client_unset_characteristic_value_changed_cb is failed : %d", ret);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
			break;

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

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_INITIALIZE:
			ret = bt_avrcp_control_initialize(__bt_avrcp_control_connection_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_DEINITIALIZE:
			ret = bt_avrcp_control_deinitialize();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_CONNECT:
			ret = bt_avrcp_control_connect(remote_addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_DISCONNECT:
			ret = bt_avrcp_control_disconnect(remote_addr);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_PLAY:
			ret = bt_avrcp_control_send_player_command(BT_AVRCP_CONTROL_PLAY);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_STOP:
			ret = bt_avrcp_control_send_player_command(BT_AVRCP_CONTROL_STOP);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_PAUSE:
			ret = bt_avrcp_control_send_player_command(BT_AVRCP_CONTROL_PAUSE);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_FF:
			ret = bt_avrcp_control_send_player_command(BT_AVRCP_CONTROL_FAST_FORWARD);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_RW:
			ret = bt_avrcp_control_send_player_command(BT_AVRCP_CONTROL_REWIND);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_NEXT:
			ret = bt_avrcp_control_send_player_command(BT_AVRCP_CONTROL_NEXT);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_PREV: {
			ret = bt_avrcp_control_send_player_command(BT_AVRCP_CONTROL_PREVIOUS);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
			}

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_GET_REPEAT: {
			bt_avrcp_repeat_mode_e mode = BT_AVRCP_REPEAT_MODE_GROUP;
			ret = bt_avrcp_control_get_repeat_mode(&mode);
			TC_PRT("Repet mode : %d returns %s\n", mode, __bt_get_error_message(ret));
			break;
			}

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_SET_REPEAT: {
			ret = bt_avrcp_control_set_repeat_mode(2);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
			}

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_GET_SHUFFLE: {
			bt_avrcp_shuffle_mode_e mode = BT_AVRCP_SHUFFLE_MODE_ALL_TRACK;
			ret = bt_avrcp_control_get_shuffle_mode(&mode);
			TC_PRT("Shuffle mode %d returns %s\n", mode, __bt_get_error_message(ret));
			break;
			}

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_SET_SHUFFLE: {
			ret = bt_avrcp_control_set_shuffle_mode(1);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
			}

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_GET_POSITION: {
			unsigned int position = 0;
			ret = bt_avrcp_control_get_position(&position);
			TC_PRT("position %d returns %s\n", position, __bt_get_error_message(ret));
			break;
			}

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_GET_PLAY_STATUS: {
			unsigned int status = 0;
			ret = bt_avrcp_control_get_play_status(&status);
			TC_PRT("status %d returns %s\n", status, __bt_get_error_message(ret));
			break;
			}

		case BT_UNIT_TEST_FUNCTION_AVRCP_CONTROL_GET_TRACK_INFO: {
			bt_avrcp_metadata_attributes_info_s *track;
			ret = bt_avrcp_control_get_track_info(&track);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			if (ret == BT_ERROR_NONE) {
				TC_PRT("Title. %s\n", track->title);
				TC_PRT("Artist %s\n", track->artist);
				TC_PRT("Album %s\n", track->album);
				TC_PRT("Genre %s\n", track->genre);
				TC_PRT("Total tracks %d\n", track->total_tracks);
				TC_PRT("Number %d\n", track->number);
				TC_PRT("Duration %d\n", track->duration);
				bt_avrcp_control_free_track_info(track);
			}
			break;
			}

		case BT_UNIT_TEST_FUNCTION_AVRCP_SET_SONG_POSITION_CHANGED_CB:
			ret = bt_avrcp_set_song_position_changed_cb(__bt_avrcp_song_position_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_SET_PLAY_STATE_CHANGED_CB:
			ret = bt_avrcp_set_play_status_changed_cb(__bt_avrcp_player_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_SET_TRACK_INFO_CHANGED_CB:
			ret = bt_avrcp_set_track_info_changed_cb(__bt_avrcp_track_info_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_SONG_POSITION_CHANGED_CB:
			ret = bt_avrcp_unset_song_position_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_PLAY_STATE_CHANGED_CB:
			ret = bt_avrcp_unset_play_status_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_AVRCP_UNSET_TRACK_INFO_CHANGED_CB:
			ret = bt_avrcp_unset_track_info_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
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
		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
			break;

		default:
			break;
		}

		break;
	}

	case BT_UNIT_TEST_TABLE_IPSP: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_IPSP_REGISTER: {
			bt_gatt_h service = NULL;
			char *service_uuid = "1820"; // IPSP Service

			ret = bt_gatt_server_initialize();
			TC_PRT("bt_gatt_server_initialize : %s \n", __bt_get_error_message(ret));

			if (server == NULL) {
				ret = bt_gatt_server_create(&server);
				TC_PRT("bt_gatt_server_create : %s \n", __bt_get_error_message(ret));
			}

			ret = bt_gatt_service_create(service_uuid, BT_GATT_SERVICE_TYPE_PRIMARY, &service);
			TC_PRT("bt_gatt_service_create : %s \n", __bt_get_error_message(ret));

			ret = bt_gatt_server_register_service(server, service);
			TC_PRT("bt_gatt_server_register_service : %s\n", __bt_get_error_message(ret));
			ipsp_h.svc = service;

			break;
		}
		case BT_UNIT_TEST_FUNCTION_IPSP_UNREGISTER:
			if (server != NULL && ipsp_h.svc != NULL) {
				ret = bt_gatt_server_unregister_service(server, ipsp_h.svc);
				TC_PRT("bt_gatt_server_unregister_service : returns %s\n", __bt_get_error_message(ret));
				if (ret == BT_ERROR_NONE)
					ipsp_h.svc = NULL;
			} else {
				TC_PRT("Gatt Server or IPSP not registered !");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_IPSP_INITIALIZE:
			/* Initialize IPSP server */
			if (server != NULL && ipsp_h.svc != NULL) {
				ret = _bt_le_ipsp_initialize(&__bt_le_ipsp_init_state_changed_cb, NULL);
				TC_PRT("bt_le_ipsp_initialize : returns %s\n", __bt_get_error_message(ret));
			} else {
				TC_PRT("Gatt Server or IPSP not registered !");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_IPSP_DEINITIALIZE:
			/* De-Initialize IPSP server */
			ret = _bt_le_ipsp_deinitialize();
			TC_PRT("bt_le_ipsp_deinitialize : returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_IPSP_CONNECT:
			ret = _bt_le_ipsp_connect(remote_addr);
			break;
		case BT_UNIT_TEST_FUNCTION_IPSP_DISCONNECT:
			ret = _bt_le_ipsp_disconnect(remote_addr);
			break;
		case BT_UNIT_TEST_FUNCTION_IPSP_START_ADVERTISING: {
			const char *ipsp_svc_uuid_16 = "1820";
			ret = _bt_le_ipsp_is_initialized();
			if (ret != BT_ERROR_NONE) {
				TC_PRT("bt_le_ipsp_add_advertising_data: returns %s\n", __bt_get_error_message(ret));
				break;
			}
			/* Add IPSP service in advertising data */
			advertiser = advertiser_list[advertiser_index];
			if (advertiser == NULL) {
				ret = bt_adapter_le_create_advertiser(&advertiser);
				if (ret != BT_ERROR_NONE) {
					TC_PRT("created le advertiser(%d)", ret);
					break;
				}

				advertiser_list[advertiser_index] = advertiser;
				ret = bt_adapter_le_set_advertising_device_name(advertiser,
						BT_ADAPTER_LE_PACKET_SCAN_RESPONSE, true);
				if (ret != BT_ERROR_NONE) {
					TC_PRT("set device name [0x%04x]", ret);
					break;
				}
			}

			ret = bt_adapter_le_add_advertising_service_solicitation_uuid(advertiser,
					BT_ADAPTER_LE_PACKET_ADVERTISING, ipsp_svc_uuid_16);
			if (ret != BT_ERROR_NONE) {
				TC_PRT("add service_solicitation_uuid [0x%04x]", ret);
				break;
			}

			/* Start advertising IPSP service */
			bt_adapter_le_advertising_state_changed_cb cb;

			if (advertiser_index == 0) cb = __bt_adapter_le_advertising_state_changed_cb;
			else if (advertiser_index == 1) cb = __bt_adapter_le_advertising_state_changed_cb_2;
			else cb = __bt_adapter_le_advertising_state_changed_cb_3;

			advertiser = advertiser_list[advertiser_index];

			advertiser_index++;
			advertiser_index %= 3;

			TC_PRT("advertiser: %p", advertiser);

			ret = bt_adapter_le_start_advertising_new(advertiser, cb, NULL);
			if (ret < BT_ERROR_NONE)
				TC_PRT("failed with [0x%04x]", ret);

			break;
		}
		case BT_UNIT_TEST_FUNCTION_IPSP_SET_CONNECTION_STATE_CHANGED_CB:
			ret = _bt_le_ipsp_set_connection_state_changed_cb(__bt_le_ipsp_connection_state_changed_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;

		case BT_UNIT_TEST_FUNCTION_IPSP_UNSET_CONNECTION_STATE_CHANGED_CB:
			ret = _bt_le_ipsp_unset_connection_state_changed_cb();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
			break;

		default:
			break;
		}

		break;
	}

	case BT_UNIT_TEST_TABLE_HDP : {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_HDP_REGISTER_SINK_APP:
			ret = bt_hdp_register_sink_app(0x1007, &appid);
			TC_PRT("bt_hdp_register_sink_app : returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_HDP_UNREGISTER_SINK_APP:
			ret = bt_hdp_unregister_sink_app(appid);
			TC_PRT("bt_hdp_unregister_sink_app : returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_HDP_CONNECT_TO_SOURCE:
			ret = bt_hdp_connect_to_source(remote_addr, appid);
			TC_PRT("bt_hdp_connect_to_source : returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_HDP_DISCONNECT:
			ret = bt_hdp_disconnect(remote_addr, channel_hdp);
			TC_PRT("bt_hdp_disconnect : returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_HDP_SEND_DATA:
			ret = bt_hdp_send_data(channel_hdp, data_hdp, strlen(data_hdp));
			TC_PRT("bt_hdp_send_data : returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_HDP_SET_CONNECTION_CB:
			ret = bt_hdp_set_connection_state_changed_cb(__bt_hdp_connected_cb, __bt_hdp_disconnected_cb, NULL);
			TC_PRT("bt_hdp_set_connection_state_changed_cb : returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_HDP_UNSET_CONNECTION_CB:
			ret = bt_hdp_unset_connection_state_changed_cb();
			TC_PRT("bt_hdp_unset_connection_state_changed_cb : returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_HDP_SET_DATA_RECEIVED_CB:
			ret = bt_hdp_set_data_received_cb(__bt_hdp_data_received_cb, NULL);
			TC_PRT("bt_hdp_set_data_received_cb : returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_HDP_UNSET_DATA_RECEIVED_CB:
			ret = bt_hdp_unset_data_received_cb();
			TC_PRT("bt_hdp_unset_data_received_cb : returns %s\n", __bt_get_error_message(ret));
			break;
		default:
			break;
		}
			break;
	}
#ifdef TIZEN_WEARABLE
	case BT_UNIT_TEST_TABLE_HF: {
		switch (test_id) {
		case BT_UNIT_TEST_FUNCTION_HF_INITIALIZE:
			ret = bt_hf_initialize();
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_DEINITIALIZE:
			ret = bt_hf_deinitialize();
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_NOTIFY_CALL_ANSWER:
			ret = bt_hf_notify_call_event(BT_HF_CALL_EVENT_ANSWER, "9663868998");
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_NOTIFY_SPEAKER_GAIN:
			ret = bt_hf_notify_speaker_gain(10);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_SET_SPEAKER_GAIN_CB:
			ret = bt_hf_set_speaker_gain_changed_cb(__bt_hf_speaker_gain_changed_cb, NULL);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_UNSET_SPEAKER_GAIN_CB:
			ret = bt_hf_unset_speaker_gain_changed_cb();
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_NOTIFY_VOICE_REC_TRUE:
			ret = bt_hf_notify_voice_recognition_state(true);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_NOTIFY_CALL_TERM:
			ret = bt_hf_notify_call_event(BT_HF_CALL_EVENT_IDLE, "9663868998");
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_NOTIFY_CALL_INIT:
			ret = bt_hf_notify_call_event(BT_HF_CALL_EVENT_DIAL, "9663868998");
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_NOTIFY_VOICE_REC_FALSE:
			ret = bt_hf_notify_voice_recognition_state(false);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_IS_SCO_OPENED: {
			bool is_opened = false;
			ret = bt_hf_is_sco_opened(&is_opened);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			TC_PRT("Sco is opened [%s]", is_opened ? "YES" : "NO");
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HF_GET_CODEC_ID: {
			unsigned int codec_id = 0;
			ret = bt_hf_get_codec_id(&codec_id);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			TC_PRT("Codec ID [%d]", codec_id);
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HF_GET_CALL_STATUS_INFO_LIST: {
			GSList *list;
			ret = bt_hf_get_call_status_info_list(&list);

			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else {
				for (; list; list = g_slist_next(list)) {
					bt_hf_call_status_info_s *call_info = list->data;
					TC_PRT("Call info [No:%s, Dir:%d, Stat:%d, Mpt:%d, Idx:%d]",
						call_info->number, call_info->direction, call_info->status, call_info->multi_party, call_info->index);
				}
				bt_hf_free_call_status_info_list(list);
			}
			break;
		}
		case BT_UNIT_TEST_FUNCTION_HF_SET_SCO_CHANGED_CB:
			ret = bt_hf_set_sco_state_changed_cb(
					__bt_hf_sco_state_changed_cb, NULL);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_UNSET_SCO_CHANGED_CB:
			ret = bt_hf_unset_sco_state_changed_cb();
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_SET_CALL_EVENT_CB:
			ret = bt_hf_set_call_handling_event_cb(__bt_hf_set_call_handling_event_cb, NULL);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_UNSET_CALL_EVENT_CB:
			ret = bt_hf_unset_call_handling_event_cb();
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_SET_CALL_STATUS_UPDATED_EVENT_CB:
			ret = bt_hf_set_call_status_updated_event_cb(__bt_hf_call_status_updated_event_cb, NULL);
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_HF_UNSET_CALL_STATUS_UPDATED_EVENT_CB:
			ret = bt_hf_unset_call_status_updated_event_cb();
			if (ret < BT_ERROR_NONE) {
				TC_PRT("failed with [0x%04x]", ret);
			} else if (ret == BT_ERROR_NONE) {
				TC_PRT("Success");
			}
			break;
		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
			break;

		default:
			break;
		}

		break;
	}
	case BT_UNIT_TEST_TABLE_PBAP_CLIENT: {
		switch (test_id) {

		/*PBAP Test Cases */
		case BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_INITIALIZE:
			ret = bt_pbap_init();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_DEINITIALIZE:
			ret = bt_pbap_deinit();
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		case BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_CONNECT: {
			ret = bt_pbap_connect(remote_addr, __bt_pbap_enabled_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_DISCONNECT: {
			ret = bt_pbap_disconnect(remote_addr, __bt_pbap_enabled_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_GETSIZE: {
			ret = bt_pbap_get_phonebook_size(remote_addr, 0, 0, __bt_pbap_phonebook_size_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_PHONEBOOKPULL: {
			long long unsigned fields = BT_PBAP_FIELD_PHOTO | BT_PBAP_FIELD_EMAIL | BT_PBAP_FIELD_ORG;
			ret = bt_pbap_get_phonebook(remote_addr, 0, 0, 0, 0, 0, 100,
					fields, __bt_pbap_phonebook_pull_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_VCARDLIST: {
			ret = bt_pbap_get_list(remote_addr, 0, 0, 0, 0, 100,
					__bt_pbap_vcard_list_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_VCARDPULL: {
			long long unsigned fields = BT_PBAP_FIELD_PHOTO | BT_PBAP_FIELD_EMAIL | BT_PBAP_FIELD_ORG;
			ret = bt_pbap_pull_vcard(remote_addr, 0, 0, 0, 0, fields,
					__bt_pbap_phonebook_pull_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_PHONEBOOKSEARCH: {
			ret = bt_pbap_phonebook_search(remote_addr, 0, 0,
					0, "abc",
					0, 0, 100,
					__bt_pbap_vcard_list_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}
		case BT_UNIT_TEST_FUNCTION_PBAP_CLIENT_VCARDLIST_MAXLIST_ZERO: {
			ret = bt_pbap_get_list(remote_addr, 0, 0, 0, 0, 0,
					__bt_pbap_vcard_list_cb, NULL);
			TC_PRT("returns %s\n", __bt_get_error_message(ret));
			break;
		}

		case BT_UNIT_TEST_FUNCTION_ACTIVATE_FLAG_TO_SET_PARAMETERS:
			need_to_set_params = true;
			TC_PRT("Select the function again");
			break;

		default:
			break;
		}

		break;
	}
#endif
	case BT_UNIT_TEST_TABLE_AUTOMATED_TEST: {
		static unsigned int delay = 0;
		bt_onoff_cnt = 0;
		bt_onoff_cnt_success = 0;
		bt_onoff_cnt_fail = 0;

		total_time = 0;
		gettimeofday(&check_time, NULL);

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

	if (need_to_set_params == true) {
		if (g_test_id == -1) {
			test_id = atoi(buf);
			g_test_id = test_id;
		}
		test_set_params(g_test_id, buf);
		return TRUE;
	} else {
		TC_PRT("%s", buf);
		test_id = atoi(buf);

		g_test_id = -1;
		need_to_set_params = false;
	}

	if (current_tc_table == BT_UNIT_TEST_TABLE_MAIN) {
		if (buf[0] == '0' && buf[2] == 0)
			current_tc_table = BT_UNIT_TEST_TABLE_SET_ADDRESS;
		else if(test_id >= BT_UNIT_TEST_TABLE_ADAPTER && test_id < BT_UNIT_TEST_TABLE_FINISH)
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

void sig_handler(int signo)
{
	if (signo == SIGINT) {
		if (bt_onoff_cnt > 0)
			__bt_print_repeat_test_final_summary();

		bt_deinitialize();
		exit(0);
	}
}

int main()
{
	GIOChannel *key_io;

//	g_type_init();
	current_tc_table = BT_UNIT_TEST_TABLE_MAIN;

	key_io = g_io_channel_unix_new(fileno(stdin));

	g_io_channel_set_encoding(key_io, NULL, NULL);
	g_io_channel_set_flags(key_io, G_IO_FLAG_NONBLOCK, NULL);

	g_io_add_watch(key_io, G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL,
			key_event_cb, NULL);

	g_io_channel_unref(key_io);

	main_loop = g_main_loop_new(NULL, FALSE);

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		TC_PRT("\n can't catch SIGINT\n");

	g_main_loop_run(main_loop);

	bt_deinitialize();

	return 0;
}
