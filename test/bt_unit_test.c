/*
 * Copyright (c) 2012-2013 Samsung Electronics Co., Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the License);
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/**
 * @file       bt_unit_test.c
 * @brief      This is the source file for capi unit test.
 */

#include <stdio.h>
#include <string.h>
#include <glib.h>
#include <dbus/dbus-glib.h>

#include "bluetooth.h"
#include "bluetooth-telephony-api.h"

#define BUFFER_LEN 10
#define MAX_SERVICES 10
#define PRT(format, args...) printf("%s:%d() "format, __FUNCTION__, __LINE__, ##args)
#define TC_PRT(format, args...) PRT(format"\n", ##args)

#define DEVICE_ADDRESS "F6:FB:8F:D8:C8:7C"
const char *spp_uuid = "00001101-0000-1000-8000-00805F9B34FB";
const char *opp_uuid = "00001105-0000-1000-8000-00805f9b34fb";

static int server_fd;
static int client_fd;

int service_index;
int char_service_index;

bt_gatt_attribute_h service_clone[MAX_SERVICES];
bt_gatt_attribute_h characteristics[MAX_SERVICES];
bt_gatt_attribute_h characteristics_services[MAX_SERVICES];

static unsigned char *hash = NULL;
static unsigned char *randomizer = NULL;
int hash_len;
int rand_len;

bt_call_list_h call_list;
GMainLoop *main_loop = NULL;

typedef struct {
	const char *tc_name;
	int tc_code;
} tc_table_t;

tc_table_t tc_table[] = {
	/* Adapter functions */
	{"bt_initialize"			, 1},
	{"bt_deinitialize"			, 2},
	{"bt_adapter_get_state"			, 3},
	{"bt_adapter_enable"			, 4},
	{"bt_adapter_disable"			, 5},
	{"bt_adapter_start_device_discovery"	, 6},
	{"bt_adapter_stop_device_discovery"	, 7},
	{"bt_adapter_is_discovering"		, 8},
	{"bt_adapter_get_bonded_device_info"	, 9},
	{"bt_adapter_is_service_used"		, 10},
	{"bt_adapter_get_visibility"		, 11},
	{"bt_adapter_set_device_discovery_state_changed_cb"	, 12},
	{"bt_adapter_unset_device_discovery_state_changed_cb"	, 13},
	{"bt_adapter_get_local_oob_data" , 14},
	{"bt_adapter_set_remote_oob_data" , 15},
	{"bt_adapter_remove_remote_oob_data"	, 16},
	{"bt_adapter_start_discover_devices"	, 17},
	{"bt_adapter_set_visibility_mode_changed_cb"    , 18},
	{"bt_adapter_unset_visibility_mode_changed_cb"  , 19},


	/* Socket functions */
	{"bt_socket_create_rfcomm"		, 50},
	{"bt_socket_destroy_rfcomm"		, 51},
	{"bt_socket_listen_and_accept_rfcomm"	, 52},
	{"bt_socket_listen"			, 53},
	{"bt_socket_accept"			, 54},
	{"bt_socket_reject"			, 55},
	{"bt_socket_connect_rfcomm"		, 56},
	{"bt_socket_disconnect_rfcomm"		, 57},
	{"bt_socket_send_data"			, 58},
	{"bt_socket_set_data_received_cb"	, 59},
	{"bt_socket_unset_data_received_cb"	, 60},
	{"bt_socket_set_connection_requested_cb"	, 61},
	{"bt_socket_unset_connection_requested_cb"	, 62},
	{"bt_socket_set_connection_state_changed_cb"	, 63},
	{"bt_socket_unset_connection_state_changed_cb"	, 64},

	/* OPP functions */
	{"bt_opp_client_initialize"		, 70},
	{"bt_opp_client_deinitialize"		, 71},
	{"bt_opp_client_add_file"		, 72},
	{"bt_opp_client_clear_files"		, 73},
	{"bt_opp_client_push_files"		, 74},
	{"bt_opp_client_cancel_push"		, 75},

	/* AG functions */
	{"bt_audio_initialize"		, 80},
	{"bt_audio_deinitialize"		, 81},
	{"bt_audio_connect"		, 82},
	{"bt_audio_disconnect"		, 83},
	{"bt_audio_set_connection_state_changed_cb",		84},
	{"bt_audio_unset_connection_state_changed_cb", 	85},
	{"bt_ag_open_sco" 		, 86},
	{"bt_ag_close_sco" 		, 87},
	{"bt_ag_is_sco_opened"	, 88},
	{"bt_ag_set_sco_state_changed_cb"	, 89},
	{"bt_ag_unset_sco_state_changed_cb"	, 90},
	{"BT_AG_CALL_EVENT_IDLE"		, 91},
	{"BT_AG_CALL_EVENT_ANSWERED"	, 92},
	{"BT_AG_CALL_EVENT_DIALING"		, 93},
	{"BT_AG_CALL_EVENT_ALERTING"	, 94},
	{"BT_AG_CALL_EVENT_INCOMING"	, 95},
	{"bt_call_list_create" 			, 96},
	{"bt_call_list_destroy" 		, 97},
	{"bt_call_list_reset" 			, 98},
	{"bt_call_list_add"	 			, 99},
	{"bt_ag_notify_call_list"			, 100},
	{"bt_ag_set_call_handling_event_cb"	, 101},
	{"bt_ag_is_nrec_enabled"	, 102},

	/* Network functions */
	{"bt_nap_activate"		, 110},
	{"bt_nap_deactivate"		, 111},
	{"bt_nap_disconnect_all"	, 112},

	/* Device functions */
	{"bt_device_set_authorization (true)"	, 120},
	{"bt_device_set_authorization (false)"	, 121},
	{"bt_device_set_authorization_changed_cb"	, 122},
	{"bt_device_unset_authorization_changed_cb"	, 123},
	{"bt_device_set_connection_state_changed_cb"	, 124},
	{"bt_device_unset_connection_state_changed_cb"	, 125},
	{"bt_device_foreach_connected_profiles"	, 126},
	{"bt_device_set_bond_created_cb" , 127},
	{"bt_device_create_bond" , 128},

	/* Gatt functions */
	{"bt_gatt_foreach_primary_services"	, 140},
	{"bt_gatt_discover_characteristics"	, 141},
	{"bt_gatt_get_service_uuid"		, 142},
	{"bt_gatt_foreach_included_services"		, 143},
	{"bt_gatt_set_characteristic_changed_cb"	, 144},
	{"bt_gatt_unset_characteristic_changed_cb"	, 145},
	{"bt_gatt_get_characteristic_declaration"	, 146},
	{"bt_gatt_set_characteristic_value"	, 147},

	/* AVRCP functions */
	{"bt_avrcp_target_initialize"	, 160},
	{"bt_avrcp_target_deinitialize"	, 161},
	{"bt_avrcp_set_equalizer_state_changed_cb"	, 162},
	{"bt_avrcp_set_repeat_mode_changed_cb"	, 163},
	{"bt_avrcp_set_shuffle_mode_changed_cb"	, 164},
	{"bt_avrcp_set_scan_mode_changed_cb"	, 165},
	{"bt_avrcp_unset_equalizer_state_changed_cb"	, 166},
	{"bt_avrcp_unset_repeat_mode_changed_cb"	, 167},
	{"bt_avrcp_unset_shuffle_mode_changed_cb"	, 168},
	{"bt_avrcp_unset_scan_mode_changed_cb"	, 169},

	/* -----------------------------------------*/
	{"Finish"				, 0x00ff},
	{NULL					, 0x0000},
};

