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
#include <bluetooth-api.h>

#include "bluetooth.h"
#include "bluetooth_private.h"
#include "bluetooth-media-control.h"
#include "bluetooth-telephony-api.h"

static bool is_initialized = false;
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
	{BT_EVENT_HID_CONNECTION_STATUS, NULL, NULL},
	{BT_EVENT_DEVICE_CONNECTION_STATUS, NULL, NULL},
	{BT_EVENT_GATT_CHARACTERISTIC_DISCOVERED, NULL, NULL},
	{BT_EVENT_GATT_VALUE_CHANGED, NULL, NULL},
	{BT_EVENT_GATT_READ_CHARACTERISTIC, NULL, NULL},
	{BT_EVENT_GATT_WRITE_CHARACTERISTIC, NULL, NULL}
};

/*
 *  Internal Functions
 */
static void __bt_event_proxy(int event, bluetooth_event_param_t * param, void *user_data);
static int __bt_get_cb_index(int event);
static void __bt_convert_lower_to_upper(char *origin);
static int __bt_get_bt_device_sdp_info_s(bt_device_sdp_info_s **dest, bt_sdp_info_t *source);
static void __bt_free_bt_device_sdp_info_s(bt_device_sdp_info_s *sdp_info);
static int __bt_get_bt_adapter_device_discovery_info_s(bt_adapter_device_discovery_info_s **discovery_info, bluetooth_device_info_t *source_info);
static void __bt_free_bt_adapter_device_discovery_info_s(bt_adapter_device_discovery_info_s *discovery_info);


/*
 *  Public Functions
 */

