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
#include <bluetooth-ipsp-api.h>

#include "bluetooth.h"
#include "bluetooth_private.h"

static bool is_ipsp_initialized = false;

#ifdef TIZEN_IPSP_SUPPORT
#define BT_CHECK_IPSP_SUPPORT()
#else
#define BT_CHECK_IPSP_SUPPORT() \
	{ \
		LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
		return BT_ERROR_NOT_SUPPORTED; \
	}
#endif

#define BT_CHECK_IPSP_INIT_STATUS() \
	if (__bt_check_ipsp_init_status() == BT_ERROR_NOT_INITIALIZED) \
	{ \
		LOGE("[%s] NOT_INITIALIZED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_INITIALIZED); \
		return BT_ERROR_NOT_INITIALIZED; \
	}

int __bt_check_ipsp_init_status(void)
{
	if (is_ipsp_initialized != true) {
		BT_ERR("NOT_INITIALIZED(0x%08x)", BT_ERROR_NOT_INITIALIZED);
		return BT_ERROR_NOT_INITIALIZED;
	}

	return BT_ERROR_NONE;
}

int _bt_le_ipsp_is_initialized(void)
{
	BT_CHECK_IPSP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_IPSP_INIT_STATUS();
	return BT_ERROR_NONE;
}

int _bt_le_ipsp_initialize(bt_le_ipsp_init_state_changed_cb callback,
								void *user_data)
{
	int error;

	if (is_ipsp_initialized)
		return BT_ERROR_ALREADY_DONE;

	BT_CHECK_IPSP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	error = bluetooth_le_ipsp_init();
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	_bt_set_cb(BT_EVENT_IPSP_INIT_STATE_CHANGED, callback, user_data);

	is_ipsp_initialized = true;
	return BT_ERROR_NONE;
}

int _bt_le_ipsp_deinitialize(void)
{
	int error;

	BT_CHECK_IPSP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_IPSP_INIT_STATUS();

	error = bluetooth_le_ipsp_deinit();
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	is_ipsp_initialized = false;
	return BT_ERROR_NONE;
}

int _bt_le_ipsp_connect(const char *address)
{
	int ret;
	bluetooth_device_address_t bd_addr = { {0,} };

	BT_CHECK_IPSP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);

	_bt_convert_address_to_hex(&bd_addr, address);

	ret = _bt_get_error_code(bluetooth_le_ipsp_connect((ipsp_device_address_t *)&bd_addr));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int _bt_le_ipsp_disconnect(const char *address)
{
	int ret;
	bluetooth_device_address_t bd_addr = { {0,} };

	BT_CHECK_IPSP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);

	_bt_convert_address_to_hex(&bd_addr, address);

	ret = _bt_get_error_code(bluetooth_le_ipsp_disconnect((ipsp_device_address_t *)&bd_addr));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int _bt_le_ipsp_set_connection_state_changed_cb(_bt_le_ipsp_connection_state_changed_cb callback, void *user_data)
{
	BT_CHECK_IPSP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_IPSP_CONNECTION_STATUS, callback, user_data);

	return BT_ERROR_NONE;
}

int _bt_le_ipsp_unset_connection_state_changed_cb(void)
{
	BT_CHECK_IPSP_SUPPORT();
	BT_CHECK_INIT_STATUS();

	_bt_unset_cb(BT_EVENT_IPSP_CONNECTION_STATUS);
	return BT_ERROR_NONE;
}
