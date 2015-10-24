/*
 * Copyright (c) 2011 Samsung Electronics Co., Ltd All Rights Reserved
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
#include "bluetooth_internal.h"
#include "bluetooth_extension.h"
#include "bluetooth_private.h"
#include "bluetooth-media-control.h"
#include "bluetooth-telephony-api.h"

static bool is_initialized = false;
static bool is_le_initialized = false;

static bt_event_sig_event_slot_s bt_event_slot_container[BT_EVENT_MAX] = { {NULL, NULL}, };

/*
 *  Internal Functions
 */
static void __bt_event_proxy(int event, bluetooth_event_param_t * param, void *user_data);
static void __bt_le_event_proxy(int event, bluetooth_event_param_t *param, void *user_data);
static int __bt_get_cb_index(int event);
static void __bt_convert_lower_to_upper(char *origin);
static int __bt_get_bt_device_sdp_info_s(bt_device_sdp_info_s **dest, bt_sdp_info_t *source);
static void __bt_free_bt_device_sdp_info_s(bt_device_sdp_info_s *sdp_info);
static int __bt_get_bt_device_connection_info_s(bt_device_connection_info_s **dest, bt_connection_info_t *source);
static void __bt_free_bt_device_connection_info_s(bt_device_connection_info_s *conn_info);
static int __bt_get_bt_adapter_device_discovery_info_s(bt_adapter_device_discovery_info_s **discovery_info, bluetooth_device_info_t *source_info);
static void __bt_free_bt_adapter_device_discovery_info_s(bt_adapter_device_discovery_info_s *discovery_info);
static int __bt_get_bt_adapter_le_device_scan_info_s(bt_adapter_le_device_scan_result_info_s **scan_info, bluetooth_le_device_info_t *source_info);
static void __bt_free_bt_adapter_le_device_scan_info_s(bt_adapter_le_device_scan_result_info_s *scan_info);
#ifndef TIZEN_WEARABLE
static int __bt_get_bt_adapter_le_device_discovery_info_s(bt_adapter_le_device_discovery_info_s **le_discovery_info, bluetooth_le_device_info_t *source_info);
static void __bt_free_bt_adapter_le_device_discovery_info_s(bt_adapter_le_device_discovery_info_s *discovery_info);
#endif
static int __bt_gatt_client_update_characteristics(bt_gatt_handle_info_t char_handles, bt_gatt_service_s *service);
static int __bt_gatt_client_update_descriptors(bt_gatt_handle_info_t desc_handles, bt_gatt_characteristic_s *characteristic);

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
	if (is_initialized != true) {
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

	if (!_bt_check_cb(BT_EVENT_LE_STATE_CHANGED) &&
	    !_bt_check_cb(BT_EVENT_LE_DEVICE_DISCOVERY_STATE_CHANGED)) {
		if (bluetooth_le_unregister_callback() != BLUETOOTH_ERROR_NONE) {
			BT_ERR("OPERATION_FAILED(0x%08x)", BT_ERROR_OPERATION_FAILED);
			return BT_ERROR_OPERATION_FAILED;
		}
		is_le_initialized = false;
	}

	return BT_ERROR_NONE;
}

int _bt_get_error_code(int origin_error)
{
	switch (origin_error) {
	case BLUETOOTH_ERROR_NONE:
		return BT_ERROR_NONE;
	case BLUETOOTH_ERROR_INVALID_DATA:
	case BLUETOOTH_ERROR_INVALID_PARAM:
	case BLUETOOTH_ERROR_NOT_CONNECTED:
		return BT_ERROR_INVALID_PARAMETER;
	case BLUETOOTH_ERROR_NOT_SUPPORT:
		return BT_ERROR_NOT_SUPPORTED;
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
	case BLUETOOTH_ERROR_ALREADY_DEACTIVATED:
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
		return BT_ERROR_SERVICE_NOT_FOUND;
	case BLUETOOTH_ERROR_NOT_INITIALIZED:
		return BT_ERROR_NOT_INITIALIZED;
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
				(*dest_dev)->service_uuid[i] =
					g_strndup(source_dev->uuids[i],
						BLUETOOTH_UUID_STRING_MAX - 1);
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
	(*dest_dev)->is_connected = source_dev->connected != BLUETOOTH_CONNECTED_LINK_NONE ? TRUE : FALSE;
	(*dest_dev)->is_authorized = (bool)source_dev->trust;

	(*dest_dev)->manufacturer_data_len = source_dev->manufacturer_data.data_len;
	if (source_dev->manufacturer_data.data_len > 0) {
		(*dest_dev)->manufacturer_data = (char *)malloc(source_dev->manufacturer_data.data_len);
		memcpy((*dest_dev)->manufacturer_data, source_dev->manufacturer_data.data, source_dev->manufacturer_data.data_len);
	} else {
		(*dest_dev)->manufacturer_data = (char *)malloc(1);
		(*dest_dev)->manufacturer_data[0] = 0;
	}

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
				g_free(device_info->service_uuid[i]);
		}
		free(device_info->service_uuid);
	}

	if (device_info->manufacturer_data != NULL)
		free(device_info->manufacturer_data);

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

	if (addr_str == NULL || addr_str[0] == '\0')
		return;

	i = sscanf(addr_str, "%X:%X:%X:%X:%X:%X", &addr[0], &addr[1], &addr[2], &addr[3], &addr[4], &addr[5]);
	if (i != BLUETOOTH_ADDRESS_LENGTH) {
		BT_ERR("Invalid format string - [%s]", addr_str);
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
	case BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED:
		return "REMOTE_DEVICE_NOT_CONNECTED";
	case BT_ERROR_PERMISSION_DENIED:
		return "PERMISSION_DENIED";
	case BT_ERROR_SERVICE_NOT_FOUND:
		return "SERVICE_NOT_FOUND";
	case BT_ERROR_NOT_SUPPORTED:
		return "NOT_SUPPORTD";
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

	*dest = (bt_device_sdp_info_s *)g_malloc0(sizeof(bt_device_sdp_info_s));

	/* Fix : NULL_RETURNS */
	if (*dest == NULL)
		return BT_ERROR_OUT_OF_MEMORY;

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

	g_free(sdp_info);
	sdp_info = NULL;
}

static int __bt_get_bt_device_connection_info_s(bt_device_connection_info_s **dest, bt_connection_info_t *source)
{
	*dest = (bt_device_connection_info_s *)g_malloc0(sizeof(bt_device_connection_info_s));

	/* Fix : NULL_RETURNS */
	if (*dest == NULL)
		return BT_ERROR_OUT_OF_MEMORY;

	if (_bt_convert_address_to_string(&((*dest)->remote_address), &(source->device_addr)) != BT_ERROR_NONE) {
		__bt_free_bt_device_connection_info_s(*dest);
		return BT_ERROR_OUT_OF_MEMORY;
	}

	switch (source->addr_type) {
	case 0:
		(*dest)->link = BT_DEVICE_CONNECTION_LINK_BREDR;
		break;
	case 1:
	case 2:
		(*dest)->link = BT_DEVICE_CONNECTION_LINK_LE;
		break;
	default:
		(*dest)->link = BT_DEVICE_CONNECTION_LINK_BREDR;
		break;
	}

	(*dest)->disconn_reason = source->disc_reason;

	return BT_ERROR_NONE;
}

static bt_gatt_server_read_value_requested_cb __bt_gatt_attribute_get_read_cb(
					bt_gatt_h service, bt_gatt_h characteristic, void **user_data)
{
	gchar *svc_path = (gchar *)service;
	gchar *chr_path = (gchar *)characteristic;
	const GSList *gatt_server_list = NULL;
	const GSList *l1, *l2, *l3;

	gatt_server_list = _bt_gatt_get_server_list();

	for (l1 = gatt_server_list; l1 != NULL; l1 = l1->next) {
		bt_gatt_server_s *serv = l1->data;

		if (!serv)
			return NULL;

		for (l2 = serv->services; l2 != NULL; l2 = l2->next) {
			bt_gatt_service_s *svc = l2->data;

			if (g_strcmp0(svc->path, svc_path) == 0) {
				for (l3 = svc->characteristics; l3 != NULL; l3 = l3->next) {
					bt_gatt_characteristic_s *chr = l3->data;

					if (chr && g_strcmp0(chr->path, chr_path) == 0) {
						if (chr->read_requested_cb) {
							*user_data = chr->read_requested_user_data;
							return chr->read_requested_cb;
						} else
							return NULL;
					}
				}
			}
		}
	}
	return NULL;
}

static bt_gatt_server_value_changed_cb __bt_gatt_attribute_get_value_change_cb(
					bt_gatt_h service, bt_gatt_h attribute, void **user_data)
{
	gchar *svc_path = (gchar *)service;
	gchar *att_path = (gchar *)attribute;
	const GSList *gatt_server_list = NULL;
	const GSList *l1, *l2, *l3, *l4;

	gatt_server_list = _bt_gatt_get_server_list();

	for (l1 = gatt_server_list; l1 != NULL; l1 = l1->next) {
		bt_gatt_server_s *serv = l1->data;

		if (!serv)
			return NULL;

		for (l2 = serv->services; l2 != NULL; l2 = l2->next) {
			bt_gatt_service_s *svc = l2->data;

			if (g_strcmp0(svc->path, svc_path) == 0) {
				for (l3 = svc->characteristics; l3 != NULL; l3 = l3->next) {
					bt_gatt_characteristic_s *chr = l3->data;

					if (chr) {
						if (g_strcmp0(chr->path, att_path) == 0) {
							if (chr->server_value_changed_cb) {
								*user_data = chr->server_value_changed_user_data;
								return chr->server_value_changed_cb;
							} else
								return NULL;
						} else {
							for (l4 = chr->descriptors; l4 != NULL; l4 = l4->next) {
								bt_gatt_descriptor_s *desc = l4->data;

								if (desc && g_strcmp0(desc->path, att_path) == 0) {
									/* TODO: Call value changed callback registerd for the descriptor */
									return NULL;
								}
							}
						}
					}
				}
			}
		}
	}
	return NULL;
}

