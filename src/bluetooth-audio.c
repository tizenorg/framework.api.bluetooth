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
#include <string.h>
#include "bluetooth.h"
#include "bluetooth_internal.h"
#include "bluetooth_extension.h"
#include "bluetooth_private.h"
#include "bluetooth-audio-api.h"
#include "bluetooth-telephony-api.h"
#include "bluetooth-scmst-api.h"

typedef struct _call_list_s {
	GList *list;
} call_list_s;

static bool is_audio_a2dp_initialized = false;
#ifdef TIZEN_WEARABLE
static bool is_audio_hf_initialized = false;
#endif
static bool is_audio_ag_initialized = false;

#if defined (TIZEN_HFP_DISABLE) && defined (TIZEN_A2DP_DISABLE)
#define BT_CHECK_AUDIO_SUPPORT() \
		{ \
			BT_CHECK_BT_SUPPORT(); \
			LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
			return BT_ERROR_NOT_SUPPORTED; \
		}
#else
#define BT_CHECK_AUDIO_SUPPORT()
#endif

#ifdef TIZEN_HFP_DISABLE
#define BT_CHECK_HFP_SUPPORT() \
		{ \
			BT_CHECK_BT_SUPPORT(); \
			LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
			return BT_ERROR_NOT_SUPPORTED; \
		}
#else
#define BT_CHECK_HFP_SUPPORT()
#endif

#ifdef TIZEN_A2DP_DISABLE
#define BT_CHECK_A2DP_SUPPORT() \
		{ \
			BT_CHECK_BT_SUPPORT(); \
			LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
			return BT_ERROR_NOT_SUPPORTED; \
		}
#else
#define BT_CHECK_A2DP_SUPPORT()
#endif

#ifdef TIZEN_AUDIO_HF_DISABLE
#define BT_CHECK_HF_SUPPORT() \
		{ \
			BT_CHECK_BT_SUPPORT(); \
			LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
			return BT_ERROR_NOT_SUPPORTED; \
		}
#else
#define BT_CHECK_HF_SUPPORT()
#endif

