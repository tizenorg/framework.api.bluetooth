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

#ifdef TIZEN_PAN_DISABLE
#define BT_CHECK_PAN_SUPPORT() \
		{ \
			BT_CHECK_BT_SUPPORT(); \
			LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
			return BT_ERROR_NOT_SUPPORTED; \
		}
#else
#define BT_CHECK_PAN_SUPPORT()
#endif

int bt_nap_activate(void)
{
	int error = BT_ERROR_NONE;

	BT_CHECK_PAN_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error = bluetooth_network_activate_server();
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_nap_deactivate(void)
{
	int error = BT_ERROR_NONE;

	BT_CHECK_PAN_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error = bluetooth_network_deactivate_server();
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

/* bluez don't support the disconnect API about NAP server */
/* So we implement this, deactivate the server and re-activate the server */
int bt_nap_disconnect_all(void)
{
	int error = BT_ERROR_NONE;

	BT_CHECK_PAN_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error = bluetooth_network_deactivate_server();
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	} else {
		return bt_nap_activate();
	}

	return error;
}

int bt_nap_disconnect(const char *remote_address)
{
	int error = BT_ERROR_INVALID_PARAMETER;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_PAN_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	_bt_convert_address_to_hex(&addr_hex, remote_address);
	error = bluetooth_network_server_disconnect(&addr_hex);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error),
				error);
	}
	return error;
}

int bt_nap_set_connection_state_changed_cb(
				bt_nap_connection_state_changed_cb callback,
				void *user_data)
{
	BT_CHECK_PAN_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_NAP_CONNECTION_STATE_CHANGED, callback, user_data);
	return BT_ERROR_NONE;

}

int bt_nap_unset_connection_state_changed_cb(void)
{
	BT_CHECK_PAN_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_NAP_CONNECTION_STATE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_panu_set_connection_state_changed_cb(
				bt_panu_connection_state_changed_cb callback,
				void *user_data)
{
	BT_CHECK_PAN_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_PAN_CONNECTION_STATE_CHANGED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_panu_unset_connection_state_changed_cb(void)
{
	BT_CHECK_PAN_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_PAN_CONNECTION_STATE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_panu_connect(const char *remote_address, bt_panu_service_type_e type)
{
	int error = BT_ERROR_INVALID_PARAMETER;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_PAN_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	_bt_convert_address_to_hex(&addr_hex, remote_address);
	if (BT_PANU_SERVICE_TYPE_NAP == type) {
		error = bluetooth_network_connect(&addr_hex,
					BLUETOOTH_NETWORK_NAP_ROLE, NULL);
		error = _bt_get_error_code(error);
		if (error != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error),
					error);
		}
	}
	return error;
}

int bt_panu_disconnect(const char *remote_address)
{
	int error = BT_ERROR_INVALID_PARAMETER;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_PAN_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	_bt_convert_address_to_hex(&addr_hex, remote_address);
	error = bluetooth_network_disconnect(&addr_hex);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error),
				error);
	}
	return error;
}

