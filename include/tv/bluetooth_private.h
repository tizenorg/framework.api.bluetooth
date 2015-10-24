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


#ifndef __TIZEN_NETWORK_BLUETOOTH_PRIVATE_H__
#define __TIZEN_NETWORK_BLUETOOTH_PRIVATE_H__

#include <dlog.h>
#include <stdbool.h>
#include <bluetooth-api.h>
#include <bluetooth-audio-api.h>
#include <bluetooth-telephony-api.h>
#include <bluetooth-media-control.h>
#include <bluetooth-hid-api.h>
#include <bluetooth-sync3d-api.h>

#include "bluetooth.h"

#ifdef __cplusplus
extern "C" {
#endif

#undef LOG_TAG
#define LOG_TAG "CAPI_NETWORK_BLUETOOTH"

#define BT_INFO(fmt, args...) SLOGI(fmt, ##args)
#define BT_DBG(fmt, args...) SLOGD(fmt, ##args)
#define BT_ERR(fmt, args...) SLOGE(fmt, ##args)
#define BT_FATAL(fmt, args...) SLOGF(fmt, ##args)
#define ENTRY() BT_INFO("ENTER")
#define EXIT()  BT_INFO("EXIT")
#define OPP_UUID "00001105-0000-1000-8000-00805f9b34fb"

/**
 * @internal
 * @brief Bluetooth callback.
 */
typedef enum
{
	BT_EVENT_STATE_CHANGED = 0x00, /**< Adapter state is changed */
	BT_EVENT_NAME_CHANGED, /**< Adapter name is changed */
	BT_EVENT_VISIBILITY_MODE_CHANGED, /**< Adapter visibility mode is changed */
	BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED, /**< Device discovery state is changed */
	BT_EVENT_BOND_CREATED, /**< A bond is created */
	BT_EVENT_BOND_DESTROYED, /**< A bond is destroyed */
	BT_EVENT_AUTHORIZATION_CHANGED, /**< Authorization is changed */
	BT_EVENT_SERVICE_SEARCHED, /**< Service search finish */
	BT_EVENT_DATA_RECEIVED, /**< Data is received */
	BT_EVENT_CONNECTION_STATE_CHANGED, /**< Connection state is changed */
	BT_EVENT_RFCOMM_CONNECTION_REQUESTED, /**< RFCOMM connection is requested */
	BT_EVENT_OPP_CONNECTION_REQUESTED, /**< OPP connection is requested */
	BT_EVENT_OPP_PUSH_REQUESTED, /**< OPP push is requested */
	BT_EVENT_OPP_SERVER_TRANSFER_PROGRESS, /**< OPP transfer progress */
	BT_EVENT_OPP_SERVER_TRANSFER_FINISHED, /**< OPP transfer is completed */
	BT_EVENT_OPP_CLIENT_PUSH_RESPONSED, /**< OPP client connection is reponsed */
	BT_EVENT_OPP_CLIENT_PUSH_PROGRESS, /**< OPP client push progress */
	BT_EVENT_OPP_CLIENT_PUSH_FINISHED, /**< OPP client push is finished */
	BT_EVENT_PAN_CONNECTION_STATE_CHANGED, /**< PAN connection change */
	BT_EVENT_NAP_CONNECTION_STATE_CHANGED, /**< NAP connection change */
	BT_EVENT_HDP_CONNECTED, /**< HDP connection change */
	BT_EVENT_HDP_DISCONNECTED, /**< HDP disconnection change */
	BT_EVENT_HDP_DATA_RECEIVED, /**< HDP Data receive Callabck */
	BT_EVENT_AUDIO_CONNECTION_STATUS, /**< Audio Connection change callback */
	BT_EVENT_AG_SCO_CONNECTION_STATUS, /**< Audio - AG SCO Connection state change callback */
	BT_EVENT_AG_CALL_HANDLING_EVENT, /**< Audio - AG call event callback */
	BT_EVENT_AG_MULTI_CALL_HANDLING_EVENT, /**< Audio - AG 3-way call event callback */
	BT_EVENT_AG_DTMF_TRANSMITTED, /**< Audio - DTMF tone sending request */
	BT_EVENT_AG_MICROPHONE_GAIN_CHANGE, /**< Audio Microphone change callback */
	BT_EVENT_AG_SPEAKER_GAIN_CHANGE, /**< Audio Speaker gain change callback */
	BT_EVENT_AVRCP_CONNECTION_STATUS, /**< AVRCP connection change callback */
	BT_EVENT_AVRCP_EQUALIZER_STATE_CHANGED, /**< AVRCP equalizer state change callback */
	BT_EVENT_AVRCP_REPEAT_MODE_CHANGED, /**< AVRCP repeat mode change callback */
	BT_EVENT_AVRCP_SHUFFLE_MODE_CHANGED, /**< AVRCP equalizer mode change callback */
	BT_EVENT_AVRCP_SCAN_MODE_CHANGED, /**< AVRCP scan mode change callback */
	BT_EVENT_AVRCP_VOLUME_CHANGED, /**AVRCP Volume Changed callback*/
	BT_EVENT_AVRCP_MUTE_STATUS_CHANGED, /**AVRCP mute status change callback*/
	BT_EVENT_A2DP_SINK_CONNECTION_STATUS, /**< A2DP Sink Connection change callback */
	BT_EVENT_A2DP_SINK_AUDIO_STATUS, /**< A2DP Sink Connection change callback */
	BT_EVENT_AUDIO_DATA_RECVD,
	BT_EVENT_HID_CONNECTION_STATUS, /**< HID connection status callback */
	BT_EVENT_DEVICE_CONNECTION_STATUS, /**< Device connection status callback */
	BT_EVENT_GATT_CHARACTERISTIC_DISCOVERED, /**< GATT characteristic discovered callback */
	BT_EVENT_GATT_VALUE_CHANGED, /**< GATT characteristic value changed callback */
	BT_EVENT_GATT_WRITE_CHARACTERISTIC, /**< GATT characteristic value read callback */
	BT_EVENT_GATT_CLIENT_CHARACTERISTIC_DISCOVERED_LEGACY, /**< GATT characteristic discovered callback */
	BT_EVENT_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERED_LEGACY, /**< GATT characteristic descriptor discovered callback */
	BT_EVENT_GATT_CLIENT_VALUE_CHANGED_LEGACY, /**< GATT characteristic value changed callback */
	BT_EVENT_GATT_CLIENT_READ_CHARACTERISTIC_LEGACY, /**< GATT characteristic value read callback */
	BT_EVENT_GATT_CLIENT_WRITE_CHARACTERISTIC_LEGACY, /**< GATT characteristic value write callback */
	BT_EVENT_HID_AUDIO_DATA_RECVD,	/**< HID AUDIO DATA RECVD  callback*/
	BT_EVENT_HID_VOICE_KEY_PRESSED, /**< HID_AUDIO_BTN_PRESSED callback*/
	BT_EVENT_HID_BATTERY_STATUS_RECEIVED, /**< HID battery status received callback*/
	BT_EVENT_HID_MBR_RETURN_RECVD, /**< HID_MBR_RETURN_RECVD callback */
	BT_EVENT_HID_SENSOR_DATA_RECEIVED, /**< HID_SENSOR_DATA_RECEIVED callback */
	BT_EVENT_HID_SMARTRC_CONNECTION_STATUS, /**< HID SmartRC Connection callback*/
	BT_EVENT_AUTHORIZE_REQUEST, /**< Incoming authorization request callback event */
	BT_EVENT_MAX_CONNECTION_REACHED, /**< Max connection limit reached event callback*/
	BT_EVENT_AUTHENTICATION_REQUEST, /**< Authentication events during pairing process*/
	BT_EVENT_SYNC3D_ASSOCIATION_NOTIFIED, /**< 3DSYNC Glass associated callback */
	BT_EVENT_SYNC3D_VSPEC_EVENT_RECEIVED, /**< 3DSYNC vendor specific event received callback */
	BT_EVENT_ADVERTISING_STATE_CHANGED, /**< Advertising state changed callback */
	BT_EVENT_ADVERTISING_MANUFACTURER_DATA_CHANGED, /**< Advertising manufacturer data changed callback */
	BT_EVENT_SCAN_RESPONSE_MANUFACTURER_DATA_CHANGED, /**< Scan response manufacturer data changed callback */
	BT_EVENT_LE_DEVICE_DISCOVERY_STATE_CHANGED, /**< LE Only Device discovery state is changed */
	BT_EVENT_HID_SMARTRC_FW_UPDATE_STATUS, /**< Smart RC FW Upgrade status */
	BT_EVENT_DUT_MODE, /**< Throughput mode is running for smartRC */
	BT_EVENT_BT_CHIP_FW_UPDATE_STATUS, /**< BT Chip FW update status */
} bt_event_e;

/**
 * @internal
 */
typedef struct bt_event_sig_event_slot_s
{
    int event_type;
    void *callback;
    void *user_data;
} bt_event_sig_event_slot_s;


#define BT_CHECK_INPUT_PARAMETER(arg) \
	if (arg == NULL) \
	{ \
		LOGE("[%s] INVALID_PARAMETER(0x%08x)", __FUNCTION__, BT_ERROR_INVALID_PARAMETER); \
		return BT_ERROR_INVALID_PARAMETER; \
	}

#define BT_NOT_SUPPORTED_FUNCTION() \
	{ \
		LOGE("[%s] NOT SUPPORTED FUNCTION(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
		return BT_ERROR_NOT_SUPPORTED; \
	}

#ifdef TIZEN_BT_DISABLE
#define BT_CHECK_BT_SUPPORT() \
        { \
                LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
                return BT_ERROR_NOT_SUPPORTED; \
        }
#else
#define BT_CHECK_BT_SUPPORT()
#endif

/**
 * @internal
 * @brief Check the initialzating status
 */
int _bt_check_init_status(void);

#define BT_CHECK_INIT_STATUS() \
	if (_bt_check_init_status() == BT_ERROR_NOT_INITIALIZED) \
	{ \
		LOGE("[%s] NOT_INITIALIZED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_INITIALIZED); \
		return BT_ERROR_NOT_INITIALIZED; \
	}

/**
 * @internal
 * @brief Initialize Bluetooth LE adapter
 */
int _bt_le_adapter_init(void);

/**
 * @internal
 * @brief Deinitialize Bluetooth LE adapter
 */
int _bt_le_adapter_deinit(void);

/**
 * @internal
 * @brief Set the event callback.
 */
void _bt_set_cb(int events, void *callback, void *user_data);

/**
 * @internal
 * @brief Unset the event callback.
 */
void _bt_unset_cb(int events);

/**
 * @internal
 * @brief Check if the event callback exist or not.
 */
bool _bt_check_cb(int events);

/**
 * @internal
 * @brief Convert Bluetooth F/W error codes to capi Bluetooth error codes.
 */
int _bt_get_error_code(int origin_error);


/**
 * @internal
 * @brief Convert Bluetooth F/W bluetooth_device_info_t to capi bt_device_info_s.
 */
int _bt_get_bt_device_info_s(bt_device_info_s **dest_dev, bluetooth_device_info_t *source_dev);


/**
 * @internal
 * @brief Free bt_device_info_s.
 */
void _bt_free_bt_device_info_s(bt_device_info_s *device_info);

/**
 * @internal
 * @brief Free bt_adapter_headless_dev_info_s.
 */
void _bt_free_headless_device_info_s(bt_adapter_headless_dev_info_s *device_info);

/**
 * @internal
 * @brief Convert Bluetooth F/W bluetooth_hid_device_info_t to capi bt_hid_device_info_s.
 */

int _bt_get_specific_bt_device_info_s(bt_hid_device_info_s **dest_dev, bluetooth_hid_device_info_t *source_dev);

/**
 * @internal
 * @brief Free bt_device_info_s.
 */
void _bt_free_specific_bt_device_info_s(bt_hid_device_info_s *device_info);

/**
 * @internal
 * @brief Convert Bluetooth F/W bluetooth_device_address_t to string.
 */
int _bt_convert_address_to_string(char **addr_str, bluetooth_device_address_t *addr_hex);


/**
 * @internal
 * @brief Convert string to Bluetooth F/W bluetooth_device_address_t.
 */
void _bt_convert_address_to_hex(bluetooth_device_address_t *addr_hex, const char *addr_str);


/**
 * @internal
 * @brief Convert error code to string.
 */
const char* _bt_convert_error_to_string(int error);

/**
 * @internal
 * @brief Convert the visibility mode
 */
bt_adapter_visibility_mode_e _bt_get_bt_visibility_mode_e(bluetooth_discoverable_mode_t mode);

/**
 * @internal
 * @brief Since the Audio call back and event proxy call backs have different prototype it is wrapper function.
 */
void _bt_audio_event_proxy(int event, bt_audio_event_param_t *param, void *user_data);

/**
 * @internal
 * @brief Since the Telephony call back and event proxy call backs have different prototype it is wrapper function.
 */
void _bt_telephony_event_proxy(int event, telephony_event_param_t *param, void *user_data);

/**
 * @internal
 * @brief Since the AVRCP call back and event proxy call backs have different prototype it is wrapper function.
 */
void _bt_avrcp_event_proxy(int event, media_event_param_t *param, void *user_data);

/**
 * @internal
 * @brief Since the HID call back and event proxy call backs have different prototype it is wrapper function.
 */
void _bt_hid_event_proxy(int event, hid_event_param_t *param, void *user_data);

/**
 * @internal
 * @brief Since the Sync3d call back and event proxy call backs have different prototype it is wrapper function.
 */
void _bt_sync3d_event_proxy(int event, bt_sync3d_event_param_t *param, void *user_data);

/**
 * @internal
 * @brief Start the smartRC throughput calculation mode.
 */
int _bt_start_smartrc_tput_mode(void);

/**
 * @internal
 * @brief Start the throughput calculation mode.
 */
int _bt_stop_smartrc_tput_mode(void);


/**
 * @internal
 * @brief Convert Bluetooth F/W bluetooth_a2dp_sink_audio_info_t to capi bt_a2dp_sink_audio_info_s.
 */
int _bt_get_bt_a2dp_sink_audio_info_s(bt_a2dp_sink_audio_info_s **dest_audio_info, bluetooth_a2dp_sink_audio_info_t *source_audio_info);

/**
 * @internal
 * @brief Frees memory allocated to bt_a2dp_sink_audio_info_s
 */
void _bt_free_bt_a2dp_sink_audio_info_s(bt_a2dp_sink_audio_info_s *audio_info);

#ifdef __cplusplus
}
#endif

#endif /* __TIZEN_NETWORK_BLUETOOTH_PRIVATE_H__ */
