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
#include "bluetooth_private.h"
#include "bluetooth-audio-api.h"
#include "bluetooth-telephony-api.h"
#include "bluetooth-scmst-api.h"

typedef struct _call_list_s {
	GList *list;
} call_list_s;

static bool is_audio_initialized = false;

#define BT_CHECK_AUDIO_INIT_STATUS() \
	if (__bt_check_audio_init_status() == BT_ERROR_NOT_INITIALIZED) \
	{ \
		LOGE("[%s] NOT_INITIALIZED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_INITIALIZED); \
		return BT_ERROR_NOT_INITIALIZED; \
	}

int __bt_check_audio_init_status(void)
{
	if (is_audio_initialized != true) {
		BT_ERR("NOT_INITIALIZED(0x%08x)", BT_ERROR_NOT_INITIALIZED);
		return BT_ERROR_NOT_INITIALIZED;
	}

	return BT_ERROR_NONE;
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

	BT_CHECK_INIT_STATUS();
	error = bluetooth_audio_init(_bt_audio_event_proxy, NULL);
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

#ifndef TELEPHONY_DISABLED /* B2_3G */
	error = bluetooth_telephony_init((void *)_bt_telephony_event_proxy, NULL);
	error = _bt_convert_telephony_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("[%s] (0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}
#endif

	is_audio_initialized = true;
	return BT_ERROR_NONE;
}

int bt_audio_deinitialize(void)
{
	int error;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();

	error = bluetooth_audio_deinit();
	error = _bt_get_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}

#ifndef TELEPHONY_DISABLED /* B2_3G */
	error = bluetooth_telephony_deinit();
	error = _bt_convert_telephony_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
		return error;
	}
#endif

	is_audio_initialized = false;
	return BT_ERROR_NONE;
}

int bt_audio_connect(const char *remote_address, bt_audio_profile_type_e type)
{
	int error;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();

#ifdef TELEPHONY_DISABLED
	if (type == BT_AUDIO_PROFILE_TYPE_HSP_HFP) {
		BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
		return BT_ERROR_NOT_SUPPORTED;
	}
#endif

	BT_CHECK_INPUT_PARAMETER(remote_address);
	_bt_convert_address_to_hex(&addr_hex, remote_address);
	switch(type) {
	case BT_AUDIO_PROFILE_TYPE_HSP_HFP:
		error = bluetooth_ag_connect(&addr_hex);
		break;
	case BT_AUDIO_PROFILE_TYPE_A2DP:
		error = bluetooth_av_connect(&addr_hex);
		break;
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

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();

#ifdef TELEPHONY_DISABLED
	if (type == BT_AUDIO_PROFILE_TYPE_HSP_HFP) {
		BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
		return BT_ERROR_NOT_SUPPORTED;
	}
#endif

	BT_CHECK_INPUT_PARAMETER(remote_address);
	_bt_convert_address_to_hex(&addr_hex, remote_address);
	switch(type) {
	case BT_AUDIO_PROFILE_TYPE_HSP_HFP:
		error = bluetooth_ag_disconnect(&addr_hex);
		break;
	case BT_AUDIO_PROFILE_TYPE_A2DP:
		error = bluetooth_av_disconnect(&addr_hex);
		break;
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
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AUDIO_CONNECTION_STATUS, callback, user_data);
	return BT_ERROR_NONE;

}
int bt_audio_unset_connection_state_changed_cb(void)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AUDIO_CONNECTION_STATUS) == true)
		_bt_unset_cb(BT_EVENT_AUDIO_CONNECTION_STATUS);
	return BT_ERROR_NONE;
}

int bt_ag_notify_speaker_gain(int gain)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	int error;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	error = bluetooth_telephony_set_speaker_gain((unsigned short)gain);
	error = _bt_convert_telephony_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
#endif
}

int bt_ag_get_speaker_gain(int *gain)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	int error;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(gain);
	error = bluetooth_telephony_get_headset_volume((unsigned int *)gain);
	error = _bt_convert_telephony_error_code(error);
	if (BT_ERROR_NONE != error) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
#endif
}

int bt_ag_is_nrec_enabled(bool *enabled)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	int error;
	gboolean is_enabled = FALSE;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
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
#endif
}

int bt_ag_set_microphone_gain_changed_cb(bt_ag_microphone_gain_changed_cb callback, void *user_data)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_MICROPHONE_GAIN_CHANGE, callback, user_data);
	return BT_ERROR_NONE;
#endif
}

int bt_ag_unset_microphone_gain_changed_cb(void)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_MICROPHONE_GAIN_CHANGE) == true)
		_bt_unset_cb(BT_EVENT_AG_MICROPHONE_GAIN_CHANGE);
	return BT_ERROR_NONE;
#endif
}

int bt_ag_set_speaker_gain_changed_cb(bt_ag_speaker_gain_changed_cb callback,
					void *user_data)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_SPEAKER_GAIN_CHANGE, callback, user_data);
	return BT_ERROR_NONE;
#endif
}

