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
#include "bluetooth_product.h"
#include "bluetooth_private.h"
#include "bluetooth-sync3d-api.h"

int bt_sync3d_init(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = bluetooth_sync3d_init(_bt_sync3d_event_proxy);

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}
	return error_code;
}

int bt_sync3d_deinit(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = bluetooth_sync3d_deinit();

	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}
	return error_code;
}

int bt_sync3d_broadcast_on_off(bool on_off)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(set_3d_broadcast_on_off(on_off));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
							error_code);
	}
	return error_code;
}

int bt_sync3d_set_association_notified_cb(bt_sync3d_association_notified_cb callback,
								void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_SYNC3D_ASSOCIATION_NOTIFIED, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_sync3d_unset_association_notified_cb(void)
{
	_bt_unset_cb(BT_EVENT_SYNC3D_ASSOCIATION_NOTIFIED);
	return BT_ERROR_NONE;
}

int bt_sync3d_set_event_received_cb(bt_sync3d_event_cb callback,
								void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_SYNC3D_VSPEC_EVENT_RECEIVED, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_sync3d_unset_event_received_cb(void)
{
	_bt_unset_cb(BT_EVENT_SYNC3D_VSPEC_EVENT_RECEIVED);
	return BT_ERROR_NONE;
}

int bt_sync3d_set_duty_for_broadcast(short duty)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(set_duty_for_broadcast(duty));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int bt_sync3d_set_dealy_for_broadcast(short delay)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(set_dealy_for_broadcast(delay));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int bt_sync3d_set_dtv_mode(bt_sync3d_dtv_mode mode,
				const char *master_bd_addr)
{
	int error_code = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_INIT_STATUS();

	if (mode == BT_SYNC3D_DTV_MODE_SLAVE)
		BT_CHECK_INPUT_PARAMETER(master_bd_addr);

	_bt_convert_address_to_hex(&addr_hex, master_bd_addr);
	error_code = _bt_get_error_code(set_dtv_mode(mode, &addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int bt_sync3d_set_pairing_mode(bool on_off)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(set_pairing_mode(on_off));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int bt_sync3d_set_view_type(bt_sync3d_view_type view_type)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(set_view_type(view_type));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int bt_sync3d_set_dual_audio_mode(bool on_off)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(set_dual_audio_mode(on_off));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int bt_sync3d_get_sync_period(unsigned short *sync_period)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(sync_period);

	error_code = _bt_get_error_code(get_sync_period(sync_period));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int  bt_sync3d_set_rssi_for_proximity_pairing(short rssi)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(set_rssi_for_proximity_pairing(rssi));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int  bt_sync3d_set_vsync_detect(bool on_off)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(set_vsync_detect(on_off));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}
