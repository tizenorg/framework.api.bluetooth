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
#include "bluetooth_internal.h"
#include "bluetooth_private.h"

#ifdef TIZEN_PBAP_DISABLE
#define BT_CHECK_PBAP_SUPPORT() \
		{ \
			BT_CHECK_BT_SUPPORT(); \
			LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
			return BT_ERROR_NOT_SUPPORTED; \
		}
#else
#define BT_CHECK_PBAP_SUPPORT()
#endif

int bt_pbap_init(void)
{
	BT_CHECK_PBAP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	int error_code = BT_ERROR_NONE;
	error_code = _bt_get_error_code(bluetooth_pbap_init());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_pbap_deinit(void)
{
	BT_CHECK_PBAP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	int error_code = BT_ERROR_NONE;
	error_code = _bt_get_error_code(bluetooth_pbap_deinit());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_pbap_connect(const char *address, bt_pbap_enabled_cb callback, void *user_data)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_PBAP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_convert_address_to_hex(&addr_hex, address);
	_bt_set_cb(BT_EVENT_PBAP_CONNECTION_STATUS, callback, user_data);

	error_code = _bt_get_error_code(bluetooth_pbap_connect(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
		_bt_unset_cb(BT_EVENT_PBAP_CONNECTION_STATUS);
	}

	return error_code;
}

int bt_pbap_disconnect(const char *address, bt_pbap_enabled_cb callback, void *user_data)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_PBAP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_convert_address_to_hex(&addr_hex, address);
	_bt_set_cb(BT_EVENT_PBAP_CONNECTION_STATUS, callback, user_data);

	error_code = _bt_get_error_code(bluetooth_pbap_disconnect(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
		_bt_unset_cb(BT_EVENT_PBAP_CONNECTION_STATUS);
	}

	return error_code;
}

int bt_pbap_get_phonebook_size(const char *address, bt_pbap_addressbook_source_e source,
		bt_pbap_folder_type_e type, bt_pbap_phonebook_size_cb callback, void *user_data)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	bt_pbap_folder_t folder = { 0, };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_PBAP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	BT_CHECK_INPUT_PARAMETER(callback);

	folder.addressbook = source;
	folder.folder_type = type;
	_bt_convert_address_to_hex(&addr_hex, address);
	_bt_set_cb(BT_EVENT_PBAP_PHONEBOOK_SIZE, callback, user_data);
	error_code = _bt_get_error_code(bluetooth_pbap_get_phonebook_size(&addr_hex, &folder));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
		_bt_unset_cb(BT_EVENT_PBAP_PHONEBOOK_SIZE);
	}

	return error_code;
}

int bt_pbap_get_phonebook(const char *address, bt_pbap_addressbook_source_e source,
		bt_pbap_folder_type_e type, bt_pbap_filter_vcard_format_e format,
		bt_pbap_filter_sort_order_e order, unsigned short offset,
		unsigned short maxlistcount, long long unsigned fields,
		bt_pbap_phonebook_pull_cb callback, void *user_data)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	bt_pbap_pull_parameters_t app_param = { 0, };
	bt_pbap_folder_t folder = { 0, };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_PBAP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	BT_CHECK_INPUT_PARAMETER(callback);

	/* To get size of phonebook, a separate API is provided
	 * Hence, passing maxlistcount as 0 is restricted. */
	if (maxlistcount <= 0)
		return BT_ERROR_INVALID_PARAMETER;

	/* Maximum value of maxlistcount is 65535 */
	if (maxlistcount > 65535)
		maxlistcount = 65535;

	app_param.format = format;
	app_param.order = order;
	app_param.offset = offset;
	app_param.maxlist = maxlistcount;
	app_param.fields = fields;

	folder.addressbook = source;
	folder.folder_type = type;

	_bt_convert_address_to_hex(&addr_hex, address);
	_bt_set_cb(BT_EVENT_PBAP_PHONEBOOK_PULL, callback, user_data);
	error_code = _bt_get_error_code(bluetooth_pbap_get_phonebook(&addr_hex, &folder, &app_param));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
		_bt_unset_cb(BT_EVENT_PBAP_PHONEBOOK_PULL);
	}

	return error_code;
}

int bt_pbap_get_list(const char *address, bt_pbap_addressbook_source_e source,
		bt_pbap_folder_type_e type, bt_pbap_filter_sort_order_e order,
		unsigned short offset, unsigned short maxlistcount,
		bt_pbap_list_vcards_cb callback, void *user_data)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	bt_pbap_list_parameters_t app_param = { 0, };
	bt_pbap_folder_t folder = { 0, };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_PBAP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_convert_address_to_hex(&addr_hex, address);
	app_param.order = order;
	app_param.offset = offset;
	app_param.maxlist = maxlistcount;

	folder.addressbook = source;
	folder.folder_type = type;

	_bt_set_cb(BT_EVENT_PBAP_VCARD_LIST, callback, user_data);
	error_code = _bt_get_error_code(bluetooth_pbap_get_list(&addr_hex, &folder, &app_param));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
		_bt_unset_cb(BT_EVENT_PBAP_VCARD_LIST);
	}

	return error_code;
}

int bt_pbap_pull_vcard(const char *address, bt_pbap_addressbook_source_e source,
		bt_pbap_folder_type_e type, int index,
		bt_pbap_filter_vcard_format_e format, long long unsigned fields,
		bt_pbap_get_vcard_cb callback, void *user_data)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	bt_pbap_pull_vcard_parameters_t app_param = { 0, };
	bt_pbap_folder_t folder = { 0, };

	int error_code = BT_ERROR_NONE;

	BT_CHECK_PBAP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_convert_address_to_hex(&addr_hex, address);
	app_param.format = format;
	app_param.fields = fields;
	app_param.index = index;

	folder.addressbook = source;
	folder.folder_type = type;

	_bt_set_cb(BT_EVENT_PBAP_VCARD_PULL, callback, user_data);
	error_code = _bt_get_error_code(bluetooth_pbap_pull_vcard(&addr_hex, &folder, &app_param));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
		_bt_unset_cb(BT_EVENT_PBAP_VCARD_PULL);
	}

	return error_code;
}

int bt_pbap_phonebook_search(const char *address,
		bt_pbap_addressbook_source_e source, bt_pbap_folder_type_e type,
		bt_pbap_search_field_e search_attribute, const char *search_value,
		bt_pbap_filter_sort_order_e order,
		unsigned short offset, unsigned short maxlistcount,
		bt_pbap_search_list_cb callback, void *user_data)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	bt_pbap_folder_t folder = { 0, };
	bt_pbap_search_parameters_t app_param = { 0, };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_PBAP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_convert_address_to_hex(&addr_hex, address);
	folder.addressbook = source;
	folder.folder_type = type;

	app_param.order = order;
	app_param.offset = offset;
	app_param.maxlist = maxlistcount;
	app_param.search_attribute = search_attribute;
	strncpy(app_param.search_value, search_value,
			BLUETOOTH_PBAP_MAX_SEARCH_VALUE_LENGTH -1 );

	_bt_set_cb(BT_EVENT_PBAP_PHONEBOOK_SEARCH, callback, user_data);
	error_code = _bt_get_error_code(bluetooth_pbap_phonebook_search(&addr_hex, &folder, &app_param));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
		_bt_unset_cb(BT_EVENT_PBAP_PHONEBOOK_SEARCH);
	}

	return error_code;
}
