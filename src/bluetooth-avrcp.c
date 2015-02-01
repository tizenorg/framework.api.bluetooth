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
#include <stdbool.h>
#include <bluetooth-api.h>
#include "bluetooth.h"
#include "bluetooth_private.h"
#include "bluetooth-audio-api.h"
#include "bluetooth-media-control.h"

static bool is_avrcp_target_initialized = false;

#define BT_CHECK_AVRCP_TARGET_INIT_STATUS() \
	if (__bt_check_avrcp_target_init_status() == BT_ERROR_NOT_INITIALIZED) \
	{ \
		LOGE("[%s] NOT_INITIALIZED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_INITIALIZED); \
		return BT_ERROR_NOT_INITIALIZED; \
	}

int __bt_check_avrcp_target_init_status(void)
{
	if (is_avrcp_target_initialized != true) {
		BT_ERR("NOT_INITIALIZED(0x%08x)", BT_ERROR_NOT_INITIALIZED);
		return BT_ERROR_NOT_INITIALIZED;
	}

	return BT_ERROR_NONE;
}

/*The below API is just to conver the error from Audio API's to CAPI error codes,
* this is temporary change and changes to proper error code will be done in
* subsequent check ins.*/
int _bt_convert_avrcp_error_code(int error)
{
	switch(error) {
	case BT_MEDIA_ERROR_NONE:
		return BT_ERROR_NONE;
	case BT_MEDIA_ERROR_INTERNAL:
		return BT_ERROR_INVALID_PARAMETER;
	case BT_MEDIA_ERROR_ALREADY_INITIALIZED:
		return BT_ERROR_OPERATION_FAILED;
	default:
		return BT_ERROR_NONE;
	}
}

int bt_avrcp_target_initialize(bt_avrcp_target_connection_state_changed_cb callback, void *user_data)
{
	int error;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AVRCP_CONNECTION_STATUS, callback, user_data);
	error = bluetooth_media_player_init(_bt_avrcp_event_proxy, NULL);
	error = _bt_convert_avrcp_error_code(error);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	is_avrcp_target_initialized = true;
	return BT_ERROR_NONE;
}

int bt_avrcp_target_deinitialize(void)
{
	int error;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AVRCP_TARGET_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AVRCP_CONNECTION_STATUS) == true)
		_bt_unset_cb(BT_EVENT_AVRCP_CONNECTION_STATUS);

	error = bluetooth_media_player_deinit();
	error = _bt_convert_avrcp_error_code(error);

	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	is_avrcp_target_initialized = false;
	return BT_ERROR_NONE;
}

int bt_avrcp_target_notify_equalizer_state(bt_avrcp_equalizer_state_e state)
{
	int error;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AVRCP_TARGET_INIT_STATUS();
	error = bluetooth_media_player_change_property(EQUALIZER, state);
	error = _bt_convert_avrcp_error_code(error);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}
int bt_avrcp_target_notify_repeat_mode(bt_avrcp_repeat_mode_e mode)
{
	int error;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AVRCP_TARGET_INIT_STATUS();
	error = bluetooth_media_player_change_property(REPEAT, mode);
	error = _bt_convert_avrcp_error_code(error);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_avrcp_target_notify_shuffle_mode(bt_avrcp_shuffle_mode_e mode)
{
	int error;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AVRCP_TARGET_INIT_STATUS();
	error = bluetooth_media_player_change_property(SHUFFLE, mode);
	error = _bt_convert_avrcp_error_code(error);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}
int bt_avrcp_target_notify_scan_mode(bt_avrcp_scan_mode_e mode)
{
	int error;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AVRCP_TARGET_INIT_STATUS();
	error = bluetooth_media_player_change_property(SCAN, mode);
	error = _bt_convert_avrcp_error_code(error);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_avrcp_target_notify_player_state(bt_avrcp_player_state_e state)
{
	int error;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AVRCP_TARGET_INIT_STATUS();
	error = bluetooth_media_player_change_property(STATUS, state);
	error = _bt_convert_avrcp_error_code(error);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_avrcp_target_notify_position(unsigned int position)
{
	int error;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AVRCP_TARGET_INIT_STATUS();
	error = bluetooth_media_player_change_property(POSITION, position);
	error = _bt_convert_avrcp_error_code(error);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_avrcp_target_notify_track(const char *title, const char *artist, const char *album,
		const char *genre, unsigned int track_num, unsigned int total_tracks, unsigned int duration)
{
	int error;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AVRCP_TARGET_INIT_STATUS();
	media_metadata_attributes_t metadata;
	metadata.title = title;
	metadata.artist = artist;
	metadata.duration = duration;
	metadata.genre = genre;
	metadata.number = track_num;
	metadata.album = album;
	metadata.total_tracks = total_tracks;
	error = bluetooth_media_player_change_track(&metadata);
	error = _bt_convert_avrcp_error_code(error);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_avrcp_set_equalizer_state_changed_cb(bt_avrcp_equalizer_state_changed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AVRCP_EQUALIZER_STATE_CHANGED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_avrcp_unset_equalizer_state_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_AVRCP_EQUALIZER_STATE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_avrcp_set_repeat_mode_changed_cb(bt_avrcp_repeat_mode_changed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AVRCP_REPEAT_MODE_CHANGED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_avrcp_unset_repeat_mode_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_AVRCP_REPEAT_MODE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_avrcp_set_shuffle_mode_changed_cb(bt_avrcp_shuffle_mode_changed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AVRCP_SHUFFLE_MODE_CHANGED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_avrcp_unset_shuffle_mode_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_AVRCP_SHUFFLE_MODE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_avrcp_set_scan_mode_changed_cb(bt_avrcp_scan_mode_changed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AVRCP_SCAN_MODE_CHANGED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_avrcp_unset_scan_mode_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_AVRCP_SCAN_MODE_CHANGED);
	return BT_ERROR_NONE;
}

