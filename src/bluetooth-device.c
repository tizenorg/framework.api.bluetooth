/*
 * Copyright (c) 2012-2013 Samsung Electronics Co., Ltd.
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
#include <bluetooth-api.h>

#include "bluetooth.h"
#include "bluetooth_private.h"

int bt_device_create_bond(const char *device_address)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	error_code = _bt_get_error_code(bluetooth_bond_device(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_device_cancel_bonding(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_cancel_bonding());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_device_destroy_bond(const char *device_address)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	error_code = _bt_get_error_code(bluetooth_unbond_device(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_device_set_alias(const char *device_address, const char *alias)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);
	BT_CHECK_INPUT_PARAMETER(alias);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	error_code = _bt_get_error_code(bluetooth_set_alias(&addr_hex, alias));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_device_set_authorization(const char *device_address, bt_device_authorization_e authorization)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	gboolean trusted = FALSE;
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	if (authorization == BT_DEVICE_AUTHORIZED)
		trusted = TRUE;

	error_code = _bt_get_error_code(bluetooth_authorize_device(&addr_hex, trusted));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_device_start_service_search(const char *device_address)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int ret = 0;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	ret = _bt_get_error_code(bluetooth_search_service(&addr_hex));

	/* In service search, BT_ERROR_SERVICE_SEARCH_FAILED is returned instead of BT_ERROR_OPERATION_FAILED. */
	if (ret == BT_ERROR_OPERATION_FAILED)
		ret = BT_ERROR_SERVICE_SEARCH_FAILED;

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_device_cancel_service_search(void)
{
	int ret = 0;

	BT_CHECK_INIT_STATUS();
	ret = _bt_get_error_code(bluetooth_cancel_service_search());
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_device_foreach_connected_profiles(const char *remote_address, bt_device_connected_profile callback, void *user_data)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	gboolean is_connected = FALSE;
	int ret;
	int i = 0;
	int profiles[] = {BT_PROFILE_RFCOMM, BT_PROFILE_A2DP,
			BT_PROFILE_HSP, BT_PROFILE_HID, 0};

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	while (profiles[i] != 0) {
		ret = _bt_get_error_code(bluetooth_is_device_connected(&addr_hex,
								profiles[i],
								&is_connected));
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			return ret;
		}

		if (is_connected == TRUE) {
			if (!callback(profiles[i], user_data))
				break;
		}

		i++;
	}

	return BT_ERROR_NONE;
}

int bt_device_set_bond_created_cb(bt_device_bond_created_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_BOND_CREATED, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_device_set_bond_destroyed_cb(bt_device_bond_destroyed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_BOND_DESTROYED, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_device_set_authorization_changed_cb(bt_device_authorization_changed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AUTHORIZATION_CHANGED, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_device_set_service_searched_cb(bt_device_service_searched_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_SERVICE_SEARCHED, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_device_set_connection_state_changed_cb(bt_device_connection_state_changed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_DEVICE_CONNECTION_STATUS, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_device_unset_bond_created_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_BOND_CREATED);
	return BT_ERROR_NONE;
}

int bt_device_unset_bond_destroyed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_BOND_DESTROYED);
	return BT_ERROR_NONE;
}

int bt_device_unset_authorization_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_AUTHORIZATION_CHANGED);
	return BT_ERROR_NONE;
}

int bt_device_unset_service_searched_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_SERVICE_SEARCHED);
	return BT_ERROR_NONE;
}

int bt_device_unset_connection_state_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_DEVICE_CONNECTION_STATUS);
	return BT_ERROR_NONE;
}

int bt_device_read_rssi_value(const char *address)
{
	int ret;
	bluetooth_device_address_t bd_addr = { {0,} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);

	_bt_convert_address_to_hex(&bd_addr, address);

	ret = _bt_get_error_code(bluetooth_read_rssi(&bd_addr));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}
