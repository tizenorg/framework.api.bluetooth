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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dlog.h>
#include <stdio.h>
#include <stdbool.h>
#include <bluetooth-api.h>

#include "bluetooth.h"
#include "bluetooth_private.h"

int bt_adapter_enable(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_enable_adapter());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_disable(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_disable_adapter());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_reset(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_reset_adapter());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_get_state(bt_adapter_state_e *adapter_state)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(adapter_state);

	*adapter_state = bluetooth_check_adapter();
	return BT_ERROR_NONE;
}

int bt_adapter_get_address(char **address)
{
	bluetooth_device_address_t loc_address = { {0} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);

	error_code = _bt_get_error_code(bluetooth_get_local_address(&loc_address));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
		return error_code;
	}

	error_code = _bt_convert_address_to_string(address, &loc_address);
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
		return error_code;
	}

	return BT_ERROR_NONE;
}

int bt_adapter_get_name(char **name)
{
	int ret = BT_ERROR_NONE;
	bluetooth_device_name_t loc_name = { {0} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(name);

	ret = _bt_get_error_code(bluetooth_get_local_name(&loc_name));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	*name = strdup(loc_name.name);
	if (*name == NULL) {
		BT_ERR("OUT_OF_MEMORY(0x%08x)", BT_ERROR_OUT_OF_MEMORY);
		return BT_ERROR_OUT_OF_MEMORY;
	}

	return BT_ERROR_NONE;
}

int bt_adapter_set_name(const char *name)
{
	bluetooth_device_name_t loc_name = { {0} };
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(name);

	strncpy(loc_name.name, name, BLUETOOTH_DEVICE_NAME_LENGTH_MAX);
	loc_name.name[BLUETOOTH_DEVICE_NAME_LENGTH_MAX] = '\0';

	ret = _bt_get_error_code(bluetooth_set_local_name(&loc_name));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_get_visibility(bt_adapter_visibility_mode_e *mode, int *duration)
{
	bluetooth_discoverable_mode_t discoverable_mode = BLUETOOTH_DISCOVERABLE_MODE_CONNECTABLE;
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(mode);

	ret = _bt_get_error_code(bluetooth_get_discoverable_mode(&discoverable_mode));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	*mode = _bt_get_bt_visibility_mode_e(discoverable_mode);

	if (duration)
		*duration = 0;

	if (discoverable_mode == BLUETOOTH_DISCOVERABLE_MODE_TIME_LIMITED_DISCOVERABLE) {
		if (duration == NULL)
			return BT_ERROR_NONE;
		ret = bluetooth_get_timeout_value(duration);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		}
	}

	return BT_ERROR_NONE;
}

int bt_adapter_set_visibility(bt_adapter_visibility_mode_e visibility_mode, int timeout_sec)
{
	bluetooth_discoverable_mode_t discoverable_mode = BLUETOOTH_DISCOVERABLE_MODE_CONNECTABLE;
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	switch (visibility_mode) {
	case BT_ADAPTER_VISIBILITY_MODE_LIMITED_DISCOVERABLE:
		discoverable_mode = BLUETOOTH_DISCOVERABLE_MODE_TIME_LIMITED_DISCOVERABLE;
		break;
	case BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE:
		discoverable_mode = BLUETOOTH_DISCOVERABLE_MODE_CONNECTABLE;
		timeout_sec = 0;
		break;
	case BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE:
		discoverable_mode = BLUETOOTH_DISCOVERABLE_MODE_GENERAL_DISCOVERABLE;
		timeout_sec = 0;
		break;
	default:
		BT_ERR("INVALID_PARAMETER(0x%08x)", BT_ERROR_INVALID_PARAMETER);
		return BT_ERROR_INVALID_PARAMETER;
	}

	error_code = _bt_get_error_code(bluetooth_set_discoverable_mode(discoverable_mode, timeout_sec));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_set_connectable_changed_cb(bt_adapter_connectable_changed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_CONNECTABLE_CHANGED_EVENT, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_unset_connectable_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_CONNECTABLE_CHANGED_EVENT);
	return BT_ERROR_NONE;
}

int bt_adapter_get_connectable(bool *connectable)
{
	gboolean is_connectable = FALSE;
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(connectable);

	ret = _bt_get_error_code(bluetooth_is_connectable(&is_connectable));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	*connectable = is_connectable ? true : false;

	return BT_ERROR_NONE;
}

int bt_adapter_set_connectable(bool connectable)
{
	int error_code = BT_ERROR_NONE;

	error_code = _bt_get_error_code(bluetooth_set_connectable(connectable));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_foreach_bonded_device(bt_adapter_bonded_device_cb foreach_cb, void *user_data)
{
	GPtrArray *dev_list = NULL;
	bt_device_info_s *dev_info = NULL;
	bluetooth_device_info_t *ptr = NULL;
	int ret = BT_ERROR_NONE;
	int i = 0;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(foreach_cb);

	dev_list = g_ptr_array_new();
	if (dev_list == NULL) {
		BT_ERR("OUT_OF_MEMORY(0x%08x)", BT_ERROR_OUT_OF_MEMORY);
		return BT_ERROR_OUT_OF_MEMORY;
	}

	ret = _bt_get_error_code(bluetooth_get_bonded_device_list(&dev_list));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to get bonded device list", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	for (i = 0; i < dev_list->len; i++) {
		ptr = g_ptr_array_index(dev_list, i);
		if (ptr != NULL) {
			ret = _bt_get_bt_device_info_s(&dev_info, (bluetooth_device_info_t *)ptr);
			if (ret != BT_ERROR_NONE) {
				BT_ERR("%s(0x%08x) : Failed to get device info", _bt_convert_error_to_string(ret), ret);
				break;
			}

			if (!foreach_cb(dev_info, user_data)) {
				_bt_free_bt_device_info_s(dev_info);
				break;
			}
			_bt_free_bt_device_info_s(dev_info);
		} else {
			BT_ERR("OPERATION_FAILED(0x%08x)", BT_ERROR_OPERATION_FAILED);
			ret = BT_ERROR_OPERATION_FAILED;
			break;
		}
	}

	if (dev_list != NULL) {
		g_ptr_array_foreach(dev_list, (GFunc)g_free, NULL);
		g_ptr_array_free(dev_list, TRUE);
	}

	return ret;
}

int bt_adapter_get_bonded_device_info(const char *remote_address, bt_device_info_s **device_info)
{
	int ret;
	bluetooth_device_address_t addr_hex = { {0,} };
	bluetooth_device_info_t *info;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	info = (bluetooth_device_info_t *)malloc(sizeof(bluetooth_device_info_t));
	if (info == NULL)
		return BT_ERROR_OUT_OF_MEMORY;

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	ret = _bt_get_error_code(bluetooth_get_bonded_device(&addr_hex, info));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to run function",
					_bt_convert_error_to_string(ret), ret);
	} else {
		ret = _bt_get_bt_device_info_s(device_info, info);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x) : Failed to get device info",
						_bt_convert_error_to_string(ret),
						ret);
		}
	}

	free(info);

	return ret;
}