#define BT_CHECK_AUDIO_INIT_STATUS() \
	if (is_audio_a2dp_initialized != true && is_audio_ag_initialized != true) \
	{ \
		LOGE("[%s] NOT_INITIALIZED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_INITIALIZED); \
		return BT_ERROR_NOT_INITIALIZED; \
	}

#define BT_CHECK_A2DP_INIT_STATUS() \
	if (is_audio_a2dp_initialized != true) \
	{ \
		LOGE("[%s] NOT_INITIALIZED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_INITIALIZED); \
		return BT_ERROR_NOT_INITIALIZED; \
	}

#define BT_CHECK_HF_INIT_STATUS() \
	if (is_audio_hf_initialized != true) \
	{ \
		LOGE("[%s] NOT_INITIALIZED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_INITIALIZED); \
		return BT_ERROR_NOT_INITIALIZED; \
	}

#define BT_CHECK_AG_INIT_STATUS() \
	if (is_audio_ag_initialized != true) \
	{ \
		LOGE("[%s] NOT_INITIALIZED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_INITIALIZED); \
		return BT_ERROR_NOT_INITIALIZED; \
	}

/*The below API is just to convert the error from Telephony API's to CAPI error codes,
* this is temporary change and changes to proper error code will be done in
* subsequent check ins.*/
int _bt_convert_telephony_error_code(int error)
{
	switch(error) {
	case BLUETOOTH_TELEPHONY_ERROR_NONE:
		return BT_ERROR_NONE;
	case BLUETOOTH_TELEPHONY_ERROR_INVALID_PARAM:
		return BT_ERROR_INVALID_PARAMETER;
	case BLUETOOTH_TELEPHONY_ERROR_NOT_INITIALIZED:
		return BT_ERROR_NOT_INITIALIZED;
	case BLUETOOTH_TELEPHONY_ERROR_NOT_ENABLED:
		return BT_ERROR_NOT_ENABLED;
	case BLUETOOTH_TELEPHONY_ERROR_AUDIO_NOT_CONNECTED:
	case BLUETOOTH_TELEPHONY_ERROR_NOT_CONNECTED:
		return BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED;
	case BLUETOOTH_TELEPHONY_ERROR_BUSY:
		return BT_ERROR_RESOURCE_BUSY;
	case BLUETOOTH_TELEPHONY_ERROR_NO_MEMORY:
		return BT_ERROR_OUT_OF_MEMORY;
	case BLUETOOTH_TELEPHONY_ERROR_ALREADY_INITIALIZED:
	case BLUETOOTH_TELEPHONY_ERROR_ALREADY_EXSIST:
	case BLUETOOTH_TELEPHONY_ERROR_ALREADY_CONNECTED:
		return BT_ERROR_ALREADY_DONE;
	case BLUETOOTH_TELEPHONY_ERROR_INTERNAL:
	case BLUETOOTH_TELEPHONY_ERROR_NOT_AVAILABLE:
	case BLUETOOTH_TELEPHONY_ERROR_I_O_ERROR:
	case BLUETOOTH_TELEPHONY_ERROR_OPERATION_NOT_AVAILABLE:
		return BT_ERROR_OPERATION_FAILED;
	case BLUETOOTH_TELEPHONY_ERROR_PERMISSION_DENIED:
		return BT_ERROR_PERMISSION_DENIED;
	default:
		return BT_ERROR_NONE;
	}
}

int bt_audio_initialize(void)
{
	int error;

	BT_CHECK_AUDIO_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error = bluetooth_audio_init(_bt_audio_event_proxy, NULL);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	else
		is_audio_a2dp_initialized = true;

#ifdef TIZEN_WEARABLE
	error = bluetooth_hf_init(_bt_hf_event_proxy, NULL);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	else
		is_audio_hf_initialized = true;
#endif

#ifndef TELEPHONY_DISABLED /* B2_3G */
	error = bluetooth_telephony_init((void *)_bt_telephony_event_proxy, NULL);
	error = _bt_convert_telephony_error_code(error);
	if (BT_ERROR_NONE != error)
		BT_ERR("[%s] (0x%08x)", _bt_convert_error_to_string(error), error);
	else
		is_audio_ag_initialized = true;
#endif

	/* There is no success case for 3 profiles */
	if (!is_audio_a2dp_initialized &&
#ifdef TIZEN_WEARABLE
		!is_audio_hf_initialized &&
#endif
		 !is_audio_ag_initialized) {
		return BT_ERROR_OPERATION_FAILED;
	}

	return BT_ERROR_NONE;
}

int bt_audio_deinitialize(void)
{
	int error;

	BT_CHECK_AUDIO_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error = bluetooth_audio_deinit();
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);

	is_audio_a2dp_initialized = false;

#ifdef TIZEN_WEARABLE
	error = bluetooth_hf_deinit();
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);

	is_audio_hf_initialized = false;
#endif

#ifndef TELEPHONY_DISABLED /* B2_3G */
	error = bluetooth_telephony_deinit();
	error = _bt_convert_telephony_error_code(error);
	if (BT_ERROR_NONE != error)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);

	is_audio_ag_initialized = false;
#endif

	return BT_ERROR_NONE;
}

int bt_audio_connect(const char *remote_address, bt_audio_profile_type_e type)
{
	int error;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_AUDIO_SUPPORT();
	BT_CHECK_INIT_STATUS();

	if (type == BT_AUDIO_PROFILE_TYPE_HSP_HFP) {
		BT_CHECK_HFP_SUPPORT();
		BT_CHECK_AG_INIT_STATUS();
	} else if (type == BT_AUDIO_PROFILE_TYPE_A2DP) {
		BT_CHECK_A2DP_SUPPORT();
		BT_CHECK_A2DP_INIT_STATUS();
	} else if (type == BT_AUDIO_PROFILE_TYPE_ALL) {
#if defined (TIZEN_HFP_DISABLE) || defined (TELEPHONY_DISABLED)
		BT_CHECK_A2DP_INIT_STATUS();
		BT_ERR("HSP NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
		type = BT_AUDIO_PROFILE_TYPE_A2DP;
#else
		BT_CHECK_AUDIO_INIT_STATUS();
		BT_ERR("HSP and A2DP SUPPORTED");
		type = BT_AUDIO_PROFILE_TYPE_ALL;
#endif
	}

	BT_CHECK_INPUT_PARAMETER(remote_address);
	_bt_convert_address_to_hex(&addr_hex, remote_address);
	switch(type) {
#ifndef TELEPHONY_DISABLED
	case BT_AUDIO_PROFILE_TYPE_HSP_HFP:
		error = bluetooth_ag_connect(&addr_hex);
		break;
#endif
	case BT_AUDIO_PROFILE_TYPE_A2DP:
		error = bluetooth_av_connect(&addr_hex);
		break;
#ifdef TIZEN_WEARABLE
	case BT_AUDIO_PROFILE_TYPE_AG:
		error = bluetooth_hf_connect(&addr_hex);
		break;
#endif
	case BT_AUDIO_PROFILE_TYPE_ALL:
	default:
		error = bluetooth_audio_connect(&addr_hex);
		break;
	}
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_audio_disconnect(const char *remote_address, bt_audio_profile_type_e type)
{
	int error;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_AUDIO_SUPPORT();
	BT_CHECK_INIT_STATUS();

	if (type == BT_AUDIO_PROFILE_TYPE_HSP_HFP) {
		BT_CHECK_HFP_SUPPORT();
		BT_CHECK_AG_INIT_STATUS();
	} else if (type == BT_AUDIO_PROFILE_TYPE_A2DP) {
		BT_CHECK_A2DP_SUPPORT();
		BT_CHECK_A2DP_INIT_STATUS();
	} else if (type == BT_AUDIO_PROFILE_TYPE_ALL) {
#if defined (TIZEN_HFP_DISABLE) || defined (TELEPHONY_DISABLED)
		BT_CHECK_A2DP_INIT_STATUS();
		BT_ERR("HSP NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
		type = BT_AUDIO_PROFILE_TYPE_A2DP;
#else
		BT_CHECK_AUDIO_INIT_STATUS();
		BT_ERR("HSP and A2DP SUPPORTED");
		type = BT_AUDIO_PROFILE_TYPE_ALL;
#endif
	}

	BT_CHECK_INPUT_PARAMETER(remote_address);
	_bt_convert_address_to_hex(&addr_hex, remote_address);
	switch(type) {
#ifndef TELEPHONY_DISABLED
	case BT_AUDIO_PROFILE_TYPE_HSP_HFP:
		error = bluetooth_ag_disconnect(&addr_hex);
		break;
#endif
	case BT_AUDIO_PROFILE_TYPE_A2DP:
		error = bluetooth_av_disconnect(&addr_hex);
		break;
#ifdef TIZEN_WEARABLE
	case BT_AUDIO_PROFILE_TYPE_AG:
		error = bluetooth_hf_disconnect(&addr_hex);
		break;
#endif
	case BT_AUDIO_PROFILE_TYPE_ALL:
	default:
		error = bluetooth_audio_disconnect(&addr_hex);
		break;
	}
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_audio_set_connection_state_changed_cb(bt_audio_connection_state_changed_cb callback, void *user_data)
{
	BT_CHECK_AUDIO_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AUDIO_CONNECTION_STATUS, callback, user_data);
	return BT_ERROR_NONE;

}
int bt_audio_unset_connection_state_changed_cb(void)
{
	BT_CHECK_AUDIO_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AUDIO_CONNECTION_STATUS) == true)
		_bt_unset_cb(BT_EVENT_AUDIO_CONNECTION_STATUS);
	return BT_ERROR_NONE;
}

int bt_ag_notify_speaker_gain(int gain)
{
	int error;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	error = bluetooth_telephony_set_speaker_gain((unsigned short)gain);
	error = _bt_convert_telephony_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_ag_get_speaker_gain(int *gain)
{
	int error;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(gain);
	error = bluetooth_telephony_get_headset_volume((unsigned int *)gain);
	error = _bt_convert_telephony_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_ag_is_nrec_enabled(bool *enabled)
{
	int error;
	gboolean is_enabled = FALSE;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(enabled);

	error = bluetooth_telephony_is_nrec_enabled(&is_enabled);
	error = _bt_convert_telephony_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	if (is_enabled)
		*enabled = true;
	else
		*enabled = false;

	return error;
}

int bt_ag_is_wbs_mode(bool *wbs_mode)
{
	int error;
	gboolean is_wbs_mode = FALSE;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(wbs_mode);

	error = bluetooth_telephony_is_wbs_mode(&is_wbs_mode);
	error = _bt_convert_telephony_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	if (is_wbs_mode)
		*wbs_mode = true;
	else
		*wbs_mode = false;

	return error;
}

int bt_ag_set_microphone_gain_changed_cb(bt_ag_microphone_gain_changed_cb callback, void *user_data)
{
	BT_CHECK_HFP_SUPPORT();

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_MICROPHONE_GAIN_CHANGE, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_ag_unset_microphone_gain_changed_cb(void)
{
	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_MICROPHONE_GAIN_CHANGE) == true)
		_bt_unset_cb(BT_EVENT_AG_MICROPHONE_GAIN_CHANGE);
	return BT_ERROR_NONE;
}

int bt_ag_set_speaker_gain_changed_cb(bt_ag_speaker_gain_changed_cb callback,
					void *user_data)
{
	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_SPEAKER_GAIN_CHANGE, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_ag_unset_speaker_gain_changed_cb(void)
{
	BT_CHECK_HFP_SUPPORT();

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_SPEAKER_GAIN_CHANGE) == true)
		_bt_unset_cb(BT_EVENT_AG_SPEAKER_GAIN_CHANGE);
	return BT_ERROR_NONE;
}

int bt_ag_open_sco(void)
{
	int error;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	error = bluetooth_telephony_audio_open();
	error = _bt_convert_telephony_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_ag_close_sco(void)
{
	int error;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	error = bluetooth_telephony_audio_close();
	error = _bt_convert_telephony_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_ag_is_sco_opened(bool *opened)
{
	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(opened);
	if (bluetooth_telephony_is_sco_connected())
		*opened = true;
	else
		*opened = false;
	return BT_ERROR_NONE;
}

int bt_ag_set_sco_state_changed_cb(bt_ag_sco_state_changed_cb callback,
					void *user_data)
{
	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_SCO_CONNECTION_STATUS, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_ag_unset_sco_state_changed_cb(void)
{
	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_SCO_CONNECTION_STATUS) == true)
		_bt_unset_cb(BT_EVENT_AG_SCO_CONNECTION_STATUS);
	return BT_ERROR_NONE;
}

int bt_ag_notify_call_event(bt_ag_call_event_e event, unsigned int call_id, const char *phone_number)
{
	int error;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_INFO("call_id [%d] / event [%d]", call_id, event);

	switch(event) {
	case BT_AG_CALL_EVENT_IDLE:
		error = bluetooth_telephony_call_end(call_id);
		break;
	case BT_AG_CALL_EVENT_ANSWERED:
		error = bluetooth_telephony_call_answered(call_id, FALSE);
		break;
	case BT_AG_CALL_EVENT_HELD:
		error = bluetooth_telephony_call_held(call_id);
		break;
	case BT_AG_CALL_EVENT_RETRIEVED:
		error = bluetooth_telephony_call_retrieved(call_id);
		break;
	case BT_AG_CALL_EVENT_DIALING:
		BT_CHECK_INPUT_PARAMETER(phone_number);
		error = bluetooth_telephony_indicate_outgoing_call(
					phone_number, call_id, FALSE);
		break;
	case BT_AG_CALL_EVENT_ALERTING:
		error = bluetooth_telephony_call_remote_ringing(call_id);
		break;
	case BT_AG_CALL_EVENT_INCOMING:
		BT_CHECK_INPUT_PARAMETER(phone_number);
		error = bluetooth_telephony_indicate_incoming_call(phone_number,
					call_id);
		break;
	default:
		error = BLUETOOTH_TELEPHONY_ERROR_INVALID_PARAM;
	}
	error = _bt_convert_telephony_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_ag_notify_call_list(bt_call_list_h list)
{
	int error;
	unsigned int call_count;
	call_list_s *handle;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(list);
	handle = (call_list_s *)list;
	call_count = g_list_length(handle->list);
	error = bluetooth_telephony_set_call_status((void *)handle->list, call_count);
	error = _bt_convert_telephony_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_ag_notify_voice_recognition_state(bool state)
{
	int error;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	if (state)
		error = bluetooth_telephony_start_voice_recognition();
	else
		error = bluetooth_telephony_stop_voice_recognition();
	error = _bt_convert_telephony_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_ag_set_call_handling_event_cb(bt_ag_call_handling_event_cb callback,
					void *user_data)
{
	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_CALL_HANDLING_EVENT, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_ag_unset_call_handling_event_cb(void)
{
	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_CALL_HANDLING_EVENT) == true)
		_bt_unset_cb(BT_EVENT_AG_CALL_HANDLING_EVENT);
	return BT_ERROR_NONE;
}

int bt_ag_set_multi_call_handling_event_cb(
					bt_ag_multi_call_handling_event_cb callback,
					void *user_data)
{
	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_MULTI_CALL_HANDLING_EVENT, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_ag_unset_multi_call_handling_event_cb(void)
{
	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_MULTI_CALL_HANDLING_EVENT) == true)
		_bt_unset_cb(BT_EVENT_AG_MULTI_CALL_HANDLING_EVENT);
	return BT_ERROR_NONE;
}

int bt_ag_set_dtmf_transmitted_cb(bt_ag_dtmf_transmitted_cb callback,
						void *user_data)
{
	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_DTMF_TRANSMITTED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_ag_unset_dtmf_transmitted_cb(void)
{
	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_DTMF_TRANSMITTED) == true)
		_bt_unset_cb(BT_EVENT_AG_DTMF_TRANSMITTED);
	return BT_ERROR_NONE;
}

int bt_ag_is_connected(bool *connected)
{
	int error;
	gboolean is_connected = false;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(connected);

	error = bluetooth_telephony_is_connected(&is_connected);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);

	if (is_connected)
		*connected = true;
	else
		*connected = false;

	return error;
}

int bt_call_list_create(bt_call_list_h *list)
{
	call_list_s *handle;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(list);
	if (*list != NULL) {
		BT_ERR("BT_ERROR_ALREADY_DONE(0x%08x)", BT_ERROR_ALREADY_DONE);
		return BT_ERROR_ALREADY_DONE;
	}
	handle = g_malloc0(sizeof(call_list_s));
	if (handle == NULL) {
		BT_ERR("BT_ERROR_OUT_OF_MEMORY(0x%08x)", BT_ERROR_OUT_OF_MEMORY);
		return BT_ERROR_OUT_OF_MEMORY;
	}

	*list = handle;
	return BT_ERROR_NONE;
}

int bt_call_list_destroy(bt_call_list_h list)
{
	int result;
	call_list_s *handle;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(list);
	handle = (call_list_s *)list;
	result = bt_call_list_reset(list);
	g_free(handle);
	return result;
}

int bt_call_list_reset(bt_call_list_h list)
{
	call_list_s *handle;
	bt_telephony_call_status_info_t *call_status;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(list);
	handle = (call_list_s *)list;
	do  {
		call_status = (bt_telephony_call_status_info_t *)g_list_nth_data(handle->list, 0);
		if (call_status == NULL)
			break;
		handle->list = g_list_remove(handle->list, call_status);

		if (NULL != call_status->phone_number)
			g_free(call_status->phone_number);

		g_free(call_status);
	} while (1);
	return BT_ERROR_NONE;
}

int bt_call_list_add(bt_call_list_h list, unsigned int call_id, bt_ag_call_state_e state, const char *phone_number)
{
	call_list_s *handle;
	bt_telephony_call_status_info_t *call_status;

	BT_CHECK_HFP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AG_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(list);
	BT_CHECK_INPUT_PARAMETER(phone_number);

	handle = (call_list_s *)list;
	call_status = g_malloc0(sizeof(bt_telephony_call_status_info_t));

	/* Fix : NULL_RETURNS */
	if (call_status == NULL)
		return BT_ERROR_OUT_OF_MEMORY;
	call_status->call_id = call_id;
	call_status->call_status = state;
	call_status->phone_number = g_strdup(phone_number);

	handle->list = g_list_append(handle->list, (gpointer)call_status);
	return BT_ERROR_NONE;
}

int bt_a2dp_set_content_protection(bool status)
{
	int error;

	BT_CHECK_A2DP_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_A2DP_INIT_STATUS();

	error = bluetooth_a2dp_set_content_protection(status);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}
	return error;
}

#ifdef TIZEN_WEARABLE
int bt_hf_initialize(void)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	int error;

	BT_CHECK_INIT_STATUS();
	error = bluetooth_hf_init(_bt_hf_event_proxy, NULL);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}
	return error;
}

int bt_hf_deinitialize(void)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	int error;

	BT_CHECK_INIT_STATUS();
	error = bluetooth_hf_deinit();
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}
	return error;
}

int bt_hf_notify_call_event(bt_hf_call_event_e event, char *phone_number)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	int error = BT_ERROR_NONE;

	BT_INFO("event [%d]", event);

	switch(event) {
	case BT_HF_CALL_EVENT_IDLE:
		error = bluetooth_hf_terminate_call();
		break;
	case BT_HF_CALL_EVENT_ANSWER:
		error = bluetooth_hf_answer_call();
		break;
	case BT_HF_CALL_EVENT_HOLD:
		break;
	case BT_HF_CALL_EVENT_RETRIEVE:
		break;
	case BT_HF_CALL_EVENT_DIAL:
		BT_CHECK_INPUT_PARAMETER(phone_number);
		error = bluetooth_hf_initiate_call(phone_number);
		break;
	case BT_HF_CALL_EVENT_REDIAL:
		break;
	case BT_HF_CALL_EVENT_RELEASE_ALL_NONACTIVE_CALLS:
		error = bluetooth_hf_release_all_call();
		break;
	case BT_HF_CALL_EVENT_ACCEPT_AND_RELEASE:
		error = bluetooth_hf_release_and_accept();
		break;
	case BT_HF_CALL_EVENT_ACCEPT_AND_HOLD:
		error = bluetooth_hf_swap_call();
		break;
	case BT_HF_CALL_EVENT_ADD_TO_CONVERSATION:
		error = bluetooth_hf_join_call();
		break;
	default:
		error = BLUETOOTH_ERROR_INVALID_PARAM;
		break;
	}

	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
}

