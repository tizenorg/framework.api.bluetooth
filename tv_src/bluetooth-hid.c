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
#include <vconf.h>
#include <glib.h>
#include <dlog.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <bluetooth-api.h>
#include <bluetooth-hid-api.h>

#include "bluetooth.h"
#include "bluetooth_product.h"
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

int bt_hid_set_audio_data_receive_cb(bt_hid_audio_data_receive_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	ret = bluetooth_hid_voice_data_path_init(_bt_hid_event_proxy);
	ret = _bt_get_error_code(ret);
        if (ret != BT_ERROR_NONE)
                BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	else
		_bt_set_cb(BT_EVENT_HID_AUDIO_DATA_RECVD , callback, user_data);

        return ret;
}

int bt_hid_unset_audio_data_receive_cb(void)
{
	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_HID_AUDIO_DATA_RECVD);
	bluetooth_hid_voice_data_path_deinit();
	return BT_ERROR_NONE;
}

int bt_hid_set_smartrc_tput_mode_cb(bt_adapter_smartrc_tput_mode_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	BT_DBG ("bt_hid_set_smartrc_tput_mode_cb intialized");
	ret = bluetooth_hid_voice_data_path_init(_bt_hid_event_proxy);
	ret = _bt_get_error_code(ret);
    if (ret != BT_ERROR_NONE)
        BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	else
		_bt_set_cb(BT_EVENT_DUT_MODE , callback, user_data);

	return ret;
}

int bt_hid_unset_smartrc_tput_mode_cb(void)
{
	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_DUT_MODE);
	bluetooth_hid_voice_data_path_deinit();
	return BT_ERROR_NONE;
}

int bt_hid_set_smart_rc_connection_cb(bt_hid_notify_smart_rc_connection_cb callback, void *user_data)
{
	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_HID_SMARTRC_CONNECTION_STATUS, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_hid_unset_smart_rc_connection_cb(void)
{
	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_HID_SMARTRC_CONNECTION_STATUS);
	return BT_ERROR_NONE;
}

int bt_hid_set_sound_key_pressed_cb(bt_hid_notify_sound_key_pressed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_set_cb(BT_EVENT_HID_VOICE_KEY_PRESSED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_hid_unset_sound_key_pressed_cb(void)
{
	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_HID_VOICE_KEY_PRESSED);
	return BT_ERROR_NONE;
}

int bt_hid_rc_stop_sending_voice(const char *remote_address)
{
	int error;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_HID_HOST_SUPPORT();

	/* If remote address is NULL, RC command should be
	   sent to all connected HID devices. So we need to
	   explicitly send NULL as remote address to BT framework */
	if(remote_address != NULL) {
		_bt_convert_address_to_hex(&addr_hex, remote_address);

		error = bluetooth_rc_stop_sending_voice(
                        (hid_device_address_t *)&addr_hex);
	} else {
		error = bluetooth_rc_stop_sending_voice(NULL);
	}

        error = _bt_get_error_code(error);
        if (error != BT_ERROR_NONE) {
                BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
        }
        return error;
}

int bt_hid_send_rc_command(const char *remote_address,
		const unsigned char *data, unsigned short length)
{
	int error;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_HID_HOST_SUPPORT();

	if (NULL == data || 0 == length)
		return BT_ERROR_INVALID_PARAMETER;

	/* If remote address is NULL, RC command should be
	   sent to all connected HID devices. So we need to
	   explicitly send NULL as remote address to BT framework */
	if(remote_address != NULL) {
		_bt_convert_address_to_hex(&addr_hex, remote_address);

		error = bluetooth_send_rc_command(
                        (hid_device_address_t *)&addr_hex,
			data, length);
	} else {
		error = bluetooth_send_rc_command(NULL, data, length);
	}

        error = _bt_get_error_code(error);
        if (error != BT_ERROR_NONE) {
                BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
        }
        return error;
}

int bt_hid_set_battery_status_received_cb(bt_hid_notify_battery_status_received_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_set_cb(BT_EVENT_HID_BATTERY_STATUS_RECEIVED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_hid_unset_battery_status_received_cb(void)
{
	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_HID_BATTERY_STATUS_RECEIVED);
	return BT_ERROR_NONE;
}

int bt_hid_set_mbr_return_receive_cb(bt_hid_mbr_return_receive_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_set_cb(BT_EVENT_HID_MBR_RETURN_RECVD , callback, user_data);
        if (ret != BT_ERROR_NONE) {
                BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
        }

        return ret;
}

int bt_hid_unset_mbr_return_receive_cb(void)
{
	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_HID_MBR_RETURN_RECVD);
	return BT_ERROR_NONE;
}

int bt_hid_set_sensor_data_receive_cb(bt_hid_sensor_data_receive_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	ret = bluetooth_hid_sensor_data_path_init(_bt_hid_event_proxy);
	ret = _bt_get_error_code(ret);
        if (ret != BT_ERROR_NONE)
                BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	else
		_bt_set_cb(BT_EVENT_HID_SENSOR_DATA_RECEIVED, callback, user_data);

        return ret;
}

int bt_hid_unset_sensor_data_receive_cb(void)
{
	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_HID_SENSOR_DATA_RECEIVED);
	bluetooth_hid_sensor_data_path_deinit();
	return BT_ERROR_NONE;
}