void tc_usage_print(void)
{
	int i = 0;

	while (tc_table[i].tc_name) {
		if (tc_table[i].tc_code != 0x00ff) {
			TC_PRT("Key %d : usage %s", tc_table[i].tc_code,
						tc_table[i].tc_name);
		} else {
			TC_PRT("Key %d : usage %s\n\n", 0x00ff,
						tc_table[i].tc_name);
		}

		i++;
	}
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
       TC_PRT("visibility_mode: %d", visibility_mode);
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
		return;
	}

	for (i = 0; i < discovery_info->service_count; i++) {
		TC_PRT("uuid: %s", discovery_info->service_uuid[i]);
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
	TC_PRT("result: %d", result);
	TC_PRT("connection_state: %d", connection_state);

	if (connection == NULL) {
		TC_PRT("No connection data!");
		return;
	}

	TC_PRT("socket fd: %d", connection->socket_fd);
	TC_PRT("role: %d", connection->local_role);
	TC_PRT("remote address: %s", connection->remote_address);
	TC_PRT("service_uuid: %s", connection->service_uuid);
}

void __bt_opp_client_push_responded_cb(int result,
					const char *remote_address,
					void *user_data)
{
	TC_PRT("result: %d", result);
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
	TC_PRT("result: %d", result);
	TC_PRT("remote_address: %s", remote_address);
}

void __bt_audio_connection_state_changed_cb(int result,
					bool connected, const char *remote_address,
					bt_audio_profile_type_e type, void *user_data)
{
	TC_PRT("connected [%d]", connected);
	TC_PRT("address [%s]", remote_address);
	TC_PRT("type [%d]", type);
}

