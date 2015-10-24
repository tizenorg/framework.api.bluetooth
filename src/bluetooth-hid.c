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

static bool is_hid_host_initialized = false;

#ifdef TIZEN_HID_HOST_DISABLE
#define BT_CHECK_HID_HOST_SUPPORT() \
	{ \
		BT_CHECK_BT_SUPPORT(); \
		LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
		return BT_ERROR_NOT_SUPPORTED; \
	}
#else
#define BT_CHECK_HID_HOST_SUPPORT()
#endif

#ifdef TIZEN_HID_DEVICE_DISABLE
#define BT_CHECK_HID_DEVICE_SUPPORT() \
        { \
                BT_CHECK_BT_SUPPORT(); \
                LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
                return BT_ERROR_NOT_SUPPORTED; \
        }
#else
#define BT_CHECK_HID_DEVICE_SUPPORT()
#endif

#define BT_CHECK_HID_HOST_INIT_STATUS() \
	if (__bt_check_hid_host_init_status() == BT_ERROR_NOT_INITIALIZED) \
	{ \
		LOGE("[%s] NOT_INITIALIZED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_INITIALIZED); \
		return BT_ERROR_NOT_INITIALIZED; \
	}

int __bt_check_hid_host_init_status(void)
{
	if (is_hid_host_initialized != true) {
		BT_ERR("NOT_INITIALIZED(0x%08x)", BT_ERROR_NOT_INITIALIZED);
		return BT_ERROR_NOT_INITIALIZED;
	}

	return BT_ERROR_NONE;
}

int bt_hid_host_initialize(bt_hid_host_connection_state_changed_cb connection_cb,
								void *user_data)
{
	int error;

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(connection_cb);

	error = bluetooth_hid_init(_bt_hid_event_proxy, user_data);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	_bt_set_cb(BT_EVENT_HID_CONNECTION_STATUS, connection_cb, user_data);

	is_hid_host_initialized = true;
	return BT_ERROR_NONE;
}

int bt_hid_host_deinitialize()
{
	int error;

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HID_HOST_INIT_STATUS();

	error = bluetooth_hid_deinit();
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	_bt_unset_cb(BT_EVENT_HID_CONNECTION_STATUS);

	is_hid_host_initialized = false;
	return BT_ERROR_NONE;
}

int bt_hid_host_connect(const char *remote_address)
{
	int error;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HID_HOST_INIT_STATUS();
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

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HID_HOST_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	error = bluetooth_hid_disconnect((hid_device_address_t *)&addr_hex);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_hid_device_activate(bt_hid_device_connection_state_changed_cb callback, void * user_data)
{
	int error;
	BT_CHECK_HID_DEVICE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	error = bluetooth_hid_device_init(_bt_hid_event_proxy, user_data);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	error = bluetooth_hid_device_activate();
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	_bt_set_cb(BT_EVENT_HID_DEVICE_CONNECTION_STATUS, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_hid_device_deactivate(void)
{
	int error;
	BT_CHECK_HID_DEVICE_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error = bluetooth_hid_device_deinit();
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	error = bluetooth_hid_device_deactivate();
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	_bt_unset_cb(BT_EVENT_HID_DEVICE_CONNECTION_STATUS);

	return BT_ERROR_NONE;
}

int bt_hid_device_connect(const char *remote_address)
{
	int error;
	BT_CHECK_HID_DEVICE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	error = bluetooth_hid_device_connect(remote_address);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}
	return BT_ERROR_NONE;
}
int bt_hid_device_disconnect(const char *remote_address)
{
	int error;
	BT_CHECK_HID_DEVICE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	error = bluetooth_hid_device_disconnect(remote_address);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	return error;
}

int bt_hid_device_send_mouse_event(const char *remote_address,
		const bt_hid_mouse_data_s *mouse_data)
{
	int ret;
	BT_CHECK_HID_DEVICE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(mouse_data);

	ret = bluetooth_hid_device_send_mouse_event(remote_address,
			*(hid_send_mouse_event_t*)mouse_data);
	if (ret <= 0) {
		if (ret == -1) {
			/* write fail case */
			if (errno == EACCES || errno == EPERM)
				set_last_result(BT_ERROR_PERMISSION_DENIED);
			else if (errno == EAGAIN || errno == EWOULDBLOCK)
				set_last_result(BT_ERROR_AGAIN);
			else
				set_last_result(BT_ERROR_OPERATION_FAILED);
		} else {
			ret = _bt_get_error_code(ret);
			set_last_result(ret);
		}

		BT_ERR("Write failed, ret = %d", ret);
	}

	return ret;
}

int bt_hid_device_send_key_event(const char *remote_address,
		const bt_hid_key_data_s *key_data)
{
	int ret;
	BT_CHECK_HID_DEVICE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(key_data);

	ret = bluetooth_hid_device_send_key_event(remote_address,
			*(hid_send_key_event_t*)key_data);
	if (ret <= 0) {
		if (ret == -1) {
			/* write fail case */
			if (errno == EACCES || errno == EPERM)
				set_last_result(BT_ERROR_PERMISSION_DENIED);
			else if (errno == EAGAIN || errno == EWOULDBLOCK)
				set_last_result(BT_ERROR_AGAIN);
			else
				set_last_result(BT_ERROR_OPERATION_FAILED);
		} else {
			ret = _bt_get_error_code(ret);
			set_last_result(ret);
		}

		BT_ERR("Write failed, ret = %d", ret);
	}

	return ret;
}

int bt_hid_device_reply_to_report(const char *remote_address,
		bluetooth_hid_header_type_t htype,
		bluetooth_hid_param_type_t ptype,
		const char *data, unsigned int data_len)
{
	int ret;
	BT_CHECK_HID_DEVICE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	ret = bluetooth_hid_device_reply_to_report(remote_address, htype,
				ptype, data, data_len);
	if (ret <= 0) {
		if (ret == -1) {
			/* write fail case */
			if (errno == EACCES || errno == EPERM)
				set_last_result(BT_ERROR_PERMISSION_DENIED);
			else if (errno == EAGAIN || errno == EWOULDBLOCK)
				set_last_result(BT_ERROR_AGAIN);
			else
				set_last_result(BT_ERROR_OPERATION_FAILED);
		} else {
			ret = _bt_get_error_code(ret);
			set_last_result(ret);
		}

		BT_ERR("Write failed, ret = %d", ret);
	}

	return ret;
}

int bt_hid_device_set_data_received_cb(bt_hid_device_data_received_cb callback, void *user_data)
{
	BT_CHECK_HID_DEVICE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_HID_DEVICE_DATA_RECEIVED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_hid_device_unset_data_received_cb(void)
{
	BT_CHECK_HID_DEVICE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_HID_DEVICE_DATA_RECEIVED);
	return BT_ERROR_NONE;
}