int bt_hf_notify_speaker_gain(int gain)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	int error;

	error = bluetooth_hf_set_speaker_gain((unsigned int)gain);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);

	return error;
}

int bt_hf_set_speaker_gain_changed_cb(bt_hf_speaker_gain_changed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_set_cb(BT_EVENT_HF_SPEAKER_GAIN_CHANGE, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_hf_unset_speaker_gain_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	if (_bt_check_cb(BT_EVENT_HF_SPEAKER_GAIN_CHANGE) == true)
		_bt_unset_cb(BT_EVENT_HF_SPEAKER_GAIN_CHANGE);
	return BT_ERROR_NONE;
}

int bt_hf_notify_voice_recognition_state(bool state)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	int error;

	error = bluetooth_hf_voice_recognition(state);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);

	return error;
}

int bt_hf_set_call_status_updated_event_cb(bt_hf_call_status_updated_event_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_HF_CALL_STATUS_UPDATED_EVENT, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_hf_unset_call_status_updated_event_cb(void)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	if (_bt_check_cb(BT_EVENT_HF_CALL_STATUS_UPDATED_EVENT) == true)
		_bt_unset_cb(BT_EVENT_HF_CALL_STATUS_UPDATED_EVENT);
	return BT_ERROR_NONE;
}

int bt_hf_close_sco(void)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	int error;

	error = bluetooth_hf_audio_disconnect();
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);

	return error;
}

