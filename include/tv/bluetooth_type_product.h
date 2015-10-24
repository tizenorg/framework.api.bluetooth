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


#ifndef __TIZEN_NETWORK_BLUETOOTH_TYPE_PRODUCT_H__
#define __TIZEN_NETWORK_BLUETOOTH_TYPE_PRODUCT_H__
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <tizen_error.h>

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @file        bluetooth_type_product.h
 */


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Enumerations of the authentication event types.
 *
 */
typedef enum {
	BT_AUTH_KEYBOARD_PASSKEY_DISPLAY = 0, /**< PIN display event to user for entering PIN in keyboard */
	BT_AUTH_PIN_REQUEST,                  /**< Legacy PIN or PASSKEY request event */
	BT_AUTH_PASSKEY_CONFIRM_REQUEST,      /**< PASSKEY confirmation event to match PASSKEY in remote device */
	BT_AUTH_SSP_CONSENT_REQUEST,		/**< Event for requesting user Consent for SSP */
} bt_authentication_type_info_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief enums for audio stream type
 */
typedef enum {
	BT_AUDIO_MAIN_STREAM,
	BT_AUDIO_SUB_STREAM,
} bt_audio_stream_type_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief enums for audio role type
 */
typedef enum {
        BT_AUDIO_SOURCE_ROLE,
        BT_AUDIO_SINK_ROLE,
} bt_audio_role_type_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief enums for a2dp sink audio states
 */

typedef enum {
	BT_A2DP_SINK_AUDIO_SUSPEND,
	BT_A2DP_SINK_AUDIO_STOPPED,
	BT_A2DP_SINK_AUDIO_STARTED,
} bt_a2dp_sink_audio_state_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief Enumerations for sending AVRCP commands to remote audio dock.
 */
typedef enum
{
	BT_AVRCP_DEVICE_TURN_OFF = 0x40, /**<  The remote audio dock is turned off */
	BT_AVRCP_DEVICE_SET_VOLUME_UP, /**< Volume is increased in remote audio dock */
	BT_AVRCP_DEVICE_SET_VOLUME_DOWN, /**< Volume is decreased in remote audio dock */
	BT_AVRCP_DEVICE_SET_MUTE, /**< Remote audio dock is put to mute */
} bt_device_send_avrcp_cmd_e;

typedef enum {
	BT_SYNC3D_VIEW_TYPE_3D,
	BT_SYNC3D_VIEW_TYPE_2D2D_120,
	BT_SYNC3D_VIEW_TYPE_2D2D_240,
	BT_SYNC3D_VIEW_TYPE_2D3D,
	BT_SYNC3D_VIEW_TYPE_3D2D,
	BT_SYNC3D_VIEW_TYPE_3D3D,
	BT_SYNC3D_VIEW_TYPE_QUADVIEW,
	BT_SYNC3D_VIEW_TYPE_NO_SHUTTERING,
} bt_sync3d_view_type;

typedef enum {
	BT_SYNC3D_DTV_MODE_IDLE,
	BT_SYNC3D_DTV_MODE_MASTER,
	BT_SYNC3D_DTV_MODE_SLAVE,
	BT_SYNC3D_DTV_MODE_STANDALONE,
} bt_sync3d_dtv_mode;

