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

#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dlog.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/time.h>
#include <bluetooth-api.h>

#include "bluetooth.h"
#include "bluetooth_product.h"
#include "bluetooth_private.h"
#include "bluetooth-media-control.h"
#include "bluetooth-telephony-api.h"
#include "BTLogger_interface.h"

static bool is_initialized = false;
static bool is_le_initialized = false;
static bool is_smartc_tput = false;
static struct bt_smartrc_throughput_data_s tput_data;
static struct timeval tput_start_time;
static bt_event_sig_event_slot_s bt_event_slot_container[] = {
	{BT_EVENT_STATE_CHANGED, NULL, NULL},
	{BT_EVENT_NAME_CHANGED, NULL, NULL},
	{BT_EVENT_VISIBILITY_MODE_CHANGED, NULL, NULL},
	{BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED, NULL, NULL},
	{BT_EVENT_BOND_CREATED, NULL, NULL},
	{BT_EVENT_BOND_DESTROYED, NULL, NULL},
	{BT_EVENT_AUTHORIZATION_CHANGED, NULL, NULL},
	{BT_EVENT_SERVICE_SEARCHED, NULL, NULL},
	{BT_EVENT_DATA_RECEIVED, NULL, NULL},
	{BT_EVENT_CONNECTION_STATE_CHANGED, NULL, NULL},
	{BT_EVENT_RFCOMM_CONNECTION_REQUESTED, NULL, NULL},
	{BT_EVENT_OPP_CONNECTION_REQUESTED, NULL, NULL},
	{BT_EVENT_OPP_PUSH_REQUESTED, NULL, NULL},
	{BT_EVENT_OPP_SERVER_TRANSFER_PROGRESS, NULL, NULL},
	{BT_EVENT_OPP_SERVER_TRANSFER_FINISHED, NULL, NULL},
	{BT_EVENT_OPP_CLIENT_PUSH_RESPONSED, NULL, NULL},
	{BT_EVENT_OPP_CLIENT_PUSH_PROGRESS, NULL, NULL},
	{BT_EVENT_OPP_CLIENT_PUSH_FINISHED, NULL, NULL},
	{BT_EVENT_PAN_CONNECTION_STATE_CHANGED, NULL, NULL},
	{BT_EVENT_NAP_CONNECTION_STATE_CHANGED, NULL, NULL},
	{BT_EVENT_HDP_CONNECTED, NULL, NULL},
	{BT_EVENT_HDP_DISCONNECTED, NULL, NULL},
	{BT_EVENT_HDP_DATA_RECEIVED, NULL, NULL},
	{BT_EVENT_AUDIO_CONNECTION_STATUS, NULL, NULL},
	{BT_EVENT_AG_SCO_CONNECTION_STATUS, NULL, NULL},
	{BT_EVENT_AG_CALL_HANDLING_EVENT, NULL, NULL},
	{BT_EVENT_AG_MULTI_CALL_HANDLING_EVENT, NULL, NULL},
	{BT_EVENT_AG_DTMF_TRANSMITTED, NULL, NULL},
	{BT_EVENT_AG_MICROPHONE_GAIN_CHANGE, NULL, NULL},
	{BT_EVENT_AG_SPEAKER_GAIN_CHANGE, NULL, NULL},
	{BT_EVENT_AVRCP_CONNECTION_STATUS, NULL, NULL},
	{BT_EVENT_AVRCP_EQUALIZER_STATE_CHANGED, NULL, NULL},
	{BT_EVENT_AVRCP_REPEAT_MODE_CHANGED, NULL, NULL},
	{BT_EVENT_AVRCP_SHUFFLE_MODE_CHANGED, NULL, NULL},
	{BT_EVENT_AVRCP_SCAN_MODE_CHANGED, NULL, NULL},
	{BT_EVENT_AVRCP_VOLUME_CHANGED, NULL, NULL},
	{BT_EVENT_AVRCP_MUTE_STATUS_CHANGED, NULL, NULL},
	{BT_EVENT_A2DP_SINK_CONNECTION_STATUS, NULL, NULL},
	{BT_EVENT_A2DP_SINK_AUDIO_STATUS, NULL, NULL},
	{BT_EVENT_AUDIO_DATA_RECVD, NULL, NULL},
	{BT_EVENT_HID_CONNECTION_STATUS, NULL, NULL},
	{BT_EVENT_DEVICE_CONNECTION_STATUS, NULL, NULL},
	{BT_EVENT_GATT_CHARACTERISTIC_DISCOVERED, NULL, NULL},
	{BT_EVENT_GATT_VALUE_CHANGED, NULL, NULL},
	{BT_EVENT_GATT_WRITE_CHARACTERISTIC, NULL, NULL},
	{BT_EVENT_GATT_CLIENT_CHARACTERISTIC_DISCOVERED_LEGACY, NULL, NULL},
	{BT_EVENT_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERED_LEGACY, NULL, NULL},
	{BT_EVENT_GATT_CLIENT_VALUE_CHANGED_LEGACY, NULL, NULL},
	{BT_EVENT_GATT_CLIENT_READ_CHARACTERISTIC_LEGACY, NULL, NULL},
	{BT_EVENT_GATT_CLIENT_WRITE_CHARACTERISTIC_LEGACY, NULL, NULL},
	{BT_EVENT_HID_AUDIO_DATA_RECVD, NULL, NULL},
	{BT_EVENT_HID_VOICE_KEY_PRESSED, NULL, NULL},
	{BT_EVENT_HID_BATTERY_STATUS_RECEIVED, NULL, NULL},
	{BT_EVENT_HID_MBR_RETURN_RECVD, NULL, NULL},
	{BT_EVENT_HID_SENSOR_DATA_RECEIVED, NULL, NULL},
	{BT_EVENT_HID_SMARTRC_CONNECTION_STATUS, NULL, NULL},
	{BT_EVENT_AUTHORIZE_REQUEST, NULL, NULL},
	{BT_EVENT_MAX_CONNECTION_REACHED, NULL, NULL},
	{BT_EVENT_AUTHENTICATION_REQUEST, NULL, NULL},
	{BT_EVENT_SYNC3D_ASSOCIATION_NOTIFIED, NULL, NULL},
	{BT_EVENT_SYNC3D_VSPEC_EVENT_RECEIVED, NULL, NULL},
	{BT_EVENT_ADVERTISING_STATE_CHANGED, NULL, NULL},
	{BT_EVENT_ADVERTISING_MANUFACTURER_DATA_CHANGED, NULL, NULL},
	{BT_EVENT_SCAN_RESPONSE_MANUFACTURER_DATA_CHANGED, NULL, NULL},
	{BT_EVENT_LE_DEVICE_DISCOVERY_STATE_CHANGED, NULL, NULL},
	{BT_EVENT_HID_SMARTRC_FW_UPDATE_STATUS, NULL, NULL},
	{BT_EVENT_DUT_MODE, NULL, NULL},
	{BT_EVENT_BT_CHIP_FW_UPDATE_STATUS, NULL, NULL}
};

/*
 *  Internal Functions
 */
static void __bt_event_proxy(int event, bluetooth_event_param_t * param, void *user_data);
static void __bt_le_event_proxy(int event, bluetooth_event_param_t *param, void *user_data);
static int __bt_get_cb_index(int event);
static void __bt_convert_lower_to_upper(char *origin);
static int __bt_get_bt_device_sdp_info_s(bt_device_sdp_info_s **dest, bt_sdp_info_t *source);
static void __bt_free_bt_device_sdp_info_s(bt_device_sdp_info_s *sdp_info);
static int __bt_get_bt_adapter_device_discovery_info_s(bt_adapter_device_discovery_info_s **discovery_info, bluetooth_device_info_t *source_info);
static void __bt_free_bt_adapter_device_discovery_info_s(bt_adapter_device_discovery_info_s *discovery_info);

static void __bt_free_bt_device_connection_info_s(bt_device_connection_info_s *conn_info);

static int __bt_get_bt_adapter_le_device_discovery_info_s(
			bt_adapter_device_discovery_info_s **le_discovery_info,
			bluetooth_le_device_info_t *source_info);
static void __bt_free_bt_adapter_le_device_discovery_info_s(bt_adapter_device_discovery_info_s *discovery_info);

/*
 *  Public Functions
 */

int bt_initialize(void)
{
	BT_CHECK_BT_SUPPORT();
	if (is_initialized != true) {
		if (bluetooth_register_callback(&__bt_event_proxy, NULL) != BLUETOOTH_ERROR_NONE) {
			BT_ERR("OPERATION_FAILED(0x%08x)", BT_ERROR_OPERATION_FAILED);
			return BT_ERROR_OPERATION_FAILED;
		}
		is_initialized = true;
	}

	return BT_ERROR_NONE;
}

int bt_deinitialize(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	if (bluetooth_unregister_callback() != BLUETOOTH_ERROR_NONE) {
		BT_ERR("OPERATION_FAILED(0x%08x)", BT_ERROR_OPERATION_FAILED);
		return BT_ERROR_OPERATION_FAILED;
	}
	is_initialized = false;

	return BT_ERROR_NONE;
}


/*
 *  Common Functions
 */
int _bt_check_init_status(void)
{
	if (is_initialized != true)
	{
		BT_ERR("NOT_INITIALIZED(0x%08x)", BT_ERROR_NOT_INITIALIZED);
		return BT_ERROR_NOT_INITIALIZED;
	}

	return BT_ERROR_NONE;
}

void _bt_set_cb(int events, void *callback, void *user_data)
{
	bt_event_slot_container[events].callback = callback;
	bt_event_slot_container[events].user_data = user_data;
}

void _bt_unset_cb(int events)
{
	if (bt_event_slot_container[events].callback != NULL) {
		bt_event_slot_container[events].callback = NULL;
		bt_event_slot_container[events].user_data = NULL;
	}
}

bool _bt_check_cb(int events)
{
	return (bt_event_slot_container[events].callback != NULL) ? true : false;
}

int _bt_le_adapter_init(void)
{
	if (is_le_initialized)
		return BT_ERROR_NONE;

	if (bluetooth_le_register_callback(&__bt_le_event_proxy, NULL) != BLUETOOTH_ERROR_NONE) {
		BT_ERR("OPERATION_FAILED(0x%08x)", BT_ERROR_OPERATION_FAILED);
		return BT_ERROR_OPERATION_FAILED;
	}
	is_le_initialized = true;

	return BT_ERROR_NONE;
}

int _bt_le_adapter_deinit(void)
{
	if (!is_le_initialized) {
		BT_ERR("NOT_INITIALIZED(0x%08x)", BT_ERROR_NOT_INITIALIZED);
		return BT_ERROR_NOT_INITIALIZED;
	}

	if (bluetooth_le_unregister_callback() != BLUETOOTH_ERROR_NONE) {
		BT_ERR("OPERATION_FAILED(0x%08x)", BT_ERROR_OPERATION_FAILED);
		return BT_ERROR_OPERATION_FAILED;
	}
	is_le_initialized = false;

	return BT_ERROR_NONE;
}