int bt_hid_send_device_command(const char *remote_address, unsigned char device_type,
				unsigned char device_cmd)
{
	int error;
        bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_HID_HOST_SUPPORT();

        /* If remote address is NULL, RC command should be
           sent to all connected HID devices. So we need to
           explicitly send NULL as remote address to BT framework */
        if(remote_address != NULL) {
                _bt_convert_address_to_hex(&addr_hex, remote_address);

		error = bluetooth_rc_send_device_cmd(
                        (hid_device_address_t *)&addr_hex,
                        device_type,
                        device_cmd);

	} else {
                error = bluetooth_rc_send_device_cmd(NULL, device_type, device_cmd);
        }

        error = _bt_get_error_code(error);
        if (error != BT_ERROR_NONE) {
                BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
        }
        return error;
}

int bt_hid_send_mbr_command(const char *remote_address, const unsigned char* data,
				unsigned short len)
{
	int error;
        bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_HID_HOST_SUPPORT();

        /* If remote address is NULL, RC command should be
           sent to all connected HID devices. So we need to
           explicitly send NULL as remote address to BT framework */
        if(remote_address != NULL) {
                _bt_convert_address_to_hex(&addr_hex, remote_address);

		error = bluetooth_rc_send_mbr_cmd(
                        (hid_device_address_t *)&addr_hex,
                        data,
                        len);
        } else {
                error = bluetooth_rc_send_mbr_cmd(NULL, data, len);
        }

        error = _bt_get_error_code(error);
        if (error != BT_ERROR_NONE) {
                BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
        }
        return error;
}

int bt_hid_send_mbr_data(const char *remote_address, const unsigned char* data,
				unsigned short len)
{

	int error;
        bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_HID_HOST_SUPPORT();

        /* If remote address is NULL, RC command should be
           sent to all connected HID devices. So we need to
           explicitly send NULL as remote address to BT framework */
        if(remote_address != NULL) {
                _bt_convert_address_to_hex(&addr_hex, remote_address);

		error = bluetooth_rc_send_mbr_data(
                        (hid_device_address_t *)&addr_hex,
                        data,
                        len);
        } else {
                error = bluetooth_rc_send_mbr_data(NULL, data, len);
        }

        error = _bt_get_error_code(error);
        if (error != BT_ERROR_NONE) {
                BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
        }
        return error;
}


int bt_hid_send_haptic_command(const unsigned char data, bool force, int voltage)
{
	int error;
	unsigned char input_data[7] = {0, };

	BT_CHECK_HID_HOST_SUPPORT();

	input_data[0] = 0x06; /* Haptic data */
	input_data[1] = data;

	error = bluetooth_send_rc_command(NULL, input_data, 2);

	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}

	return error;
}

int bt_hid_send_haptic_data(const char *remote_address,
		const unsigned char *data, unsigned short length)
{
	int error;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(data);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	error = bluetooth_send_rc_haptic_data(
			(hid_device_address_t *)&addr_hex,
			data, length);

	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_hid_get_smart_remote_conn_status(bool *is_connected)
{
	int error;
	gboolean status = FALSE;

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(is_connected);

	error = bluetooth_get_smart_remote_conn_status(&status);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("bluetooth_get_Smart_remote_conn_status returned %s(0x%08x)",
				_bt_convert_error_to_string(error), error);
	}

	*is_connected = status ? TRUE : FALSE;

	return error;
}

int bt_hid_get_smart_rc_battery_status(unsigned char *status)
{
	int battery_status =0;

	BT_CHECK_HID_HOST_SUPPORT();
        BT_CHECK_INPUT_PARAMETER(status);

	if(vconf_get_int("file/bluetooth/bt_smart_rc_battery", &battery_status)) {
		BT_ERR("get vconf failed\n");
		*status = (char)battery_status;
		return BT_ERROR_OPERATION_FAILED;
	}
	*status = (char)battery_status;
	return BT_ERROR_NONE;
}

int bt_hid_set_calibration_mode(bool enable, int mode)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_HID_HOST_SUPPORT();

	error_code = _bt_get_error_code(bluetooth_set_calibration_mode(enable, mode));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int bt_hid_set_smart_rc_fw_update_status_cb(bt_hid_notify_smart_rc_fw_update_status_cb callback, void *user_data)
{
	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_HID_SMARTRC_FW_UPDATE_STATUS, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_hid_unset_smart_rc_fw_update_status_cb(void)
{
	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_HID_SMARTRC_FW_UPDATE_STATUS);
	return BT_ERROR_NONE;
}

int bt_hid_update_smart_rc_fw(const char *remote_address, const char *path)
{
	int error;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(path);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	error = bluetooth_update_smart_rc_fw((hid_device_address_t *)&addr_hex, path);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_hid_get_smart_rc_fw_version(const char *remote_address, int *version)
{
	BT_CHECK_HID_HOST_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(version);

        if(vconf_get_int("file/bluetooth/bt_smart_rc_version", version) != 0){
                BT_ERR("BT_VCONF_SMART_RC_VERSION vconf get failed");
                return BT_ERROR_OPERATION_FAILED;
        }
        BT_ERR("smart rc fw version : %X",*version);
	return BT_ERROR_NONE;
}