typedef enum {
	BT_HAPTIC_EXPENDING = 0x08, /**< Soft Bump - 30% */
	BT_HAPTIC_SHRINKING = 0x08,
	BT_HAPTIC_INITIALENTRY = 0x39, /**< Sharp Tick 1 - 100% */
	BT_HAPTIC_ARROWBUTTON = 0x1D, /**< Short Double Click Strong 3 - 60% */
	BT_HAPTIC_EDGE = 0x37, /**< Pulsing Medium 2 - 60% */
	BT_HAPTIC_MOUSEIN = 0x18,
	BT_HAPTIC_LONGPRESS = 0x23,
	BT_HAPTIC_WARNING = 0x34, /**< Pulsing Strong 1 - 100% */
	BT_HAPTIC_POPUP = 0x21, /**< Short Double Click Medium 3 - 60% */
	BT_HAPTIC_FINDREMOTE = 0x34, /**< Pulsing Strong 1 - 100% */
	BT_HAPTIC_NOKEYMODERATE = 0x39, /**< Pulsing Sharp 2 - 60% */
	BT_HAPTIC_NOKEYMINIMUM = 0x21,

	BT_HAPTIC_RESERVED1=0xC9,
	BT_HAPTIC_RESERVED2=0xCA,
	BT_HAPTIC_RESERVED3=0xCB,
	BT_HAPTIC_RESERVED4=0xCC,
	BT_HAPTIC_RESERVED5=0xCD,
	BT_HAPTIC_NEWINITIALENTRY = 0x00
} bt_hid_haptic_t;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
 *
 * @brief enums for the sync3d event received data.
 *
 *
 * @see bt_sync3d_event_cb()
 */
typedef enum {
	SYNC3D_CODE_CHANGE_EVENT = 0,
	SYNC3D_FRAME_PERIOD_CHANGE_EVENT
} bt_sync3d_event_type;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief Class structure of Headless DB Device Info
 *
 */
typedef struct headlessdb_device_info
{
	char*bdaddress;	/**< BD Address. */
	char*dev_type;       /**< BT Device Type for e.g Mobile,TV etc. */
} bt_headlessdb_device_info;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Structure of one headless dev info data.
 *
 * @see int bt_adapter_foreach_headless_device_info(bt_adapter_headless_device_info_cb foreach_cb,
 *  void *user_data);
 */
typedef struct
{
	char *remote_address;
	char *hexclass;
	unsigned char null_pin_code;
} bt_adapter_headless_dev_info_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief enum for vendor specific TX Power class.
 *
 * @see #bt_class_s
 * @see bt_adapter_set_tx_power(bt_adapter_tx_pwr_class tx_power_class);
 */
typedef enum {
	BT_TX_PWR_CLASS_1_5 = 0x01,
	BT_TX_PWR_CLASS_2,
} bt_adapter_tx_pwr_class;


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief HID Device information structure.
 *
 * @see #bt_class_s
 * @see bt_device_bond_created_cb()
 * battey_status can assume following values:
 * 0x00 : Battery Level Monitoring is not supported
 * 0x01 : Battey Level 1
 * 0x02 : Battery Level 2
 * 0x03 : Battery Level 3
 * 0x04 : Battery Full
 * 0x05 : Low Battery
 */
typedef struct
{
	unsigned char device_type;	/**< Device type of the remote device */
	unsigned short capability;	/**< Capability of the remote device */
	unsigned char battery_status;	/**< Battery Status of the remote device */
	short resolution_width; /**< Reslotion: width */
	short resolution_height; /**< Reslotion: height */
	short size_x; /**< Size: x-co-ordinate */
	short size_y; /**< Size: y-co-ordinate */
} bt_hid_device_info_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief HID battery status call back structure.
 *
 * @see #battery_status;
 * @see #remote_address;
 */
typedef struct
{
        unsigned char battery_status;	/**< Battery status. */
        unsigned char* remote_address;	/**< Remote device address. */
} bt_hid_battery_status_info_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 *
 * @brief Structure of voice data received.
 *
 * @see bt_hid_audio_data_receive_cb()
 */
typedef struct
{
	unsigned short length; /**< buffer length */
	unsigned char *audio_buf; /**< audio data buffer */
} bt_hid_voice_data_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 *
 * @brief Structure for mbr return received.
 *
 * @see bt_hid_mbr_return_receive_cb()
 */
typedef struct
{
	unsigned short length; /**< buffer length */
	unsigned char *mbr_return; /**< MBR Return buffer */
} bt_hid_mbr_return_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 *
 * @brief Structure for smart remote sensor data.
 *
 * @see bt_hid_sensor_data_receive_cb()
 */