int _bt_get_error_code(int origin_error)
{
	switch (origin_error) {
	case BLUETOOTH_ERROR_NONE:
		return BT_ERROR_NONE;
	case BLUETOOTH_ERROR_NOT_CONNECTED:
		return BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED;
	case BLUETOOTH_ERROR_INVALID_DATA:
	case BLUETOOTH_ERROR_INVALID_PARAM:
	case BLUETOOTH_ERROR_NOT_SUPPORT:
		return BT_ERROR_INVALID_PARAMETER;
	case BLUETOOTH_ERROR_MEMORY_ALLOCATION:
	case BLUETOOTH_ERROR_OUT_OF_MEMORY:
		return BT_ERROR_OUT_OF_MEMORY;
	case BLUETOOTH_ERROR_TIMEOUT:
		return BT_ERROR_TIMED_OUT;
	case BLUETOOTH_ERROR_DEVICE_NOT_ENABLED:
		return BT_ERROR_NOT_ENABLED;
	case BLUETOOTH_ERROR_DEVICE_ALREADY_ENABLED:
	case BLUETOOTH_ERROR_ALREADY_INITIALIZED:
	case BLUETOOTH_ERROR_AGENT_DOES_NOT_EXIST:
		return BT_ERROR_ALREADY_DONE;
	case BLUETOOTH_ERROR_NOT_PAIRED:
		return BT_ERROR_REMOTE_DEVICE_NOT_BONDED;
	case BLUETOOTH_ERROR_NOT_IN_OPERATION:
		return BT_ERROR_NOT_IN_PROGRESS;
	case BLUETOOTH_ERROR_IN_PROGRESS:
		return BT_ERROR_NOW_IN_PROGRESS;
	case BLUETOOTH_ERROR_DEVICE_BUSY:
		return BT_ERROR_RESOURCE_BUSY;
	case BLUETOOTH_ERROR_CANCEL:
	case BLUETOOTH_ERROR_CANCEL_BY_USER:
		return BT_ERROR_CANCELLED;
	case BLUETOOTH_ERROR_AUTHENTICATION_FAILED:
		return BT_ERROR_AUTH_FAILED;
	case BLUETOOTH_ERROR_HOST_DOWN:
		return BT_ERROR_REMOTE_DEVICE_NOT_FOUND;
	case BLUETOOTH_ERROR_SERVICE_SEARCH_ERROR:
		return BT_ERROR_SERVICE_SEARCH_FAILED;
	case BLUETOOTH_ERROR_MAX_CONNECTION:
		return BT_ERROR_MAX_CONNECTION_LIMIT_REACHED;
	case BLUETOOTH_ERROR_SERVICE_NOT_FOUND:
	case BLUETOOTH_ERROR_PARING_FAILED:
	case BLUETOOTH_ERROR_ALREADY_CONNECT:
	case BLUETOOTH_ERROR_NO_RESOURCES:
	case BLUETOOTH_ERROR_REGISTRATION_FAILED:
	case BLUETOOTH_ERROR_CONNECTION_BUSY:
	case BLUETOOTH_ERROR_MAX_CLIENT:
	case BLUETOOTH_ERROR_INVALID_CALLBACK:
	case BLUETOOTH_ERROR_NOT_FOUND:
	case BLUETOOTH_ERROR_INTERNAL:
	case BLUETOOTH_ERROR_CONNECTION_ERROR:
	case BLUETOOTH_ERROR_ACCESS_DENIED:
	default:
		return BT_ERROR_OPERATION_FAILED;
	}

}

