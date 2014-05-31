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
#include <bluetooth-hid-api.h>

#include "bluetooth.h"
#include "bluetooth_private.h"

int bt_hid_host_initialize(bt_hid_host_connection_state_changed_cb connection_cb,
								void *user_data)
{
	int error;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(connection_cb);

	error = bluetooth_hid_init(_bt_hid_event_proxy, user_data);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	} else {
		_bt_set_cb(BT_EVENT_HID_CONNECTION_STATUS, connection_cb, user_data);
	}
	return error;
}

int bt_hid_host_deinitialize()
{
	int error;

	BT_CHECK_INIT_STATUS();

	error = bluetooth_hid_deinit();
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	} else {
		_bt_unset_cb(BT_EVENT_HID_CONNECTION_STATUS);
	}

	return error;
}

int bt_hid_host_connect(const char *remote_address)
{
	int error;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	error = bluetooth_hid_connect((hid_device_address_t *)&addr_hex);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_hid_host_disconnect(const char *remote_address)
{
	int error;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	error = bluetooth_hid_disconnect((hid_device_address_t *)&addr_hex);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}