typedef struct {
	short accel_data;
	short gyro_data;
} sensor_info_s;

typedef struct {
	unsigned char sampling_time;
	unsigned char sensor_type;
	unsigned char touch_mode; /* 0 == Pressed, 2 == released */
	sensor_info_s x;
	sensor_info_s y;
	sensor_info_s z;
} bt_smart_rc_sensor_data_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
 *
 * @brief Structure of Association notification received data.
 *
 *
 * @see bt_sync3d_association_notified_cb()
 */
typedef struct {
	unsigned short association_notification;
				/* Association Notification status value */
	unsigned short battery_lvl;
				/* Battery level of the remote 3D glass */
	unsigned char *dst_addr;
} bt_sync3d_association_notify_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
 *
 * @brief structure for the sync3d event frame period change event.
 *
 *
 * @see bt_sync3d_event_cb()
 */
typedef struct {
	unsigned int frame_period;
	unsigned short period_fraction;
	unsigned short init_measurement;
} bt_sync3d_frame_period_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
 *
 * @brief structure for the smartrc throughput data.
 *
 *
 * @see bt_adapter_smartrc_tput_mode_cb()
 */
typedef struct bt_smartrc_throughput_data_s
{
	long total_bytes;
	long total_msecs;
	long throughput;
} bt_smartrc_throughput_data_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 *
 * @brief Structure of voice data received.
 *
 * @see bt_audio_data_receive_cb()
 */
typedef struct
{
        unsigned short length; /**< buffer length */
        unsigned char *audio_buf; /**< audio data buffer */
} bt_audio_data_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 *
 * @brief Fetches a2dp stream information.
 *
 * @see bt_a2dp_sink_get_audio_info()
 */
typedef struct
{
	int info_status;
	unsigned short sampling_freq; /**<sampling frquency */
	unsigned short num_channel; /**<channel number */
	int bits_per_sample; /**<bits per sample */
} bt_a2dp_sink_audio_info_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Called when the smartRC throughput calculation is done.
 *
 * @remarks you can get some SmartRC information, such as Total bytes, total time, Total throughput.
 *
 * @param[in] tput_data Throughput calculation data like total bytes, time etc.
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @pre Either bt_adapter_start_smartrc_tput_mode() or bt_adapter_stop_smartrc_tput_mode() will invoke this function
 * if you register this callback using bt_adapter_set_device_discovery_state_changed_cb().
 *
 * @see bt_adapter_start_smartrc_tput_mode()
 * @see bt_adapter_stop_smartrc_tput_mode()
 * @see bt_hid_set_smartrc_tput_mode_cb()
 * @see bt_hid_unset_smartrc_tput_mode_cb()
 *
 */