int bt_initialize(void)
{
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

int _bt_get_error_code(int origin_error)
{
	switch (origin_error) {
	case BLUETOOTH_ERROR_NONE:
		return BT_ERROR_NONE;
	case BLUETOOTH_ERROR_INVALID_DATA:
	case BLUETOOTH_ERROR_INVALID_PARAM:
	case BLUETOOTH_ERROR_NOT_CONNECTED:
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
	case BLUETOOTH_ERROR_PERMISSION_DEINED :
		return BT_ERROR_PERMISSION_DENIED;
	case BLUETOOTH_ERROR_SERVICE_NOT_FOUND:
	case BLUETOOTH_ERROR_PARING_FAILED:
	case BLUETOOTH_ERROR_MAX_CONNECTION:
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
	if (source_dev->service_index > 0) {
		(*dest_dev)->service_uuid = (char **)malloc(sizeof(char *) * source_dev->service_index);
		if ((*dest_dev)->service_uuid != NULL) {
			for (i = 0; i < source_dev->service_index; i++) {
				(*dest_dev)->service_uuid[i] = strdup(source_dev->uuids[i]);
				if ((*dest_dev)->service_uuid[i] != NULL) {
					__bt_convert_lower_to_upper((*dest_dev)->service_uuid[i]);
				}
			}
		}
	} else {
		(*dest_dev)->service_uuid = NULL;
	}
	(*dest_dev)->service_count = source_dev->service_index;
	(*dest_dev)->is_bonded = (bool)source_dev->paired;
	(*dest_dev)->is_connected = (bool)source_dev->connected;
	(*dest_dev)->is_authorized = (bool)source_dev->trust;

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

char *_bt_convert_error_to_string(int error)
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

static void __bt_event_proxy(int event, bluetooth_event_param_t *param, void *user_data)
{
	int i;
	int call_id;
	int *avrcp_mode;
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
	char *device_addr = NULL;
	int error_code = BT_ERROR_NONE;
	int event_index = -1;
	bluetooth_network_device_info_t *dev_info = NULL;
	bt_hdp_connected_t *hdp_conn_info = NULL;
	bt_hdp_disconnected_t *hdp_disconn_info = NULL;
	bt_hdp_data_ind_t *hdp_data_ind = NULL;
	bt_gatt_discovered_char_t *svc_char = NULL;
	bt_gatt_char_value_t *char_val = NULL;

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
	case BLUETOOTH_EVENT_REMOTE_DEVICE_FOUND:
		BT_INFO("bt_adapter_device_discovery_state_changed_cb() will be called with BT_ADAPTER_DEVICE_DISCOVERY_FOUND");
		if (__bt_get_bt_adapter_device_discovery_info_s(&discovery_info, (bluetooth_device_info_t *)(param->param_data)) == BT_ERROR_NONE) {
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
		_bt_convert_address_to_string(&device_addr, (bluetooth_device_address_t *)(param->param_data));
		((bt_device_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		    (true, device_addr, bt_event_slot_container[event_index].user_data);
		if (device_addr != NULL)
			free(device_addr);
		break;
	case BLUETOOTH_EVENT_DEVICE_DISCONNECTED:
		BT_INFO("bt_device_connection_state_changed_cb() will be called");
		_bt_convert_address_to_string(&device_addr, (bluetooth_device_address_t *)(param->param_data));
		((bt_device_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		    (false, device_addr, bt_event_slot_container[event_index].user_data);
		if (device_addr != NULL)
			free(device_addr);
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
		telephony_event_dtmf_t *call_data = param->param_data;

		BT_INFO("BLUETOOTH_EVENT_TELEPHONY_SEND_DTMF ");
		((bt_ag_dtmf_transmitted_cb)bt_event_slot_container[event_index].callback)
		(call_data->dtmf, bt_event_slot_container[event_index].user_data);
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
	case BLUETOOTH_HID_CONNECTED:
		BT_INFO("BLUETOOTH_HID_CONNECTED");
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);
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
	case BLUETOOTH_EVENT_GATT_READ_CHAR:
		BT_INFO("BLUETOOTH_EVENT_GATT_READ_CHAR");
		break;
	default:
		BT_INFO("__bt_event_proxy default case %d",event);
		break;
	}
}

static int __bt_get_bt_adapter_device_discovery_info_s(bt_adapter_device_discovery_info_s **discovery_info, bluetooth_device_info_t *source_info) {
	int i;

	BT_CHECK_INPUT_PARAMETER(source_info);

	*discovery_info = (bt_adapter_device_discovery_info_s *)malloc(sizeof(bt_adapter_device_discovery_info_s));
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
		return BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED;
	case BLUETOOTH_EVENT_BONDING_FINISHED:
		return BT_EVENT_BOND_CREATED;
	case BLUETOOTH_EVENT_BONDED_DEVICE_REMOVED:
		return BT_EVENT_BOND_DESTROYED;
	case BLUETOOTH_EVENT_DEVICE_AUTHORIZED:
	case BLUETOOTH_EVENT_DEVICE_UNAUTHORIZED:
		return BT_EVENT_AUTHORIZATION_CHANGED;
	case BLUETOOTH_EVENT_DEVICE_CONNECTED:
	case BLUETOOTH_EVENT_DEVICE_DISCONNECTED:
		return BT_EVENT_DEVICE_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_GATT_CONNECTED:
	case BLUETOOTH_EVENT_GATT_DISCONNECTED:
		return BT_EVENT_GATT_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_GATT_RSSI:
		return BT_EVENT_GATT_RSSI_VALUE;
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
	case BLUETOOTH_HID_CONNECTED:
		return  BT_EVENT_HID_CONNECTION_STATUS;
	case BLUETOOTH_HID_DISCONNECTED:
		return  BT_EVENT_HID_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_GATT_SVC_CHAR_DISCOVERED:
		return	BT_EVENT_GATT_CHARACTERISTIC_DISCOVERED;
	case BLUETOOTH_EVENT_GATT_CHAR_VAL_CHANGED:
		return	BT_EVENT_GATT_VALUE_CHANGED;
	case BLUETOOTH_EVENT_GATT_READ_CHAR:
		return	BT_EVENT_GATT_READ_CHARACTERISTIC;
	case BLUETOOTH_EVENT_GATT_WRITE_CHAR:
		return	BT_EVENT_GATT_WRITE_CHARACTERISTIC;
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
