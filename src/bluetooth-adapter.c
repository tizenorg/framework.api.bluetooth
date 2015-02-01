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
#ifdef TIZEN_WEARABLE
#include <privacy_checker_client.h>
#endif
#include <bluetooth-api.h>

#include "bluetooth.h"
#include "bluetooth_private.h"

int bt_adapter_enable(void)
{
	int error_code = BT_ERROR_NONE;
#ifdef TIZEN_WEARABLE
	static const char* PRIVILEGE_ID_BLUETOOTH_ADMIN = "http://tizen.org/privilege/bluetooth.admin";

	error_code =  privacy_checker_check_by_privilege(PRIVILEGE_ID_BLUETOOTH_ADMIN);
	if (error_code != PRIV_MGR_ERROR_SUCCESS) {
		privacy_checker_finalize();
		BT_ERR("PERMISSION_DENIED(0x%08x)", error_code);
		return BT_ERROR_PERMISSION_DENIED;
	}
	privacy_checker_finalize();
#endif

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

int bt_adapter_recover(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_recover_adapter());
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

int bt_adapter_le_enable(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_enable_adapter_le());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_le_disable(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_disable_adapter_le());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_le_get_state(bt_adapter_le_state_e *adapter_le_state)
{
	BT_CHECK_INPUT_PARAMETER(adapter_le_state);

	*adapter_le_state = bluetooth_check_adapter_le();
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

int bt_adapter_get_version(char **version)
{
	int ret = BT_ERROR_NONE;
	bluetooth_version_t loc_ver = { { 0 } };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(version);

	ret = _bt_get_error_code(bluetooth_get_local_version(&loc_ver));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	*version = strdup(loc_ver.version);
	if (*version == NULL) {
		BT_ERR("OUT_OF_MEMORY(0x%08x)", BT_ERROR_OUT_OF_MEMORY);
		return BT_ERROR_OUT_OF_MEMORY;
	}

	return BT_ERROR_NONE;
}

#define BT_ADAPTER_FIRMWARE_INFO_FILE_PATH "/var/lib/bluetooth/bcmtool_log"
#define BT_ADAPTER_STACK_INFO_FILE_PATH "/usr/etc/bluetooth/stack_info"

int bt_adapter_get_local_info(char **chipset, char **firmware, char **stack_version, char **profiles)
{
	int ret = BT_ERROR_NONE;
	FILE *fp = NULL;
	char *buf = NULL;
	long lsize;
	size_t result;
	char *info_start = NULL;
	char *info_end = NULL;
	long info_size;
	char *local_chipset = NULL;
	char *local_firmware = NULL;
	char *local_stack_version = NULL;
	char *local_profiles = NULL;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(chipset);
	BT_CHECK_INPUT_PARAMETER(firmware);
	BT_CHECK_INPUT_PARAMETER(stack_version);
	BT_CHECK_INPUT_PARAMETER(profiles);

	if ((fp = fopen(BT_ADAPTER_FIRMWARE_INFO_FILE_PATH, "r")) == NULL) {
		BT_ERR("fopen() is failed(%s)", BT_ADAPTER_FIRMWARE_INFO_FILE_PATH);

		local_firmware = (char *)malloc(sizeof(char) * 1);
		if (local_firmware == NULL) {
			ret = BT_ERROR_OUT_OF_MEMORY;
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			goto ERROR;
		}
		local_firmware[0] = '\0';
		local_chipset = (char *)malloc(sizeof(char) * 1);
		if (local_chipset == NULL) {
			ret = BT_ERROR_OUT_OF_MEMORY;
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			goto ERROR;
		}
		local_chipset[0] = '\0';
	} else {
		fseek(fp, 0, SEEK_END);
		lsize = ftell(fp);
		if (lsize < 0) {
			ret = BT_ERROR_OPERATION_FAILED;
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			goto ERROR;
		}
		buf = (char *)malloc(sizeof(char) * (lsize + 1));
		if (buf == NULL) {
			ret = BT_ERROR_OUT_OF_MEMORY;
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			goto ERROR;
		}
		fseek(fp, 0, SEEK_SET);
		result = fread(buf, 1, lsize, fp);
		buf[lsize] = '\0';

		if (result != lsize)
			BT_ERR("fread() error");

		info_start = strstr(buf, "bluetooth");
		if (info_start == NULL) {
			ret = BT_ERROR_OPERATION_FAILED;
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			goto ERROR;
		}
		info_start += 10;
		info_end = strstr(buf, "hcd");
		if (info_end == NULL) {
			ret = BT_ERROR_OPERATION_FAILED;
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			goto ERROR;
		}
		info_size = info_end - info_start - 1;

		local_firmware = (char *)malloc(sizeof(char) * (info_size + 1));
		if (local_firmware == NULL) {
			ret = BT_ERROR_OUT_OF_MEMORY;
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			goto ERROR;
		}

		strncpy(local_firmware, info_start, info_size);
		local_firmware[info_size] = '\0';

		info_end = strchr(buf, '_');
		if (info_end == NULL) {
			ret = BT_ERROR_OPERATION_FAILED;
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			goto ERROR;
		}
		info_size = info_end - info_start;

		local_chipset = (char *)malloc(sizeof(char) * (info_size + 1));
		if (local_chipset == NULL) {
			ret = BT_ERROR_OUT_OF_MEMORY;
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			goto ERROR;
		}

		strncpy(local_chipset, info_start, info_size);
		local_chipset[info_size] = '\0';

		if (buf)
			free(buf);
		buf = NULL;
		fclose(fp);
	}

	if ((fp = fopen(BT_ADAPTER_STACK_INFO_FILE_PATH, "r")) == NULL) {
		ret = BT_ERROR_OPERATION_FAILED;
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		goto ERROR;
	}

	fseek(fp, 0, SEEK_END);
	lsize = ftell(fp);
	if (lsize < 0) {
		ret = BT_ERROR_OPERATION_FAILED;
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		goto ERROR;
	}
	buf = (char *)malloc(sizeof(char) * (lsize + 1));
	if (buf == NULL) {
		ret = BT_ERROR_OUT_OF_MEMORY;
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		goto ERROR;
	}
	fseek(fp, 0, SEEK_SET);
	result = fread(buf, 1, lsize, fp);
	buf[lsize] = '\0';

	if (result != lsize)
		BT_ERR("fread() error");

	info_start = buf;
	info_end = strchr(buf, ',');
	if (info_end == NULL) {
		ret = BT_ERROR_OPERATION_FAILED;
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		goto ERROR;
	}
	info_size = info_end - info_start;

	local_stack_version = (char *)malloc(sizeof(char) * (info_size + 1));
	if (local_stack_version == NULL) {
		ret = BT_ERROR_OUT_OF_MEMORY;
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		goto ERROR;
	}

	strncpy(local_stack_version, info_start, info_size);
	local_stack_version[info_size] = '\0';

	info_start = info_end + 2;
	info_size = lsize - info_size - 3;

	local_profiles = (char *)malloc(sizeof(char) * (info_size + 1));
	if (local_profiles == NULL) {
		ret = BT_ERROR_OUT_OF_MEMORY;
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		goto ERROR;
	}

	strncpy(local_profiles, info_start, info_size);
	local_profiles[info_size] = '\0';

	if (buf)
		free(buf);
	fclose(fp);

	*chipset = local_chipset;
	*firmware = local_firmware;
	*stack_version = local_stack_version;
	*profiles = local_profiles;

	return BT_ERROR_NONE;

ERROR:
	if (local_chipset)
		free(local_chipset);
	if (local_firmware)
		free(local_firmware);
	if (local_stack_version)
		free(local_stack_version);
	if (local_profiles)
		free(local_profiles);
	if (buf)
		free(buf);
	if (fp)
		fclose(fp);
	return ret;
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

	BT_CHECK_INIT_STATUS();

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
	gboolean is_used = FALSE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(service_uuid);
	BT_CHECK_INPUT_PARAMETER(used);

	if (strcmp(service_uuid, OPP_UUID) == 0) {
		*used = bluetooth_obex_server_is_activated();
	} else {
		ret = _bt_get_error_code(bluetooth_is_service_used(service_uuid,
							&is_used));
		*used = is_used ? true : false;
	}

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to run function", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_state_changed_cb(bt_adapter_state_changed_cb callback, void *user_data)
{
	BT_DBG("");
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_STATE_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_le_set_state_changed_cb(bt_adapter_le_state_changed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_le_adapter_init();
	_bt_set_cb(BT_EVENT_LE_STATE_CHANGED, callback, user_data);
	return BT_ERROR_NONE;
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

int bt_adapter_le_set_device_discovery_state_changed_cb(bt_adapter_le_device_discovery_state_changed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_le_adapter_init();
	_bt_set_cb(BT_EVENT_LE_DEVICE_DISCOVERY_STATE_CHANGED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_adapter_unset_state_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_STATE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_le_unset_state_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_LE_STATE_CHANGED);
	_bt_le_adapter_deinit();
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
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_VISIBILITY_DURATION_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_unset_visibility_duration_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_VISIBILITY_DURATION_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_unset_device_discovery_state_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_le_unset_device_discovery_state_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_LE_DEVICE_DISCOVERY_STATE_CHANGED);
	_bt_le_adapter_deinit();
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

int bt_adapter_le_start_device_discovery(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_start_le_discovery());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_le_stop_device_discovery(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_stop_le_discovery());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_le_is_discovering(bool *is_discovering)
{
	int ret = 0;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(is_discovering);

	ret = bluetooth_is_le_discovering();
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

int bt_adapter_le_add_white_list(const char *address, bt_device_address_type_e address_type)
{
	int error_code = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	_bt_convert_address_to_hex(&addr_hex, address);

	error_code = _bt_get_error_code(bluetooth_add_white_list(&addr_hex, address_type));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_le_remove_white_list(const char *address, bt_device_address_type_e address_type)
{
	int error_code = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	_bt_convert_address_to_hex(&addr_hex, address);

	error_code = _bt_get_error_code(bluetooth_remove_white_list(&addr_hex, address_type));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_le_clear_white_list(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_clear_white_list());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_le_create_advertiser(bt_advertiser_h *advertiser)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(advertiser);

	*advertiser = (bt_advertiser_h)g_malloc0(sizeof(bt_advertiser_s));
	if (*advertiser == NULL) {
		ret = BT_ERROR_OUT_OF_MEMORY;
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_le_destroy_advertiser(bt_advertiser_h advertiser)
{
	int ret = BT_ERROR_NONE;
	int error_code;
	bt_advertiser_s *__adv = (bt_advertiser_s *)advertiser;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(advertiser);

	_bt_unset_cb(BT_EVENT_ADVERTISING_STATE_CHANGED);

#if 0
	if (__adv->handle) {
		error_code = bluetooth_stop_advertising(__adv->handle);
		ret = _bt_get_error_code(error_code);
		if (ret != BT_ERROR_NONE)
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		__adv->handle = 0;
	}
#else
	error_code = bluetooth_set_advertising(FALSE);
	ret = _bt_get_error_code(error_code);
	if (ret != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
#endif

	/* Free advertising data */
	if (__adv->adv_data)
		free(__adv->adv_data);
	if (__adv->scan_rsp_data)
		free(__adv->scan_rsp_data);
	free(__adv);

	return BT_ERROR_NONE;
}

static int __bt_remove_ad_data_by_type(char *in_data, unsigned int in_len,
		char in_type, char **data, unsigned int *data_len)
{
	if (in_data == NULL || data == NULL || data_len == NULL)
		return BT_ERROR_OPERATION_FAILED;

	int i;
	int len = 0;
	int type = 0;
	char *p;

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
		BT_INFO("AD Type 0x%02x data is not set", in_type);
		return BT_ERROR_OPERATION_FAILED;
	}

	p = (char *)malloc(sizeof(char) * (in_len - len));
	if (p == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	memcpy(p, in_data, sizeof(char) * i);
	memcpy(p + i, in_data + i + len, sizeof(char) * (in_len - len - i));

	*data = p;
	*data_len = in_len - len;

	return BT_ERROR_NONE;
}

int bt_adapter_le_add_advertising_data(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type, bt_adapter_le_packet_data_type_e data_type,
		void *data, unsigned int data_size)
{
	int ret = BT_ERROR_NONE;
	bt_advertiser_s *__adv = (bt_advertiser_s *)advertiser;
	char **p;
	unsigned int *len;
	char *new_p;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(advertiser);

	switch (pkt_type) {
	case BT_ADAPTER_LE_PACKET_ADVERTISING:
		p = &__adv->adv_data;
		len = &__adv->adv_data_len;
		break;

	case BT_ADAPTER_LE_PACKET_SCAN_RESPONSE:
		p = &__adv->scan_rsp_data;
		len = &__adv->scan_rsp_data_len;
		break;

	default:
		BT_ERR("Unknown LE packet type : %d", pkt_type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	/* 2 bytes are required for Length and AD Type */
	if (*len + data_size + 2 > 31) {
		return BT_ERROR_QUOTA_EXCEEDED;
	}

	if (*len == 0)
		*p = NULL;

	new_p = realloc(*p, sizeof(char) * (*len + data_size + 2));
	if (new_p == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	new_p[*len] = data_size + 1;
	new_p[*len + 1] = data_type;
	memcpy(new_p + (*len + 2), data, data_size);

	*p = new_p;
	*len += data_size + 2;

	return ret;
}

int bt_adapter_le_remove_advertising_data(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type, bt_adapter_le_packet_data_type_e data_type)
{
	int ret = BT_ERROR_NONE;
	bt_advertiser_s *__adv = (bt_advertiser_s *)advertiser;
	char **p;
	unsigned int *len;
	char *new_p = NULL;
	unsigned int new_len = 0;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(advertiser);

	switch (pkt_type) {
	case BT_ADAPTER_LE_PACKET_ADVERTISING:
		p = &__adv->adv_data;
		len = &__adv->adv_data_len;
		break;

	case BT_ADAPTER_LE_PACKET_SCAN_RESPONSE:
		p = &__adv->scan_rsp_data;
		len = &__adv->scan_rsp_data_len;
		break;

	default:
		BT_ERR("Unknown LE packet type : %d", pkt_type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	ret = __bt_remove_ad_data_by_type(*p, *len, data_type, &new_p, &new_len);
	if (ret != BT_ERROR_NONE) {
		return ret;
	}

	free(*p);
	*p = new_p;
	*len = new_len;

	return ret;
}

int bt_adapter_le_clear_advertising_data(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type)
{
	int ret = BT_ERROR_NONE;
	bt_advertiser_s *__adv = (bt_advertiser_s *)advertiser;
	char **p;
	unsigned int *len;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(advertiser);

	switch (pkt_type) {
	case BT_ADAPTER_LE_PACKET_ADVERTISING:
		p = &__adv->adv_data;
		len = &__adv->adv_data_len;
		break;

	case BT_ADAPTER_LE_PACKET_SCAN_RESPONSE:
		p = &__adv->scan_rsp_data;
		len = &__adv->scan_rsp_data_len;
		break;

	default:
		BT_ERR("Unknown LE packet type : %d", pkt_type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	if (*p) {
		free(*p);
		*p = NULL;
	}
	*len = 0;

	return ret;
}

int bt_adapter_le_start_advertising(bt_advertiser_h advertiser,
		bt_adapter_le_advertising_params_s *adv_params,
		bt_adapter_le_advertising_state_changed_cb cb, void *user_data)
{
	int ret = BT_ERROR_NONE;
	int error_code;
	bt_advertiser_s *__adv = (bt_advertiser_s *)advertiser;
	bluetooth_advertising_data_t adv = { {0} };
	bluetooth_scan_resp_data_t resp = { {0} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(advertiser);

#if 0
	error_code = bluetooth_start_advertising(__adv, cb, user_data);
	ret = _bt_get_error_code(error_code);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
#endif

	if (__adv->adv_data_len > 0 && __adv->adv_data) {
		memcpy(adv.data, __adv->adv_data, __adv->adv_data_len);
		error_code = bluetooth_set_advertising_data(&adv, __adv->adv_data_len);
		ret = _bt_get_error_code(error_code);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			return ret;
		}
	}

	if (__adv->scan_rsp_data_len > 0 && __adv->scan_rsp_data) {
		memcpy(resp.data, __adv->scan_rsp_data, __adv->scan_rsp_data_len);
		error_code = bluetooth_set_scan_response_data(&resp, __adv->scan_rsp_data_len);
		ret = _bt_get_error_code(error_code);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			return ret;
		}
	}

	if (adv_params == NULL) {
		error_code = bluetooth_set_advertising(TRUE);
	} else {
		error_code = bluetooth_set_custom_advertising(TRUE,
				(bluetooth_advertising_params_t *)adv_params);
	}

	ret = _bt_get_error_code(error_code);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	_bt_set_cb(BT_EVENT_ADVERTISING_STATE_CHANGED, cb, user_data);

	return ret;
}

int bt_adapter_le_stop_advertising(bt_advertiser_h advertiser)
{
	int ret = BT_ERROR_NONE;
//	bt_advertiser_s *__adv = (bt_advertiser_s *)advertiser;

	BT_CHECK_INIT_STATUS();

#if 0
	error_code = bluetooth_stop_advertising(__adv, cb, user_data);
	ret = _bt_get_error_code(error_code);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
#endif

	ret = _bt_get_error_code(bluetooth_set_advertising(FALSE));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_le_enable_privacy(bool enable_privacy)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_enable_le_privacy(enable_privacy));

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

