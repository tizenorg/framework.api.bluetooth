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

#include <errno.h>
#include <tizen.h>
#include <dlog.h>
#include <stdio.h>
#include <bluetooth-api.h>

#include "bluetooth.h"
#include "bluetooth_private.h"

int bt_socket_create_rfcomm(const char *uuid, int *socket_fd)
{
	int ret = 0;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(socket_fd);

	ret = bluetooth_rfcomm_create_socket(uuid);
	if (ret < 0) {
		ret = _bt_get_error_code(ret);
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	} else {
		*socket_fd = ret;
		return BT_ERROR_NONE;
	}
}

int bt_socket_destroy_rfcomm(int socket_fd)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_rfcomm_remove_socket(socket_fd));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_socket_is_service_used(const char* service_uuid, bool *used)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(service_uuid);
	BT_CHECK_INPUT_PARAMETER(used);

	*used = bluetooth_rfcomm_is_server_uuid_available(service_uuid);

	return BT_ERROR_NONE;
}

int bt_socket_listen_and_accept_rfcomm(int socket_fd, int max_pending_connections)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_rfcomm_listen_and_accept(socket_fd, max_pending_connections));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_socket_listen(int socket_fd, int max_pending_connections)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_rfcomm_listen(socket_fd, max_pending_connections));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_socket_accept(int socket_fd)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_rfcomm_accept_connection(socket_fd));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_socket_reject(int socket_fd)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_rfcomm_reject_connection(socket_fd));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_socket_create_rfcomm_ex(const char *uuid, const char *bus_name, const char *object_path)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(bus_name);
	BT_CHECK_INPUT_PARAMETER(object_path);

	error_code = bluetooth_rfcomm_create_socket_ex(uuid, bus_name, object_path);
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_socket_destroy_rfcomm_ex(const char *uuid)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(uuid);

	error_code = _bt_get_error_code(bluetooth_rfcomm_remove_socket_ex(uuid));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_socket_listen_and_accept_rfcomm_ex(const char *uuid, int max_pending_connections, const char* bus_name, const char *object_path)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(bus_name);
	BT_CHECK_INPUT_PARAMETER(object_path);

	error_code = _bt_get_error_code(bluetooth_rfcomm_listen_and_accept_ex(uuid, max_pending_connections, bus_name, object_path));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_socket_connect_rfcomm(const char *remote_address, const char *remote_port_uuid)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(remote_port_uuid);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	error_code = _bt_get_error_code(bluetooth_rfcomm_connect(&addr_hex, remote_port_uuid));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_socket_disconnect_rfcomm(int socket_fd)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_rfcomm_disconnect(socket_fd));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_socket_send_data(int socket_fd, const char *data, int length)
{
	int ret = 0;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(data);

	ret = bluetooth_rfcomm_write(socket_fd, data, length);
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

int bt_socket_set_data_received_cb(bt_socket_data_received_cb callback, void *user_data)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_DATA_RECEIVED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_socket_unset_data_received_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_DATA_RECEIVED);
	return BT_ERROR_NONE;
}

int bt_socket_set_connection_requested_cb(bt_socket_connection_requested_cb callback, void *user_data)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_RFCOMM_CONNECTION_REQUESTED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_socket_unset_connection_requested_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_RFCOMM_CONNECTION_REQUESTED);
	return BT_ERROR_NONE;
}

int bt_socket_set_connection_state_changed_cb(bt_socket_connection_state_changed_cb callback, void *user_data)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_CONNECTION_STATE_CHANGED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_socket_unset_connection_state_changed_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_CONNECTION_STATE_CHANGED);
	return BT_ERROR_NONE;
}