int _bt_get_bt_device_info_s(bt_device_info_s **dest_dev, bluetooth_device_info_t *source_dev)
{
	int i = 0;

	BT_CHECK_INPUT_PARAMETER(source_dev);

	*dest_dev = (bt_device_info_s *)malloc(sizeof(bt_device_info_s));
	if (*dest_dev == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	if (strlen(source_dev->device_name.name) > 0) {
		(*dest_dev)->remote_name = strdup(source_dev->device_name.name);
	} else {
		(*dest_dev)->remote_name = NULL;
	}

	_bt_convert_address_to_string(&((*dest_dev)->remote_address), &(source_dev->device_address));

	(*dest_dev)->bt_class.major_device_class = source_dev->device_class.major_class;
	(*dest_dev)->bt_class.minor_device_class = source_dev->device_class.minor_class;
	(*dest_dev)->bt_class.major_service_class_mask = source_dev->device_class.service_class;
	if (source_dev->service_index > 0 &&
		(BLUETOOTH_MAX_SERVICES_FOR_DEVICE >= source_dev->service_index)) {
		(*dest_dev)->service_uuid = (char **)malloc(sizeof(char *) * source_dev->service_index);
		if ((*dest_dev)->service_uuid != NULL) {
			for (i = 0; i < source_dev->service_index; i++) {
				(*dest_dev)->service_uuid[i] = g_strndup(source_dev->uuids[i], BLUETOOTH_UUID_STRING_MAX);
				if ((*dest_dev)->service_uuid[i] != NULL) {
					__bt_convert_lower_to_upper((*dest_dev)->service_uuid[i]);
				}
			}
		}
		(*dest_dev)->service_count = source_dev->service_index;
	} else {
		BT_ERR("source_dev->service_index = %d", source_dev->service_index);
		(*dest_dev)->service_uuid = NULL;
		(*dest_dev)->service_count = 0;
	}
	(*dest_dev)->is_bonded = (bool)source_dev->paired;
	(*dest_dev)->is_connected = (bool)source_dev->connected;
	(*dest_dev)->is_authorized = (bool)source_dev->trust;
	(*dest_dev)->vid = (unsigned int)source_dev->vid;
	(*dest_dev)->pid = (unsigned int)source_dev->pid;
	(*dest_dev)->service_mask = (unsigned int)source_dev->service_mask;

	return BT_ERROR_NONE;
}

void _bt_free_bt_device_info_s(bt_device_info_s *device_info)
{
	int i = 0;

	if (device_info == NULL)
		return;

	if (device_info->remote_name != NULL)
		free(device_info->remote_name);

	if (device_info->remote_address != NULL)
		free(device_info->remote_address);

	if (device_info->service_uuid != NULL) {
		for (i = 0; i < device_info->service_count; i++) {
			if (device_info->service_uuid[i] != NULL)
				free(device_info->service_uuid[i]);
		}
		free(device_info->service_uuid);
	}

	free(device_info);
	device_info = NULL;
}

int _bt_get_specific_bt_device_info_s(bt_hid_device_info_s **dest_dev, bluetooth_hid_device_info_t *source_dev)
{

	BT_CHECK_INPUT_PARAMETER(source_dev);
	*dest_dev = (bt_hid_device_info_s *)malloc(sizeof(bt_hid_device_info_s));
	if (*dest_dev == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	(*dest_dev)->device_type = (unsigned char)source_dev->device_type;
	(*dest_dev)->capability = (unsigned short)source_dev->capability;
	(*dest_dev)->battery_status = (unsigned char)source_dev->battery_status;
	(*dest_dev)->resolution_width = (short)source_dev->hid_data[0];
	(*dest_dev)->resolution_height = (short)source_dev->hid_data[1];
	(*dest_dev)->size_x = (short)source_dev->hid_data[2];
	(*dest_dev)->size_y = (short)source_dev->hid_data[3];

	return BT_ERROR_NONE;
}

void _bt_free_specific_bt_device_info_s(bt_hid_device_info_s *device_info)
{
	if (device_info == NULL)
		return;

	free(device_info);
	device_info = NULL;
}

int _bt_convert_address_to_string(char **addr_str, bluetooth_device_address_t *addr_hex)
{
	char address[18] = { 0, };

	BT_CHECK_INPUT_PARAMETER(addr_hex);

	snprintf(address, 18, "%2.2X:%2.2X:%2.2X:%2.2X:%2.2X:%2.2X", addr_hex->addr[0], addr_hex->addr[1], addr_hex->addr[2], addr_hex->addr[3], addr_hex->addr[4], addr_hex->addr[5]);
	*addr_str = strdup(address);

	if (*addr_str != NULL) {
		return BT_ERROR_NONE;
	} else {
		return BT_ERROR_OUT_OF_MEMORY;
	}
}

void _bt_convert_address_to_hex(bluetooth_device_address_t *addr_hex, const char *addr_str)
{
	int i = 0;
	unsigned int addr[BLUETOOTH_ADDRESS_LENGTH] = { 0, };

	if (addr_str == NULL)
		return;

	i = sscanf(addr_str, "%X:%X:%X:%X:%X:%X", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);
	if (i != BLUETOOTH_ADDRESS_LENGTH) {
		BT_ERR("Invalid format string - %s", addr_str);
	}

	for (i = 0; i < BLUETOOTH_ADDRESS_LENGTH; i++) {
		addr_hex->addr[i] = (unsigned char)addr[i];
	}
}

const char *_bt_convert_error_to_string(int error)
{
	switch (error) {
	case BT_ERROR_CANCELLED:
		return "CANCELLED";
	case BT_ERROR_INVALID_PARAMETER:
		return "INVALID_PARAMETER";
	case BT_ERROR_OUT_OF_MEMORY:
		return "OUT_OF_MEMORY";
	case BT_ERROR_RESOURCE_BUSY:
		return "RESOURCE_BUSY";
	case BT_ERROR_TIMED_OUT:
		return "TIMED_OUT";
	case BT_ERROR_NOW_IN_PROGRESS:
		return "NOW_IN_PROGRESS";
	case BT_ERROR_NOT_INITIALIZED:
		return "NOT_INITIALIZED";
	case BT_ERROR_NOT_ENABLED:
		return "DEVICE_NOT_ENABLED";
	case BT_ERROR_ALREADY_DONE:
		return "ALREADY_DONE";
	case BT_ERROR_OPERATION_FAILED:
		return "OPERATION_FAILED";
	case BT_ERROR_NOT_IN_PROGRESS:
		return "NOT_IN_PROGRESS";
	case BT_ERROR_REMOTE_DEVICE_NOT_BONDED:
		return "REMOTE_DEVICE_NOT_BONDED";
	case BT_ERROR_AUTH_REJECTED:
		return "AUTH_REJECTED";
	case BT_ERROR_AUTH_FAILED:
		return "AUTH_FAILED";
	case BT_ERROR_REMOTE_DEVICE_NOT_FOUND:
		return "REMOTE_DEVICE_NOT_FOUND";
	case BT_ERROR_SERVICE_SEARCH_FAILED:
		return "SERVICE_SEARCH_FAILED";
	case BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED:
		return "DEVICE_NOT_CONNECTED";
	case BT_ERROR_MAX_CONNECTION_LIMIT_REACHED:
		return "MAX_CONNECTION_LIMIT_REACHED";
	default:
		return "UNKNOWN";
	}
}

bt_adapter_visibility_mode_e _bt_get_bt_visibility_mode_e(bluetooth_discoverable_mode_t mode)
{
	switch (mode) {
	case BLUETOOTH_DISCOVERABLE_MODE_TIME_LIMITED_DISCOVERABLE:
		return BT_ADAPTER_VISIBILITY_MODE_LIMITED_DISCOVERABLE;
	case BLUETOOTH_DISCOVERABLE_MODE_GENERAL_DISCOVERABLE:
		return BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE;
	default:
		return BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE;
	}
}

/*
 *  Internal Functions
 */

static int __bt_get_bt_device_sdp_info_s(bt_device_sdp_info_s **dest, bt_sdp_info_t *source)
{
	int i = 0;

	*dest = (bt_device_sdp_info_s *)malloc(sizeof(bt_device_sdp_info_s));
	if (*dest == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	if (_bt_convert_address_to_string(&((*dest)->remote_address), &(source->device_addr)) != BT_ERROR_NONE) {
		__bt_free_bt_device_sdp_info_s(*dest);
		return BT_ERROR_OUT_OF_MEMORY;
	}

	if (source->service_index > 0) {
		(*dest)->service_uuid = (char **)malloc(sizeof(char *) * source->service_index);
		if ((*dest)->service_uuid == NULL) {
			__bt_free_bt_device_sdp_info_s(*dest);
			return BT_ERROR_OUT_OF_MEMORY;
		}

		for (i = 0; i < source->service_index; i++) {
			(*dest)->service_uuid[i] = strdup(source->uuids[i]);
			if ((*dest)->service_uuid[i] == NULL) {
				__bt_free_bt_device_sdp_info_s(*dest);
				return BT_ERROR_OUT_OF_MEMORY;
			}
			__bt_convert_lower_to_upper((*dest)->service_uuid[i]);
		}
	} else {
		(*dest)->service_uuid = NULL;
	}
	(*dest)->service_count = source->service_index;

	return BT_ERROR_NONE;
}

static void __bt_free_bt_device_sdp_info_s(bt_device_sdp_info_s *sdp_info)
{
	int i = 0;

	if (sdp_info == NULL)
		return;

	if (sdp_info->remote_address != NULL)
		free(sdp_info->remote_address);

	if (sdp_info->service_uuid != NULL) {
		for (i = 0; i < sdp_info->service_count; i++) {
			if (sdp_info->service_uuid[i] != NULL)
				free(sdp_info->service_uuid[i]);
		}
		free(sdp_info->service_uuid);
	}

	free(sdp_info);
	sdp_info = NULL;
}

void _bt_audio_event_proxy(int event, bt_audio_event_param_t *param, void *user_data)
{
	bluetooth_event_param_t new_param;
	new_param.event = param->event;
	new_param.param_data = param->param_data;
	new_param.result = param->result;
	new_param.user_data = NULL;
	__bt_event_proxy(event, &new_param, user_data);
}

void _bt_telephony_event_proxy(int event, telephony_event_param_t *param, void *user_data)
{
	bluetooth_event_param_t new_param;
	new_param.event = param->event;
	new_param.param_data = param->param_data;
	new_param.result = param->result;
	new_param.user_data = NULL;
	__bt_event_proxy(event, &new_param, user_data);
}

void _bt_avrcp_event_proxy(int event, media_event_param_t *param, void *user_data)
{
	bluetooth_event_param_t new_param;
	new_param.event = param->event;
	new_param.param_data = param->param_data;
	new_param.result = param->result;
	new_param.user_data = param->user_data;
	__bt_event_proxy(event, &new_param, user_data);
}

void _bt_hid_event_proxy(int event, hid_event_param_t *param, void *user_data)
{
	bluetooth_event_param_t new_param;
	new_param.event = param->event;
	new_param.param_data = param->param_data;
	new_param.result = param->result;
	new_param.user_data = param->user_data;
	__bt_event_proxy(event, &new_param, user_data);
}

void _bt_sync3d_event_proxy(int event, bt_sync3d_event_param_t *param, void *user_data)
{
	bluetooth_event_param_t new_param;
	new_param.event = param->event;
	new_param.param_data = param->param_data;
	new_param.result = param->result;
	new_param.user_data = param->user_data;
	__bt_event_proxy(event, &new_param, user_data);
}

static int __bt_get_bt_device_connection_info_s(bt_device_connection_info_s **dest, bluetooth_device_address_t *source)
{
	*dest = (bt_device_connection_info_s *)g_malloc0(sizeof(bt_device_connection_info_s));
	if (*dest == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	if (_bt_convert_address_to_string(&((*dest)->remote_address), source) != BT_ERROR_NONE) {
		__bt_free_bt_device_connection_info_s(*dest);
		return BT_ERROR_OUT_OF_MEMORY;
	}

	return BT_ERROR_NONE;
}

static void __bt_free_bt_device_connection_info_s(bt_device_connection_info_s *conn_info)
{
	if (conn_info == NULL)
		return;

	if (conn_info->remote_address != NULL)
		free(conn_info->remote_address);

	free(conn_info);
	conn_info = NULL;
}

static void __bt_event_proxy(int event, bluetooth_event_param_t *param, void *user_data)
{
	int i;
	int call_id;
	int *avrcp_mode;
	unsigned short *volume;
	bluetooth_rfcomm_connection_t *connection_ind = NULL;
	bluetooth_rfcomm_disconnection_t *disconnection_ind = NULL;
	bt_socket_connection_s rfcomm_connection;
	bt_device_sdp_info_s *sdp_info = NULL;
	bt_adapter_device_discovery_info_s *discovery_info = NULL;
	bt_device_info_s *bonded_device = NULL;
	bluetooth_rfcomm_connection_request_t *reqeust_ind = NULL;
	bt_obex_server_authorize_into_t *auth_info = NULL;
	bt_obex_server_transfer_info_t *transfer_info = NULL;
	bt_opc_transfer_info_t *client_info = NULL;
	bluetooth_device_address_t *bd_addr = NULL;
	telephony_event_callid_t *call_data = NULL;
	telephony_event_dtmf_t *dtmf_data = NULL;
	char *device_addr = NULL;
	int error_code = BT_ERROR_NONE;
	int event_index = -1;
	bluetooth_network_device_info_t *dev_info = NULL;
	bt_hdp_connected_t *hdp_conn_info = NULL;
	bt_hdp_disconnected_t *hdp_disconn_info = NULL;
	bt_hdp_data_ind_t *hdp_data_ind = NULL;
	bt_gatt_discovered_char_t *svc_char = NULL;
	bt_gatt_char_value_t *char_val = NULL;
	bluetooth_authentication_request_info_t *auth_information = NULL;
	bluetooth_authorization_request_info_t *authorization_info = NULL;
	bt_device_connection_info_s *conn_info = NULL;
	bt_a2dp_sink_audio_info_s *audio_info = NULL;
	unsigned char progress;
	bool *mute_status = FALSE;

	event_index = __bt_get_cb_index(event);
	if (event_index == -1 || bt_event_slot_container[event_index].callback == NULL) {
		return;
	}

	memset(&rfcomm_connection, 0x00, sizeof(bt_socket_connection_s));

	switch (event) {
	case BLUETOOTH_EVENT_ENABLED:
		BT_INFO("bt_adapter_state_changed_cb() will be called with BT_ADAPTER_ENABLED");
		((bt_adapter_state_changed_cb) bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), BT_ADAPTER_ENABLED, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_DISABLED:
		BT_INFO("bt_adapter_state_changed_cb() will be called with BT_ADAPTER_DISABLED");
		((bt_adapter_state_changed_cb) bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), BT_ADAPTER_DISABLED, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_LOCAL_NAME_CHANGED:
		BT_INFO("bt_adapter_name_changed_cb() will be called");
		((bt_adapter_name_changed_cb)bt_event_slot_container[event_index].callback)
		    ((char *)(param->param_data), bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_DISCOVERABLE_MODE_CHANGED:
		BT_INFO("bt_adapter_visibility_mode_changed_cb() will be called");
		((bt_adapter_visibility_mode_changed_cb)bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), _bt_get_bt_visibility_mode_e(*(bt_adapter_visibility_mode_e *)(param->param_data)), bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_DISCOVERY_STARTED:
		BT_INFO("bt_adapter_device_discovery_state_changed_cb() will be called with BT_ADAPTER_DEVICE_DISCOVERY_STARTED");
		((bt_adapter_device_discovery_state_changed_cb) bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), BT_ADAPTER_DEVICE_DISCOVERY_STARTED, NULL, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_DISCOVERY_FINISHED:
		BT_INFO("bt_adapter_device_discovery_state_changed_cb() will be called with BT_ADAPTER_DEVICE_DISCOVERY_FINISHED");
		((bt_adapter_device_discovery_state_changed_cb)bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), BT_ADAPTER_DEVICE_DISCOVERY_FINISHED, NULL, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_REMOTE_DEVICE_NAME_UPDATED:
		BT_INFO("bt_adapter_device_discovery_state_changed_cb() will be called with BT_ADAPTER_DEVICE_DISCOVERY_FOUND: error= %d",_bt_get_error_code(param->result));
		if (__bt_get_bt_adapter_device_discovery_info_s(&discovery_info, (bluetooth_device_info_t *)(param->param_data)) == BT_ERROR_NONE) {
			((bt_adapter_device_discovery_state_changed_cb)bt_event_slot_container[event_index].callback)
			    (_bt_get_error_code(param->result), BT_ADAPTER_DEVICE_DISCOVERY_FOUND, discovery_info, bt_event_slot_container[event_index].user_data);
			__bt_free_bt_adapter_device_discovery_info_s(discovery_info);
		} else {
			((bt_adapter_device_discovery_state_changed_cb)bt_event_slot_container[event_index].callback)
			    (_bt_get_error_code(param->result), BT_ADAPTER_DEVICE_DISCOVERY_FOUND, NULL, bt_event_slot_container[event_index].user_data);
		}
		break;
	case BLUETOOTH_EVENT_REMOTE_LE_DEVICE_FOUND:
		BT_INFO("bt_adapter_device_discovery_state_changed_cb() will be called with BLUETOOTH_EVENT_REMOTE_LE_DEVICE_FOUND");
		if (__bt_get_bt_adapter_le_device_discovery_info_s(&discovery_info, (bluetooth_le_device_info_t *)(param->param_data)) == BT_ERROR_NONE) {
			((bt_adapter_device_discovery_state_changed_cb)bt_event_slot_container[event_index].callback)
			    (_bt_get_error_code(param->result), BT_ADAPTER_DEVICE_DISCOVERY_FOUND, discovery_info, bt_event_slot_container[event_index].user_data);
			__bt_free_bt_adapter_device_discovery_info_s(discovery_info);
		} else {
			((bt_adapter_device_discovery_state_changed_cb)bt_event_slot_container[event_index].callback)
			    (_bt_get_error_code(param->result), BT_ADAPTER_DEVICE_DISCOVERY_FOUND, NULL, bt_event_slot_container[event_index].user_data);
		}
		break;
	case BLUETOOTH_EVENT_BONDING_FINISHED:
		BT_INFO("bt_device_bond_created_cb() will be called");
		_bt_get_bt_device_info_s(&bonded_device, (bluetooth_device_info_t *)(param->param_data));

		((bt_device_bond_created_cb)bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), bonded_device, bt_event_slot_container[event_index].user_data);
		_bt_free_bt_device_info_s(bonded_device);
		bonded_device = NULL;
		break;
	case BLUETOOTH_EVENT_BONDED_DEVICE_REMOVED:
		BT_INFO("bt_device_bond_destroyed_cb() will be called");
		_bt_convert_address_to_string(&device_addr, (bluetooth_device_address_t *)(param->param_data));
		((bt_device_bond_destroyed_cb)bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), device_addr, bt_event_slot_container[event_index].user_data);
		if (device_addr != NULL)
			free(device_addr);
		device_addr = NULL;
		break;
	case BLUETOOTH_EVENT_DEVICE_LIMIT_REACHED:
		BT_INFO("bt_device_max_conection_reached_cb() will be called with \
			BT_EVENT_MAX_CONNECTION_REACHED ");
		device_addr =  (char *)(param->param_data);
		((bt_adapter_max_conection_reached_cb)bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), device_addr, bt_event_slot_container[event_index].user_data);
		if (device_addr != NULL)
			free(device_addr);
		break;
	case BLUETOOTH_EVENT_KEYBOARD_PASSKEY_DISPLAY:
		BT_INFO("bt_adapter_authentication_req_cb() will be called with \
			BLUETOOTH_EVENT_KEYBOARD_PASSKEY_DISPLAY");
		auth_information = (bluetooth_authentication_request_info_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, &auth_information->device_address);
		BT_DBG("BLUETOOTH_EVENT_KEYBOARD_PASSKEY_DISPLAY: name = %s address = %s passkey = %s", auth_information->device_name.name,
			device_addr, auth_information->str_passkey);

		((bt_adapter_authentication_req_cb)bt_event_slot_container[event_index].callback)
			(_bt_get_error_code(param->result), BT_AUTH_KEYBOARD_PASSKEY_DISPLAY, auth_information->device_name.name,
				device_addr, auth_information->str_passkey, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		device_addr = NULL;
		break;
	case BLUETOOTH_EVENT_PIN_REQUEST:
		BT_INFO("bt_adapter_authentication_req_cb() will be called with \
				BLUETOOTH_EVENT_PIN_REQUEST");
		auth_information = (bluetooth_authentication_request_info_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, &auth_information->device_address);

		BT_DBG("BUETOOTH_EVENT_PIN_REQUEST: name = %s address = %s", auth_information->device_name.name,
			device_addr);

		((bt_adapter_authentication_req_cb)bt_event_slot_container[event_index].callback)
			(_bt_get_error_code(param->result), BT_AUTH_PIN_REQUEST, auth_information->device_name.name, device_addr,
				auth_information->str_passkey, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		device_addr = NULL;
		break;
	case BLUETOOTH_EVENT_PASSKEY_REQUEST:
		BT_INFO("bt_adapter_authentication_req_cb will be called with \
				BLUETOOTH_EVENT_PASSKEY_REQUEST");

		auth_information = (bluetooth_authentication_request_info_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, &auth_information->device_address);

		BT_DBG("BLUETOOTH_EVENT_PASSKEY_REQUEST: name = %s address = %s", auth_information->device_name.name,
			device_addr);

		((bt_adapter_authentication_req_cb)bt_event_slot_container[event_index].callback)
			(_bt_get_error_code(param->result), BT_AUTH_PIN_REQUEST, auth_information->device_name.name, device_addr,
				auth_information->str_passkey, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		device_addr = NULL;

		break;
	case BLUETOOTH_EVENT_PASSKEY_CONFIRM_REQUEST:
		BT_INFO("bt_adapter_authentication_req_cb will be called with \
			BLUETOOTH_EVENT_PASSKEY_CONFIRM_REQUEST");

		auth_information = (bluetooth_authentication_request_info_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, &auth_information->device_address);

		BT_DBG("BLUETOOTH_EVENT_PASSKEY_CONFIRM_REQUEST: name = %s address = %s passkey = %s ",
				auth_information->device_name.name, device_addr, auth_information->str_passkey);
		((bt_adapter_authentication_req_cb)bt_event_slot_container[event_index].callback)
			(_bt_get_error_code(param->result), BT_AUTH_PASSKEY_CONFIRM_REQUEST, auth_information->device_name.name, device_addr,
				auth_information->str_passkey, bt_event_slot_container[event_index].user_data);
		if (device_addr != NULL)
			free(device_addr);
		device_addr = NULL;

		break;
	case BLUETOOTH_EVENT_SSP_CONSENT_REQUEST:
		BT_INFO("bt_adapter_authentication_req_cb() will be called with \
				BLUETOOTH_EVENT_SSP_CONSENT_REQUEST");
		auth_information = (bluetooth_authentication_request_info_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, &auth_information->device_address);

		BT_DBG("BLUETOOTH_EVENT_SSP_CONSENT_REQUEST: name = %s address = %s",
				auth_information->device_name.name, device_addr);

		((bt_adapter_authentication_req_cb)bt_event_slot_container[event_index].callback)
			(_bt_get_error_code(param->result), BT_AUTH_SSP_CONSENT_REQUEST,
			 auth_information->device_name.name, device_addr,
				auth_information->str_passkey, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		device_addr = NULL;
		break;
	case BLUETOOTH_EVENT_DEVICE_AUTHORIZED:
		BT_INFO("bt_device_authorization_changed_cb() will be called with BT_DEVICE_AUTHORIZED");
		_bt_convert_address_to_string(&device_addr, (bluetooth_device_address_t *)(param->param_data));
		((bt_device_authorization_changed_cb)bt_event_slot_container[event_index].callback)
		    (BT_DEVICE_AUTHORIZED, device_addr, bt_event_slot_container[event_index].user_data);
		if (device_addr != NULL)
			free(device_addr);
		break;
	case BLUETOOTH_EVENT_DEVICE_UNAUTHORIZED:
		BT_INFO("bt_device_authorization_changed_cb() will be called with BT_DEVICE_UNAUTHORIZED");
		_bt_convert_address_to_string(&device_addr, (bluetooth_device_address_t *)(param->param_data));
		((bt_device_authorization_changed_cb)bt_event_slot_container[event_index].callback)
		    (BT_DEVICE_UNAUTHORIZED, device_addr, bt_event_slot_container[event_index].user_data);
		if (device_addr != NULL)
			free(device_addr);
		break;
	case BLUETOOTH_EVENT_DEVICE_CONNECTED:
		BT_INFO("bt_device_connection_state_changed_cb() will be called");
		__bt_get_bt_device_connection_info_s(&conn_info, (bluetooth_device_address_t *)(param->param_data));

		((bt_device_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		    (true, conn_info, bt_event_slot_container[event_index].user_data);
		__bt_free_bt_device_connection_info_s(conn_info);
		break;
	case BLUETOOTH_EVENT_DEVICE_DISCONNECTED:
		BT_INFO("bt_device_connection_state_changed_cb() will be called");
		__bt_get_bt_device_connection_info_s(&conn_info, (bluetooth_device_address_t *)(param->param_data));

		((bt_device_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		    (false, conn_info, bt_event_slot_container[event_index].user_data);
		__bt_free_bt_device_connection_info_s(conn_info);
		break;
	case BLUETOOTH_EVENT_AUTHORIZE_REQUEST:
		authorization_info = (bluetooth_authorization_request_info_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, &authorization_info->device_address);

		BT_DBG("BLUETOOTH_EVENT_AUTHORIZE_REQUEST: name = %s address = %s",
				authorization_info->device_name.name, device_addr);
		BT_INFO("bt_device_auth_event_cb() will be called with BT_EVENT_AUTHORIZE_REQUEST");
		((bt_device_auth_event_cb) bt_event_slot_container[event_index].callback)
			(_bt_get_error_code(param->result),
			 device_addr, authorization_info->device_name.name,
			 bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		device_addr = NULL;

		break;
	case BLUETOOTH_EVENT_SERVICE_SEARCHED:
		BT_INFO("bt_device_service_searched_cb() will be called");
		__bt_get_bt_device_sdp_info_s(&sdp_info, (bt_sdp_info_t *)(param->param_data));
		error_code = _bt_get_error_code(param->result);
		// In service search, BT_ERROR_SERVICE_SEARCH_FAILED is returned instead of BT_ERROR_OPERATION_FAILED.
		if (error_code == BT_ERROR_OPERATION_FAILED)
			error_code = BT_ERROR_SERVICE_SEARCH_FAILED;
		((bt_device_service_searched_cb)bt_event_slot_container[event_index].callback)
		    (error_code, sdp_info, bt_event_slot_container[event_index].user_data);
		__bt_free_bt_device_sdp_info_s(sdp_info);
		break;
	case BLUETOOTH_EVENT_RFCOMM_DATA_RECEIVED:
		BT_INFO("bt_socket_data_received_cb() will be called");
		((bt_socket_data_received_cb)bt_event_slot_container[event_index].callback)
		    ((bt_socket_received_data_s *)(param->param_data), bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_RFCOMM_CONNECTED:
		BT_INFO("bt_socket_connection_state_changed_cb() will be called with BT_SOCKET_CONNECTED");
		if (param->result == BLUETOOTH_ERROR_INVALID_PARAM)
			error_code = BT_ERROR_OPERATION_FAILED;
		else
			error_code = _bt_get_error_code(param->result);

		connection_ind = (bluetooth_rfcomm_connection_t *)(param->param_data);

		if (connection_ind) {
			rfcomm_connection.socket_fd = connection_ind->socket_fd;
			rfcomm_connection.local_role = connection_ind->device_role;

			if (strlen(connection_ind->uuid) > 0) {
				rfcomm_connection.service_uuid = strdup(connection_ind->uuid);
				BT_DBG("uuid: [%s]", rfcomm_connection.service_uuid);
			}

			_bt_convert_address_to_string(&(rfcomm_connection.remote_address),
						&(connection_ind->device_addr));
		}

		((bt_socket_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		    (error_code, BT_SOCKET_CONNECTED, &rfcomm_connection, bt_event_slot_container[event_index].user_data);

		if (rfcomm_connection.remote_address != NULL) {
			free(rfcomm_connection.remote_address);
			rfcomm_connection.remote_address = NULL;
		}

		if (rfcomm_connection.service_uuid != NULL) {
			free(rfcomm_connection.service_uuid);
			rfcomm_connection.service_uuid = NULL;
		}

		break;
	case BLUETOOTH_EVENT_RFCOMM_DISCONNECTED:
		BT_INFO("bt_socket_connection_state_changed_cb() will be called with BT_SOCKET_DISCONNECTED");

		disconnection_ind = (bluetooth_rfcomm_disconnection_t *)(param->param_data);

		if (disconnection_ind) {
			rfcomm_connection.socket_fd = disconnection_ind->socket_fd;
			rfcomm_connection.local_role = disconnection_ind->device_role;

			if (strlen(disconnection_ind->uuid) > 0) {
				rfcomm_connection.service_uuid = strdup(disconnection_ind->uuid);
				BT_DBG("uuid: [%s]", rfcomm_connection.service_uuid);
			}

			_bt_convert_address_to_string(&(rfcomm_connection.remote_address),
						&(disconnection_ind->device_addr));
		}

		((bt_socket_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), BT_SOCKET_DISCONNECTED, &rfcomm_connection, bt_event_slot_container[event_index].user_data);

		if (rfcomm_connection.remote_address != NULL) {
			free(rfcomm_connection.remote_address);
			rfcomm_connection.remote_address = NULL;
		}

		if (rfcomm_connection.service_uuid != NULL) {
			free(rfcomm_connection.service_uuid);
			rfcomm_connection.service_uuid = NULL;
		}

		break;
	case BLUETOOTH_EVENT_RFCOMM_AUTHORIZE:
		BT_INFO("bt_socket_connection_requested_cb() will be called");
		reqeust_ind = (bluetooth_rfcomm_connection_request_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, &(reqeust_ind->device_addr));
		((bt_socket_connection_requested_cb)bt_event_slot_container[event_index].callback)
		    (reqeust_ind->socket_fd, device_addr, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		break;
	case BLUETOOTH_EVENT_OBEX_SERVER_CONNECTION_AUTHORIZE:
		BT_INFO("bt_opp_server_connection_requested_cb() will be called");
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);
		((bt_opp_server_connection_requested_cb)bt_event_slot_container[event_index].callback)
		    (device_addr, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		break;
	case BLUETOOTH_EVENT_OBEX_SERVER_TRANSFER_AUTHORIZE:
		BT_INFO("bt_opp_server_push_requested_cb() will be called");
		auth_info = (bt_obex_server_authorize_into_t *)(param->param_data);
		((bt_opp_server_push_requested_cb)bt_event_slot_container[event_index].callback)
		    (auth_info->filename, auth_info->length, bt_event_slot_container[event_index].user_data);
		break;

	case BLUETOOTH_EVENT_OBEX_SERVER_TRANSFER_STARTED:
		BT_INFO("bt_opp_server_transfer_started_cb() will be called");
		transfer_info = (bt_obex_server_transfer_info_t *)(param->param_data);
		((bt_opp_server_transfer_progress_cb)bt_event_slot_container[event_index].callback)
		    (transfer_info->filename, transfer_info->file_size, 0, bt_event_slot_container[event_index].user_data);
		break;

	case BLUETOOTH_EVENT_OBEX_SERVER_TRANSFER_PROGRESS:
		BT_INFO("bt_opp_server_transfer_in_progress_cb() will be called");
		transfer_info = (bt_obex_server_transfer_info_t *)(param->param_data);
		((bt_opp_server_transfer_progress_cb)bt_event_slot_container[event_index].callback)
		    (transfer_info->filename, transfer_info->file_size, transfer_info->percentage, bt_event_slot_container[event_index].user_data);
		break;

	case BLUETOOTH_EVENT_OBEX_SERVER_TRANSFER_COMPLETED:
		BT_INFO("bt_opp_server_transfer_completed_cb() will be called");
		transfer_info = (bt_obex_server_transfer_info_t *)(param->param_data);

		((bt_opp_server_transfer_finished_cb)bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), transfer_info->filename, transfer_info->file_size, bt_event_slot_container[event_index].user_data);

		break;

	case BLUETOOTH_EVENT_OPC_CONNECTED:
		BT_INFO("bt_opp_client_push_responded_cb() will be called");
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);

		((bt_opp_client_push_responded_cb)bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), device_addr, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		break;

	case BLUETOOTH_EVENT_OPC_DISCONNECTED:
		BT_INFO("bt_opp_client_push_finished_cb() will be called");
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);

		((bt_opp_client_push_finished_cb)bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), device_addr, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		break;

	case BLUETOOTH_EVENT_OPC_TRANSFER_STARTED:
		BT_INFO("BLUETOOTH_EVENT_OPC_TRANSFER_STARTED");
		/* This event don't be used in CAPI */
		break;

	case BLUETOOTH_EVENT_OPC_TRANSFER_PROGRESS:
		BT_INFO("bt_opp_client_transfer_in_progress_cb() will be called");
		client_info = (bt_opc_transfer_info_t *)(param->param_data);
		((bt_opp_client_push_progress_cb)bt_event_slot_container[event_index].callback)
		    (client_info->filename, client_info->size, client_info->percentage, bt_event_slot_container[event_index].user_data);
		break;

	case BLUETOOTH_EVENT_OPC_TRANSFER_COMPLETE:
		BT_INFO("BLUETOOTH_EVENT_OPC_TRANSFER_COMPLETE");
		/* This event don't be used in CAPI */
		break;

	case BLUETOOTH_EVENT_NETWORK_SERVER_CONNECTED:
		BT_INFO("BLUETOOTH_EVENT_NETWORK_SERVER_CONNECTED");
		dev_info = (bluetooth_network_device_info_t *)(param->param_data);

		if (param->result != BLUETOOTH_ERROR_NONE) {
			BT_ERR("Fail to connect the network server");
		}
		_bt_convert_address_to_string(&device_addr, &dev_info->device_address);
		((bt_nap_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(TRUE, device_addr, dev_info->interface_name, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		break;

	case BLUETOOTH_EVENT_NETWORK_SERVER_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_NETWORK_SERVER_DISCONNECTED");
		dev_info = (bluetooth_network_device_info_t *)(param->param_data);

		if (param->result != BLUETOOTH_ERROR_NONE) {
			BT_ERR("Fail to disconnect the network server");
		}
		_bt_convert_address_to_string(&device_addr, &dev_info->device_address);
		((bt_nap_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(FALSE, device_addr, dev_info->interface_name, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		break;
	case BLUETOOTH_EVENT_NETWORK_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_NETWORK_DISCONNECTED");
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);

		((bt_panu_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), FALSE, device_addr, BLUETOOTH_NETWORK_NAP_ROLE,
		bt_event_slot_container[event_index].user_data);
		if (device_addr != NULL)
			free(device_addr);
		break;

	case BLUETOOTH_EVENT_NETWORK_CONNECTED:
		BT_INFO("BLUETOOTH_EVENT_NETWORK_CONNECTED");
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);

		((bt_panu_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), TRUE, device_addr, BLUETOOTH_NETWORK_NAP_ROLE,
		bt_event_slot_container[event_index].user_data);
		if (device_addr != NULL)
			free(device_addr);
		break;

	case BLUETOOTH_EVENT_HDP_CONNECTED:
		BT_INFO("HDP Connected ");
		hdp_conn_info = (bt_hdp_connected_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, &hdp_conn_info->device_address);
		((bt_hdp_connected_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), device_addr, hdp_conn_info->app_handle,
		hdp_conn_info->type, hdp_conn_info->channel_id,
		bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		break;

	case BLUETOOTH_EVENT_HDP_DISCONNECTED:
		BT_INFO("HDP disconnected callback will be ");
		hdp_disconn_info = (bt_hdp_disconnected_t *)(param->param_data);

		_bt_convert_address_to_string(&device_addr, &hdp_disconn_info->device_address);
		((bt_hdp_disconnected_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), device_addr,
		hdp_disconn_info->channel_id, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		break;
	case BLUETOOTH_EVENT_HDP_DATA_RECEIVED:
		BT_INFO("HDP data recieved callback will be ");
		hdp_data_ind = (bt_hdp_data_ind_t *)(param->param_data);
		if (param->result != BLUETOOTH_ERROR_NONE) {
			BT_ERR("Fail to receive HDP data");
		}

		((bt_hdp_data_received_cb)bt_event_slot_container[event_index].callback)
		(hdp_data_ind->channel_id, hdp_data_ind->buffer, hdp_data_ind->size,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AG_CONNECTED:
		BT_INFO("BLUETOOTH_EVENT_AG_CONNECTED ");
		device_addr = (char *)(param->param_data);
		((bt_audio_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), TRUE, device_addr, BT_AUDIO_PROFILE_TYPE_HSP_HFP,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AG_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_AG_DISCONNECTED ");
		device_addr = (char *)(param->param_data);
		((bt_audio_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), FALSE, device_addr, BT_AUDIO_PROFILE_TYPE_HSP_HFP,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_TELEPHONY_AUDIO_CONNECTED:
		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_AUDIO_CONNECTED ");
		((bt_ag_sco_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), TRUE,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_TELEPHONY_AUDIO_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_AUDIO_DISCONNECTED ");
		((bt_ag_sco_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), FALSE,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_TELEPHONY_ANSWER_CALL:
		call_data = (telephony_event_callid_t *)param->param_data;
		call_id = call_data->callid;
		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_ANSWER_CALL ");
		((bt_ag_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_AG_CALL_HANDLING_EVENT_ANSWER, call_id,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_TELEPHONY_RELEASE_CALL:
		call_data = (telephony_event_callid_t *)param->param_data;
		call_id = call_data->callid;
		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_RELEASE_CALL ");
		((bt_ag_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_AG_CALL_HANDLING_EVENT_RELEASE, call_id,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_TELEPHONY_REJECT_CALL:
		call_data = (telephony_event_callid_t *)param->param_data;
		call_id = call_data->callid;
		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_REJECT_CALL ");
		((bt_ag_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_AG_CALL_HANDLING_EVENT_REJECT, call_id,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_TELEPHONY_CHLD_0_RELEASE_ALL_HELD_CALL:
		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_CHLD_0_RELEASE_ALL_HELD_CALL ");
		((bt_ag_multi_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_AG_MULTI_CALL_HANDLING_EVENT_RELEASE_HELD_CALLS,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_TELEPHONY_CHLD_1_RELEASE_ALL_ACTIVE_CALL:
		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_CHLD_1_RELEASE_ALL_ACTIVE_CALL ");
		((bt_ag_multi_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_AG_MULTI_CALL_HANDLING_EVENT_RELEASE_ACTIVE_CALLS,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_TELEPHONY_CHLD_2_ACTIVE_HELD_CALL:
		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_CHLD_2_ACTIVE_HELD_CALL ");
		((bt_ag_multi_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_AG_MULTI_CALL_HANDLING_EVENT_ACTIVATE_HELD_CALL,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_TELEPHONY_CHLD_3_MERGE_CALL:
		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_CHLD_3_MERGE_CALL ");
		((bt_ag_multi_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_AG_MULTI_CALL_HANDLING_EVENT_MERGE_CALLS,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_TELEPHONY_CHLD_4_EXPLICIT_CALL_TRANSFER:
		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_CHLD_4_EXPLICIT_CALL_TRANSFER ");
		((bt_ag_multi_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_AG_MULTI_CALL_HANDLING_EVENT_EXPLICIT_CALL_TRANSFER,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_TELEPHONY_SEND_DTMF: {
		dtmf_data = (telephony_event_dtmf_t *)param->param_data;

		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_SEND_DTMF ");
		((bt_ag_dtmf_transmitted_cb)bt_event_slot_container[event_index].callback)
		(dtmf_data->dtmf, bt_event_slot_container[event_index].user_data);
		break;
	}
	case BLUETOOTH_EVENT_AG_SPEAKER_GAIN:
		BT_INFO("BLUETOOTH_EVENT_AG_SPEAKER_GAIN");
		int *spk_gain = (int *)(param->param_data);
		((bt_ag_speaker_gain_changed_cb)bt_event_slot_container[event_index].callback)
		(*spk_gain, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AG_MIC_GAIN:
		BT_INFO("BLUETOOTH_EVENT_AG_MIC_GAIN");
		int *mik_gain = (int *)(param->param_data);
		((bt_ag_microphone_gain_changed_cb)bt_event_slot_container[event_index].callback)
		(*mik_gain, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AG_AUDIO_CONNECTED:
		BT_INFO("BLUETOOTH_EVENT_AG_AUDIO_CONNECTED");
		((bt_audio_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), TRUE, NULL, BT_AUDIO_PROFILE_TYPE_HSP_HFP,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AG_AUDIO_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_AG_AUDIO_DISCONNECTED");
		device_addr = (char *)(param->param_data);

		((bt_audio_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), FALSE, device_addr, BT_AUDIO_PROFILE_TYPE_HSP_HFP,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AV_CONNECTED:
		BT_INFO("BLUETOOTH_EVENT_AV_CONNECTED ");
		device_addr = (char *)(param->param_data);
		((bt_audio_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), TRUE, device_addr, BT_AUDIO_PROFILE_TYPE_A2DP,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AV_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_Av_DISCONNECTED ");
		device_addr = (char *)(param->param_data);
		((bt_audio_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), FALSE, device_addr, BT_AUDIO_PROFILE_TYPE_A2DP,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_CONNECTED:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_CONNECTED ");
		device_addr = (char *)(param->param_data);
		((bt_avrcp_target_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(TRUE, device_addr, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_DISCONNECTED ");
		device_addr = (char *)(param->param_data);
		((bt_avrcp_target_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(FALSE, device_addr, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_SETTING_SHUFFLE_STATUS:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_SETTING_SHUFFLE_STATUS ");
		avrcp_mode = (int *)(param->param_data);
		((bt_avrcp_shuffle_mode_changed_cb)bt_event_slot_container[event_index].callback)
		((bt_avrcp_shuffle_mode_e)*avrcp_mode, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_SETTING_EQUALIZER_STATUS:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_SETTING_EQUALIZER_STATUS ");
		avrcp_mode = (int *)(param->param_data);
		((bt_avrcp_equalizer_state_changed_cb)bt_event_slot_container[event_index].callback)
		((bt_avrcp_equalizer_state_e)*avrcp_mode, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_SETTING_REPEAT_STATUS:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_SETTING_REPEAT_STATUS ");
		avrcp_mode = (int *)(param->param_data);
		((bt_avrcp_repeat_mode_changed_cb)bt_event_slot_container[event_index].callback)
		((bt_avrcp_repeat_mode_e)*avrcp_mode, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_SETTING_SCAN_STATUS:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_SETTING_SCAN_STATUS ");
		avrcp_mode = (int *)(param->param_data);
		((bt_avrcp_scan_mode_changed_cb)bt_event_slot_container[event_index].callback)
		((bt_avrcp_scan_mode_e)*avrcp_mode, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_VOLUME_CHANGED:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_SETTING_SCAN_STATUS ");
		volume = (unsigned short *)(param->param_data);
		((bt_avrcp_volume_changed_cb)bt_event_slot_container[event_index].callback)
		(*volume, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_MUTE_STATUS_CHANGED:
		mute_status = (bool *)(param->param_data);
		((bt_avrcp_mute_state_changed_cb)bt_event_slot_container[event_index].callback)
		(*mute_status, bt_event_slot_container[event_index].user_data);
		break;
        case BLUETOOTH_EVENT_A2DP_SINK_CONNECTED:
                BT_INFO("BLUETOOTH_EVENT_A2DP_SINK_CONNECTED ");
                device_addr = (char *)(param->param_data);
                ((bt_a2dp_sink_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(TRUE, device_addr, bt_event_slot_container[event_index].user_data);
                break;
        case BLUETOOTH_EVENT_A2DP_SINK_DISCONNECTED:
                BT_INFO("BLUETOOTH_EVENT_A2DP_SINK_DISCONNECTED ");
                device_addr = (char *)(param->param_data);
                ((bt_a2dp_sink_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(FALSE, device_addr, bt_event_slot_container[event_index].user_data);
                break;
        case BLUETOOTH_EVENT_A2DP_SINK_AUDIO_SUSPEND:
                BT_INFO("BLUETOOTH_EVENT_A2DP_SINK_AUDIO_SUSPEND ");
                device_addr = (char *)(param->param_data);
                ((bt_a2dp_sink_audio_state_changed_cb)bt_event_slot_container[event_index].callback)
		(BT_A2DP_SINK_AUDIO_SUSPEND, device_addr, NULL, bt_event_slot_container[event_index].user_data);
                break;
        case BLUETOOTH_EVENT_A2DP_SINK_AUDIO_STOPPED:
                BT_INFO("BLUETOOTH_EVENT_A2DP_SINK_AUDIO_STOPPED ");
                device_addr = (char *)(param->param_data);
                ((bt_a2dp_sink_audio_state_changed_cb)bt_event_slot_container[event_index].callback)
		(BT_A2DP_SINK_AUDIO_STOPPED, device_addr, NULL, bt_event_slot_container[event_index].user_data);
                break;
        case BLUETOOTH_EVENT_A2DP_SINK_AUDIO_STARTED:
                BT_INFO("BLUETOOTH_EVENT_A2DP_SINK_AUDIO_STARTED ");
		bluetooth_a2dp_sink_audio_info_t *info = (bluetooth_a2dp_sink_audio_info_t *)(param->param_data);
		audio_info = (bt_a2dp_sink_audio_info_s *)g_malloc0(sizeof(bt_a2dp_sink_audio_info_s));
                audio_info->info_status = info->info_status;
		audio_info->sampling_freq = info->sampling_freq;
		audio_info->num_channel = info->num_channel;
		audio_info->bits_per_sample = info->bits_per_sample;
                ((bt_a2dp_sink_audio_state_changed_cb)bt_event_slot_container[event_index].callback)
		(BT_A2DP_SINK_AUDIO_STARTED, info->addr, audio_info, bt_event_slot_container[event_index].user_data);
                break;
	case BLUETOOTH_AUDIO_DATA_RECVD:{
                ((bt_audio_data_receive_cb)bt_event_slot_container[event_index].callback)
                ((bt_audio_data_s *)(param->param_data),bt_event_slot_container[event_index].user_data);
                break;
		}
	case BLUETOOTH_HID_CONNECTED:
		BT_INFO("BLUETOOTH_HID_CONNECTED");
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);
		BT_ERR("@@@ result= %d" , _bt_get_error_code(param->result));
		((bt_hid_host_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), TRUE, device_addr, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		break;
	case BLUETOOTH_HID_DISCONNECTED:
		BT_INFO("BLUETOOTH_HID_DISCONNECTED");
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);
		((bt_hid_host_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), FALSE, device_addr, bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		break;
	case BLUETOOTH_HID_SMARTRC_FW_UPDATE_PROGRESS: {
		progress = *((unsigned char *) (param->param_data));
		BT_INFO("BLUETOOTH_HID_SMARTRC_FW_UPDATE_PROGRESS progress=0x%x", progress);
			((bt_hid_notify_smart_rc_fw_update_status_cb)bt_event_slot_container[event_index].callback)
				(_bt_get_error_code(param->result), progress, bt_event_slot_container[event_index].user_data);
		break;
		}
	case BLUETOOTH_HID_SMARTRC_FW_UPDATE_COMPLETE: {
		BT_INFO("BLUETOOTH_HID_SMARTRC_FW_UPDATE_COMPLETE");
		if(_bt_get_error_code(param->result) == BT_ERROR_NONE)
			progress = 0x64; /* To indicate 100% completion */
		else
			progress = 0xFF; /* To indicate invalid progress in case of error */
		((bt_hid_notify_smart_rc_fw_update_status_cb)bt_event_slot_container[event_index].callback)
				(_bt_get_error_code(param->result), progress, bt_event_slot_container[event_index].user_data);
		break;
		}
	case BLUETOOTH_EVENT_CHIP_FW_UPDATE_PROGRESS: {
		progress = *((unsigned char *) (param->param_data));
		if (_bt_get_error_code(param->result) == BT_ERROR_NONE) {
			BT_INFO("BLUETOOTH_EVENT_CHIP_FW_UPDATE_PROGRESS: No Error");
		} else {
			BT_ERR("BLUETOOTH_EVENT_CHIP_FW_UPDATE_PROGRESS: Error!!!");
			progress = 0xFF; /* To indicate invalid progress in case of error */
		}
		BT_INFO("BLUETOOTH_EVENT_CHIP_FW_UPDATE_PROGRESS: progress=0x%x", progress);
			((bt_adapter_notify_bt_chip_fw_update_status_cb)bt_event_slot_container[event_index].callback)
				(_bt_get_error_code(param->result), progress, bt_event_slot_container[event_index].user_data);
		break;
		}
	case BLUETOOTH_HID_AUDIO_DATA_RECVD:
		if (is_smartc_tput)
		{
			tput_data.total_bytes += ((bt_hid_voice_data_s *)(param->param_data))->length;
		}
		else
		{
			((bt_hid_audio_data_receive_cb)bt_event_slot_container[event_index].callback)
				((bt_hid_voice_data_s *)(param->param_data),
					bt_event_slot_container[event_index].user_data);
		}
		break;
	case BLUETOOTH_HID_VOICE_KEY_PRESSED: {
			int btn_state = *((int *) (param->param_data));
			BT_INFO("bt_hid_notify_sound_key_pressed_cb() will be called "
					"with BT_EVENT_HID_VOICE_KEY_PRESSED");
			((bt_hid_notify_sound_key_pressed_cb)bt_event_slot_container[event_index].callback)
				(btn_state, bt_event_slot_container[event_index].user_data);
			break;
		}

	case BLUETOOTH_HID_BATTERY_STATUS_RECEIVED:
		BT_INFO("(bt_hid_notify_battery_status_received_cb will be called \
				with BT_EVENT_HID_BATTERY_STATUS_RECEIVED");
		((bt_hid_notify_battery_status_received_cb)bt_event_slot_container[event_index].callback)
			((bt_hid_battery_status_info_s *)(param->param_data),
				bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_HID_MBR_RETURN_PKT_RECEIVED:
		BT_INFO("(bt_hid_mbr_return_receive_cb will be called \
				with BT_EVENT_HID_MBR_RETURN_RECVD");
		((bt_hid_mbr_return_receive_cb)bt_event_slot_container[event_index].callback)
			((bt_hid_mbr_return_s *)(param->param_data),
				bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_HID_SENSOR_DATA_RECEIVED:
		((bt_hid_sensor_data_receive_cb)bt_event_slot_container[event_index].callback)
			((bt_smart_rc_sensor_data_s *)(param->param_data),
				bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_HID_SMARTRC_CONNECTED: {
		BT_INFO("(bt_hid_notify_smart_rc_connection_cb will be called \
                                with BT_HID_SMARTRC_CONNECTION_STATUS as connected\n");
		_bt_get_bt_device_info_s(&bonded_device, (bluetooth_device_info_t *)(param->param_data));
                ((bt_hid_notify_smart_rc_connection_cb)bt_event_slot_container[event_index].callback)
                    (TRUE, bonded_device, bt_event_slot_container[event_index].user_data);
                _bt_free_bt_device_info_s(bonded_device);
                bonded_device = NULL;

		break;
		}
	case BLUETOOTH_HID_SMARTRC_DISCONNECTED: {
		BT_INFO("(bt_hid_notify_smart_rc_connection_cb will be called \
                                with BT_HID_SMARTRC_CONNECTION_STATUS as disconnected\n");
		_bt_get_bt_device_info_s(&bonded_device, (bluetooth_device_info_t *)(param->param_data));
                ((bt_hid_notify_smart_rc_connection_cb)bt_event_slot_container[event_index].callback)
                    (FALSE, bonded_device, bt_event_slot_container[event_index].user_data);
                _bt_free_bt_device_info_s(bonded_device);
                bonded_device = NULL;

		break;
		}

	case BLUETOOTH_EVENT_GATT_SVC_CHAR_DISCOVERED:
		BT_INFO("BLUETOOTH_EVENT_GATT_SVC_CHAR_DISCOVERED");
		svc_char = (bt_gatt_discovered_char_t *)(param->param_data);

		if (svc_char == NULL)
			return;

		for (i = 1; i <= svc_char->handle_info.count; i++) {
			((bt_gatt_characteristics_discovered_cb)bt_event_slot_container[event_index].callback)
			 (_bt_get_error_code(param->result), i, svc_char->handle_info.count, svc_char->handle_info.handle[i-1],
			  bt_event_slot_container[event_index].user_data);
		}

		_bt_unset_cb(BT_EVENT_GATT_CHARACTERISTIC_DISCOVERED);

		break;
	case BLUETOOTH_EVENT_GATT_CHAR_VAL_CHANGED:
		BT_INFO("BLUETOOTH_EVENT_GATT_CHAR_VAL_CHANGED");
		char_val = (bt_gatt_char_value_t *)(param->param_data);

		if (char_val == NULL)
			return;

		((bt_gatt_characteristic_changed_cb)bt_event_slot_container[event_index].callback)
		 (char_val->char_handle, char_val->char_value, char_val->val_len,
		  bt_event_slot_container[event_index].user_data);

		break;
	case BLUETOOTH_EVENT_SYNC3D_ASSOCIATION_NOTIFY:
		BT_INFO("BLUETOOTH_EVENT_SYNC3D_ASSOCIATION_NOTIFY");
		((bt_sync3d_association_notified_cb)bt_event_slot_container[event_index].callback)
		((bt_sync3d_association_notify_s *)(param->param_data),
		bt_event_slot_container[event_index].user_data);

		break;
	case BLUETOOTH_EVENT_SYNC3D_VSPEC_CODE_CHANGE:
		BT_INFO("BLUETOOTH_EVENT_SYNC3D_VSPEC_CODE_CHANGE");
		((bt_sync3d_event_cb)bt_event_slot_container[event_index].callback)
		(NULL, SYNC3D_CODE_CHANGE_EVENT,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_SYNC3D_VSPEC_FRAME_PERIOD:
		BT_INFO("BLUETOOTH_EVENT_SYNC3D_VSPEC_FRAME_PERIOD");
		((bt_sync3d_event_cb)bt_event_slot_container[event_index].callback)
		((bt_sync3d_frame_period_s *)(param->param_data),
		SYNC3D_FRAME_PERIOD_CHANGE_EVENT, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_ADVERTISING_STARTED:
		BT_INFO("BLUETOOTH_EVENT_ADVERTISING_STARTED");
		((bt_adapter_advertising_state_changed_cb)bt_event_slot_container[event_index].callback)
		 (_bt_get_error_code(param->result), BT_ADAPTER_ADVERTISING_STARTED,
		  (bt_adapter_advertising_params_s *)param->param_data,
		  bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_ADVERTISING_STOPPED:
		BT_INFO("BLUETOOTH_EVENT_ADVERTISING_STOPPED");
		((bt_adapter_advertising_state_changed_cb)bt_event_slot_container[event_index].callback)
		 (_bt_get_error_code(param->result), BT_ADAPTER_ADVERTISING_STOPPED, NULL,
		  bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_ADVERTISING_MANUFACTURER_DATA_CHANGED:
		BT_INFO("BLUETOOTH_EVENT_ADVERTISING_MANUFACTURER_DATA_CHANGED");
		((bt_adapter_advertising_manufacturer_data_changed_cb)bt_event_slot_container[event_index].callback)
		 (param->param_data, param->result,
		  bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_SCAN_RESPONSE_MANUFACTURER_DATA_CHANGED:
		BT_INFO("BLUETOOTH_EVENT_SCAN_RESPONSE_MANUFACTURER_DATA_CHANGED");
		((bt_adapter_scan_resp_manufacturer_data_changed_cb)bt_event_slot_container[event_index].callback)
		 (param->param_data, param->result,
		  bt_event_slot_container[event_index].user_data);
		break;

	default:
		break;
	}
}

/* For LE Only Scan */
static void __bt_le_event_proxy(int event, bluetooth_event_param_t *param, void *user_data)
{
	bt_socket_connection_s rfcomm_connection;
	bt_adapter_device_discovery_info_s *discovery_info = NULL;
	int event_index = -1;

	event_index = __bt_get_cb_index(event);
	if (event_index == -1 || bt_event_slot_container[event_index].callback == NULL) {
		return;
	}
	memset(&rfcomm_connection, 0x00, sizeof(bt_socket_connection_s));

	switch (event) {
		case BLUETOOTH_EVENT_LE_DISCOVERY_STARTED:
			BT_INFO("BLUETOOTH_EVENT_LE_DISCOVERY_STARTED");
			((bt_adapter_le_device_discovery_state_changed_cb) bt_event_slot_container[event_index].callback)
				(_bt_get_error_code(param->result), BT_ADAPTER_LE_DEVICE_DISCOVERY_STARTED, NULL, bt_event_slot_container[event_index].user_data);
			break;
		case BLUETOOTH_EVENT_LE_DISCOVERY_FINISHED:
			BT_INFO("BLUETOOTH_EVENT_LE_DISCOVERY_FINISHED");
			((bt_adapter_le_device_discovery_state_changed_cb)bt_event_slot_container[event_index].callback)
				(_bt_get_error_code(param->result), BT_ADAPTER_LE_DEVICE_DISCOVERY_FINISHED, NULL, bt_event_slot_container[event_index].user_data);
			break;
		case BLUETOOTH_EVENT_REMOTE_LE_ONLY_DEVICE_FOUND:
			BT_INFO("BLUETOOTH_EVENT_REMOTE_LE_ONLY_DEVICE_FOUND");
			if (__bt_get_bt_adapter_le_device_discovery_info_s(&discovery_info, (bluetooth_le_device_info_t *)(param->param_data)) == BT_ERROR_NONE) {
				((bt_adapter_le_device_discovery_state_changed_cb)bt_event_slot_container[event_index].callback)
					(_bt_get_error_code(param->result), BT_ADAPTER_LE_DEVICE_DISCOVERY_FOUND, discovery_info, bt_event_slot_container[event_index].user_data);
				__bt_free_bt_adapter_le_device_discovery_info_s(discovery_info);
			} else {
				((bt_adapter_le_device_discovery_state_changed_cb)bt_event_slot_container[event_index].callback)
					(_bt_get_error_code(param->result), BT_ADAPTER_LE_DEVICE_DISCOVERY_FOUND, NULL, bt_event_slot_container[event_index].user_data);
			}
			break;
		default:
			break;
	}
}

static int __bt_get_bt_adapter_device_discovery_info_s(bt_adapter_device_discovery_info_s **discovery_info, bluetooth_device_info_t *source_info) {
	int i;

	BT_CHECK_INPUT_PARAMETER(source_info);

	*discovery_info = (bt_adapter_device_discovery_info_s *)g_malloc0(sizeof(bt_adapter_device_discovery_info_s));
	if (*discovery_info == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	if (strlen(source_info->device_name.name) > 0) {
		(*discovery_info)->remote_name = strdup(source_info->device_name.name);
	} else {
		(*discovery_info)->remote_name = NULL;
	}

	_bt_convert_address_to_string(&((*discovery_info)->remote_address), &(source_info->device_address));

	(*discovery_info)->bt_class.major_device_class = source_info->device_class.major_class;
	(*discovery_info)->bt_class.minor_device_class = source_info->device_class.minor_class;
	(*discovery_info)->bt_class.major_service_class_mask = source_info->device_class.service_class;
	(*discovery_info)->vendor_id = source_info->vid;
	(*discovery_info)->product_id = source_info->pid;

	BT_INFO("Remote Device Found: vid = [0x%x] pid= [0x%x]", (*discovery_info)->vendor_id, (*discovery_info)->product_id);
	if (source_info->service_index > 0) {
		(*discovery_info)->service_uuid = (char **)malloc(sizeof(char *) * source_info->service_index);
		if ((*discovery_info)->service_uuid != NULL) {
			for (i = 0; i < source_info->service_index; i++) {
				(*discovery_info)->service_uuid[i] = strdup(source_info->uuids[i]);
				if ((*discovery_info)->service_uuid[i] != NULL) {
					__bt_convert_lower_to_upper((*discovery_info)->service_uuid[i]);
				}

				BT_DBG("UUID: %s", (*discovery_info)->service_uuid[i]);
			}
		}
	} else {
		(*discovery_info)->service_uuid = NULL;
	}
	(*discovery_info)->service_count = source_info->service_index;

	(*discovery_info)->rssi = (int)source_info->rssi;
	
	(*discovery_info)->is_bonded = (bool)source_info->paired;

	return BT_ERROR_NONE;
}

static void __bt_free_bt_adapter_device_discovery_info_s(bt_adapter_device_discovery_info_s *discovery_info)
{
	int i;

	if (discovery_info == NULL)
		return;

	if (discovery_info->remote_name != NULL)
		free(discovery_info->remote_name);

	if (discovery_info->remote_address != NULL)
		free(discovery_info->remote_address);

	if (discovery_info->service_uuid != NULL) {
		for (i = 0; i < discovery_info->service_count; i++) {
			if (discovery_info->service_uuid[i] != NULL)
				free(discovery_info->service_uuid[i]);
		}
		free(discovery_info->service_uuid);
	}

	if (discovery_info->adv_data != NULL)
		free(discovery_info->adv_data);
	if (discovery_info->scan_data != NULL)
		free(discovery_info->scan_data);

	free(discovery_info);
	discovery_info = NULL;
}

static int __bt_get_bt_adapter_le_device_discovery_info_s(
			bt_adapter_device_discovery_info_s **le_discovery_info,
			bluetooth_le_device_info_t *source_info)
{

	BT_CHECK_INPUT_PARAMETER(source_info);

	*le_discovery_info = (bt_adapter_device_discovery_info_s *)g_malloc0(sizeof(bt_adapter_device_discovery_info_s));
	if (*le_discovery_info == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	if (strlen(source_info->device_name.name) > 0) {
		(*le_discovery_info)->remote_name = strdup(source_info->device_name.name);
	} else {
		(*le_discovery_info)->remote_name = NULL;
	}

	_bt_convert_address_to_string(&((*le_discovery_info)->remote_address), &(source_info->device_address));

	(*le_discovery_info)->appearance = 0;
	(*le_discovery_info)->service_uuid = NULL;
	(*le_discovery_info)->service_count = 0;

	(*le_discovery_info)->rssi = (int)source_info->rssi;
	(*le_discovery_info)->is_bonded = (bool)source_info->paired;
	(*le_discovery_info)->adv_data_len = source_info->adv_ind_data.data_len;
	if ((*le_discovery_info)->adv_data_len > 0) {
		(*le_discovery_info)->adv_data = malloc(source_info->adv_ind_data.data_len);
		memcpy((*le_discovery_info)->adv_data, source_info->adv_ind_data.data.data, source_info->adv_ind_data.data_len);
	} else {
		(*le_discovery_info)->adv_data = NULL;
	}

	(*le_discovery_info)->scan_data_len = source_info->scan_resp_data.data_len;
	if ((*le_discovery_info)->scan_data_len > 0) {
		(*le_discovery_info)->scan_data = malloc(source_info->scan_resp_data.data_len);
		memcpy((*le_discovery_info)->scan_data, source_info->scan_resp_data.data.data, source_info->scan_resp_data.data_len);
	} else {
		(*le_discovery_info)->scan_data = NULL;
	}
	return BT_ERROR_NONE;
}

static void __bt_free_bt_adapter_le_device_discovery_info_s(bt_adapter_device_discovery_info_s *discovery_info)
{
	int i;

	if (discovery_info == NULL)
		return;

	if (discovery_info->remote_address != NULL)
		free(discovery_info->remote_address);

	if (discovery_info->remote_name != NULL)
		free(discovery_info->remote_name);

	if (discovery_info->service_uuid != NULL) {
		for (i = 0; i < discovery_info->service_count; i++) {
			if (discovery_info->service_uuid[i] != NULL)
				free(discovery_info->service_uuid[i]);
		}
		free(discovery_info->service_uuid);
	}

	if (discovery_info->adv_data != NULL)
		free(discovery_info->adv_data);
	if (discovery_info->scan_data != NULL)
		free(discovery_info->scan_data);

	free(discovery_info);
	discovery_info = NULL;
}

static int __bt_get_cb_index(int event)
{
	switch (event) {
	case BLUETOOTH_EVENT_ENABLED:
	case BLUETOOTH_EVENT_DISABLED:
		return BT_EVENT_STATE_CHANGED;
	case BLUETOOTH_EVENT_LOCAL_NAME_CHANGED:
		return BT_EVENT_NAME_CHANGED;
	case BLUETOOTH_EVENT_DISCOVERABLE_MODE_CHANGED:
		return BT_EVENT_VISIBILITY_MODE_CHANGED;
	case BLUETOOTH_EVENT_DISCOVERY_STARTED:
	case BLUETOOTH_EVENT_DISCOVERY_FINISHED:
	case BLUETOOTH_EVENT_REMOTE_DEVICE_NAME_UPDATED:
	case BLUETOOTH_EVENT_REMOTE_LE_DEVICE_FOUND:
		BT_ERR("BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED");
		return BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED;
	case BLUETOOTH_EVENT_LE_DISCOVERY_STARTED:
	case BLUETOOTH_EVENT_LE_DISCOVERY_FINISHED:
	/* In order to call LE specific discovery callback, we need a separate callback index,
	   BLUETOOTH_EVENT_REMOTE_LE_DEVICE_FOUND already returns BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED
	   which is the index for genric or interleaved discovery callback, so for LE ONLY scan, we dont want
	   to use genric discovery callback, so a separate event BLUETOOTH_EVENT_REMOTE_LE_ONLY_DEVICE_FOUND
	   is created */
	case BLUETOOTH_EVENT_REMOTE_LE_ONLY_DEVICE_FOUND:
		BT_ERR("BT_EVENT_LE_DEVICE_DISCOVERY_STATE_CHANGED");
		return BT_EVENT_LE_DEVICE_DISCOVERY_STATE_CHANGED;
	case BLUETOOTH_EVENT_BONDING_FINISHED:
		return BT_EVENT_BOND_CREATED;
	case BLUETOOTH_EVENT_BONDED_DEVICE_REMOVED:
		return BT_EVENT_BOND_DESTROYED;
	case BLUETOOTH_EVENT_DEVICE_LIMIT_REACHED:
		return BT_EVENT_MAX_CONNECTION_REACHED;
	case BLUETOOTH_EVENT_KEYBOARD_PASSKEY_DISPLAY:
	case BLUETOOTH_EVENT_PASSKEY_REQUEST:
	case BLUETOOTH_EVENT_PIN_REQUEST:
	case BLUETOOTH_EVENT_PASSKEY_CONFIRM_REQUEST:
	case BLUETOOTH_EVENT_SSP_CONSENT_REQUEST:
		return BT_EVENT_AUTHENTICATION_REQUEST;
	case BLUETOOTH_EVENT_DEVICE_AUTHORIZED:
	case BLUETOOTH_EVENT_DEVICE_UNAUTHORIZED:
		return BT_EVENT_AUTHORIZATION_CHANGED;
	case BLUETOOTH_EVENT_DEVICE_CONNECTED:
	case BLUETOOTH_EVENT_DEVICE_DISCONNECTED:
		return BT_EVENT_DEVICE_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_AUTHORIZE_REQUEST:
		return BT_EVENT_AUTHORIZE_REQUEST;
	case BLUETOOTH_EVENT_SERVICE_SEARCHED:
		return BT_EVENT_SERVICE_SEARCHED;
	case BLUETOOTH_EVENT_RFCOMM_DATA_RECEIVED:
		return BT_EVENT_DATA_RECEIVED;
	case BLUETOOTH_EVENT_RFCOMM_CONNECTED:
	case BLUETOOTH_EVENT_RFCOMM_DISCONNECTED:
		return BT_EVENT_CONNECTION_STATE_CHANGED;
	case BLUETOOTH_EVENT_RFCOMM_AUTHORIZE:
		return BT_EVENT_RFCOMM_CONNECTION_REQUESTED;
	case BLUETOOTH_EVENT_OBEX_SERVER_CONNECTION_AUTHORIZE:
		return BT_EVENT_OPP_CONNECTION_REQUESTED;
	case BLUETOOTH_EVENT_OBEX_SERVER_TRANSFER_AUTHORIZE:
		return BT_EVENT_OPP_PUSH_REQUESTED;
	case BLUETOOTH_EVENT_OBEX_SERVER_TRANSFER_STARTED:
		return BT_EVENT_OPP_SERVER_TRANSFER_PROGRESS;
	case BLUETOOTH_EVENT_OBEX_SERVER_TRANSFER_PROGRESS:
		return BT_EVENT_OPP_SERVER_TRANSFER_PROGRESS;
	case BLUETOOTH_EVENT_OBEX_SERVER_TRANSFER_COMPLETED:
		return BT_EVENT_OPP_SERVER_TRANSFER_FINISHED;
	case BLUETOOTH_EVENT_OPC_CONNECTED:
		return BT_EVENT_OPP_CLIENT_PUSH_RESPONSED;
	case BLUETOOTH_EVENT_OPC_TRANSFER_STARTED:
		return BT_EVENT_OPP_CLIENT_PUSH_PROGRESS;
	case BLUETOOTH_EVENT_OPC_TRANSFER_PROGRESS:
	case BLUETOOTH_EVENT_OPC_TRANSFER_COMPLETE:
		return BT_EVENT_OPP_CLIENT_PUSH_PROGRESS;
	case BLUETOOTH_EVENT_OPC_DISCONNECTED:
		return BT_EVENT_OPP_CLIENT_PUSH_FINISHED;
	case BLUETOOTH_EVENT_NETWORK_SERVER_CONNECTED:
	case BLUETOOTH_EVENT_NETWORK_SERVER_DISCONNECTED:
		return BT_EVENT_NAP_CONNECTION_STATE_CHANGED;
	case BLUETOOTH_EVENT_NETWORK_CONNECTED:
	case BLUETOOTH_EVENT_NETWORK_DISCONNECTED:
		return BT_EVENT_PAN_CONNECTION_STATE_CHANGED;
	case BLUETOOTH_EVENT_HDP_DATA_RECEIVED:
		return BT_EVENT_HDP_DATA_RECEIVED;
	case BLUETOOTH_EVENT_HDP_CONNECTED:
		return BT_EVENT_HDP_CONNECTED;
	case BLUETOOTH_EVENT_HDP_DISCONNECTED:
		return BT_EVENT_HDP_DISCONNECTED;
	case BLUETOOTH_EVENT_AG_CONNECTED:
		return BT_EVENT_AUDIO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_AG_DISCONNECTED:
		return BT_EVENT_AUDIO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_AG_SPEAKER_GAIN:
		return BT_EVENT_AG_SPEAKER_GAIN_CHANGE;
	case BLUETOOTH_EVENT_AG_MIC_GAIN:
		return BT_EVENT_AG_MICROPHONE_GAIN_CHANGE;
	case BLUETOOTH_EVENT_AG_AUDIO_CONNECTED:
		return BT_EVENT_AUDIO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_AG_AUDIO_DISCONNECTED:
		return BT_EVENT_AUDIO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_TELEPHONY_AUDIO_CONNECTED:
		return BT_EVENT_AG_SCO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_TELEPHONY_AUDIO_DISCONNECTED:
		return BT_EVENT_AG_SCO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_TELEPHONY_ANSWER_CALL:
	case BLUETOOTH_EVENT_TELEPHONY_RELEASE_CALL:
	case BLUETOOTH_EVENT_TELEPHONY_REJECT_CALL:
		return BT_EVENT_AG_CALL_HANDLING_EVENT;
	case BLUETOOTH_EVENT_TELEPHONY_CHLD_0_RELEASE_ALL_HELD_CALL:
	case BLUETOOTH_EVENT_TELEPHONY_CHLD_1_RELEASE_ALL_ACTIVE_CALL:
	case BLUETOOTH_EVENT_TELEPHONY_CHLD_2_ACTIVE_HELD_CALL:
	case BLUETOOTH_EVENT_TELEPHONY_CHLD_3_MERGE_CALL:
	case BLUETOOTH_EVENT_TELEPHONY_CHLD_4_EXPLICIT_CALL_TRANSFER:
		return BT_EVENT_AG_MULTI_CALL_HANDLING_EVENT;
	case BLUETOOTH_EVENT_TELEPHONY_SEND_DTMF:
		return BT_EVENT_AG_DTMF_TRANSMITTED;
	case BLUETOOTH_EVENT_AV_CONNECTED:
		return BT_EVENT_AUDIO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_AV_DISCONNECTED:
		return BT_EVENT_AUDIO_CONNECTION_STATUS;
	case  BLUETOOTH_EVENT_AVRCP_CONNECTED:
	case  BLUETOOTH_EVENT_AVRCP_DISCONNECTED:
		return BT_EVENT_AVRCP_CONNECTION_STATUS;
	case  BLUETOOTH_EVENT_AVRCP_SETTING_SHUFFLE_STATUS:
		return BT_EVENT_AVRCP_SHUFFLE_MODE_CHANGED;
	case  BLUETOOTH_EVENT_AVRCP_SETTING_EQUALIZER_STATUS:
		return BT_EVENT_AVRCP_EQUALIZER_STATE_CHANGED;
	case  BLUETOOTH_EVENT_AVRCP_SETTING_REPEAT_STATUS:
		return BT_EVENT_AVRCP_REPEAT_MODE_CHANGED;
	case  BLUETOOTH_EVENT_AVRCP_SETTING_SCAN_STATUS:
		return BT_EVENT_AVRCP_SCAN_MODE_CHANGED;
	case BLUETOOTH_EVENT_AVRCP_VOLUME_CHANGED:
		return BT_EVENT_AVRCP_VOLUME_CHANGED;
	case BLUETOOTH_EVENT_AVRCP_MUTE_STATUS_CHANGED:
		return BT_EVENT_AVRCP_MUTE_STATUS_CHANGED;
	case BLUETOOTH_EVENT_A2DP_SINK_CONNECTED:
                return BT_EVENT_A2DP_SINK_CONNECTION_STATUS;
        case BLUETOOTH_EVENT_A2DP_SINK_DISCONNECTED:
                return BT_EVENT_A2DP_SINK_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_A2DP_SINK_AUDIO_SUSPEND:
	case BLUETOOTH_EVENT_A2DP_SINK_AUDIO_STOPPED:
	case BLUETOOTH_EVENT_A2DP_SINK_AUDIO_STARTED:
		return BT_EVENT_A2DP_SINK_AUDIO_STATUS;
	case BLUETOOTH_AUDIO_DATA_RECVD:
		return BT_EVENT_AUDIO_DATA_RECVD;
	case BLUETOOTH_HID_CONNECTED:
		return  BT_EVENT_HID_CONNECTION_STATUS;
	case BLUETOOTH_HID_DISCONNECTED:
		return  BT_EVENT_HID_CONNECTION_STATUS;
	case BLUETOOTH_HID_SMARTRC_FW_UPDATE_PROGRESS:
	case BLUETOOTH_HID_SMARTRC_FW_UPDATE_COMPLETE:
		BT_ERR("BLUETOOTH_HID_SMARTRC_FW_UPDATE_STATUS");
		return BT_EVENT_HID_SMARTRC_FW_UPDATE_STATUS;
	case BLUETOOTH_EVENT_CHIP_FW_UPDATE_PROGRESS:
		BT_ERR("BLUETOOTH_EVENT_CHIP_FW_UPDATE_STATUS");
		return BT_EVENT_BT_CHIP_FW_UPDATE_STATUS;
	case BLUETOOTH_HID_AUDIO_DATA_RECVD:
		if (is_smartc_tput)
			return BT_EVENT_DUT_MODE;
		else
			return BT_EVENT_HID_AUDIO_DATA_RECVD;
	case BLUETOOTH_HID_VOICE_KEY_PRESSED:
		return  BT_EVENT_HID_VOICE_KEY_PRESSED;
	case BLUETOOTH_HID_BATTERY_STATUS_RECEIVED:
		return  BT_EVENT_HID_BATTERY_STATUS_RECEIVED;
	case BLUETOOTH_HID_MBR_RETURN_PKT_RECEIVED:
		return  BT_EVENT_HID_MBR_RETURN_RECVD;
	case BLUETOOTH_HID_SENSOR_DATA_RECEIVED:
		return  BT_EVENT_HID_SENSOR_DATA_RECEIVED;
	case BLUETOOTH_HID_SMARTRC_CONNECTED:
	case BLUETOOTH_HID_SMARTRC_DISCONNECTED:
		return  BT_EVENT_HID_SMARTRC_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_GATT_SVC_CHAR_DISCOVERED:
		return	BT_EVENT_GATT_CHARACTERISTIC_DISCOVERED;
	case BLUETOOTH_EVENT_GATT_CHAR_VAL_CHANGED:
		return	BT_EVENT_GATT_VALUE_CHANGED;
	case BLUETOOTH_EVENT_GATT_WRITE_CHAR:
		return	BT_EVENT_GATT_WRITE_CHARACTERISTIC;
	case BLUETOOTH_EVENT_SYNC3D_ASSOCIATION_NOTIFY:
		return BT_EVENT_SYNC3D_ASSOCIATION_NOTIFIED;
	case BLUETOOTH_EVENT_SYNC3D_VSPEC_CODE_CHANGE:
	case BLUETOOTH_EVENT_SYNC3D_VSPEC_FRAME_PERIOD:
		return BT_EVENT_SYNC3D_VSPEC_EVENT_RECEIVED;
	case BLUETOOTH_EVENT_ADVERTISING_STARTED:
	case BLUETOOTH_EVENT_ADVERTISING_STOPPED:
		return BT_EVENT_ADVERTISING_STATE_CHANGED;
	case BLUETOOTH_EVENT_ADVERTISING_MANUFACTURER_DATA_CHANGED:
		return BT_EVENT_ADVERTISING_MANUFACTURER_DATA_CHANGED;
	case BLUETOOTH_EVENT_SCAN_RESPONSE_MANUFACTURER_DATA_CHANGED:
		return BT_EVENT_SCAN_RESPONSE_MANUFACTURER_DATA_CHANGED;
	default:
		return -1;
	}
}

static void __bt_convert_lower_to_upper(char *origin)
{
	int length = strlen(origin);
	int i = 0;

	for (i = 0; i < length; i++) {
		if (islower(origin[i])) {
			origin[i] = toupper(origin[i]);
		}
	}
}

static gboolean _bt_smartrc_tput_mode_timer_expired(gpointer user_data)
{
	int ret = BT_ERROR_NONE;
	unsigned char input_data[3];
	struct timeval tput_end_time;
	long seconds, useconds, total_time;
	input_data[0] = 0x03; input_data[1]=0x00; input_data[2]=0x00;

	BT_DBG("Throughput mode timer expired");
	if (!is_smartc_tput)
		return (false);

	ret = bt_hid_send_rc_command(NULL, input_data, 1);
	if (ret < BT_ERROR_NONE)
		BT_DBG("failed with [0x%04x]", ret);
	else
		BT_DBG("Send RC command successfull for stop");

	gettimeofday(&tput_end_time, NULL);
	seconds	= tput_end_time.tv_sec - tput_start_time.tv_sec;
	useconds = tput_end_time.tv_usec - tput_start_time.tv_usec;
	total_time = (seconds * 1000) + (useconds/1000);
	tput_data.throughput = ((long)(tput_data.total_bytes*8)/total_time)*1000;
	tput_data.total_msecs = total_time;

	((bt_adapter_smartrc_tput_mode_cb)bt_event_slot_container[BT_EVENT_DUT_MODE].callback)
		(&tput_data, bt_event_slot_container[BT_EVENT_DUT_MODE].user_data);

	is_smartc_tput = false;

	return (false);
}

int _bt_start_smartrc_tput_mode(void)
{
	int ret = BT_ERROR_NONE;
	unsigned char input_data[3];

	memset(&tput_data, 0x00, sizeof(tput_data));
	input_data[0] = 0x04; input_data[1]=0x00; input_data[2]=0x00;
	BT_DBG("Starting DUT mode for SmartRC");
	ret = bt_hid_send_rc_command(NULL, input_data, 1);
	gettimeofday(&tput_start_time, NULL);
	if (ret < BT_ERROR_NONE)
	{
		BT_DBG("failed with [0x%04x]", ret);
		return (ret);
	}

	g_timeout_add_seconds(15, _bt_smartrc_tput_mode_timer_expired, NULL);
	is_smartc_tput = true;
	BT_DBG("Started DUT mode for SmartRC");
	return (BT_ERROR_NONE);
}

int _bt_stop_smartrc_tput_mode(void)
{
	g_timeout_add_seconds(0.2, _bt_smartrc_tput_mode_timer_expired, NULL);

	return (BT_ERROR_NONE);
}

int _bt_get_bt_a2dp_sink_audio_info_s(bt_a2dp_sink_audio_info_s **dest_audio_info, bluetooth_a2dp_sink_audio_info_t *source_audio_info)
{
	BT_CHECK_INPUT_PARAMETER(source_audio_info);

	*dest_audio_info = (bt_a2dp_sink_audio_info_s*) g_try_malloc0(sizeof(bt_a2dp_sink_audio_info_s));

	if (*dest_audio_info == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	(*dest_audio_info)->sampling_freq = source_audio_info->sampling_freq;

	(*dest_audio_info)->num_channel = source_audio_info->num_channel;

	(*dest_audio_info)->bits_per_sample = source_audio_info->bits_per_sample;

	return BT_ERROR_NONE;

}

void _bt_free_bt_a2dp_sink_audio_info_s(bt_a2dp_sink_audio_info_s *audio_info)
{
	if (audio_info != NULL)
		g_free(audio_info);
	return;
}