int bt_hf_send_dtmf(char *dtmf)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	int error;

	error = bluetooth_hf_send_dtmf(dtmf);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);

	return error;
}

int bt_hf_is_connected(bool *connected)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(connected);

	int error;
	gboolean is_connected = false;

	error = bluetooth_hf_is_connected(&is_connected);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);

	if (is_connected)
		*connected = true;
	else
		*connected = false;

	return error;
}

int bt_hf_is_sco_opened(bool *opened)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(opened);

	int error;
	unsigned int audio_connected = BLUETOOTH_HF_AUDIO_DISCONNECTED;

	error = bluetooth_hf_get_audio_connected(&audio_connected);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);

	if (audio_connected == BLUETOOTH_HF_AUDIO_CONNECTED)
		*opened = true;
	else
		*opened = false;

	return error;
}

int bt_hf_get_codec_id(unsigned int *codec_id)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(codec_id);

	int error;

	error = bluetooth_hf_get_codec(codec_id);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);

	return error;
}

int bt_hf_get_call_status_info_list(GSList **call_list)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(call_list);

	int error;
	bt_hf_call_list_s *hf_call_list = NULL;
	GList *l;

	error = bluetooth_hf_request_call_list(&hf_call_list);
	error = _bt_get_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

	*call_list = NULL;
	for (l = g_list_first(hf_call_list->list); l; l = g_list_next(l)) {
		bt_hf_call_status_info_t *hf_call_info = l->data;
		bt_hf_call_status_info_s *call_info = NULL;

		call_info = g_malloc0(sizeof(bt_hf_call_status_info_s));
		call_info->number = strdup(hf_call_info->number);
		call_info->direction = hf_call_info->direction;
		call_info->status = hf_call_info->status;
		call_info->multi_party = hf_call_info->mpart;
		call_info->index = hf_call_info->idx;
		*call_list = g_slist_append(*call_list, call_info);
	}

	bluetooth_hf_free_call_list(hf_call_list);

	return error;
}