int bt_ag_unset_speaker_gain_changed_cb(void)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_SPEAKER_GAIN_CHANGE) == true)
		_bt_unset_cb(BT_EVENT_AG_SPEAKER_GAIN_CHANGE);
	return BT_ERROR_NONE;
#endif
}

int bt_ag_open_sco(void)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	int error;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	error = bluetooth_telephony_audio_open();
	error = _bt_convert_telephony_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
#endif
}

int bt_ag_close_sco(void)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	int error;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	error = bluetooth_telephony_audio_close();
	error = _bt_convert_telephony_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
#endif
}

int bt_ag_is_sco_opened(bool *opened)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(opened);
	if (bluetooth_telephony_is_sco_connected())
		*opened = true;
	else
		*opened = false;
	return BT_ERROR_NONE;
#endif
}

int bt_ag_set_sco_state_changed_cb(bt_ag_sco_state_changed_cb callback,
					void *user_data)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_SCO_CONNECTION_STATUS, callback, user_data);
	return BT_ERROR_NONE;
#endif
}

int bt_ag_unset_sco_state_changed_cb(void)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_SCO_CONNECTION_STATUS) == true)
		_bt_unset_cb(BT_EVENT_AG_SCO_CONNECTION_STATUS);
	return BT_ERROR_NONE;
#endif
}

int bt_ag_notify_call_event(bt_ag_call_event_e event, unsigned int call_id, const char *phone_number)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	int error;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
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
		error = BT_ERROR_INVALID_PARAMETER;
	}
	error = _bt_convert_telephony_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
#endif
}

int bt_ag_notify_call_list(bt_call_list_h list)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	int error;
	unsigned int call_count;
	call_list_s *handle;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(list);
	handle = (call_list_s *)list;
	call_count = g_list_length(handle->list);
	error = bluetooth_telephony_set_call_status((void *)handle->list, call_count);
	error = _bt_convert_telephony_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
#endif
}

int bt_ag_notify_voice_recognition_state(bool state)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	int error;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	if (state)
		error = bluetooth_telephony_start_voice_recognition();
	else
		error = bluetooth_telephony_stop_voice_recognition();
	error = _bt_convert_telephony_error_code(error);
	if (error != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error), error);
	}
	return error;
#endif
}

int bt_ag_set_call_handling_event_cb(bt_ag_call_handling_event_cb callback,
					void *user_data)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_CALL_HANDLING_EVENT, callback, user_data);
	return BT_ERROR_NONE;
#endif
}

int bt_ag_unset_call_handling_event_cb(void)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_CALL_HANDLING_EVENT) == true)
		_bt_unset_cb(BT_EVENT_AG_CALL_HANDLING_EVENT);
	return BT_ERROR_NONE;
#endif
}

int bt_ag_set_multi_call_handling_event_cb(
					bt_ag_multi_call_handling_event_cb callback,
					void *user_data)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_MULTI_CALL_HANDLING_EVENT, callback, user_data);
	return BT_ERROR_NONE;
#endif
}

int bt_ag_unset_multi_call_handling_event_cb(void)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_MULTI_CALL_HANDLING_EVENT) == true)
		_bt_unset_cb(BT_EVENT_AG_MULTI_CALL_HANDLING_EVENT);
	return BT_ERROR_NONE;
#endif
}

int bt_ag_set_dtmf_transmitted_cb(bt_ag_dtmf_transmitted_cb callback,
						void *user_data)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AG_DTMF_TRANSMITTED, callback, user_data);
	return BT_ERROR_NONE;
#endif
}

int bt_ag_unset_dtmf_transmitted_cb(void)
{
#ifdef TELEPHONY_DISABLED
	BT_ERR("NOT SUPPORTED(0x%08x)", BT_ERROR_NOT_SUPPORTED);
	return BT_ERROR_NOT_SUPPORTED;
#else
	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	if (_bt_check_cb(BT_EVENT_AG_DTMF_TRANSMITTED) == true)
		_bt_unset_cb(BT_EVENT_AG_DTMF_TRANSMITTED);
	return BT_ERROR_NONE;
#endif
}

int bt_call_list_create(bt_call_list_h *list)
{
	call_list_s *handle;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(list);
	if (*list != NULL) {
		BT_ERR("BT_ERROR_ALREADY_DONE(0x%08x)", BT_ERROR_ALREADY_DONE);
		return BT_ERROR_ALREADY_DONE;
	}
	handle = g_malloc0(sizeof(call_list_s));
	*list = handle;
	return BT_ERROR_NONE;
}

int bt_call_list_destroy(bt_call_list_h list)
{
	int result;
	call_list_s *handle;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
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

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
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

	BT_CHECK_INIT_STATUS();
	BT_CHECK_AUDIO_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(list);
	BT_CHECK_INPUT_PARAMETER(phone_number);

	handle = (call_list_s *)list;
	call_status = g_malloc0(sizeof(bt_telephony_call_status_info_t));
	call_status->call_id = call_id;
	call_status->call_status = state;
	call_status->phone_number = g_strdup(phone_number);

	handle->list = g_list_append(handle->list, (gpointer)call_status);
	return BT_ERROR_NONE;
}
