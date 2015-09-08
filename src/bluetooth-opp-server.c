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

#include <dlog.h>
#include <stdio.h>
#include <stdbool.h>
#include <bluetooth-api.h>

#include "bluetooth.h"
#include "bluetooth_private.h"

static bool is_opp_server_initialized = false;

#ifdef TIZEN_OPP_SERVER_DISABLE
#define BT_CHECK_OPP_SERVER_SUPPORT() \
	{ \
		BT_CHECK_BT_SUPPORT(); \
		LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
		return BT_ERROR_NOT_SUPPORTED; \
	}
#else
#define BT_CHECK_OPP_SERVER_SUPPORT()
#endif


#define BT_CHECK_OPP_SERVER_INIT_STATUS() \
	if (__bt_check_opp_server_init_status() == BT_ERROR_NOT_INITIALIZED) \
	{ \
		LOGE("[%s] NOT_INITIALIZED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_INITIALIZED); \
		return BT_ERROR_NOT_INITIALIZED; \
	}

int __bt_check_opp_server_init_status(void)
{
	if (is_opp_server_initialized != true) {
		BT_ERR("NOT_INITIALIZED(0x%08x)", BT_ERROR_NOT_INITIALIZED);
		return BT_ERROR_NOT_INITIALIZED;
	}

	return BT_ERROR_NONE;
}

int bt_opp_server_initialize(const char *destination,
			bt_opp_server_push_requested_cb push_requested_cb,
			void *user_data)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_OPP_SERVER_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(destination);

	error_code = _bt_get_error_code(bluetooth_obex_server_init(destination));

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
		return error_code;
	}

	_bt_set_cb(BT_EVENT_OPP_PUSH_REQUESTED, push_requested_cb, user_data);

	is_opp_server_initialized = true;
	return BT_ERROR_NONE;
}

int bt_opp_server_initialize_by_connection_request(const char *destination,
			bt_opp_server_connection_requested_cb connection_requested_cb,
			void *user_data)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_OPP_SERVER_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(destination);

	error_code = _bt_get_error_code(bluetooth_obex_server_init_without_agent(destination));

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
		return error_code;
	}

	_bt_set_cb(BT_EVENT_OPP_CONNECTION_REQUESTED, connection_requested_cb, user_data);

	is_opp_server_initialized = true;
	return BT_ERROR_NONE;
}

int bt_opp_server_deinitialize(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_OPP_SERVER_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_OPP_SERVER_INIT_STATUS();

	if (_bt_check_cb(BT_EVENT_OPP_CONNECTION_REQUESTED) == false) {
		error_code = _bt_get_error_code(bluetooth_obex_server_deinit());
	} else {
		/* BADA API */
		error_code = _bt_get_error_code(bluetooth_obex_server_deinit_without_agent());
	}

	_bt_unset_cb(BT_EVENT_OPP_CONNECTION_REQUESTED);
	_bt_unset_cb(BT_EVENT_OPP_PUSH_REQUESTED);

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
		return error_code;
	}

	is_opp_server_initialized = false;
	return BT_ERROR_NONE;
}

int bt_opp_server_accept(bt_opp_server_transfer_progress_cb progress_cb,
			bt_opp_server_transfer_finished_cb finished_cb,
			const char *name,
			void *user_data,
			int *transfer_id)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_OPP_SERVER_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_OPP_SERVER_INIT_STATUS();

	/* Unset the transfer callbacks */
	_bt_unset_cb(BT_EVENT_OPP_SERVER_TRANSFER_PROGRESS);
	_bt_unset_cb(BT_EVENT_OPP_SERVER_TRANSFER_FINISHED);

	if (_bt_check_cb(BT_EVENT_OPP_CONNECTION_REQUESTED) == false) {
		error_code = _bt_get_error_code(bluetooth_obex_server_accept_authorize(name));
	} else {
		/* BADA API */
		error_code = _bt_get_error_code(bluetooth_obex_server_accept_connection());
	}

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	} else {
		_bt_set_cb(BT_EVENT_OPP_SERVER_TRANSFER_PROGRESS, progress_cb, user_data);
		_bt_set_cb(BT_EVENT_OPP_SERVER_TRANSFER_FINISHED, finished_cb, user_data);
	}

	return error_code;
}

int bt_opp_server_reject(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_OPP_SERVER_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_OPP_SERVER_INIT_STATUS();

	if (_bt_check_cb(BT_EVENT_OPP_CONNECTION_REQUESTED) == false) {
		error_code = _bt_get_error_code(bluetooth_obex_server_reject_authorize());
	} else {
		/* BADA API */
		error_code = _bt_get_error_code(bluetooth_obex_server_reject_connection());
	}

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	/* Unset the transfer callbacks */
	_bt_unset_cb(BT_EVENT_OPP_SERVER_TRANSFER_PROGRESS);
	_bt_unset_cb(BT_EVENT_OPP_SERVER_TRANSFER_FINISHED);

	return error_code;
}

int bt_opp_server_set_destination(const char *destination)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_OPP_SERVER_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_OPP_SERVER_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(destination);

	error_code = _bt_get_error_code(bluetooth_obex_server_set_destination_path(destination));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_opp_server_cancel_transfer(int transfer_id)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_OPP_SERVER_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_OPP_SERVER_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_obex_server_cancel_transfer(transfer_id));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