void __bt_ag_sco_state_changed_cb(int result, bool opened, void *user_data)
{
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
						const char *remote_address,
						void *user_data)
{
	TC_PRT("__bt_device_authorization_changed_cb [%d]", connected);
	TC_PRT("address: %s", remote_address);
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
		for(i=0; i<device_info->service_count; i++)
		{
			TC_PRT("Callback: service[%d] - %s", i+1, device_info->service_uuid[i]);
		}
		TC_PRT("Callback: is_bonded - %d.", device_info->is_bonded);
		TC_PRT("Callback: is_connected - %d.", device_info->is_connected);
	}
	else
	{
		TC_PRT("Callback: Creating a bond is failed.");
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
	TC_PRT("__bt_gatt_characteristics_discovered_cb: %d", result);
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

int test_input_callback(void *data)
{
	int ret = 0;
	long test_id = (long)data;

	switch (test_id) {
	case 0x00ff:
		TC_PRT("Finished");
		g_main_loop_quit(main_loop);
		break;

	case 1:
		ret = bt_initialize();
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	case 2:
		ret = bt_deinitialize();
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	case 3: {
		bt_adapter_state_e state = BT_ADAPTER_DISABLED;

		ret = bt_adapter_get_state(&state);
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);

		TC_PRT("state: %d", state);
		break;
	}
	case 4:
		ret = bt_adapter_enable();
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	case 5:
		ret = bt_adapter_disable();
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	case 6:
		ret = bt_adapter_start_device_discovery();
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	case 7:
		ret = bt_adapter_stop_device_discovery();
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	case 8: {
		bool is_discovering = FALSE;
		ret = bt_adapter_is_discovering(&is_discovering);
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		else
			TC_PRT("is_discovering: %d", is_discovering);

		break;
	}
	case 9: {
		char *address;
		bt_device_info_s *device_info = NULL;

		address = g_strdup("00:19:0E:01:61:17");

		ret = bt_adapter_get_bonded_device_info((const char *)address,
							&device_info);
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);

		g_free(address);

		if (device_info) {
			TC_PRT("address: %s", device_info->remote_address);
			TC_PRT("name: %s", device_info->remote_name);
		}

		bt_adapter_free_device_info(device_info);
		break;
	}
	case 10: {
		bool used = FALSE;

		ret = bt_adapter_is_service_used(opp_uuid, &used);
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);

		TC_PRT("used: %d", used);
		break;
	}
	case 11: {
		bt_adapter_visibility_mode_e visibility_mode = BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE;
		int time = 0;

		ret = bt_adapter_get_visibility(&visibility_mode, &time);
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);

		TC_PRT("mode: %d", visibility_mode);
		TC_PRT("time: %d", time);
		break;
	}
	case 12:
		ret = bt_adapter_set_device_discovery_state_changed_cb(__bt_adapter_device_discovery_state_changed_cb, NULL);
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	case 13:
		ret = bt_adapter_unset_device_discovery_state_changed_cb();
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;

	case 14: {
		ret = bt_adapter_get_local_oob_data(&hash, &randomizer, &hash_len, &rand_len);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		} else {
			TC_PRT("hash = [%s]", hash);
			TC_PRT("randomizer = [%s]", randomizer);
		}
		break;
	}

	case 15: {
		char *address;

		address = g_strdup("00:02:18:2F:78:32");

		TC_PRT("remote address = [%s]", address);

		ret = bt_adapter_set_remote_oob_data(address, hash, randomizer, hash_len, rand_len);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}

	case 16: {
		char *address;

		address = g_strdup("00:02:18:2F:78:32");

		TC_PRT("remote address = [%s]", address);

		ret = bt_adapter_remove_remote_oob_data(address);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}

	case 17: {
		ret = bt_adapter_start_discover_devices(BT_ADAPTER_DEVICE_DISCOVERY_BREDR);
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	}

	case 18:
		ret = bt_adapter_set_visibility_mode_changed_cb(__bt_adapter_device_visibility_mode_changed_cb, NULL);
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	case 19:
			ret = bt_adapter_unset_visibility_mode_changed_cb();
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;


	/* Socket functions */
	case 50: {
		int socket_fd = 0;

		ret = bt_socket_create_rfcomm(spp_uuid, &socket_fd);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		} else {
			TC_PRT("socket_fd: %d", socket_fd);
			server_fd = socket_fd;
		}
		break;
	}
	case 51:
		ret = bt_socket_destroy_rfcomm(server_fd);
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	case 52:
		ret = bt_socket_listen_and_accept_rfcomm(server_fd, 1);
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	case 53:
		ret = bt_socket_listen(server_fd, 1);
		if (ret < BT_ERROR_NONE)
			TC_PRT("failed with [0x%04x]", ret);
		break;
	case 54: {
		int socket_fd = 0;

		ret = bt_socket_accept(server_fd, &socket_fd);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		} else {
			TC_PRT("socket_fd: %d", socket_fd);
			client_fd = socket_fd;
		}
		break;
	}
	case 55:
		ret = bt_socket_reject(server_fd);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 56: {
		char *address;

		address = g_strdup("00:02:48:F4:3E:D2");

		ret = bt_socket_connect_rfcomm(address, spp_uuid);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}

		g_free(address);
		break;
	}
	case 57:
		ret = bt_socket_disconnect_rfcomm(client_fd);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 58:
		ret = bt_socket_send_data(client_fd, "Sending test\0", 20);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 59:
		ret = bt_socket_set_data_received_cb(__bt_socket_data_received_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 60:
		ret = bt_socket_unset_data_received_cb();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 61:
		ret = bt_socket_set_connection_requested_cb(__bt_socket_connection_requested_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 62:
		ret = bt_socket_unset_connection_requested_cb();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 63:
		ret = bt_socket_set_connection_state_changed_cb(__bt_socket_connection_state_changed_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 64:
		ret = bt_socket_unset_connection_state_changed_cb();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 70:
		ret = bt_opp_client_initialize();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 71:
		ret = bt_opp_client_deinitialize();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 72:
		ret = bt_opp_client_add_file("/opt/media/Images/image1.jpg");
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 73:
		ret = bt_opp_client_clear_files();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 74: {
		char *address;

		address = g_strdup("00:02:37:A9:17:9E");

		ret = bt_opp_client_push_files(address,__bt_opp_client_push_responded_cb,
					__bt_opp_client_push_progress_cb,
					__bt_opp_client_push_finished_cb,
					NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 75:
		ret = bt_opp_client_cancel_push();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 80:
		ret = bt_audio_initialize();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 81:
		ret = bt_audio_deinitialize();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 82: {
		char *address;
		address = g_strdup("11:11:22:33:33:33");

		ret = bt_audio_connect(address,  BT_AUDIO_PROFILE_TYPE_HSP_HFP);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 83: {
		char *address;
		address = g_strdup("11:11:22:33:33:33");

		ret = bt_audio_disconnect(address,  BT_AUDIO_PROFILE_TYPE_HSP_HFP);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 84:
		ret = bt_audio_set_connection_state_changed_cb(
					__bt_audio_connection_state_changed_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 85:
		ret = bt_audio_unset_connection_state_changed_cb();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 86:
		ret = bt_ag_open_sco();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 87:
		ret = bt_ag_close_sco();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 88: {
		bool opened = false;
		ret = bt_ag_is_sco_opened(&opened);
		TC_PRT("opend[%s]", opened ? "YES" : "NO");
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 89:
		ret = bt_ag_set_sco_state_changed_cb(__bt_ag_sco_state_changed_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 90:
		ret = bt_ag_unset_sco_state_changed_cb();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 91: {
		unsigned int call_id = 1;

		ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_IDLE, call_id, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 92: {
		unsigned int call_id = 1;
		bool sco_required = true;

		ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_ANSWERED, call_id, NULL);
		if (sco_required)
			bt_ag_open_sco();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 93: {
		unsigned int call_id = 1;
		bool sco_required = TRUE;
		const char *phone_number;

		phone_number = g_strdup("01046500173");
		ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_DIALING,
					call_id, phone_number);
		if (sco_required)
			bt_ag_open_sco();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 94: {
		unsigned int call_id = 1;

		ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_ALERTING,
					call_id, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 95: {
		unsigned int call_id = 1;
		const char *phone_number;

		phone_number = g_strdup("01046500173");
		ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_INCOMING,
					call_id, phone_number);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 96:
		ret = bt_call_list_create(&call_list);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 97:
		ret = bt_call_list_destroy(call_list);
		call_list = NULL;
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 98:
		ret = bt_call_list_reset(call_list);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 99:
		ret = bt_call_list_add(call_list, 1, BT_AG_CALL_STATE_ACTIVE);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 100:
		ret = bt_ag_notify_call_list(call_list);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 101:
		ret = bt_ag_set_call_handling_event_cb(
					__bt_ag_set_call_handling_event_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 102: {
		bool enabled = false;
		ret = bt_ag_is_nrec_enabled(&enabled);
		TC_PRT("opend[%s]", enabled ? "YES" : "NO");
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 110:
		ret = bt_nap_activate();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 111:
		ret = bt_nap_deactivate();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 112:
		ret = bt_nap_disconnect_all();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 120: {
		char *address;

		address = g_strdup("00:1B:66:01:23:1C");

		ret = bt_device_set_authorization(address, BT_DEVICE_AUTHORIZED);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 121: {
		char *address;

		address = g_strdup("00:1B:66:01:23:1C");

		ret = bt_device_set_authorization(address, BT_DEVICE_UNAUTHORIZED);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}
	case 122:
		ret = bt_device_set_authorization_changed_cb(__bt_device_authorization_changed_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 123:
		ret = bt_device_unset_authorization_changed_cb();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 124:
		ret = bt_device_set_connection_state_changed_cb(__bt_device_connection_state_changed_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 125:
		ret = bt_device_unset_connection_state_changed_cb();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 126: {
		char *address;

		address = g_strdup("00:1B:66:01:23:1C");

		ret = bt_device_foreach_connected_profiles(address, __bt_device_connected_profile, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}

	case 127 : {
		ret = bt_device_set_bond_created_cb(__bt_device_bond_created_cb,
			NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}

	case 128 : {
		char *address;

		address = g_strdup(DEVICE_ADDRESS);

		ret = bt_device_create_bond(address);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}

	case 140: {
		char *address;

		address = g_strdup("D5:FE:81:38:39:B9");

		__bt_free_gatt_services();

		ret = bt_gatt_foreach_primary_services(address, __bt_gatt_primary_service_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}

	case 141: {
		__bt_free_gatt_characteristics();

		ret = bt_gatt_discover_characteristics(service_clone[1], __bt_gatt_characteristics_discovered_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}

	case 142: {
		char *uuid = NULL;

		ret = bt_gatt_get_service_uuid(service_clone[1], &uuid);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		} else {
			TC_PRT("uuid: %s", uuid);
			g_free(uuid);
		}
		break;
	}

	case 143: {
		__bt_free_gatt_characteristics_services();

		ret = bt_gatt_foreach_included_services(service_clone[1], __bt_gatt_included_service_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}

	case 144: {
		ret = bt_gatt_set_characteristic_changed_cb(service_clone[1], __bt_gatt_characteristic_changed_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}

	case 145: {
		ret = bt_gatt_unset_characteristic_changed_cb(service_clone[1]);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}

	case 146: {
		int i;
		char *uuid = NULL;
		unsigned char *value = NULL;
		int value_length = 0;

		ret = bt_gatt_get_characteristic_declaration(characteristics_services[0], &uuid, &value, &value_length);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
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

	case 147: {
		unsigned char value[5] = { 0, 1, 2, 3, 4 };

		ret = bt_gatt_set_characteristic_value(characteristics_services[0], value, 5);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	}

	case 160:
		ret = bt_avrcp_target_initialize(__bt_avrcp_target_connection_state_changed_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 161:
		ret = bt_avrcp_target_deinitialize();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 162:
		ret = bt_avrcp_set_equalizer_state_changed_cb(__bt_avrcp_equalizer_state_changed_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 163:
		ret = bt_avrcp_set_repeat_mode_changed_cb(__bt_avrcp_repeat_mode_changed_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 164:
		ret = bt_avrcp_set_shuffle_mode_changed_cb(__bt_avrcp_shuffle_mode_changed_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 165:
		ret = bt_avrcp_set_scan_mode_changed_cb(__bt_avrcp_scan_mode_changed_cb, NULL);
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 166:
		ret = bt_avrcp_unset_equalizer_state_changed_cb();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 167:
		ret = bt_avrcp_unset_repeat_mode_changed_cb();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;

	case 168:
		ret = bt_avrcp_unset_shuffle_mode_changed_cb();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
	case 169:
		ret = bt_avrcp_unset_scan_mode_changed_cb();
		if (ret < BT_ERROR_NONE) {
			TC_PRT("failed with [0x%04x]", ret);
		}
		break;
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

	gsize len = 0;
	long test_id;

	memset(buf, 0, sizeof(buf));

	if (g_io_channel_read_chars(chan, buf, sizeof(buf),
				&len, NULL) == G_IO_STATUS_ERROR) {
		TC_PRT("IO Channel read error");
		return FALSE;
	}

	TC_PRT("%s", buf);
	tc_usage_print();

	test_id = atoi(buf);

	if (test_id)
		g_idle_add(test_input_callback, (void *)test_id);

	return TRUE;
}

int main()
{
	GIOChannel *key_io;

#if !GLIB_CHECK_VERSION(2,35,0)
	g_type_init();
#endif

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