static bt_gatt_server_notification_state_change_cb __bt_gatt_attribute_get_notification_change_cb(
					bt_gatt_h service, bt_gatt_h attribute, void **user_data)
{
	gchar *svc_path = (gchar *)service;
	gchar *att_path = (gchar *)attribute;
	const GSList *gatt_server_list = NULL;
	const GSList *l1, *l2, *l3;

	gatt_server_list = _bt_gatt_get_server_list();

	for (l1 = gatt_server_list; l1 != NULL; l1 = l1->next) {
		bt_gatt_server_s *serv = l1->data;

		if (!serv)
			return NULL;

		for (l2 = serv->services; l2 != NULL; l2 = l2->next) {
			bt_gatt_service_s *svc = l2->data;

			if (g_strcmp0(svc->path, svc_path) == 0) {
				for (l3 = svc->characteristics; l3 != NULL; l3 = l3->next) {
					bt_gatt_characteristic_s *chr = l3->data;

					if (chr && g_strcmp0(chr->path, att_path) == 0) {
						if (chr->notification_changed_cb) {
							*user_data = chr->notification_changed_user_data;
							return chr->notification_changed_cb;
						} else
							return NULL;
					}
				}
			}
		}
	}
	return NULL;
}

static void __bt_free_bt_device_connection_info_s(bt_device_connection_info_s *conn_info)
{
	if (conn_info == NULL)
		return;

	if (conn_info->remote_address != NULL)
		free(conn_info->remote_address);

	g_free(conn_info);
	conn_info = NULL;
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

#ifdef TIZEN_WEARABLE
void _bt_hf_event_proxy(int event, bt_hf_event_param_t *param, void *user_data)
{
	bluetooth_event_param_t new_param;
	new_param.event = param->event;
	new_param.param_data = param->param_data;
	new_param.result = param->result;
	new_param.user_data = NULL;
	__bt_event_proxy(event, &new_param, user_data);
}
#endif

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
	int call_id;
	int *avrcp_mode;
	int *discoverable_timeout;
	int *adv_handle;
	bluetooth_rfcomm_connection_t *connection_ind = NULL;
	bluetooth_rfcomm_disconnection_t *disconnection_ind = NULL;
	bluetooth_hid_request_t *hid_connection_status = NULL;
	bt_socket_connection_s rfcomm_connection;
	bt_device_sdp_info_s *sdp_info = NULL;
	bt_device_connection_info_s *conn_info = NULL;
	bt_adapter_device_discovery_info_s *discovery_info = NULL;
	bt_device_info_s *bonded_device = NULL;
	bluetooth_rfcomm_connection_request_t *reqeust_ind = NULL;
	bt_obex_server_authorize_into_t *auth_info = NULL;
	bt_obex_server_transfer_info_t *transfer_info = NULL;
	bt_opc_transfer_info_t *client_info = NULL;
	bluetooth_device_address_t *bd_addr = NULL;
	telephony_event_callid_t *call_data = NULL;
	char *device_addr = NULL;
#ifdef TIZEN_WEARABLE
	char *phone_number = NULL;
#endif
	int error_code = BT_ERROR_NONE;
	int event_index = -1;
	bluetooth_network_device_info_t *dev_info = NULL;
	bt_hdp_connected_t *hdp_conn_info = NULL;
	bt_hdp_disconnected_t *hdp_disconn_info = NULL;
	bt_hdp_data_ind_t *hdp_data_ind = NULL;
	bt_gatt_char_value_t *char_val = NULL;
	media_metadata_attributes_t *metadata = NULL;
	bluetooth_authentication_request_info_t *auth_information = NULL;
	bt_le_data_length_params_t  *data_length_info = NULL;

	event_index = __bt_get_cb_index(event);

	if (event == BLUETOOTH_EVENT_GATT_CONNECTED) {
		const GSList *clients = NULL;
		const GSList *l = NULL;
		int ret;

		_bt_convert_address_to_string(&device_addr,
				(bluetooth_device_address_t *)(param->param_data));

		clients = _bt_gatt_get_client_list();
		for (l = clients; l; l = g_slist_next(l)) {
			bt_gatt_client_s *client_s = l->data;

			if (!g_strcmp0(client_s->remote_address, device_addr)) {
				if (client_s->services_discovered == false) {
					BT_INFO("Matched GATT Client is found");
					ret = _bt_gatt_client_update_all((bt_gatt_client_h)l->data);
					if (ret != BT_ERROR_NONE)
						BT_ERR("bluetooth_gatt_get_primary_services is failed");
					else
						client_s->services_discovered = true;
				}
				break;
			}
		}
		g_free(device_addr);
		device_addr = NULL;
	}

	if (event == BLUETOOTH_EVENT_GATT_SERVER_VALUE_CHANGED || event == BLUETOOTH_EVENT_GATT_SERVER_NOTIFICATION_STATE_CHANGED ||
		event == BLUETOOTH_EVENT_GATT_SERVER_READ_REQUESTED || event == BLUETOOTH_EVENT_ADVERTISING_STARTED ||
		event == BLUETOOTH_EVENT_ADVERTISING_STOPPED)
		BT_INFO("NOT use bt_event_slot_container");
	else if (event_index == -1 || bt_event_slot_container[event_index].callback == NULL)
		return;

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
	case BLUETOOTH_EVENT_DISCOVERABLE_TIMEOUT_CHANGED:
		BT_INFO("bt_adapter_visibility_duration_changed_cb() will be called");
		discoverable_timeout = (int *)(param->param_data);
		((bt_adapter_visibility_duration_changed_cb)bt_event_slot_container[event_index].callback)
		    (*discoverable_timeout, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_CONNECTABLE_CHANGED:
		BT_INFO("bt_adapter_connectable_changed_cb() will be called");
		((bt_adapter_connectable_changed_cb)bt_event_slot_container[event_index].callback)
			(_bt_get_error_code(param->result), *(bool *)(param->param_data), bt_event_slot_container[event_index].user_data);
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
		break;
	case BLUETOOTH_EVENT_PIN_REQUEST:
		BT_INFO("bt_adapter_authentication_req_cb() will be called with \
				BLUETOOTH_EVENT_PIN_REQUEST");
		auth_information = (bluetooth_authentication_request_info_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, &auth_information->device_address);

		BT_DBG("BUETOOTH_EVENT_PIN_REQUEST: name = %s address = %s", auth_information->device_name,
			device_addr);

		((bt_adapter_authentication_req_cb)bt_event_slot_container[event_index].callback)
			(_bt_get_error_code(param->result), BT_AUTH_PIN_REQUEST, auth_information->device_name.name, device_addr,
				auth_information->str_passkey, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_PASSKEY_REQUEST:
		BT_INFO("bt_adapter_authentication_req_cb will be called with \
				BLUETOOTH_EVENT_PASSKEY_REQUEST");

		auth_information = (bluetooth_authentication_request_info_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, &auth_information->device_address);

		BT_DBG("BLUETOOTH_EVENT_PASSKEY_REQUEST: name = %s address = %s", auth_information->device_name,
			device_addr);

		((bt_adapter_authentication_req_cb)bt_event_slot_container[event_index].callback)
			(_bt_get_error_code(param->result), BT_AUTH_PIN_REQUEST, auth_information->device_name.name, device_addr,
				auth_information->str_passkey, bt_event_slot_container[event_index].user_data);
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
		__bt_get_bt_device_connection_info_s(&conn_info, (bt_connection_info_t *)(param->param_data));
		((bt_device_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		    (true, conn_info, bt_event_slot_container[event_index].user_data);
		__bt_free_bt_device_connection_info_s(conn_info);
		break;
	case BLUETOOTH_EVENT_DEVICE_DISCONNECTED:
		BT_INFO("bt_device_connection_state_changed_cb() will be called");
		__bt_get_bt_device_connection_info_s(&conn_info, (bt_connection_info_t *)(param->param_data));
		((bt_device_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		    (false, conn_info, bt_event_slot_container[event_index].user_data);
		__bt_free_bt_device_connection_info_s(conn_info);
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
		/*BT_INFO("bt_socket_data_received_cb() will be * called"); */
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
			rfcomm_connection.server_fd = connection_ind->server_id;

			if (strlen(connection_ind->uuid) > 0) {
				rfcomm_connection.service_uuid = strdup(connection_ind->uuid);
				BT_INFO("uuid: [%s]", rfcomm_connection.service_uuid);
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
				BT_INFO("uuid: [%s]", rfcomm_connection.service_uuid);
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
		BT_INFO("BLUETOOTH_EVENT_AV_DISCONNECTED ");
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
	case BLUETOOTH_EVENT_AVRCP_CONTROL_CONNECTED:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_CONTROL_CONNECTED ");
		device_addr = (char *)(param->param_data);
		((bt_avrcp_target_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(TRUE, device_addr, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_CONTROL_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_CONTROL_DISCONNECTED ");
		device_addr = (char *)(param->param_data);
		((bt_avrcp_target_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(FALSE, device_addr, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_CONTROL_SHUFFLE_STATUS:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_CONTROL_SHUFFLE_STATUS ");
		avrcp_mode = (int *)(param->param_data);
		((bt_avrcp_shuffle_mode_changed_cb)bt_event_slot_container[event_index].callback)
		((bt_avrcp_shuffle_mode_e)*avrcp_mode, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_CONTROL_EQUALIZER_STATUS:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_CONTROL_EQUALIZER_STATUS ");
		avrcp_mode = (int *)(param->param_data);
		((bt_avrcp_equalizer_state_changed_cb)bt_event_slot_container[event_index].callback)
		((bt_avrcp_equalizer_state_e)*avrcp_mode, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_CONTROL_REPEAT_STATUS:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_CONTROL_REPEAT_STATUS ");
		avrcp_mode = (int *)(param->param_data);
		((bt_avrcp_repeat_mode_changed_cb)bt_event_slot_container[event_index].callback)
		((bt_avrcp_repeat_mode_e)*avrcp_mode, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_CONTROL_SCAN_STATUS:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_CONTROL_SCAN_STATUS ");
		avrcp_mode = (int *)(param->param_data);
		((bt_avrcp_scan_mode_changed_cb)bt_event_slot_container[event_index].callback)
		((bt_avrcp_scan_mode_e)*avrcp_mode, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_PLAY_STATUS_CHANGED:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_PLAY_STATUS_CHANGED ");
		avrcp_mode = (int *)(param->param_data);
		((bt_avrcp_play_status_changed_cb)bt_event_slot_container[event_index].callback)
		((bt_avrcp_player_state_e)*avrcp_mode, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_SONG_POSITION_STATUS:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_SONG_POSITION_STATUS ");
		unsigned int *postion = (unsigned int *)(param->param_data);
		((bt_avrcp_song_position_changed_cb)bt_event_slot_container[event_index].callback)
		(*postion, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_AVRCP_TRACK_CHANGED:
		BT_INFO("BLUETOOTH_EVENT_AVRCP_TRACK_CHANGED ");
		bt_avrcp_metadata_attributes_info_s meta_info = {0, };
		metadata = (media_metadata_attributes_t *)(param->param_data);
		meta_info.title = metadata->title; /* No need to allocate memory*/
		meta_info.artist = metadata->artist; /* No need to allocate memory*/
		meta_info.album = metadata->album; /* No need to allocate memory*/
		meta_info.genre = metadata->genre; /* No need to allocate memory*/
		meta_info.total_tracks = metadata->total_tracks;
		meta_info.number = metadata->number;
		meta_info.duration = metadata->duration;
		((bt_avrcp_track_info_changed_cb)bt_event_slot_container[event_index].callback)
		(&meta_info, bt_event_slot_container[event_index].user_data);
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
	case BLUETOOTH_HID_DEVICE_CONNECTED:
		BT_INFO("BLUETOOTH_HID_DEVICE_CONNECTED");
		hid_connection_status = (bluetooth_hid_request_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr,
						&(hid_connection_status->device_addr));
		((bt_hid_device_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), TRUE, device_addr, bt_event_slot_container[event_index].user_data);

		g_free(device_addr);
		break;
	case BLUETOOTH_HID_DEVICE_DISCONNECTED:
		BT_INFO("BLUETOOTH_HID_DEVICE_DISCONNECTED");
		hid_connection_status = (bluetooth_hid_request_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr,
						&(hid_connection_status->device_addr));
		((bt_hid_device_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), FALSE, device_addr, bt_event_slot_container[event_index].user_data);

		g_free(device_addr);
		break;
	case BLUETOOTH_HID_DEVICE_DATA_RECEIVED:
		BT_INFO("HID Device Data");
		((bt_hid_device_data_received_cb)bt_event_slot_container[event_index].callback)
		    ((bt_hid_device_received_data_s *)(param->param_data), bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_GATT_CONNECTED: {
		BT_INFO("BLUETOOTH_EVENT_GATT_CONNECTED");
		gboolean connected = TRUE;
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);
		if (_bt_get_error_code(param->result) != BT_ERROR_NONE)
			connected = FALSE;
		((bt_gatt_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
			(_bt_get_error_code(param->result), connected, device_addr,
			 bt_event_slot_container[event_index].user_data);
		}
		break;
	case BLUETOOTH_EVENT_GATT_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_GATT_DISCONNECTED");
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);
		((bt_gatt_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), FALSE, device_addr,
		 bt_event_slot_container[event_index].user_data);
		break;
#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
	case BLUETOOTH_EVENT_GATT_SVC_CHAR_DISCOVERED: {
		BT_INFO("BLUETOOTH_EVENT_GATT_SVC_CHAR_DISCOVERED");
		if (_bt_gatt_is_legacy_client_mode()) {
			bt_gatt_discovered_char_t *svc_char = param->param_data;
			int i;

			if (svc_char == NULL)
				return;
			for (i = 1; i <= svc_char->handle_info.count; i++) {
				((bt_gatt_characteristics_discovered_cb)bt_event_slot_container[event_index].callback)
					(_bt_get_error_code(param->result), i, svc_char->handle_info.count, svc_char->handle_info.handle[i-1],
					 bt_event_slot_container[event_index].user_data);
			}
			_bt_unset_cb(BT_EVENT_GATT_CLIENT_CHARACTERISTIC_DISCOVERED_LEGACY);
		}
		break;
	}
#endif
	case BLUETOOTH_EVENT_GATT_CHAR_VAL_CHANGED:
		BT_INFO("BLUETOOTH_EVENT_GATT_CHAR_VAL_CHANGED");
		char_val = (bt_gatt_char_value_t *)(param->param_data);

#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
		if (_bt_gatt_is_legacy_client_mode()) {
			bt_gatt_char_value_t *char_val = param->param_data;

			if (char_val == NULL)
				return;
			((bt_gatt_characteristic_changed_cb)bt_event_slot_container[event_index].callback)
				(char_val->char_handle, char_val->char_value, char_val->val_len,
				 bt_event_slot_container[event_index].user_data);
			break;
		}
#endif
		if (char_val == NULL)
			return;

		((_bt_gatt_client_value_changed_cb)bt_event_slot_container[event_index].callback)
		 (char_val->char_handle, char_val->char_value, char_val->val_len,
		  bt_event_slot_container[event_index].user_data);

		break;
	case BLUETOOTH_EVENT_GATT_READ_CHAR: {
		int ret;
		bt_gatt_client_request_completed_cb cb = bt_event_slot_container[event_index].callback;
		bt_gatt_client_cb_data_s *cb_data = bt_event_slot_container[event_index].user_data;

		BT_INFO("BLUETOOTH_EVENT_GATT_READ_CHAR");

#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
		if (_bt_gatt_is_legacy_client_mode()) {
			bt_gatt_char_value_t *char_val = param->param_data;

			((bt_gatt_characteristic_read_cb)bt_event_slot_container[event_index].callback)
				(char_val->char_value, char_val->val_len,
				 bt_event_slot_container[event_index].user_data);
			_bt_unset_cb(BT_EVENT_GATT_CLIENT_READ_CHARACTERISTIC_LEGACY);
			break;
		}
#endif
		_bt_unset_cb(BT_EVENT_GATT_CLIENT_READ_CHARACTERISTIC);
		ret = _bt_get_error_code(param->result);
		if (ret == BT_ERROR_NONE) {
			char_val = (bt_gatt_char_value_t *)(param->param_data);
			bt_gatt_set_value(cb_data->gatt_handle,
					(char *)char_val->char_value, (int)char_val->val_len);
		}
		cb(ret, cb_data->gatt_handle, cb_data->user_data);
		g_free(cb_data);

		break;
	}
	case BLUETOOTH_EVENT_GATT_WRITE_CHAR: {
		int ret;
		bt_gatt_client_request_completed_cb cb = bt_event_slot_container[event_index].callback;
		bt_gatt_client_cb_data_s *cb_data = bt_event_slot_container[event_index].user_data;

		BT_INFO("BLUETOOTH_EVENT_GATT_WRITE_CHAR");

#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
		if (_bt_gatt_is_legacy_client_mode()) {
			((bt_gatt_characteristic_write_cb)bt_event_slot_container[event_index].callback)
				(bt_event_slot_container[event_index].user_data);

			_bt_unset_cb(BT_EVENT_GATT_CLIENT_WRITE_CHARACTERISTIC_LEGACY);
			break;
		}
#endif
		_bt_unset_cb(BT_EVENT_GATT_CLIENT_WRITE_CHARACTERISTIC);
		ret = _bt_get_error_code(param->result);
		cb(ret, cb_data->gatt_handle, cb_data->user_data);
		g_free(cb_data);

		break;
	}
	case BLUETOOTH_EVENT_GATT_READ_DESC: {
		int ret;
		bt_gatt_client_request_completed_cb cb = bt_event_slot_container[event_index].callback;
		bt_gatt_client_cb_data_s *cb_data = bt_event_slot_container[event_index].user_data;
		bt_gatt_char_property_t *desc_val = NULL;

		BT_INFO("BLUETOOTH_EVENT_GATT_READ_DESC");

		_bt_unset_cb(BT_EVENT_GATT_CLIENT_READ_DESCRIPTOR);
		ret = _bt_get_error_code(param->result);
		if (ret == BT_ERROR_NONE) {
			desc_val = (bt_gatt_char_property_t *)(param->param_data);
			bt_gatt_set_value(cb_data->gatt_handle,
					desc_val->description, (int)desc_val->val_len);
		}
		cb(ret, cb_data->gatt_handle, cb_data->user_data);
		g_free(cb_data);

		break;
	}
	case BLUETOOTH_EVENT_GATT_WRITE_DESC: {
		int ret;
		bt_gatt_client_request_completed_cb cb = bt_event_slot_container[event_index].callback;
		bt_gatt_client_cb_data_s *cb_data = bt_event_slot_container[event_index].user_data;

		BT_INFO("BLUETOOTH_EVENT_GATT_WRITE_DESC");

		_bt_unset_cb(BT_EVENT_GATT_CLIENT_WRITE_DESCRIPTOR);
		ret = _bt_get_error_code(param->result);
		cb(ret, cb_data->gatt_handle, cb_data->user_data);
		g_free(cb_data);

		break;
	}
	case BLUETOOTH_EVENT_GATT_SERVER_READ_REQUESTED: {
		bt_gatt_read_req_t *read_req = param->param_data;
		bt_gatt_server_read_value_requested_cb cb;
		void *user_data = NULL;
		cb = __bt_gatt_attribute_get_read_cb(read_req->service_handle,
						read_req->att_handle, &user_data);

		BT_INFO("BLUETOOTH_EVENT_GATT_SERVER_READ_REQUESTED");
		if (cb == NULL) {
			bluetooth_gatt_send_response(read_req->req_id,
								BLUETOOTH_GATT_ATT_REQUEST_TYPE_READ,
								BLUETOOTH_ERROR_INTERNAL, 0, NULL, 0);
			return;
		}

		cb(read_req->address, read_req->req_id, read_req->service_handle,
					read_req->att_handle, read_req->offset,
					user_data);
		break;
	}
	case BLUETOOTH_EVENT_GATT_SERVER_VALUE_CHANGED: {
		bt_gatt_value_change_t *value_change = param->param_data;
		bt_gatt_server_value_changed_cb cb;
		void *user_data = NULL;
		cb = __bt_gatt_attribute_get_value_change_cb(value_change->service_handle,
						value_change->att_handle, &user_data);

		BT_INFO("BLUETOOTH_EVENT_GATT_SERVER_VALUE_CHANGE");
		if (cb == NULL) {
			bluetooth_gatt_send_response(value_change->req_id,
								BLUETOOTH_GATT_ATT_REQUEST_TYPE_WRITE,
								BLUETOOTH_ERROR_INTERNAL, 0, NULL, 0);
			return;
		}

		bluetooth_gatt_send_response(value_change->req_id,
							BLUETOOTH_GATT_ATT_REQUEST_TYPE_WRITE,
							BLUETOOTH_ERROR_NONE, 0, NULL, 0);

		cb(value_change->address, value_change->service_handle,
					value_change->att_handle, value_change->offset,
					(char *)value_change->att_value, value_change->val_len, user_data);
		break;
	}
	case BLUETOOTH_EVENT_GATT_SERVER_NOTIFICATION_STATE_CHANGED: {
		bt_gatt_char_notify_change_t *value_change = param->param_data;
		bt_gatt_server_notification_state_change_cb cb;
		void *user_data = NULL;
		cb = __bt_gatt_attribute_get_notification_change_cb(value_change->service_handle,
						value_change->att_handle, &user_data);

		BT_INFO("BLUETOOTH_EVENT_GATT_SERVER_NOTIFICATION_STATE_CHANGED");
		if (cb == NULL)
			return;

		cb(value_change->att_notify, value_change->service_handle,
				value_change->att_handle, user_data);
		break;
	}

#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
	case BLUETOOTH_EVENT_GATT_SVC_CHAR_DESC_DISCOVERED: {
		BT_INFO("BLUETOOTH_EVENT_GATT_SVC_CHAR_DESC_DISCOVERED");
		if (_bt_gatt_is_legacy_client_mode()) {
			bt_gatt_char_property_t *char_desc = param->param_data;

			if (char_desc == NULL)
				return;
			((bt_gatt_characteristic_descriptor_discovered_cb)
			 bt_event_slot_container[event_index].callback)(_bt_get_error_code(param->result),
				 char_desc->format.format, char_desc->val_len, char_desc->description,
				 bt_event_slot_container[event_index].user_data);
			_bt_unset_cb(BT_EVENT_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERED_LEGACY);
		}
		break;
	}
#endif
	case BLUETOOTH_EVENT_GATT_SERVER_CHARACTERISTIC_VALUE_CHANGED: {
		const GSList *server_list = NULL;
		BT_INFO("BLUETOOTH_EVENT_GATT_SERVER_CHARACTERISTIC_VALUE_CHANGED");

		char_val = (bt_gatt_char_value_t *)(param->param_data);

		if (char_val == NULL)
			return;

		for (server_list = _bt_gatt_get_server_list() ; server_list; server_list = g_slist_next(server_list)) {
			bt_gatt_server_s *serv = (bt_gatt_server_s*)server_list->data;
			GSList *service_list = NULL;

			for (service_list = serv->services; service_list; service_list = g_slist_next(service_list)) {
				bt_gatt_service_s *svc = (bt_gatt_service_s*)service_list->data;
				GSList *char_list = NULL;

				for (char_list = svc->characteristics; char_list; char_list = g_slist_next(char_list)) {
					bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)char_list->data;
					if (chr->path && strcmp(char_val->char_handle, chr->path) == 0) {
						g_free(chr->value);
						chr->value = NULL;
						if (char_val->val_len > 0)
							chr->value = g_memdup(char_val->char_value, char_val->val_len);
						chr->value_length = char_val->val_len;
						/* TODO : Fix build error temporary */
						if (chr->server_value_changed_cb)
							chr->server_value_changed_cb(NULL, (bt_gatt_server_h)serv, (bt_gatt_h)chr,
											0, (char*)char_val->char_value, char_val->val_len,
											chr->server_value_changed_user_data);
					}
				}
			}
		}
		break;
	}
	case BLUETOOTH_EVENT_ADVERTISING_STARTED:
		BT_INFO("BLUETOOTH_EVENT_ADVERTISING_STARTED");
		adv_handle = (int *)(param->param_data);

		if (bt_event_slot_container[event_index].callback != NULL) {
			((bt_adapter_le_advertising_state_changed_cb)bt_event_slot_container[event_index].callback)
				(_bt_get_error_code(param->result), NULL, BT_ADAPTER_LE_ADVERTISING_STARTED,
				bt_event_slot_container[event_index].user_data);
		}

		_bt_adapter_le_invoke_advertising_state_cb(*adv_handle,
				_bt_get_error_code(param->result), BT_ADAPTER_LE_ADVERTISING_STARTED);

		break;
	case BLUETOOTH_EVENT_ADVERTISING_STOPPED:
		BT_INFO("BLUETOOTH_EVENT_ADVERTISING_STOPPED");
		adv_handle = (int *)(param->param_data);

		if (bt_event_slot_container[event_index].callback != NULL) {
			((bt_adapter_le_advertising_state_changed_cb)bt_event_slot_container[event_index].callback)
				(_bt_get_error_code(param->result), NULL, BT_ADAPTER_LE_ADVERTISING_STOPPED,
				bt_event_slot_container[event_index].user_data);
		}

		_bt_adapter_le_invoke_advertising_state_cb(*adv_handle,
				_bt_get_error_code(param->result), BT_ADAPTER_LE_ADVERTISING_STOPPED);

		break;
	case BLUETOOTH_EVENT_MANUFACTURER_DATA_CHANGED:
		BT_INFO("BLUETOOTH_EVENT_MANUFACTURER_DATA_CHANGED");

		((bt_adapter_manufacturer_data_changed_cb)bt_event_slot_container[event_index].callback)
		 (param->param_data, param->result,
		  bt_event_slot_container[event_index].user_data);

		break;
	case BLUETOOTH_EVENT_IPSP_CONNECTED:
		BT_INFO("BLUETOOTH_EVENT_IPSP_CONNECTED");
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);
		((_bt_le_ipsp_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), TRUE, device_addr,
		 bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_IPSP_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_IPSP_DISCONNECTED");
		bd_addr = (bluetooth_device_address_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, bd_addr);
		((_bt_le_ipsp_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), FALSE, device_addr,
		 bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_LE_DATA_LENGTH_CHANGED:
		BT_INFO("__bt_le_set_data_length_changed_cb() will be called");
		data_length_info = (bt_le_data_length_params_t *)(param->param_data);
		_bt_convert_address_to_string(&device_addr, &data_length_info->device_address);

		((_bt_le_set_data_length_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), device_addr, data_length_info->max_tx_octets,
		data_length_info->max_tx_time, data_length_info->max_rx_octets, data_length_info->max_rx_time,
		bt_event_slot_container[event_index].user_data);

		if (device_addr != NULL)
			free(device_addr);
		break;
#ifdef TIZEN_WEARABLE
	case BLUETOOTH_PBAP_CONNECTED: {
		bt_pbap_enabled_cb cb = bt_event_slot_container[event_index].callback;
		void *user_data = bt_event_slot_container[event_index].user_data;
		bt_pbap_connected_t *connected = (bt_pbap_connected_t *)param->param_data;

		BT_INFO("BLUETOOTH_PBAP_CONNECTED/DISCONNECTED");
		if (connected->connected == 0)
			_bt_unset_cb(BT_EVENT_PBAP_CONNECTION_STATUS);
		_bt_convert_address_to_string(&device_addr, &connected->btaddr);
		cb(device_addr, connected->connected, user_data);

		if (device_addr != NULL)
			free(device_addr);

		break;
	}
	case BLUETOOTH_PBAP_PHONEBOOK_SIZE: {
		bt_pbap_phonebook_size_cb cb = bt_event_slot_container[event_index].callback;
		void *user_data = bt_event_slot_container[event_index].user_data;
		bt_pbap_phonebook_size_t *pb_size = (bt_pbap_phonebook_size_t *)param->param_data;

		BT_INFO("BLUETOOTH_PBAP_PHONEBOOK_SIZE");

		_bt_unset_cb(BT_EVENT_PBAP_PHONEBOOK_SIZE);
		_bt_convert_address_to_string(&device_addr, &pb_size->btaddr);
		cb(device_addr, pb_size->size, user_data);

		if (device_addr != NULL)
			free(device_addr);

		break;
	}
	case BLUETOOTH_PBAP_PHONEBOOK_PULL: {
		bt_pbap_phonebook_pull_cb cb = bt_event_slot_container[event_index].callback;
		void *user_data = bt_event_slot_container[event_index].user_data;
		bt_pbap_phonebook_pull_t *pb_pull = (bt_pbap_phonebook_pull_t *)(param->param_data);

		BT_INFO("BLUETOOTH_PBAP_PHONEBOOK_PULL");

		_bt_unset_cb(BT_EVENT_PBAP_PHONEBOOK_PULL);
		_bt_convert_address_to_string(&device_addr, &pb_pull->btaddr);
		cb(device_addr, pb_pull->vcf_file, pb_pull->success, user_data);

		if (device_addr != NULL)
			free(device_addr);

		break;
	}
	case BLUETOOTH_PBAP_VCARD_LIST: {
		bt_pbap_list_vcards_cb cb = bt_event_slot_container[event_index].callback;
		void *user_data = bt_event_slot_container[event_index].user_data;
		bt_pbap_vcard_list_t *vc_list = (bt_pbap_vcard_list_t *)(param->param_data);

		BT_INFO("BLUETOOTH_PBAP_VCARD_LIST");

		_bt_unset_cb(BT_EVENT_PBAP_VCARD_LIST);
		_bt_convert_address_to_string(&device_addr, &vc_list->btaddr);
		cb(device_addr, vc_list->vcards, vc_list->length, vc_list->success, user_data);

		if (device_addr != NULL)
			free(device_addr);

		break;
	}
	case BLUETOOTH_PBAP_VCARD_PULL: {
		bt_pbap_get_vcard_cb cb = bt_event_slot_container[event_index].callback;
		void *user_data = bt_event_slot_container[event_index].user_data;
		bt_pbap_vcard_pull_t *vc_pull = (bt_pbap_vcard_pull_t *)(param->param_data);

		BT_INFO("BLUETOOTH_PBAP_VCARD_PULL");

		_bt_unset_cb(BT_EVENT_PBAP_VCARD_PULL);
		_bt_convert_address_to_string(&device_addr, &vc_pull->btaddr);
		cb(device_addr, vc_pull->vcf_file, vc_pull->success, user_data);

		if (device_addr != NULL)
			free(device_addr);

		break;
	}
	case BLUETOOTH_PBAP_PHONEBOOK_SEARCH: {
		bt_pbap_search_list_cb cb = bt_event_slot_container[event_index].callback;
		void *user_data = bt_event_slot_container[event_index].user_data;
		bt_pbap_phonebook_search_list_t *vc_list = (bt_pbap_phonebook_search_list_t *)(param->param_data);

		BT_INFO("BLUETOOTH_PBAP_PHONEBOOK_SEARCH");

		_bt_unset_cb(BT_EVENT_PBAP_PHONEBOOK_SEARCH);
		_bt_convert_address_to_string(&device_addr, &vc_list->btaddr);
		cb(device_addr, vc_list->vcards, vc_list->length, vc_list->success, user_data);

		if (device_addr != NULL)
			free(device_addr);

		break;
	}
	case BLUETOOTH_EVENT_HF_CONNECTED:
		BT_INFO("BLUETOOTH_EVENT_HF_CONNECTED");
		device_addr = (char *)(param->param_data);
		((bt_audio_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), TRUE, device_addr, BT_AUDIO_PROFILE_TYPE_AG,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_HF_DISCONNECTED");
		device_addr = (char *)(param->param_data);
		((bt_audio_connection_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), FALSE, device_addr, BT_AUDIO_PROFILE_TYPE_AG,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_AUDIO_CONNECTED:
		BT_INFO("BLUETOOTH_EVENT_HF_AUDIO_CONNECTED");
		device_addr = (char *)(param->param_data);
		((bt_hf_sco_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), TRUE,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_AUDIO_DISCONNECTED:
		BT_INFO("BLUETOOTH_EVENT_HF_AUDIO_DISCONNECTED");
		device_addr = (char *)(param->param_data);
		((bt_hf_sco_state_changed_cb)bt_event_slot_container[event_index].callback)
		(_bt_get_error_code(param->result), FALSE,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_RING_INDICATOR:
		BT_INFO("BLUETOOTH_EVENT_HF_RING_INDICATOR");
		phone_number = (char *)(param->param_data);
		((bt_hf_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_HF_CALL_HANDLING_EVENT_RING, phone_number,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_CALL_TERMINATED:
		BT_INFO("BLUETOOTH_EVENT_HF_CALL_TERMINATED");
		((bt_hf_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_HF_CALL_HANDLING_EVENT_RELEASE, NULL,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_CALL_STARTED:
		BT_INFO("BLUETOOTH_EVENT_HF_CALL_STARTED");
		((bt_hf_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_HF_CALL_HANDLING_EVENT_CALL_STARTED, NULL,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_CALL_ENDED:
		BT_INFO("BLUETOOTH_EVENT_HF_CALL_ENDED");
		((bt_hf_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_HF_CALL_HANDLING_EVENT_CALL_ENDED, NULL,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_VOICE_RECOGNITION_ENABLED:
		BT_INFO("BLUETOOTH_EVENT_HF_VOICE_RECOGNITION_ENABLED");
		((bt_hf_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_HF_CALL_HANDLING_EVENT_VOICE_RECOGNITION_ENABLED, NULL,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_VOICE_RECOGNITION_DISABLED:
		BT_INFO("BLUETOOTH_EVENT_HF_VOICE_RECOGNITION_DISABLED");
		((bt_hf_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_HF_CALL_HANDLING_EVENT_VOICE_RECOGNITION_DISABLED, NULL,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_CALL_WAITING:
		BT_INFO("BLUETOOTH_EVENT_HF_CALL_WAITING");
		phone_number = (char *)(param->param_data);
		((bt_hf_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_HF_CALL_HANDLING_EVENT_WAITING, phone_number,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_CALL_ON_HOLD:
		BT_INFO("BLUETOOTH_EVENT_HF_CALL_ON_HOLD");
		((bt_hf_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_HF_CALL_HANDLING_EVENT_HELD, NULL,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_CALL_UNHOLD:
		BT_INFO("BLUETOOTH_EVENT_HF_CALL_UNHOLD");
		((bt_hf_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_HF_CALL_HANDLING_EVENT_UNHELD, NULL,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_CALL_SWAPPED:
		BT_INFO("BLUETOOTH_EVENT_HF_CALL_SWAPPED");
		((bt_hf_call_handling_event_cb)bt_event_slot_container[event_index].callback)
		(BT_HF_CALL_HANDLING_EVENT_SWAPPED, NULL,
		bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_HF_VOLUME_SPEAKER: {
		BT_INFO("BLUETOOTH_EVENT_HF_VOLUME_SPEAKER");
		int *spk_gain = (int *)(param->param_data);
		((bt_hf_speaker_gain_changed_cb)bt_event_slot_container[event_index].callback)
		(*spk_gain, bt_event_slot_container[event_index].user_data);
		break;
	}
	case BLUETOOTH_EVENT_HF_CALL_STATUS: {
		BT_INFO("BLUETOOTH_EVENT_HF_CALL_STATUS");
		bt_hf_call_list_s *handle = (bt_hf_call_list_s *)(param->param_data);
		GList *l = NULL;
		GSList *call_list = NULL;

		for (l = g_list_first(handle->list); l; l = g_list_next(l)) {
			bt_hf_call_status_info_t *hf_call_info = l->data;
			bt_hf_call_status_info_s *call_info = NULL;

			call_info = g_malloc0(sizeof(bt_hf_call_status_info_s));
			call_info->number = strdup(hf_call_info->number);
			call_info->direction = hf_call_info->direction;
			call_info->status = hf_call_info->status;
			call_info->multi_party = hf_call_info->mpart;
			call_info->index = hf_call_info->idx;
			call_list = g_slist_append(call_list, call_info);
		}

		((bt_hf_call_status_updated_event_cb)bt_event_slot_container[event_index].callback)
		(call_list, bt_event_slot_container[event_index].user_data);
		break;
	}
#endif
	case BLUETOOTH_EVENT_IPSP_INIT_STATE_CHANGED: {
		BT_DBG("BLUETOOTH_EVENT_IPSP_INIT_STATE_CHANGED");
		((bt_le_ipsp_init_state_changed_cb)bt_event_slot_container[event_index].callback)
			(_bt_get_error_code(param->result), *(bool *)(param->param_data),
			bt_event_slot_container[event_index].user_data);
		break;
	}
	default:
		break;
	}
}


static void __bt_le_event_proxy(int event, bluetooth_event_param_t *param, void *user_data)
{
	bt_adapter_le_device_scan_result_info_s *scan_info = NULL;
#ifndef TIZEN_WEARABLE
	bt_adapter_le_device_discovery_info_s *discovery_info = NULL;
#endif
	int event_index = -1;

	event_index = __bt_get_cb_index(event);
	if (event == BLUETOOTH_EVENT_REMOTE_LE_DEVICE_FOUND)
		BT_INFO("Need to check 2 slots in the case");
	else if (event_index == -1 || bt_event_slot_container[event_index].callback == NULL)
		return;

	switch (event) {
	case BLUETOOTH_EVENT_LE_ENABLED:
		BT_INFO("BLUETOOTH_EVENT_LE_ENABLED");
		((bt_adapter_le_state_changed_cb) bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), BT_ADAPTER_LE_ENABLED, bt_event_slot_container[event_index].user_data);
		break;
	case BLUETOOTH_EVENT_LE_DISABLED:
		BT_INFO("BLUETOOTH_EVENT_LE_DISABLED");
		((bt_adapter_le_state_changed_cb) bt_event_slot_container[event_index].callback)
		    (_bt_get_error_code(param->result), BT_ADAPTER_LE_DISABLED, bt_event_slot_container[event_index].user_data);
		break;

#ifndef TIZEN_WEARABLE
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
#endif

	case BLUETOOTH_EVENT_REMOTE_LE_DEVICE_FOUND:
		BT_INFO("BLUETOOTH_EVENT_REMOTE_LE_DEVICE_FOUND");
		event_index = BT_EVENT_LE_SCAN_RESULT_UPDATED;
		if (bt_event_slot_container[event_index].callback != NULL) {
			if (__bt_get_bt_adapter_le_device_scan_info_s(&scan_info, (bluetooth_le_device_info_t *)(param->param_data)) == BT_ERROR_NONE) {
				((bt_adapter_le_scan_result_cb)bt_event_slot_container[event_index].callback)
					(_bt_get_error_code(param->result), scan_info, bt_event_slot_container[event_index].user_data);
				__bt_free_bt_adapter_le_device_scan_info_s(scan_info);
			} else {
				((bt_adapter_le_scan_result_cb)bt_event_slot_container[event_index].callback)
					(_bt_get_error_code(param->result), NULL, bt_event_slot_container[event_index].user_data);
			}
		}
#ifndef TIZEN_WEARABLE
		event_index = BT_EVENT_LE_DEVICE_DISCOVERY_STATE_CHANGED;
		if (bt_event_slot_container[event_index].callback != NULL) {
			if (__bt_get_bt_adapter_le_device_discovery_info_s(&discovery_info, (bluetooth_le_device_info_t *)(param->param_data)) == BT_ERROR_NONE) {
				((bt_adapter_le_device_discovery_state_changed_cb)bt_event_slot_container[event_index].callback)
					(_bt_get_error_code(param->result), BT_ADAPTER_LE_DEVICE_DISCOVERY_FOUND, discovery_info, bt_event_slot_container[event_index].user_data);
				__bt_free_bt_adapter_le_device_discovery_info_s(discovery_info);
			} else {
				((bt_adapter_le_device_discovery_state_changed_cb)bt_event_slot_container[event_index].callback)
					(_bt_get_error_code(param->result), BT_ADAPTER_LE_DEVICE_DISCOVERY_FOUND, NULL, bt_event_slot_container[event_index].user_data);
			}
		}
#endif
		break;

	default:
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

				BT_INFO("UUID: %s", (*discovery_info)->service_uuid[i]);
			}
		}
	} else {
		(*discovery_info)->service_uuid = NULL;
	}
	(*discovery_info)->service_count = source_info->service_index;

	(*discovery_info)->rssi = (int)source_info->rssi;
	(*discovery_info)->is_bonded = (bool)source_info->paired;

	(*discovery_info)->appearance = 0;

	(*discovery_info)->manufacturer_data_len = source_info->manufacturer_data.data_len;
	if ((*discovery_info)->manufacturer_data_len > 0) {
		(*discovery_info)->manufacturer_data = malloc(source_info->manufacturer_data.data_len);
		memcpy((*discovery_info)->manufacturer_data, source_info->manufacturer_data.data, source_info->manufacturer_data.data_len);
	} else {
		(*discovery_info)->manufacturer_data = NULL;
	}

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
	if (discovery_info->manufacturer_data != NULL)
		free(discovery_info->manufacturer_data);

	free(discovery_info);
	discovery_info = NULL;
}

static int __bt_get_bt_adapter_le_device_scan_info_s(
			bt_adapter_le_device_scan_result_info_s **scan_info,
			bluetooth_le_device_info_t *source_info)
{

	BT_CHECK_INPUT_PARAMETER(source_info);

	*scan_info = (bt_adapter_le_device_scan_result_info_s *)malloc(sizeof(bt_adapter_le_device_scan_result_info_s));
	if (*scan_info == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	_bt_convert_address_to_string(&((*scan_info)->remote_address), &(source_info->device_address));

	if (source_info->addr_type == 0x02)
		(*scan_info)->address_type = BT_DEVICE_RANDOM_ADDRESS;
	else
		(*scan_info)->address_type = BT_DEVICE_PUBLIC_ADDRESS;
	(*scan_info)->rssi = (int)source_info->rssi;
	(*scan_info)->adv_data_len = source_info->adv_ind_data.data_len;
	if ((*scan_info)->adv_data_len > 0) {
		(*scan_info)->adv_data = malloc(source_info->adv_ind_data.data_len);
		memcpy((*scan_info)->adv_data, source_info->adv_ind_data.data.data, source_info->adv_ind_data.data_len);
	} else {
		(*scan_info)->adv_data = NULL;
	}

	(*scan_info)->scan_data_len = source_info->scan_resp_data.data_len;
	if ((*scan_info)->scan_data_len > 0) {
		(*scan_info)->scan_data = malloc(source_info->scan_resp_data.data_len);
		memcpy((*scan_info)->scan_data, source_info->scan_resp_data.data.data, source_info->scan_resp_data.data_len);
	} else {
		(*scan_info)->scan_data = NULL;
	}

	return BT_ERROR_NONE;
}

static void __bt_free_bt_adapter_le_device_scan_info_s(bt_adapter_le_device_scan_result_info_s *scan_info)
{
	if (scan_info == NULL)
		return;

	if (scan_info->remote_address != NULL)
		free(scan_info->remote_address);

	if (scan_info->adv_data != NULL)
		free(scan_info->adv_data);
	if (scan_info->scan_data != NULL)
		free(scan_info->scan_data);

	free(scan_info);
	scan_info = NULL;
}

#ifndef TIZEN_WEARABLE
static int __bt_get_bt_adapter_le_device_discovery_info_s(
			bt_adapter_le_device_discovery_info_s **le_discovery_info,
			bluetooth_le_device_info_t *source_info)
{

	BT_CHECK_INPUT_PARAMETER(source_info);

	*le_discovery_info = (bt_adapter_le_device_discovery_info_s *)malloc(sizeof(bt_adapter_le_device_discovery_info_s));
	if (*le_discovery_info == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	_bt_convert_address_to_string(&((*le_discovery_info)->remote_address), &(source_info->device_address));

	(*le_discovery_info)->address_type = (int)source_info->addr_type;
	(*le_discovery_info)->rssi = (int)source_info->rssi;
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

static void __bt_free_bt_adapter_le_device_discovery_info_s(bt_adapter_le_device_discovery_info_s *discovery_info)
{
	if (discovery_info == NULL)
		return;

	if (discovery_info->remote_address != NULL)
		free(discovery_info->remote_address);

	if (discovery_info->adv_data != NULL)
		free(discovery_info->adv_data);
	if (discovery_info->scan_data != NULL)
		free(discovery_info->scan_data);

	free(discovery_info);
	discovery_info = NULL;
}

#endif

#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
static int __bt_get_gatt_client_cb_index(int event)
{
	switch (event) {
	case BLUETOOTH_EVENT_GATT_CHAR_VAL_CHANGED:
		if (_bt_gatt_is_legacy_client_mode())
			return BT_EVENT_GATT_CLIENT_VALUE_CHANGED_LEGACY;
		else
			return BT_EVENT_GATT_CLIENT_VALUE_CHANGED;
	case BLUETOOTH_EVENT_GATT_READ_CHAR:
		if (_bt_gatt_is_legacy_client_mode())
			return BT_EVENT_GATT_CLIENT_READ_CHARACTERISTIC_LEGACY;
		else
			return BT_EVENT_GATT_CLIENT_READ_CHARACTERISTIC;
	case BLUETOOTH_EVENT_GATT_WRITE_CHAR:
		if (_bt_gatt_is_legacy_client_mode())
			return BT_EVENT_GATT_CLIENT_WRITE_CHARACTERISTIC_LEGACY;
		else
			return BT_EVENT_GATT_CLIENT_WRITE_CHARACTERISTIC;
	}

	return -1;
}
#endif

static int __bt_get_cb_index(int event)
{
	switch (event) {
	case BLUETOOTH_EVENT_ENABLED:
	case BLUETOOTH_EVENT_DISABLED:
		return BT_EVENT_STATE_CHANGED;
	case BLUETOOTH_EVENT_LE_ENABLED:
	case BLUETOOTH_EVENT_LE_DISABLED:
		return BT_EVENT_LE_STATE_CHANGED;
	case BLUETOOTH_EVENT_LOCAL_NAME_CHANGED:
		return BT_EVENT_NAME_CHANGED;
	case BLUETOOTH_EVENT_DISCOVERABLE_MODE_CHANGED:
		return BT_EVENT_VISIBILITY_MODE_CHANGED;
	case BLUETOOTH_EVENT_DISCOVERABLE_TIMEOUT_CHANGED:
		return BT_EVENT_VISIBILITY_DURATION_CHANGED;
	case BLUETOOTH_EVENT_DISCOVERY_STARTED:
	case BLUETOOTH_EVENT_DISCOVERY_FINISHED:
	case BLUETOOTH_EVENT_REMOTE_DEVICE_NAME_UPDATED:
		return BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED;
	case BLUETOOTH_EVENT_LE_DISCOVERY_STARTED:
	case BLUETOOTH_EVENT_LE_DISCOVERY_FINISHED:
	case BLUETOOTH_EVENT_REMOTE_LE_DEVICE_FOUND:
		return BT_EVENT_LE_DEVICE_DISCOVERY_STATE_CHANGED;
	case BLUETOOTH_EVENT_BONDING_FINISHED:
		return BT_EVENT_BOND_CREATED;
	case BLUETOOTH_EVENT_BONDED_DEVICE_REMOVED:
		return BT_EVENT_BOND_DESTROYED;
	case BLUETOOTH_EVENT_KEYBOARD_PASSKEY_DISPLAY:
	case BLUETOOTH_EVENT_PASSKEY_REQUEST:
	case BLUETOOTH_EVENT_PIN_REQUEST:
	case BLUETOOTH_EVENT_PASSKEY_CONFIRM_REQUEST:
		return BT_EVENT_AUTHENTICATION_REQUEST;
	case BLUETOOTH_EVENT_DEVICE_AUTHORIZED:
	case BLUETOOTH_EVENT_DEVICE_UNAUTHORIZED:
		return BT_EVENT_AUTHORIZATION_CHANGED;
	case BLUETOOTH_EVENT_DEVICE_CONNECTED:
	case BLUETOOTH_EVENT_DEVICE_DISCONNECTED:
		return BT_EVENT_DEVICE_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_GATT_CONNECTED:
	case BLUETOOTH_EVENT_GATT_DISCONNECTED:
		return BT_EVENT_GATT_CONNECTION_STATUS;
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
	case BLUETOOTH_EVENT_AV_SOURCE_CONNECTED:
		return BT_EVENT_AUDIO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_AV_SOURCE_DISCONNECTED:
		return BT_EVENT_AUDIO_CONNECTION_STATUS;
	case  BLUETOOTH_EVENT_AVRCP_CONNECTED:
	case  BLUETOOTH_EVENT_AVRCP_DISCONNECTED:
	case  BLUETOOTH_EVENT_AVRCP_CONTROL_CONNECTED:
	case  BLUETOOTH_EVENT_AVRCP_CONTROL_DISCONNECTED:
		return BT_EVENT_AVRCP_CONNECTION_STATUS;
	case  BLUETOOTH_EVENT_AVRCP_SETTING_SHUFFLE_STATUS:
	case  BLUETOOTH_EVENT_AVRCP_CONTROL_SHUFFLE_STATUS:
		return BT_EVENT_AVRCP_SHUFFLE_MODE_CHANGED;
	case  BLUETOOTH_EVENT_AVRCP_SETTING_EQUALIZER_STATUS:
	case  BLUETOOTH_EVENT_AVRCP_CONTROL_EQUALIZER_STATUS:
		return BT_EVENT_AVRCP_EQUALIZER_STATE_CHANGED;
	case  BLUETOOTH_EVENT_AVRCP_SETTING_REPEAT_STATUS:
	case  BLUETOOTH_EVENT_AVRCP_CONTROL_REPEAT_STATUS:
		return BT_EVENT_AVRCP_REPEAT_MODE_CHANGED;
	case  BLUETOOTH_EVENT_AVRCP_SETTING_SCAN_STATUS:
	case  BLUETOOTH_EVENT_AVRCP_CONTROL_SCAN_STATUS:
		return BT_EVENT_AVRCP_SCAN_MODE_CHANGED;
	case  BLUETOOTH_EVENT_AVRCP_SONG_POSITION_STATUS:
		return BT_EVENT_AVRCP_SONG_POSITION_CHANGED;
	case  BLUETOOTH_EVENT_AVRCP_PLAY_STATUS_CHANGED:
		return BT_EVENT_AVRCP_PLAY_STATUS_CHANGED;
	case  BLUETOOTH_EVENT_AVRCP_TRACK_CHANGED:
		return BT_EVENT_AVRCP_TRACK_INFO_CHANGED;
	case BLUETOOTH_HID_CONNECTED:
		return  BT_EVENT_HID_CONNECTION_STATUS;
	case BLUETOOTH_HID_DISCONNECTED:
		return  BT_EVENT_HID_CONNECTION_STATUS;
	case BLUETOOTH_HID_DEVICE_CONNECTED:
		return BT_EVENT_HID_DEVICE_CONNECTION_STATUS;
	case BLUETOOTH_HID_DEVICE_DISCONNECTED:
		return BT_EVENT_HID_DEVICE_CONNECTION_STATUS;
	case BLUETOOTH_HID_DEVICE_DATA_RECEIVED:
		return BT_EVENT_HID_DEVICE_DATA_RECEIVED;
#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
	case BLUETOOTH_EVENT_GATT_SVC_CHAR_DISCOVERED:
		return BT_EVENT_GATT_CLIENT_CHARACTERISTIC_DISCOVERED_LEGACY;
	case BLUETOOTH_EVENT_GATT_SVC_CHAR_DESC_DISCOVERED:
		return BT_EVENT_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERED_LEGACY;
	case BLUETOOTH_EVENT_GATT_CHAR_VAL_CHANGED:
	case BLUETOOTH_EVENT_GATT_READ_CHAR:
	case BLUETOOTH_EVENT_GATT_WRITE_CHAR:
		return __bt_get_gatt_client_cb_index(event);
#else
	case BLUETOOTH_EVENT_GATT_CHAR_VAL_CHANGED:
		return BT_EVENT_GATT_CLIENT_VALUE_CHANGED;
	case BLUETOOTH_EVENT_GATT_READ_CHAR:
		return BT_EVENT_GATT_CLIENT_READ_CHARACTERISTIC;
	case BLUETOOTH_EVENT_GATT_WRITE_CHAR:
		return BT_EVENT_GATT_CLIENT_WRITE_CHARACTERISTIC;
#endif
	case BLUETOOTH_EVENT_GATT_READ_DESC:
		return BT_EVENT_GATT_CLIENT_READ_DESCRIPTOR;
	case BLUETOOTH_EVENT_GATT_WRITE_DESC:
		return BT_EVENT_GATT_CLIENT_WRITE_DESCRIPTOR;
	case BLUETOOTH_EVENT_ADVERTISING_STARTED:
	case BLUETOOTH_EVENT_ADVERTISING_STOPPED:
		return BT_EVENT_ADVERTISING_STATE_CHANGED;
	case BLUETOOTH_EVENT_MANUFACTURER_DATA_CHANGED:
		return BT_EVENT_MANUFACTURER_DATA_CHANGED;
	case BLUETOOTH_EVENT_CONNECTABLE_CHANGED:
		return BT_EVENT_CONNECTABLE_CHANGED_EVENT;
	case BLUETOOTH_EVENT_IPSP_CONNECTED:
	case BLUETOOTH_EVENT_IPSP_DISCONNECTED:
		return BT_EVENT_IPSP_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_LE_DATA_LENGTH_CHANGED:
		return BT_EVENT_LE_DATA_LENGTH_CHANGED;
#ifdef TIZEN_WEARABLE
	case BLUETOOTH_PBAP_CONNECTED:
		return BT_EVENT_PBAP_CONNECTION_STATUS;
	case BLUETOOTH_PBAP_PHONEBOOK_SIZE:
		return BT_EVENT_PBAP_PHONEBOOK_SIZE;
	case BLUETOOTH_PBAP_PHONEBOOK_PULL:
		return BT_EVENT_PBAP_PHONEBOOK_PULL;
	case BLUETOOTH_PBAP_VCARD_LIST:
		return BT_EVENT_PBAP_VCARD_LIST;
	case BLUETOOTH_PBAP_VCARD_PULL:
		return BT_EVENT_PBAP_VCARD_PULL;
	case BLUETOOTH_PBAP_PHONEBOOK_SEARCH:
		return BT_EVENT_PBAP_PHONEBOOK_SEARCH;
	case BLUETOOTH_EVENT_HF_CONNECTED:
		return BT_EVENT_AUDIO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_HF_DISCONNECTED:
		return BT_EVENT_AUDIO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_HF_AUDIO_CONNECTED:
		return BT_EVENT_HF_SCO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_HF_AUDIO_DISCONNECTED:
		return BT_EVENT_HF_SCO_CONNECTION_STATUS;
	case BLUETOOTH_EVENT_HF_RING_INDICATOR:
	case BLUETOOTH_EVENT_HF_CALL_TERMINATED:
	case BLUETOOTH_EVENT_HF_CALL_STARTED:
	case BLUETOOTH_EVENT_HF_CALL_ENDED:
	case BLUETOOTH_EVENT_HF_VOICE_RECOGNITION_ENABLED:
	case BLUETOOTH_EVENT_HF_VOICE_RECOGNITION_DISABLED:
	case BLUETOOTH_EVENT_HF_CALL_WAITING:
	case BLUETOOTH_EVENT_HF_CALL_ON_HOLD:
	case BLUETOOTH_EVENT_HF_CALL_UNHOLD:
	case BLUETOOTH_EVENT_HF_CALL_SWAPPED:
		return BT_EVENT_HF_CALL_HANDLING_EVENT;
	case BLUETOOTH_EVENT_HF_VOLUME_SPEAKER:
		return BT_EVENT_HF_SPEAKER_GAIN_CHANGE;
	case BLUETOOTH_EVENT_HF_CALL_STATUS:
		return BT_EVENT_HF_CALL_STATUS_UPDATED_EVENT;
#endif
	case BLUETOOTH_EVENT_IPSP_INIT_STATE_CHANGED:
		return  BT_EVENT_IPSP_INIT_STATE_CHANGED;
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

static void bt_gatt_client_handle_destroy(bt_gatt_h gatt_handle)
{
	if (gatt_handle)
		bt_gatt_destroy(gatt_handle);
}

int _bt_gatt_client_update_all(bt_gatt_client_h client)
{
	bt_gatt_client_s *client_s = client;
	bluetooth_device_address_t addr_hex = { {0, } };
	bt_gatt_handle_info_t *prim_svc = NULL;
	int ret;
	int i;
	int j;
	GSList *svc_list = NULL;

	prim_svc = g_new0(bt_gatt_handle_info_t, 1);

	_bt_convert_address_to_hex(&addr_hex, client_s->remote_address);
	ret = bluetooth_gatt_get_primary_services(&addr_hex, prim_svc);
	ret = _bt_get_error_code(ret);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("bluetooth_gatt_get_primary_services is failed");
		return ret;
	}

	if (prim_svc->count == 0)
		goto done;

	for (i = 0; i < prim_svc->count; i++) {
		bt_gatt_service_s *svc = NULL;
		bt_gatt_service_property_t property;

		if (prim_svc->handle[i] == NULL)
			continue;
		BT_DBG("handle: %s", prim_svc->handle[i]);

		memset(&property, 0x00, sizeof(property));

		ret = bluetooth_gatt_get_service_property(
				prim_svc->handle[i], &property);
		ret = _bt_get_error_code(ret);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("bluetooth_gatt_get_service_property is failed");
			continue;
		}

		/* Add primary service */
		ret = bt_gatt_service_create(property.uuid,
				BT_GATT_SERVICE_TYPE_PRIMARY,
				(bt_gatt_h *)&svc);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("bt_gatt_service_create is failed");
			continue;
		}

		svc->role = BT_GATT_ROLE_CLIENT;
		svc->parent = (void *)client_s;
		svc->path = g_strdup(prim_svc->handle[i]);

		svc_list = g_slist_append(svc_list, svc);

		if (property.include_handles.count != 0 && property.include_handles.handle) {
			GSList *include_list = NULL;
			/* Add included services */
			for (j = 0; j < property.include_handles.count; j++) {
				bt_gatt_service_s *sec_svc = NULL;
				bt_gatt_service_property_t sec_property;

				if (property.include_handles.handle[j] == NULL)
					continue;

				memset(&sec_property, 0x00, sizeof(sec_property));

				ret = bluetooth_gatt_get_service_property(
						property.include_handles.handle[j],
						&sec_property);
				ret = _bt_get_error_code(ret);
				if (ret != BT_ERROR_NONE)
					continue;

				ret = bt_gatt_service_create(sec_property.uuid,
						BT_GATT_SERVICE_TYPE_SECONDARY,
						(bt_gatt_h *)&sec_svc);
				if (ret != BT_ERROR_NONE)
					continue;

				sec_svc->role = BT_GATT_ROLE_CLIENT;
				sec_svc->parent = (void *)svc;
				sec_svc->is_included_service = true;
				sec_svc->path = g_strdup(property.include_handles.handle[j]);

				include_list = g_slist_append(include_list, sec_svc);
				bluetooth_gatt_free_service_property(&sec_property);
			}

			g_slist_free_full(svc->included_services,
					bt_gatt_client_handle_destroy);
			svc->included_services = include_list;
		}
		if (property.char_handle.count != 0 && property.char_handle.handle) {

			/* Add characteristics */
			__bt_gatt_client_update_characteristics(property.char_handle, svc);
		}
		bluetooth_gatt_free_service_property(&property);
	}
	g_strfreev(prim_svc->handle);

	g_free(prim_svc);

	if (svc_list == NULL)
		return BT_ERROR_OPERATION_FAILED;

done:
	g_slist_free_full(client_s->services,
			bt_gatt_client_handle_destroy);

	client_s->services = svc_list;

	return BT_ERROR_NONE;
}

static int __bt_gatt_client_update_characteristics(bt_gatt_handle_info_t char_handles,
		bt_gatt_service_s *service)
{
	GSList *chr_list = NULL;
	int i;

	for (i = 0; i < char_handles.count; i++) {
		bt_gatt_characteristic_s *chr = NULL;
		bt_gatt_char_property_t char_property;
		int ret;

		memset(&char_property, 0x00, sizeof(char_property));
		ret = bluetooth_gatt_get_characteristics_property(
				char_handles.handle[i], &char_property);
		ret = _bt_get_error_code(ret);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("bluetooth_gatt_get_characteristics_property is failed");
			goto next;
		}

		ret = bt_gatt_characteristic_create(char_property.uuid, 0,
				char_property.permission, (char *)char_property.val,
				(int)char_property.val_len, (bt_gatt_h *)&chr);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("bt_gatt_characteristic_create is failed");
			goto next;
		}

		if (char_property.permission & BT_GATT_PROPERTY_WRITE_WITHOUT_RESPONSE)
			chr->write_type = BT_GATT_WRITE_TYPE_WRITE_NO_RESPONSE;
		else if (char_property.permission & BT_GATT_PROPERTY_WRITE)
			chr->write_type = BT_GATT_WRITE_TYPE_WRITE;

		chr->path = g_strdup(char_handles.handle[i]);
		BT_DBG("path : %s", chr->path);
		chr->parent = (void *)service;
		chr->role = BT_GATT_ROLE_CLIENT;

		if (char_property.char_desc_handle.count != 0 && char_property.char_desc_handle.handle) {

			/* Add characteristics descriptor */
			__bt_gatt_client_update_descriptors(char_property.char_desc_handle, chr);
		}

		chr_list = g_slist_append(chr_list, chr);
next:
		bluetooth_gatt_free_char_property(&char_property);
	}

	g_slist_free_full(service->characteristics,
			bt_gatt_client_handle_destroy);
	service->characteristics = chr_list;
	return BT_ERROR_NONE;
}

static int __bt_gatt_client_update_descriptors(bt_gatt_handle_info_t desc_handles,
		bt_gatt_characteristic_s *characteristic)

{
	GSList *desc_list = NULL;
	int i;

	/* Add characteristics descriptor */

	for (i = 0; i < desc_handles.count; i++) {
		bt_gatt_descriptor_s *desc = NULL;
		bt_gatt_char_descriptor_property_t desc_property;
		int ret;

		memset(&desc_property, 0x00, sizeof(desc_property));
		ret = bluetooth_gatt_get_char_descriptor_property(
				desc_handles.handle[i], &desc_property);
		ret = _bt_get_error_code(ret);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("bluetooth_gatt_get_char_descriptor_property is failed");
			goto next;
		}

		ret = bt_gatt_descriptor_create(desc_property.uuid, 0,
					(char*)desc_property.val, (int)desc_property.val_len,
					(bt_gatt_h *)&desc);

		if (ret != BT_ERROR_NONE) {
			BT_ERR("bt_gatt_characteristic_create is failed");
			goto next;
		}
		desc->path = g_strdup(desc_handles.handle[i]);
		BT_DBG("path : %s", desc->path);
		desc->parent = (void *)characteristic;
		desc->role = BT_GATT_ROLE_CLIENT;
		desc_list = g_slist_append(desc_list, desc);

next:
		bluetooth_gatt_free_desc_property(&desc_property);
	}

	g_slist_free_full(characteristic->descriptors,
			bt_gatt_client_handle_destroy);
	characteristic->descriptors = desc_list;
	return BT_ERROR_NONE;
}
