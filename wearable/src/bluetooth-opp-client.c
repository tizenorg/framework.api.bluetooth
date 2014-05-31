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

GList *sending_files;

char** __bt_opp_get_file_array(GList *file_list)
{
	char **files = NULL;
	char *c_file = NULL;
	int file_num = 0;
	int i = 0;

	if (file_list == NULL)
		return NULL;

	file_num = g_list_length(file_list);

	if (file_num == 0)
		return NULL;

	files = (char **)calloc(file_num + 1, sizeof(char *));
	if (!files)
		return NULL;

	for (i = 0; i < file_num; i++) {
		c_file = (char *)g_list_nth_data(file_list, i);

		if (c_file == NULL)
			continue;

		files[i] = c_file;
	}

	for (i = 0; i < file_num; i++)
		BT_DBG("file: %s", files[i]);

	return files;
}

int bt_opp_client_initialize(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_opc_init());

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_opp_client_deinitialize(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_opc_deinit());

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	bt_opp_client_clear_files();

	return error_code;
}

int bt_opp_client_add_file(const char *file)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(file);

	if (access(file, F_OK) == 0) {
		sending_files = g_list_append(sending_files, strdup(file));
	} else {
		error_code = BT_ERROR_INVALID_PARAMETER;
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

int bt_opp_client_clear_files(void)
{
	int i = 0;
	int file_num = 0;
	char *c_file = NULL;

	BT_CHECK_INIT_STATUS();

	if (sending_files) {
		file_num = g_list_length(sending_files);

		for (i = 0; i < file_num; i++) {
			c_file = (char *)g_list_nth_data(sending_files, i);

			if (c_file == NULL)
				continue;

			free(c_file);
		}

		g_list_free(sending_files);
		sending_files = NULL;
	}

	return BT_ERROR_NONE;
}

int bt_opp_client_push_files(const char *remote_address,
			bt_opp_client_push_responded_cb responded_cb,
			bt_opp_client_push_progress_cb progress_cb,
			bt_opp_client_push_finished_cb finished_cb,
			void *user_data)
{
	int error_code = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };
	char **files = NULL;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	files = __bt_opp_get_file_array(sending_files);

	error_code = _bt_get_error_code(bluetooth_opc_push_files(&addr_hex, files));

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	} else {
		_bt_set_cb(BT_EVENT_OPP_CLIENT_PUSH_RESPONSED, responded_cb, user_data);
		_bt_set_cb(BT_EVENT_OPP_CLIENT_PUSH_PROGRESS, progress_cb, user_data);
		_bt_set_cb(BT_EVENT_OPP_CLIENT_PUSH_FINISHED, finished_cb, user_data);
	}

	bt_opp_client_clear_files();

	if (files)
		free(files);

	return error_code;
}

int bt_opp_client_cancel_push(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_opc_cancel_push());

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
				error_code);
	}

	return error_code;
}

