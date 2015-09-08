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
#include <dlog.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <bluetooth-api.h>

#include "bluetooth.h"
#include "bluetooth_private.h"

#ifdef TIZEN_HDP_DISABLE
#define BT_CHECK_HDP_SUPPORT() \
	{ \
		BT_CHECK_BT_SUPPORT(); \
		LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
		return BT_ERROR_NOT_SUPPORTED; \
	}
#else
#define BT_CHECK_HDP_SUPPORT()
#endif

int bt_hdp_register_sink_app(unsigned short data_type, char **app_id)
{
	int error = BT_ERROR_NONE;
	char *app_handle = NULL;

	BT_CHECK_HDP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(app_id);
	error = bluetooth_hdp_activate(data_type, HDP_ROLE_SINK, HDP_QOS_ANY, &app_handle);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	*app_id = strdup(app_handle);
	if (*app_id == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	return BT_ERROR_NONE;
}

int bt_hdp_unregister_sink_app(const char *app_id)
{
	int error = BT_ERROR_NONE;

	BT_CHECK_HDP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(app_id);
	error = bluetooth_hdp_deactivate(app_id);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_hdp_send_data(unsigned int channel, const char *data, unsigned int size)
{
	int error = BT_ERROR_NONE;

	BT_CHECK_HDP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	if (NULL == data || 0 >= size) {
		BT_ERR("%s", _bt_convert_error_to_string(BT_ERROR_INVALID_PARAMETER));
	}
	error = bluetooth_hdp_send_data(channel, data, size);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_hdp_connect_to_source(const char *remote_address, const char *app_id)
{
	int error = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_HDP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(app_id);
	BT_CHECK_INPUT_PARAMETER(remote_address);
	_bt_convert_address_to_hex(&addr_hex, remote_address);
	error = bluetooth_hdp_connect(app_id, HDP_QOS_ANY, &addr_hex);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_hdp_disconnect(const char *remote_address, unsigned int channel)
{
	int error = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_HDP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	_bt_convert_address_to_hex(&addr_hex, remote_address);

	error = bluetooth_hdp_disconnect(channel, &addr_hex);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_hdp_set_connection_state_changed_cb(bt_hdp_connected_cb connected_cb,
		bt_hdp_disconnected_cb disconnected_cb, void *user_data)
{
	BT_CHECK_HDP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(connected_cb);
	BT_CHECK_INPUT_PARAMETER(disconnected_cb);
	_bt_set_cb(BT_EVENT_HDP_CONNECTED, connected_cb, user_data);
	_bt_set_cb(BT_EVENT_HDP_DISCONNECTED, disconnected_cb, user_data);
	return BT_ERROR_NONE;

}

int bt_hdp_unset_connection_state_changed_cb(void)
{
	BT_CHECK_HDP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	if ( _bt_check_cb(BT_EVENT_HDP_CONNECTED) == true)
		_bt_unset_cb(BT_EVENT_HDP_CONNECTED);
	if ( _bt_check_cb(BT_EVENT_HDP_DISCONNECTED) == true)
		_bt_unset_cb(BT_EVENT_HDP_DISCONNECTED);

	return BT_ERROR_NONE;
}

int bt_hdp_set_data_received_cb(bt_hdp_data_received_cb callback,
				void *user_data)
{
	BT_CHECK_HDP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_HDP_DATA_RECEIVED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_hdp_unset_data_received_cb(void)
{
	BT_CHECK_HDP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	if ( _bt_check_cb(BT_EVENT_HDP_DATA_RECEIVED) == true)
		_bt_unset_cb(BT_EVENT_HDP_DATA_RECEIVED);
	return BT_ERROR_NONE;
}