typedef void (*bt_adapter_smartrc_tput_mode_cb) (bt_smartrc_throughput_data_s *tput_data, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Called when MaximunDeviceConnected event is received.
 * @param[in] result
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_adapter_set_max_device_connected_cb()
 */
typedef void (*bt_adapter_max_conection_reached_cb)(int result, char *remote_address, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Called when remote device requests authentication.
 * @param[in] result
 * @param[in] auth_type
 *             typedef enum {
 *              BT_AUTH_KEYBOARD_PASSKEY_DISPLAY = 0, : PIN display event to user for entering PIN in keyboard
 *              BT_AUTH_PIN_REQUEST,                  : Legacy PIN or PASSKEY request event
 *              BT_AUTH_PASSKEY_CONFIRM_REQUEST,      : PASSKEY confirmation event to match PASSKEY in remote device
 *		BT_AUTH_SSP_CONSENT_REQUEST,	      :	Event for requesting user Consent for SSP
 *             } bt_authentication_type_info_e;
 * @param[in] device_name  Name of the remote device
 * @param[in] remote_addr  Remote BD address
 * @param[in] pass_key     PASSKEY string
 *            PASSKEY string is valid only if authentication types are following
 *             a/ BT_AUTH_KEYBOARD_PASSKEY_DISPLAY
 *             b/ BT_AUTH_PASSKEY_CONFIRM_REQUEST
 *            pass_key string will be invalid if authentication event is of type BT_AUTH_PIN_REQUEST
 *            as this event indicates that user MUST enter PIN or PASSKEY and perform authentication.
 *
 *            Upon receiving BT_AUTH_KEYBOARD_PASSKEY_DISPLAY event, user should enter PASSKEY in keyboard
 *            Application can also call bt_device_cancel_bonding() Upon receiving BT_AUTH_KEYBOARD_PASSKEY_DISPLAY
 *            event which will fail the on-going pairing with remote device.
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_adapter_set_authentication_req_cb()
 */
typedef void (*bt_adapter_authentication_req_cb)(int result, bt_authentication_type_info_e auth_type,
						char *device_name, char *remote_addr,
						char *pass_key, void *user_data);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Called when you get headless devices repeatedly.
 *
 * @param[in] headless_device_info The registered headless device information
 * @param[in] user_data The user data passed from the foreach function
 * @return @c true to continue with the next iteration of the loop,
 * \n @c false to break out of the loop.
 * @pre bt_adapter_foreach_headless_device_info() will invoke this function.
 *
 * @see bt_adapter_foreach_headless_device_info()
 *
 */
typedef bool (*bt_adapter_headless_device_info_cb)(bt_adapter_headless_dev_info_s *headless_device_info,
							void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Called when the BT Chip FW update status is ongoing
 * @details  This callback is called when BT Chip FW update progress is received
 * @param[in] result of FW update progress: result must be checked first.
 * If result is not BT_ERROR_NONE, progress percentage will not be valid
 * @param[in] progress percentage
 * @param[in] user_data The user data passed from the callback registration function
 */
typedef void (*bt_adapter_notify_bt_chip_fw_update_status_cb) (int result,
					unsigned char progress, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief  Called when authorization event is received.
 * @param[in] result The result  of the authorization event.
 * @param[in] remote_address The address of the remote Bluetooth device.
 * @param[in] user_data The user data passed from the callback registration function.
 * @pre bt_device_set_auth_event_state_changed_cb() will invoke this function
 *
 * @see bt_initialize()
 */
typedef void (*bt_device_auth_event_state_changed_cb)
		(int result, const char *remote_address, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief  Called when authorization event is received.
 * @param[in] result The result  of the authorization event.
 * @param[in] remote_address The address of the remote Bluetooth device.
 * @param[in] name: Name of the remote Bluetooth device.
 * @param[in] user_data The user data passed from the callback registration function.
 * @pre bt_device_set_auth_event_cb() will invoke this function
 *
 * @see bt_initialize()
 */
typedef void (*bt_device_auth_event_cb)
	(int result, char *remote_address, char *name, void *user_data);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when the connection state is changed.
 * @details  This callback is called when the connection state is changed.
 * When you call bt_hid_host_connect() or bt_hid_host_disconnect(), this callback is also called with error result even though these functions fail.
 * @param[in] result  The result of changing the connection state
 * @param[in] connected  The state to be changed. @a true means connected state, Otherwise, @a false.
 * @param[in] remote_address  The remote address
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_hid_host_connect()
 * @see bt_hid_host_disconnect()
 */
typedef void (*bt_hid_host_connection_state_changed_cb) (int result, bool connected, const char *remote_address, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when sound data from hid smart RC is received.
 * @details  This callback is called when the hid smart RC sends voice data.
 * @param[in] pointer to bt_hid_voice_data_s. (contains raw pcm sample buffer and
 * length of the buffer)
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @see bt_hid_set_audio_data_receive_cb()
 * @see bt_hid_unset_audio_data_receive_cb()
 */
typedef void (*bt_hid_audio_data_receive_cb)
			(bt_hid_voice_data_s *voice_data, void *user_data);

/**
* @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
* @brief Called on both hid Smart RC connection and disconnection.
* @details This callback is called on both smart RC connection or disconnection.
* @param[in] is smart RC connected. @a true means connection is Samrt RC connection, Otherwise, @a false.
* @param[in] device_info The bonded smart RC device information.
* @param[in] user_data The user data passed from the callback registration function
* @see bt_hid_set_smart_rc_connection_cb()
* @see bt_hid_unset_smart_rc_connection_cb()
*/
typedef void (*bt_hid_notify_smart_rc_connection_cb)(bool is_smart_rc_connected, bt_device_info_s *device_info, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when the sound button is pressed on hid smart RC.
 * @details  This callback is called when the user presses the sound button on
 * hid smart RC.
 * @param[in] sound button state. @a true means pressed state, Otherwise, @a false.
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_hid_set_sound_key_pressed_cb()
 * @see bt_hid_unset_sound_key_pressed_cb()
 */
typedef void (*bt_hid_notify_sound_key_pressed_cb)(int btn_state, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when the battery status is received from hid smart RC.
 * @details  This callback is called when the battery level is received
 * from HID smart remote.
 * @param[in] pointer to battery status info structure
 * @param[in] user_data The user data passed from the callback registration function
 */
typedef void (*bt_hid_notify_battery_status_received_cb)(bt_hid_battery_status_info_s *battery_status,
							void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when the FW update status is received from hid smart RC.
 * @details  This callback is called when FW update progress is received
 * from HID smart remote.
 * @param[in] result of FW update progress: result must be checked first.
 * If result is not BT_ERROR_NONE, progress percentage will not be valid
 * @param[in] progress percentage
 * @param[in] user_data The user data passed from the callback registration function
 */
typedef void (*bt_hid_notify_smart_rc_fw_update_status_cb) (int result,
					unsigned char progress, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when mbr return from hid smart RC is received.
 * @details  This callback is called when the hid smart RC sends mbr return.
 * @param[in] pointer to bt_hid_mbr_return_s. (contains mbr return buffer
 * and length of the buffer)
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @see bt_hid_set_mbr_return_receive_cb()
 * @see bt_hid_unset_mbr_return_receive_cb()
 */
typedef void (*bt_hid_mbr_return_receive_cb)
			(bt_hid_mbr_return_s *mbr_return, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when 6-axis sensor data from hid smart RC is received.
 * @details  This callback is called when the hid smart RC sends sensor data.
 * @param[in] pointer to bt_smart_rc_sensor_data_s. (contains sensor data)
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @see bt_hid_set_sensor_data_receive_cb()
 * @see bt_hid_unset_sensor_data_receive_cb()
 */
typedef void (*bt_hid_sensor_data_receive_cb)
			(bt_smart_rc_sensor_data_s *sensor_data, void *user_data);

 /**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Called when the connection state is changed.
 * @details  This callback is called when the connection state is changed.
 * When you call bt_a2dp_sink_connect() or bt_a2dp_sink_disconnect(), this callback is also called with error result even though these functions fail.
 * @param[in] result  The result of changing the connection state
 * @param[in] connected  The state to be changed. @a true means connected state, Otherwise, @a false.
 * @param[in] remote_address  The remote address
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_a2dp_sink_set_connection_state_changed_cb()
 * @see bt_a2dp_sink_unset_connection_state_changed_cb()
 */
typedef void (*bt_a2dp_sink_connection_state_changed_cb) (bool connected, const char *remote_address, void *user_data);


 /**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Called when the connection state is changed.
 * @details  This callback is called when the audio state is changed.
 * When you call bt_a2dp_sink_connect() or bt_a2dp_sink_disconnect(), this callback is also called with error result even though these functions fail.
 * @param[in] result  The result of changing the audio state
 * @param[in] connected  The state to be changed. @a true means connected state, Otherwise, @a false.
 * @param[in] remote_address  The remote address
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_a2dp_sink_set_audio_state_changed_cb()
 * @see bt_a2dp_sink_unset_audio_state_changed_cb()
 */
typedef void (*bt_a2dp_sink_audio_state_changed_cb) (bt_a2dp_sink_audio_state_e audio_state, const char *remote_address,
								bt_a2dp_sink_audio_info_s *audio_info, void *user_data);


 /**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when sound data from remote audio source is received.
 * @details  This callback is called when the remote source sends audio data.
 * @param[in] pointer to bt_audio_data_s. (contains raw pcm sample buffer and
 * length of the buffer)
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @see bt_audio_set_audio_data_receive_cb()
 * @see bt_audio_unset_audio_data_receive_cb()
 */
typedef void (*bt_audio_data_receive_cb)
                        (bt_audio_data_s *audio_data, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Called when SetAbsoluteVolume event is received from the remote device.
 * @param[in] volume the volume value received
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_avrcp_target_initialize()
 * @see bt_set_avrcp_volume_changed_cb()
 */
typedef void (*bt_avrcp_volume_changed_cb) (unsigned short volume, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Called when mute/unmute  event is received from the remote device.
 * @param[in] mute status  the current status mute status of remote device
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_avrcp_target_initialize()
 * @see bt_avrcp_mute_state_changed_cb()
 * @see bt_avrcp_unset_mute_status_changed_cb()
 */
typedef void (*bt_avrcp_mute_state_changed_cb) (bool mute_status, void *user_data);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
 * @brief Called when 3D Glasses are associated with teh 3D TV.
 * @param[in] result The result of the associated status
 * @param[in] associated_notified_info The Association notification information
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @pre This callback will be invokeed if you register this callback
 * using bt_sync3d_set_association_notified_cb().
 *
 */
typedef void (*bt_sync3d_association_notified_cb)(bt_sync3d_association_notify_s *associated_notified_info, void *user_data);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
 * @brief Called when chip sends sync3d events.
 * @param[in] result The result of the event status
 * @param[in] frame_period_info The frame period change event information
 * @param[in] event event type
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @pre This callback will be invokeed if you register this callback
 * using bt_sync3d_set_event_received_cb().
 *
 */
typedef void (*bt_sync3d_event_cb)(bt_sync3d_frame_period_s *frame_period_info, bt_sync3d_event_type event, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Enumerations of the Bluetooth advertising state.
 */
typedef enum {
	BT_ADAPTER_ADVERTISING_STOPPED = 0x00, /**< Bluetooth advertising is stopped */
	BT_ADAPTER_ADVERTISING_STARTED, /**< Bluetooth advertising is started */
} bt_adapter_advertising_state_e;


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Enumerations of the Bluetooth advertising filter policy.
 */
typedef enum {
	BT_ADAPTER_ADVERTISING_FILTER_DEFAULT = 0x00, /**< White list is not in use */
	BT_ADAPTER_ADVERTISING_FILTER_ALLOW_SCAN_WL = 0x01, /**< Allow the scan
					request that in the White list */
	BT_ADAPTER_ADVERTISING_FILTER_ALLOW_CONN_WL = 0x02, /**< Allow the connectoin
					request that in the White list */
	BT_ADAPTER_ADVERTISING_FILTER_ALLOW_SCAN_CONN_WL = 0x03, /**< Allow the
					scan and connectoin request that in the White list */
} bt_adapter_advertising_filter_policy_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Enumerations of the Bluetooth advertising type.
 */
typedef enum {
	BT_ADAPTER_ADVERTISING_CONNECTABLE = 0x00, /**< Connectable undirected advertising (ADV_IND) */
	BT_ADAPTER_ADVERTISING_CONNECTABLE_DIRECT_HIGH = 0x01, /**< Connectable high duty cycle directed advertising (ADV_DIRECT_IND) */
	BT_ADAPTER_ADVERTISING_SCANNABLE = 0x02, /**< Scannable undirected advertising (ADV_SCAN_IND) */
	BT_ADAPTER_ADVERTISING_NON_CONNECTABLE = 0x03, /**< Non connectable undirected advertising (ADV_NONCOND_IND) */
	BT_ADAPTER_ADVERTISING_CONNECTABLE_DIRECT_LOW = 0x04, /**< Connectable low duty cycle directed advertising (ADV_DIRECT_IND) */
} bt_adapter_advertising_type_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Structure of advertising parameters
 *
 * @see #bt_class_s
 * @see bt_adapter_advertising_state_changed_cb()
 * @see bt_adapter_start_advertising()
 */
typedef struct {
	float interval_min; /**< Minimum advertising interval for non-directed advertising.
			      A multiple of 0.625ms is only allowed (Time range : 20ms to 10.24sec). */
	float interval_max; /**< Maximum advertising interval for non-directed advertising.
			      A multiple of 0.625ms is only allowed (Time range : 20ms to 10.24sec). */
	char filter_policy; /* Advertising filter policy */
	char type; /* Advertising type */
} bt_adapter_advertising_params_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Called when the state of advertising changes.
 *
 * @param[in] result The result of the advertising
 * @param[in] adv_state The advertising state to be changed
 * @param[in] adv_params The parameters of advertising \n
 *				If \a adv_state is #BT_ADAPTER_ADVERTISING_STOPPED, then \a adv_params is NULL.
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @pre Either bt_adapter_start_advertising() or bt_adapter_stop_stop_advertising() will invoke this function
 * if you register this callback using bt_adapter_set_advertising_state_changed_cb().
 *
 * @see bt_adapter_start_advertising(bt_adapter_advertising_params_s *adv_params)
 * @see bt_adapter_stop_advertising()
 * @see bt_adapter_set_advertising_state_changed_cb()
 * @see bt_adapter_unset_advertising_state_changed_cb()
 *
 */
typedef void (*bt_adapter_advertising_state_changed_cb)(int result,
		bt_adapter_advertising_state_e adv_state,
		bt_adapter_advertising_params_s *adv_params, void *user_data);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Called when the manufacturer specific data for advertising changes.
 * @param[in]   data		The manufacurer specific data of the Bluetooth device to be changed
 * @param[in]   len		The length of @a data
 * @param[in]   user_data	The user data passed from the callback registration function
 * @pre This function will be invoked when the manufacturer specific data of Bluetooth adapter changes
 * if you register this callback using bt_adapter_set_advertising_manufacturer_data_changed_cb().
 * @see bt_adapter_set_advertising_manufacturer_data()
 * @see bt_adapter_set_advertising_manufacturer_data_changed_cb()
 * @see bt_adapter_unset_advertising_manufacturer_data_changed_cb()
 */
typedef void (*bt_adapter_advertising_manufacturer_data_changed_cb)(char *data,
		int len, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Called when the manufacturer specific data for LE scan response changes.
 * @param[in]   data		The manufacurer specific data of the Bluetooth device to be changed
 * @param[in]   len		The length of @a data
 * @param[in]   user_data	The user data passed from the callback registration function
 * @pre This function will be invoked when the manufacturer specific data of Bluetooth adapter changes
 * if you register this callback using bt_adapter_set_scan_resp_manufacturer_data_changed_cb().
 * @see bt_adapter_set_scan_resp_manufacturer_data()
 * @see bt_adapter_set_scan_resp_manufacturer_data_changed_cb()
 * @see bt_adapter_unset_scan_resp_manufacturer_data_changed_cb()
 */
typedef void (*bt_adapter_scan_resp_manufacturer_data_changed_cb)(char *data,
		int len, void *user_data);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __TIZEN_NETWORK_BLUETOOTH_TYPE_PRODUCT_H__