int bt_adapter_free_device_info(bt_device_info_s *device_info)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_info);

	_bt_free_bt_device_info_s(device_info);

	return BT_ERROR_NONE;
}

int bt_adapter_is_service_used(const char *service_uuid, bool *used)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(service_uuid);
	BT_CHECK_INPUT_PARAMETER(used);

	if (strcmp(service_uuid, OPP_UUID) == 0) {
		*used = bluetooth_obex_server_is_activated();
	} else {
		ret = _bt_get_error_code(bluetooth_is_service_used(service_uuid,
							(gboolean *)used));
	}

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to run function", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_state_changed_cb(bt_adapter_state_changed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_STATE_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_name_changed_cb(bt_adapter_name_changed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_NAME_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_visibility_mode_changed_cb(bt_adapter_visibility_mode_changed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_VISIBILITY_MODE_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_device_discovery_state_changed_cb(bt_adapter_device_discovery_state_changed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_unset_state_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_STATE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_unset_name_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_NAME_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_unset_visibility_mode_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_VISIBILITY_MODE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_set_visibility_duration_changed_cb(bt_adapter_visibility_duration_changed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();

	/* Will impelement it */

	return BT_ERROR_NONE;
}

int bt_adapter_unset_visibility_duration_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();

	/* Will impelement it */

	return BT_ERROR_NONE;
}