static void __bt_hf_free_call_status_info(void *data)
{
	bt_hf_call_status_info_s *call_info = (bt_hf_call_status_info_s*)data;
	g_free(call_info->number);
	g_free(call_info);
}

int bt_hf_free_call_status_info_list(GSList *call_list)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(call_list);

	g_slist_free_full(call_list, __bt_hf_free_call_status_info);

	return BT_ERROR_NONE;
}

int bt_hf_set_sco_state_changed_cb(bt_hf_sco_state_changed_cb callback,
					void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_HF_SCO_CONNECTION_STATUS, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_hf_unset_sco_state_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	if (_bt_check_cb(BT_EVENT_HF_SCO_CONNECTION_STATUS) == true)
		_bt_unset_cb(BT_EVENT_HF_SCO_CONNECTION_STATUS);
	return BT_ERROR_NONE;
}

int bt_hf_set_call_handling_event_cb(bt_hf_call_handling_event_cb callback,
					void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_set_cb(BT_EVENT_HF_CALL_HANDLING_EVENT, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_hf_unset_call_handling_event_cb(void)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	if (_bt_check_cb(BT_EVENT_HF_CALL_HANDLING_EVENT) == true)
		_bt_unset_cb(BT_EVENT_HF_CALL_HANDLING_EVENT);
	return BT_ERROR_NONE;
}

int bt_hf_set_multi_call_handling_event_cb(
					bt_hf_multi_call_handling_event_cb callback,
					void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_set_cb(BT_EVENT_HF_MULTI_CALL_HANDLING_EVENT, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_hf_unset_multi_call_handling_event_cb(void)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_HF_SUPPORT();

	if (_bt_check_cb(BT_EVENT_HF_MULTI_CALL_HANDLING_EVENT) == true)
		_bt_unset_cb(BT_EVENT_HF_MULTI_CALL_HANDLING_EVENT);
	return BT_ERROR_NONE;
}
#endif