int bt_adapter_unset_device_discovery_state_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_start_device_discovery(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_start_discovery(0, 0, BLUETOOTH_DEVICE_MAJOR_MASK_MISC));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_start_discover_devices(bt_adapter_discover_devices_type_e type)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_start_custom_discovery(type,
					0, 0, BLUETOOTH_DEVICE_MAJOR_MASK_MISC));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_stop_device_discovery(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_cancel_discovery());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_is_discovering(bool *is_discovering)
{
	int ret = 0;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(is_discovering);

	ret = bluetooth_is_discovering();
	if (ret >= BLUETOOTH_ERROR_BASE) {
		*is_discovering = (ret == 1) ? true : false;
		return BT_ERROR_NONE;
	} else {
		ret = _bt_get_error_code(ret);
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}
}

int bt_adapter_get_local_oob_data(unsigned char **hash, unsigned char **randomizer,
					int *hash_len, int *randomizer_len)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(hash);
	BT_CHECK_INPUT_PARAMETER(randomizer);
	BT_CHECK_INPUT_PARAMETER(hash_len);
	BT_CHECK_INPUT_PARAMETER(randomizer_len);

	bt_oob_data_t oob_data;

	ret = _bt_get_error_code(bluetooth_oob_read_local_data(&oob_data));
	if (BT_ERROR_NONE == ret) {
		*hash = g_memdup(oob_data.hash, BLUETOOTH_OOB_DATA_LENGTH);
		*randomizer = g_memdup(oob_data.randomizer,
						BLUETOOTH_OOB_DATA_LENGTH);
		*hash_len = BLUETOOTH_OOB_DATA_LENGTH;
		*randomizer_len = BLUETOOTH_OOB_DATA_LENGTH;
	} else {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_adapter_set_remote_oob_data(const char *remote_address,
				unsigned char *hash, unsigned char *randomizer,
				int hash_len, int randomizer_len)
{
	int ret = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };
	bt_oob_data_t oob_data = { {0},};

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	if (hash != NULL && randomizer != NULL) {
		memcpy(oob_data.hash, hash, hash_len);
		memcpy(oob_data.randomizer, randomizer, randomizer_len);
		oob_data.hash_len = hash_len;
		oob_data.randomizer_len = randomizer_len;
	}

	ret = _bt_get_error_code(bluetooth_oob_add_remote_data(&addr_hex, &oob_data));
	if (BT_ERROR_NONE != ret) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_adapter_remove_remote_oob_data(const char *remote_address)
{
	int ret = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	ret = _bt_get_error_code(bluetooth_oob_remove_remote_data(&addr_hex));
	if (BT_ERROR_NONE != ret) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_adapter_set_advertising_state_changed_cb(bt_adapter_advertising_state_changed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_ADVERTISING_STATE_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_unset_advertising_state_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_ADVERTISING_STATE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_start_advertising(bt_adapter_advertising_params_s *adv_params)
{
	BT_DBG("+");
	int org_error_code = BLUETOOTH_ERROR_NONE;
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	if (adv_params == NULL) {
		org_error_code = bluetooth_set_advertising(TRUE);
	} else {
		org_error_code = bluetooth_set_custom_advertising(TRUE,
				adv_params->interval_min,
				adv_params->interval_max,
				adv_params->filter_policy);
	}
	BT_DBG("org error code : 0x%04X", org_error_code);

	error_code = _bt_get_error_code(org_error_code);
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	BT_DBG("-");
	return error_code;
}

int bt_adapter_stop_advertising(void)
{
	BT_DBG("+");
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_set_advertising(FALSE));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	BT_DBG("-");
	return error_code;
}

int bt_adapter_is_advertising(bool *is_advertising)
{
	int ret;
	gboolean advertising = FALSE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(is_advertising);

	ret = bluetooth_is_advertising(&advertising);
	if (ret >= BLUETOOTH_ERROR_BASE) {
		*is_advertising = advertising ? true : false;
		return BT_ERROR_NONE;
	} else {
		ret = _bt_get_error_code(ret);
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}
}

int bt_adapter_add_white_list(const char *address)
{
	int error_code = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	_bt_convert_address_to_hex(&addr_hex, address);

	error_code = _bt_get_error_code(bluetooth_add_white_list(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_remove_white_list(const char *address)
{
	int error_code = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	_bt_convert_address_to_hex(&addr_hex, address);

	error_code = _bt_get_error_code(bluetooth_remove_white_list(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_clear_white_list(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_clear_white_list());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

static int __bt_get_ad_data_by_type(char *in_data, int in_len,
		char in_type, char **data, int *data_len)
{
	if (in_data == NULL || data == NULL || data_len == NULL)
		return BT_ERROR_OPERATION_FAILED;

	if (in_len < 0)
		return BT_ERROR_OPERATION_FAILED;

	int i;
	int len = 0;
	int type = 0;

	for (i = 0; i < in_len; i++) {
		len = in_data[i];
		if (len <= 0 || i + 1 >= in_len) {
			BT_ERR("Invalid advertising data");
			return BT_ERROR_OPERATION_FAILED;
		}

		type = in_data[i + 1];
		if (type == in_type) {
			i = i + 2;
			len--;
			break;
		}

		i += len;
		len = 0;
	}

	if (i + len > in_len) {
		BT_ERR("Invalid advertising data");
		return BT_ERROR_OPERATION_FAILED;
	} else if (len == 0) {
		BT_DBG("AD Type 0x%02x data is not set", in_type);
		*data = NULL;
		*data_len = 0;
		return BT_ERROR_NONE;
	}

	*data = g_memdup(&in_data[i], len);
	if (*data == NULL)
		return BT_ERROR_OUT_OF_MEMORY;
	*data_len = len;

	return BT_ERROR_NONE;
}


int bt_adapter_get_advertising_manufacturer_data(char **data, int *len)
{
	bluetooth_advertising_data_t adv = { {0} };
	int adv_len = 0;
	int ret;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(data);
	BT_CHECK_INPUT_PARAMETER(len);

	ret = _bt_get_error_code(bluetooth_get_advertising_data(&adv, &adv_len));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	ret = __bt_get_ad_data_by_type((char *)adv.data, adv_len, 0xff,
			data, len);
	if (ret != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);

	return ret;
}

int bt_adapter_set_advertising_manufacturer_data(char *data, int len)
{
	int ret;
	bluetooth_advertising_data_t adv = { {0} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(data);

	if (len + 2 > sizeof(adv.data) || len < 0)
		return BT_ERROR_INVALID_PARAMETER;

	adv.data[0] = len + 1;
	adv.data[1] = 0xFF;

	memcpy(&adv.data[2], data, len);

	ret = _bt_get_error_code(bluetooth_set_advertising_data(&adv, len + 2));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_advertising_manufacturer_data_changed_cb(
		bt_adapter_advertising_manufacturer_data_changed_cb callback,
		void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_ADVERTISING_MANUFACTURER_DATA_CHANGED,
			callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_unset_advertising_manufacturer_data_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_ADVERTISING_MANUFACTURER_DATA_CHANGED);
	return BT_ERROR_NONE;
}
