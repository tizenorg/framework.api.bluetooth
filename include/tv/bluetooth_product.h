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


#ifndef __TIZEN_NETWORK_BLUETOOTH_PRODUCT_H__
#define __TIZEN_NETWORK_BLUETOOTH_PRODUCT_H__
#include <glib.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <tizen_error.h>

#include "bluetooth_type.h"
#include "bluetooth_type_product.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @file        bluetooth_product.h
 */

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Registers a callback function that will be invoked when MaxDeviceConnected event is received.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_NOT_INITIALIZED Not initialized
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @see bt_initialize()
 */
int bt_adapter_set_max_device_connected_cb(bt_adapter_max_conection_reached_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Unregisters a callback function that will be invoked when MaxDeviceConnected event is received.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_NOT_INITIALIZED Not initialized
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @see bt_initialize()
 * @see bt_adapter_set_max_device_connected_cb()
 */
int bt_adapter_unset_max_device_connected_cb(void);


int bt_adapter_set_authentication_req_cb(bt_adapter_authentication_req_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Unregisters a callback function that will be invoked when remote device requests authentication.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_NOT_INITIALIZED Not initialized
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @see bt_initialize()
 * @see bt_adapter_set_authentication_req_cb()
 */
int bt_adapter_unset_authentication_req_cb(void);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief  API to reply with PIN or PASSKEY with authentication type - TRUE or FALSE.
 * @remarks  This function can be called by application when remote device requests PIN or PASSKEY from
 *           local adapter.
 * @param[in]  passkey  The passkey to be provided by application when remote devices requests for it.
 * @param[in]  authentication_reply This indicates whether application wants to accept or cancel the on-going pairing
 * @pre  This function can only be called when application receieves authentication event (BT_AUTH_PIN_REQUEST)
 *       from remote device.
 * @see  bt_adapter_set_authentication_req_cb()
 */
int bt_passkey_reply(char *passkey, bool authentication_reply);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief  API to provide consent for on-going SSP with remote device.
 * @remarks  This function can be called by application, when local adapter wants PASSKEY confirmation from user.
 * @param[in]  reply This indicates whether application wants to accepts or cancels the on-going pairing
 *             reply : TRUE will indicate that Application has appeoved pairing request.
 *             reply : FALSE will indicate that Application has denied pairing request.
 *                     In this situation the pairing will be failed.
 * @pre  This function should only be called when application receives authentication event (BT_AUTH_SSP_CONSENT_REQUEST)
 *       from remote device.
 * @see  bt_adapter_set_authentication_req_cb()
 */
int bt_ssp_consent_reply(bool reply);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief  API to reply to the PASSKEY confirmation for on-going pairing with remote device.
 * @remarks  This function can be called by application, when local adapter wants PASSKEY confirmation from user.
 * @param[in]  confirmation_reply This indicates whether application wants to accepts or cancels the on-going pairing
 *             confirmation_reply : TRUE will indicate that Application has confirmed the PASSKEY
 *             confirmation_reply : FALSE will indicate that Application has failed to confirm the PASSKEY. In this situation
 *             the pairing will be failed.
 * @pre  This function can only be called when application receives authentication event (BT_AUTH_PASSKEY_CONFIRM_REQUEST)
 *       from remote device.
 * @see  bt_adapter_set_authentication_req_cb()
 */
int bt_passkey_confirmation_reply(bool confirmation_reply);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Set duration of discovery.
 *
 * @remarks To connect to peer Bluetooth device, you need to know its Bluetooth address. \n
 * The device discovery can be stopped by bt_adapter_stop_device_discovery().
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_NOW_IN_PROGRESS  Operation is now in progress
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @post After this function, bt_adapter_device_discovery_start() should be called.
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_is_discovering()
 * @see bt_adapter_stop_device_discovery()
 * @see bt_adapter_device_discovery_state_changed_cb()
 * @see bt_adapter_set_device_discovery_state_changed_cb()
 * @see bt_adapter_unset_device_discovery_state_changed_cb()
 */
int bt_adapter_set_duration(int duration);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Starts the device discovery, asynchronously with duration
 *
 * @details If a device is discovered, bt_adapter_device_discovery_state_changed_cb() will be invoked
 * with #BT_ADAPTER_DEVICE_DISCOVERY_FOUND, and then bt_adapter_device_discovery_state_changed_cb()
 * will be called with #BT_ADAPTER_DEVICE_DISCOVERY_FINISHED in case of the completion or cancellation of the discovery.
 *
 * @remarks To connect to peer Bluetooth device, you need to know its Bluetooth address. \n
 * The device discovery can be stopped by bt_adapter_stop_device_discovery().
 *
 * @param[in] duration the discvoery duration
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_NOW_IN_PROGRESS  Operation is now in progress
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @post This function invokes bt_adapter_device_discovery_state_changed_cb().
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_is_discovering()
 * @see bt_adapter_stop_device_discovery()
 * @see bt_adapter_device_discovery_state_changed_cb()
 * @see bt_adapter_set_device_discovery_state_changed_cb()
 * @see bt_adapter_unset_device_discovery_state_changed_cb()
 */
int bt_adapter_start_device_discovery_with_duration(int duration);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Starts the device discovery, asynchronously based on class of device.
 *
 * @details If a device is discovered, bt_adapter_device_discovery_state_changed_cb() will be invoked
 * with #BT_ADAPTER_DEVICE_DISCOVERY_FOUND, and then bt_adapter_device_discovery_state_changed_cb()
 * will be called with #BT_ADAPTER_DEVICE_DISCOVERY_FINISHED in case of the completion or cancellation of the discovery.
 *
 * @remarks To connect to peer Bluetooth device, you need to know its Bluetooth address. \n
 * The device discovery can be stopped by bt_adapter_stop_device_discovery().
 *
 * @param[in] major major class of the device
 * @param[in] minor minor class of the device
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_NOW_IN_PROGRESS  Operation is now in progress
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @post This function invokes bt_adapter_device_discovery_state_changed_cb().
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_is_discovering()
 * @see bt_adapter_stop_device_discovery()
 * @see bt_adapter_device_discovery_state_changed_cb()
 * @see bt_adapter_set_device_discovery_state_changed_cb()
 * @see bt_adapter_unset_device_discovery_state_changed_cb()
 */
int bt_adapter_start_device_discovery_with_class_of_device(int major, int minor);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Starts the device discovery, asynchronously based on the remote bluetooth address.
 *
 * @details If a device is discovered, bt_adapter_device_discovery_state_changed_cb() will be invoked
 * with #BT_ADAPTER_DEVICE_DISCOVERY_FOUND, and then bt_adapter_device_discovery_state_changed_cb()
 * will be called with #BT_ADAPTER_DEVICE_DISCOVERY_FINISHED in case of the completion or cancellation of the discovery.
 *
 * @remarks To connect to peer Bluetooth device, you need to know its Bluetooth address. \n
 * The device discovery can be stopped by bt_adapter_stop_device_discovery().
 *
 * @param[in] remote_address	bluetooth address of the remote device
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_NOW_IN_PROGRESS  Operation is now in progress
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @post This function invokes bt_adapter_device_discovery_state_changed_cb().
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_is_discovering()
 * @see bt_adapter_stop_device_discovery()
 * @see bt_adapter_device_discovery_state_changed_cb()
 * @see bt_adapter_set_device_discovery_state_changed_cb()
 * @see bt_adapter_unset_device_discovery_state_changed_cb()
 */
int bt_adapter_start_device_discovery_with_address(const char *remote_address);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Starts the device discovery, asynchronously for peer 3D TV.
 *
 * @details If a device is discovered, bt_adapter_device_discovery_state_changed_cb() will be invoked
 * with #BT_ADAPTER_DEVICE_DISCOVERY_FOUND, and then bt_adapter_device_discovery_state_changed_cb()
 * will be called with #BT_ADAPTER_DEVICE_DISCOVERY_FINISHED in case of the completion or cancellation of the discovery.
 *
 * The device discovery can be stopped by bt_adapter_stop_device_discovery().
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_NOW_IN_PROGRESS  Operation is now in progress
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @post This function invokes bt_adapter_device_discovery_state_changed_cb().
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_is_discovering()
 * @see bt_adapter_stop_device_discovery()
 * @see bt_adapter_device_discovery_state_changed_cb()
 * @see bt_adapter_set_device_discovery_state_changed_cb()
 * @see bt_adapter_unset_device_discovery_state_changed_cb()
 */
int bt_adapter_start_device_discovery_for_3dtv(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Gets the specific device information of a bonded device.
 * @remarks The @a device_info must be released with bt_adapter_free_hid_device_info() by you .
 *
 * @param [in] remote_address The address of remote device
 * @param [out] device_info The bonded device information
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_BONDED  Remote device not bonded
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @post This function invokes bt_adapter_bonded_device_cb().
 *
 * @see bt_adapter_bonded_device_cb()
 * @see bt_adapter_enable()
 */
int bt_adapter_get_specific_bonded_device_info(const char *remote_address, bt_hid_device_info_s **device_info);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Frees HID device info.
 *
 * @param [in] device_info The bonded device information of HID device
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @see bt_adapter_get_bonded_device_info()
 */
int bt_adapter_free_hid_device_info(bt_hid_device_info_s *device_info);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
* @brief  Set Vendor Specific Tx Power Data based
* on TX Power Class.
* @param[in] tx power class type
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int  bt_adapter_set_tx_power(bt_adapter_tx_pwr_class tx_power_class);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Enable or Disable Headless Mode
 * Headless Mode MUST not be enabled during normal
 * operation. Once Headless mode mode is enabled,
 * HCI will stop working. Any further command to
 * Bluetooth adapter will cause undefined behaviour.
 * Headless Mode can only be enabled just before
 * Power Down of TV stack.
 *
 * @param[in] enable  TRUE: Enable  FALSE: disable
 * @retval  #BLUETOOTH_ERROR_NONE  - Success
 * @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
 */
int bt_adapter_set_headless_mode(bool enable);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Registers a headless device (which supports
 * Headless Mode)to Bluetooth adapter.
 * @param[in] remote_address BD address of the Headless
 *            device to be registered.
 * @retval  #BLUETOOTH_ERROR_NONE  - Success
 * @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
 */
int bt_adapter_register_headless_device(const char *remote_address);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Un-registers a headless device (which supports
 * Headless Mode)to Bluetooth adapter.
 * @param[in] remote_address BD address of the Headless
 *            device to be un-registered.
 * @retval  #BLUETOOTH_ERROR_NONE  - Success
 * @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
 */
int bt_adapter_unregister_headless_device(const char *remote_address);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Retrieves the device information of all bonded devices.
 *
 * @param [in] callback The callback function to invoke
 * @param [in] user_data The user data passed from the foreach function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @post This function invokes bt_adapter_headless_device_info_cb().
 *
 */
int bt_adapter_foreach_headless_device_info(bt_adapter_headless_device_info_cb foreach_cb, void *user_data);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Frees headless dev info structures allocated previously
 *
 * @param[in] device_info
 *            device_info is a poiter to one headless device info structure
 *
 * @retval  #BLUETOOTH_ERROR_NONE  - Success
 * @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
 */
int  bt_adapter_free_headless_device_info(bt_adapter_headless_dev_info_s *device_info);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Set Headless Scan Mode.
 * LLR mode will be enabled by BT chip or firmware.
 *
 * @param[in] llr_scan Enable or Disable LLR scan
 *
 * @retval  #BLUETOOTH_ERROR_NONE  - Success
 * @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
 */
int bt_adapter_set_headless_scan_control(bool llr_scan);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  bluetooth sleep API with Standby
 * enable or disable.
 *
 * Case: Standby  = false
 * Intant TV on support
 *
 * Headless Mode will be turned on internally MUST not be enabled
 * during normal operation. Once Headless mode mode
 * is enabled.
 * HCI will stop working. Any further command to
 * Bluetooth adapter will cause undefined behaviour.
 * Headless Mode can only be enabled just before
 * Power Down of TV stack.
 *
 * Case: Standby = true
 * This will trigger disconnection of all
 * connected Bluetooth devices
 *
 * @param[in] standby TRUE:TV Instant On Mode
 *	      standby FALSE: Headless Mode
 * @retval  #BT_ERROR_NONE  - Success
 * @retval  #BT_ERROR_OPERATION_FAILED -Failure
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 */
int bt_adapter_sleep_bluetooth(bool standby);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  bluetooth power off API
 *
 * Headless Mode will be turned on internally MUST not be enabled
 * during normal operation. Once Headless mode mode
 * is enabled.
 * HCI will stop working. Any further command to
 * Bluetooth adapter will cause undefined behaviour.
 * Headless Mode can only be enabled just before
 * Power Down of TV stack.
 *
 * @retval  #BT_ERROR_NONE  - Success
 * @retval  #BT_ERROR_OPERATION_FAILED -Failure
 */
int bt_adapter_power_off(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Checks if Samsung Audio dock device is connected.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_OPERATION_FAILED  If A2DP device is not connected.
 *
 * @see bt_adapter_enable()
 */
int bt_adapter_is_samsung_audio_dock_connected(bool *connected);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Called to register BT Chip FW update progress status.
 * @details  This callback is called when application wants to register
 * the callback for receiving BT Chip FW update status event.
 * @param[in] Callback to be registered for FW update status event.
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 */
int bt_adapter_set_bt_chip_fw_update_status_cb(bt_adapter_notify_bt_chip_fw_update_status_cb callback,
							void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Unregisters a callback function that will be invoked when the
 * BT Chip FW update status event is received.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @see bt_adapter_set_bt_chip_fw_update_status_cb()
 */
int bt_adapter_unset_bt_chip_fw_update_status_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief Accepts the authorization of a bonded device, asynchronously.
 *
 * @param[in] remote_address The address of the remote Bluetooth device to authorize
 * @param[in] service_id The service id of the remote Bluetooth device
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_BONDED  Remote device not bonded
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @pre The bond with the remote device must be created with bt_device_create_bond().
 * @post bt_device_authorization_changed_cb() will be invoked.
 *
 * @see bt_adapter_enable()
 * @see bt_device_create_bond()
 * @see bt_device_authorization_changed_cb()
 * @see bt_device_set_authorization_changed_cb()
 * @see bt_device_unset_authorization_changed_cb()
 */
int bt_device_accept_authorization(const char *device_address, int service_id);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief Rejects the authorization of a bonded device, asynchronously.
 *
 * @param[in] remote_address The address of the remote Bluetooth device
 * @param[in] remote_address The service id of the remote Bluetooth device
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_BONDED  Remote device not bonded
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @see bt_adapter_enable()
 * @see bt_device_create_bond()
 * @see bt_device_authorization_changed_cb()
 * @see bt_device_set_authorization_changed_cb()
 * @see bt_device_unset_authorization_changed_cb()
 */
int bt_device_reject_authorization(const char *device_address, int service_id);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief Checks whether stereo headset is connnected or not.
 * @param[out] opened The SCO status: (@c true = connected, @c  false = not connected)
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 */
int bt_device_is_a2dp_connected(bool *connected);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Registers a callback function to be invoked when incoming authorization event is received.
 *
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 * @post bt_device_auth_event_state_changed_cb() will be invoked.
 *
 * @see bt_initialize()
 */
int  bt_device_set_auth_event_state_changed_cb(bt_device_auth_event_state_changed_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Registers a callback function to be invoked when incoming authorization event is received.
 *
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 * @post bt_device_auth_event_cb() will be invoked.
 *
 * @see bt_initialize()
 */
int  bt_device_set_auth_event_cb(bt_device_auth_event_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when sound data from hid smart RC is received.
 * @details  This callback is called when the hid smart RC sends voice data.
 * @param[in] pointer to bt_hid_voice_data_s. (contains raw pcm sample buffer and
 * length of the buffer)
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_OPERATION_FAILED Failed
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 */
int bt_hid_set_audio_data_receive_cb(bt_hid_audio_data_receive_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Unregisters a callback function that will be invoked when sound
 * data from hid smart RC is received.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 * @see bt_hid_set_audio_data_receive_cb()
 */
int bt_hid_unset_audio_data_receive_cb(void);

/**
* @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
* @brief To set bt_hid_notify_smart_rc_connection_cb callback.
* @details This is API to set is smart rc connection callback.
* @param[in] Callback to be register for is smart_rc connection.
* @param[in] user_data The user data passed from the callback registration function.
* @return   0 on success, otherwise a negative error value.
* @retval #BT_ERROR_NONE  Successful.
* @retval #BT_ERROR_NOT_INITIALIZED  Not initialized.
* @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
*/
int bt_hid_set_smart_rc_connection_cb(bt_hid_notify_smart_rc_connection_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Unregisters a callback function that will be invoked when smart_rc is disconnected.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful.
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized.
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 * @see bt_hid_set_smart_rc_connection_cb().
 */
int bt_hid_unset_smart_rc_connection_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when the sound button is pressed on hid smart RC.
 * @details  This callback is called when the user presses the sound button on
 * hid smart RC.
 * @param[in] sound button state (pressed = 1, released = 0)
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 */
int bt_hid_set_sound_key_pressed_cb(bt_hid_notify_sound_key_pressed_cb callback,
					void *user_data);
/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Unregisters a callback function that will be invoked when the sound
 * button is pressed on hid smart RC.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 * @see bt_hid_set_sound_key_pressed_cb()
 */
int bt_hid_unset_sound_key_pressed_cb(void);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called to register callback for battery status receieve event.
 * @details  This callback is called when application wants to register
 * the callback for receiving battery status event.
 * @param[in] Callback to be registered for battery status event.
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 */
int bt_hid_set_battery_status_received_cb(bt_hid_notify_battery_status_received_cb callback,
                                        void *user_data);
/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Unregisters a callback function that will be invoked when the
 * battery status event is received from smart RC.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 * @see bt_hid_set_battery_status_received_cb()
 */
int bt_hid_unset_battery_status_received_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called to register FW update progress status event from Smart RC.
 * @details  This callback is called when application wants to register
 * the callback for receiving FW update status event.
 * @param[in] Callback to be registered for FW update status event.
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 */
int bt_hid_set_smart_rc_fw_update_status_cb(bt_hid_notify_smart_rc_fw_update_status_cb callback,
							void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Unregisters a callback function that will be invoked when the
 * FW update status event is received from smart RC.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 * @see bt_hid_set_smart_rc_fw_update_status_cb()
 */
int bt_hid_unset_smart_rc_fw_update_status_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODUL
 * @brief Update SmartRC Firmware.
 * This function is a synchronous call.
 * @return  #BT_ERROR_NONE Success
 * @return  #BT_ERROR_INVALID_PARAMETER Invalid paramete
 * @return  #BT_ERROR_NOT_ENABLED Not enable
 * @return  #BT_ERROR_OPERATION_FAILED Operation failed
 * @exception   None
 * @param[in]   remote_address This indicates an address of the device to which
 *                              command should be sent
 * @param[in]   path of the Smart RC FW. Should be a valid path.
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 * @remark      Can only be used when SmartRC is connected
 */
int bt_hid_update_smart_rc_fw(const char *remote_address, const char *path);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODUL
 * @brief Get SmartRC Firmware Version.
 * This function is a synchronous call.
 * @return  #BT_ERROR_NONE Success
 * @return  #BT_ERROR_INVALID_PARAMETER Invalid paramete
 * @return  #BT_ERROR_NOT_ENABLED Not enable
 * @return  #BT_ERROR_OPERATION_FAILED Operation failed
 * @exception   None
 * @param[in]   remote_address This indicates an address of the device to which
 *                              command should be sent
 * @param[out]   version Firmware version of connected SmartRC
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 * @remark      Can only be used when SmartRC is connected
 */
int bt_hid_get_smart_rc_fw_version(const char *remote_address, int *version);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when mbr return from hid smart RC is received.
 * @details  This callback is called when the hid smart RC sends mbr return.
 * @param[in] pointer to bt_hid_mbr_return_s. (contains mbr return buffer
 * and length of the buffer
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 */
int bt_hid_set_mbr_return_receive_cb(bt_hid_mbr_return_receive_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Unregisters a callback function that will be invoked when mbr
 * command response from hid smart RC is received.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 * @see bt_hid_set_mbr_return_receive_cb()
 */
int bt_hid_unset_mbr_return_receive_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Called when sensor data from hid smart RC is received.
 * @details  This callback is called when the hid smart RC sends sensor data.
 * @param[in] pointer to bt_hid_sensor_data_s. (contains sensor data)
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 */
int bt_hid_set_sensor_data_receive_cb(bt_hid_sensor_data_receive_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Unregisters a callback function that will be invoked when
 * sensor data from hid smart RC is received.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 * @see bt_hid_set_sensor_data_receive_cb()
 */
int bt_hid_unset_sensor_data_receive_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief Send stop voice data command to smart RC.
 * service, asynchronously.
 * @param[in] remote_address  The remote address
 * If remote address is NULL, RC command will be sent
 * to all connected HID remotes.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @pre The remote device must be connected by bt_hid_host_connect().
 * @see bt_hid_host_connect()
*/
int bt_hid_rc_stop_sending_voice(const char *remote_address);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief Send RC commands smart remote.
 * @param[in] remote_address  The remote address
 * If remote address is NULL, RC command will be sent
 * to all connected HID remotes.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @pre The remote device must be connected by bt_hid_host_connect().
 * @see bt_hid_host_connect()
*/
int bt_hid_send_rc_command(const char *remote_address,
		const unsigned char *data, unsigned short length);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief Send haptic commands smart remote.
 * @param[in] data  Haptic command to send
 * @param[in] force  Whether if send command forcely or following menu option.
 * @param[in] voltage  voltage level
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @pre The remote device must be connected by bt_hid_host_connect().
 * @see bt_hid_host_connect()
*/
int bt_hid_send_haptic_command(const unsigned char data, bool force, int voltage);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief Send haptic data to Smart Touch Remote.
 * @param[in] remote_address  The remote address
 * @param[in] data  Haptic data to send
 *	      Total 7 bytes of Haptic data are allowed
 *	      Byte 0: Device Type
 *	      Byte 1: Strength 0
 *	      Byte 2: Strength 1
 *	      Byte 3: Pattern Index #0
 *	      Byte 4: Pattern Index #1
 *	      Byte 5: Pattern Index #2
 *	      Byte 6: Reserved
 * @param[in] length length of the data
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @pre The remote device must be connected by bt_hid_host_connect().
 * @see bt_hid_host_connect()
*/
int bt_hid_send_haptic_data(const char *remote_address, const unsigned char *data,
				unsigned short length);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief Send Device Command to smart RC.
 * @param[in] remote_address  The remote address
 * If remote address is NULL, RC command will be sent
 * to all connected HID devices.
 * @param[in] device_type Device type (for e.g Smart Remote)
 * @param[in] device_cmd Device command.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @pre The remote device must be connected by bt_hid_host_connect().
 * @see bt_hid_host_connect()
*/
int bt_hid_send_device_command(const char *remote_address, unsigned char device_type,
				 unsigned char device_cmd);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief Send MBR command to smart RC.
 * @param[in] remote_address  The remote address
 * If remote address is NULL, RC command will be sent
 * to all connected HID devices.
 * @param[in] data data buffer pointer.
 * @param[in] len Length of data.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @pre The remote device must be connected by bt_hid_host_connect().
 * @see bt_hid_host_connect()
*/
int bt_hid_send_mbr_command(const char *remote_address, const unsigned char* data,
				 unsigned short len);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief Send MBR Data to smart RC.
 * @param[in] remote_address  The remote address
 * If remote address is NULL, RC command will be sent
 * to all connected HID devices.
 * @param[in] data data buffer pointer.
 * @param[in] len Length of data.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @pre The remote device must be connected by bt_hid_host_connect().
 * @see bt_hid_host_connect()
*/
int bt_hid_send_mbr_data(const char *remote_address, const unsigned char* data,
				 unsigned short len);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief Set smart remote sensor calibration mode.
 * @param[in] enable = calibration status (enabled = TRUE, Disabled = FALSE)
 * @param[in] mode = calibration mode(calibration algorithm to be used)
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 */
int bt_hid_set_calibration_mode(bool enable, int mode);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief Get connection status for smart remote.
 * @param[out] is_connected: True if connected false otherwise.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 */
int bt_hid_get_smart_remote_conn_status(bool *is_connected);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief Get battery status for smart remote.
 * @param[out] status: battery status for smart remote
 * status can assume following values:
 * 0x00 : Battery Level Monitoring is not supported
 * 0x01 : Battey Level 1
 * 0x02 : Battery Level 2
 * 0x03 : Battery Level 3
 * 0x04 : Battery Full
 * 0x05 : Low Battery
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  device not connected
 */
int bt_hid_get_smart_rc_battery_status(unsigned char *status);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief Selects the audio stream to be routed to Bluetooth audio devices.
 * @details One of following two audio streams can be selected for bluetooth audio devices:
 * BT_AUDIO_MAIN_STREAM - to select main audio stream to be routed to bluetoot devices
 * BT_AUDIO_SUB_STREAM - to select audio  sub stream to be routed to bluetoot devices
 *
 * @param[in] stream The type of audio stream to be selected for bluetooth audio devices
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 */
int bt_audio_select_stream(bt_audio_stream_type_s stream);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief Selects the audio profile for Bluetooth audio devices.
 * @details One of following two audio profiles can be selected for bluetooth audio devices:
 * BT_AUDIO_SOURCE_ROLE - to select A2DP source profile for bluetoot devices
 * BT_AUDIO_SINK_ROLE - to select A2DP sink profile for bluetoot devices
 *
 * @param[in] profile name The type of audio profile to be selected for bluetooth audio devices
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 */
int bt_audio_select_role(bt_audio_role_type_s role);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief Connects the remote device with the given a2dp sink profile, asynchronously.
 * @details If you input type as #BT_AUDIO_PROFILE_TYPE_ALL and connection request succeeds, then bt_audio_connection_state_changed_cb() will be called twice
 * when #BT_AUDIO_PROFILE_TYPE_HSP_HFP is connected and #BT_AUDIO_PROFILE_TYPE_A2DP is connected.
 * @param[in] remote_address  The remote address
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @pre The Bluetooth audio service must be initialized with bt_a2dp_sink_init().
 * @post bt_audio_connection_state_changed_cb() will be invoked.
 * @see bt_a2dp_sink_disconnect()
 * @see bt_a2dp_sink_connection_state_changed_cb()
 */
int bt_a2dp_sink_connect(const char *remote_address);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief Disconnects the remote device with the given a2dp sink profile, asynchronously.
 * @details if disconnection request succeeds, then bt_audio_connection_state_changed_cb() will be called
 * @param[in] remote_address  The remote address
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @pre The remote device must be connected by bt_a2dp_sink_connect().
 * @post bt_a2dp_sink_connection_state_changed_cb() will be invoked.
 * @see bt_a2dp_sink_connect()
 * @see bt_a2dp_sink_connection_state_changed_cb()
 */
int bt_a2dp_sink_disconnect(const char *remote_address);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Registers a callback function that will be invoked when the connection state is changed.
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @see bt_a2dp_sink_connection_state_changed_cb()
 */
int bt_a2dp_sink_set_connection_state_changed_cb(bt_a2dp_sink_connection_state_changed_cb callback, void *user_data);


 /**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Unregisters a callback function that will be invoked when the connection state is changed.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @see bt_a2dp_sink_connection_state_changed_cb()
 * @see bt_a2dp_sink_set_connection_state_changed_cb()
 */
int bt_a2dp_sink_unset_connection_state_changed_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Gets a2dp sink stream audio information
 * @param[in] remote_address  The remote address
 * @param[out] audio_info Structure containing audio stream info
 * @return 0 on success, otherwise a negative error value.
 *
 */
int bt_a2dp_sink_get_audio_info(const char *remote_address,  bt_a2dp_sink_audio_info_s **audio_info);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Frees  memory allocated to bt_a2dp_sink_audio_info_s
 * @param[in] audio_info Structure containing audio stream info
 * @return 0 on success, otherwise a negative error value.
 *
 */
int bt_a2dp_sink_free_audio_info(bt_a2dp_sink_audio_info_s *audio_info);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Registers a callback function that will be invoked when the audio state is changed.
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @see bt_a2dp_sink_audio_state_changed_cb()
 */
int bt_a2dp_sink_set_audio_state_changed_cb(bt_a2dp_sink_audio_state_changed_cb callback, void *user_data);


 /**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Unregisters a callback function that will be invoked when the connection state is changed.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @see bt_a2dp_sink_audio_state_changed_cb()
 * @see bt_a2dp_sink_set_audio_state_changed_cb()
 */
int bt_a2dp_sink_unset_audio_state_changed_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  registers a callback function that will be invoked when sound
 * data from remote audio source is received.
 * @details  This callback is called when the remote audio source sends audio data.
 * @param[in] pointer to bt_audio_data_s. (contains raw pcm sample buffer and
 * length of the buffer)
 * @param[in] user_data The user data passed from the callback registration
 * function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_OPERATION_FAILED Failed
 */
int bt_set_audio_data_receive_cb(bt_audio_data_receive_cb callback, void *user_data);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_HID_MODULE
 * @brief  Unregisters a callback function that will be invoked when sound
 * data from remote audio source is received.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth HID service must be initialized with bt_hid_host_initialize().
 * @see bt_set_audio_data_receive_cb()
 */
int bt_unset_audio_data_receive_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Send AVRCP command to the target device
 * @param[in] AVRCP command
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enableds
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @pre The remote device must be connected.
 */
int bt_avrcp_target_send_cmd(bt_device_send_avrcp_cmd_e avrcp_cmd);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Set AVRCP name
 * @param[in] provide name
 * @param[in] service name
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enableds
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 */
int bt_avrcp_set_name(char *provider_name, char *service_name);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Registers a callback function that will be invoked when the SetAbsoluteVolume event is received.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @pre Initialize the AVRCP target by bt_avrcp_target_initialize.
 * @see bt_initialize()
 * @see bt_avrcp_target_initialize()
 */
int bt_set_avrcp_volume_changed_cb(bt_avrcp_volume_changed_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Registers a callback function that will be invoked when the mute change status event is received.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @pre Initialize the AVRCP target by bt_avrcp_target_initialize.
 * @see bt_initialize()
 * @see bt_avrcp_target_initialize()
 */
int bt_set_avrcp_mute_state_changed_cb(bt_avrcp_mute_state_changed_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Unregisters a callback function that will be invoked when the mute status is  changed by the remote control device.
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @see bt_initialize()
 * @see bt_set_avrcp_mute_state_changed_cb()
 */
int bt_avrcp_unset_mute_status_changed_cb(void);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
* @brief  Sets the vendor ID and product ID.
* @param[in]  vendor ID, 16 bit value
* @param[in]  product ID, 16 bit value
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int bt_adapter_set_device_id(unsigned short vid, unsigned short pid);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
* @brief  set AFH channel classification to block bluetooth channels for using wifi channel.
* @param[in] first_disabled_channel
* @param[in] last_disabled_channel
* @Note: wifichannel : 0 => no block, 1 ~ 13 => 0~79
*	[Chanels within first and last disabled channels will be blocked.
*
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int bt_adapter_block_bt_channels_for_using_wifi(unsigned char first_disabled_channel,
							unsigned char last_disabled_channel);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  initializes the sync3d profile.
* @param[in} None
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
* @post bt_sync3d_broadcast_on_off(on) shall be called to register
* the sync3d profile and prepare for 3D broadcast
*/
int bt_sync3d_init(void);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  De-initializes the sync3d profile.
* @param[in} None
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
* @pre bt_sync3d_broadcast_on_off(off) shall be called to stop
* the 3D broadcast
*/
int bt_sync3d_deinit(void);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  Enables the sync 3D profile to prepare for 3D broad cast data.
* @param[in]  on_off, bool value
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int bt_sync3d_broadcast_on_off(bool on_off);

/**
 * @brief  Registers a callback function to be invoked when the 3D glasses connects
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @pre The Sync3D profile must be tunred on.
 * @see bt_sync3d_association_notified_cb()
*/
int bt_sync3d_set_association_notified_cb(bt_sync3d_association_notified_cb callback, void *user_data);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
 * @brief       Unregisters the callback function.
 *
 * @return      0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 *
 * @pre The Bluetooth Sync3D must be initialized with bt_sync3d_broadcast_on_off().
 *
 * @see bt_sync3d_broadcast_on_off()
 * @see bt_sync3d_set_association_notified_cb()
 */
int bt_sync3d_unset_association_notified_cb(void);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
 * @brief  Registers a callback function to be invoked when the sync3d events are generated by the chip
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @pre The Sync3D profile must be tunred on.
 * @see bt_sync3d_event_cb()
*/
int bt_sync3d_set_event_received_cb(bt_sync3d_event_cb callback, void *user_data);

/**
 * @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
 * @brief       Unregisters the callback function.
 *
 * @return      0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 *
 * @pre The Bluetooth Sync3D must be initialized with bt_sync3d_broadcast_on_off().
 *
 * @see bt_sync3d_broadcast_on_off()
 * @see bt_sync3d_set_event_received_cb()
 */
int bt_sync3d_unset_event_received_cb(void);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  Sets the duty value for 3D Broadcast.
* @param[in]  duty, short value
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int bt_sync3d_set_duty_for_broadcast(short duty);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  Sets the duty value for 3D Broadcast.
* @param[in]  delay, short value
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int bt_sync3d_set_dealy_for_broadcast(short delay);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  Sets the DTV mode and the master bd addr.
* @param[in]  mode, sync3d_dtv_mode value
* @param[in]  master_bd_addr, pointer to the master bd address
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int bt_sync3d_set_dtv_mode(bt_sync3d_dtv_mode mode, const char *master_bd_addr);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  Sets the pairing mode.
* @param[in]  on_off, unsigned char value
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int bt_sync3d_set_pairing_mode(bool on_off);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  Sets the veiw type for 3D Broadcast.
* @param[in]  view_type, sync3d_view_type value
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int bt_sync3d_set_view_type(bt_sync3d_view_type view_type);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  Sets the daul audio mode.
* @param[in]  on_off, bool value
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int bt_sync3d_set_dual_audio_mode(bool on_off);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  Gets the sync period set.
* @param[out]  sync_period, unsigned shor value
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int bt_sync3d_get_sync_period(unsigned short *sync_period);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  Sets the RSSI for proximity pairing.
* @param[in]  rssi, short value
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int bt_sync3d_set_rssi_for_proximity_pairing(short rssi);

/**
* @ingroup  CAPI_NETWORK_BLUETOOTH_SYNC3D_MODULE
* @brief  Enable/Disable Sync detect.
* @param[in]  bool, bool value
* @retval  #BLUETOOTH_ERROR_NONE  - Success
* @retval  #BLUETOOTH_ERROR_INTERNAL -Failure
*/
int  bt_sync3d_set_vsync_detect(bool on_off);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Registers a callback function to be invoked when the advertising state changes.
 *
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 * @post bt_adapter_advertising_state_changed_cb() will be invoked.
 *
 * @see bt_initialize()
 * @see bt_adapter_advertising_state_changed_cb()
 * @see bt_adapter_unset_advertising_state_changed_cb()
 */
int bt_adapter_set_advertising_state_changed_cb(bt_adapter_advertising_state_changed_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief	Unregisters the callback function.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 * @see bt_initialize()
 * @see bt_adapter_set_advertising_state_changed_cb()
 */
int bt_adapter_unset_advertising_state_changed_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Starts the advertising with passed parameters.
 *
 * @details Once Bluetooth advertising is started, nearby Bluetooth LE(Low Energy) supported
 * devices can know this device's existence. And one of them can make a connection reqeust,
 * if it is allowed.
 *
 * @param[in]  adv_params  The parameters of advertising \n
 *				If NULL is passed, default values which are defined in driver / controller are used.
 *
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_NOW_IN_PROGRESS  Operation is now in progress
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @post This function invokes bt_adapter_advertising_state_changed_cb().
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_is_advertising()
 * @see bt_adapter_stop_advertising()
 * @see bt_adapter_advertising_state_changed_cb()
 * @see bt_adapter_set_advertising_state_changed_cb()
 * @see bt_adapter_unset_advertising_state_changed_cb()
 */
int bt_adapter_start_advertising(bt_adapter_advertising_params_s *adv_params);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Stops the advertising.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_NOT_IN_PROGRESS  Operation is not in progress
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The advertising must be going on with bt_adapter_start_advertising().
 * @post This function invokes bt_adapter_advertising_state_changed_cb().
 *
 * @see bt_adapter_is_advertising()
 * @see bt_adapter_start_advertising()
 * @see bt_adapter_set_advertising_state_changed_cb()
 * @see bt_adapter_unset_advertising_state_changed_cb()
 * @see bt_adapter_advertising_state_changed_cb()
 */
int bt_adapter_stop_advertising(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Checks for the advertising is going on or stopped.
 *
 * @remarks If Bluetooth advertising is going on, other LE devices are able to
 * search this device.

 * @param[out] is_advertising The advertising status: (@c true = going on, @c  false = stopped)
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_start_advertising()
 * @see bt_adapter_stop_advertising()
 */
int bt_adapter_is_advertising(bool *is_advertising);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief add address to whitelist for accepting scanning request.
 *
 * @remarks If the adress is in the whitelist then other LE devices are able to
 * search this device. Before calling this API, make sure that the adapter is
 * enabled. There is no callback event for this API.

 * @param[in] address The other device's address
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Adapter is not enabled
 * @retval #BT_ERROR_RESOURCE_BUSY  Device or resource busy
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_start_advertising()
 * @see bt_adapter_stop_advertising()
 */
int bt_adapter_add_white_list(const char *address);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief remove address from the whitelist for not accepting scanning request.
 *
 * @remarks If the adress is in the whitelist then other LE devices are able to
 * search this device. Before calling this API, make sure that the adapter is
 * enabled. There is no callback event for this API.
 *
 * @param[in] address The other device's address
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Adapter is not enabled
 * @retval #BT_ERROR_RESOURCE_BUSY  Device or resource busy
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_start_advertising()
 * @see bt_adapter_stop_advertising()
 */
int bt_adapter_remove_white_list(const char *address);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief clear address from the whitelist for not accepting scanning request.
 *
 * @remarks If the adress is in the whitelist then other LE devices are able to
 * search this device. Before calling this API, make sure that the adapter is
 * enabled. There is no callback event for this API.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Adapter is not enabled
 * @retval #BT_ERROR_RESOURCE_BUSY  Device or resource busy
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_start_advertising()
 * @see bt_adapter_stop_advertising()
 */
int bt_adapter_clear_white_list(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Gets the manfucaturer specific data for advertising of local Bluetooth adapter.
 *
 * @details Use this function to get the manfucaturer specific data associated with Bluetooth
 * device, retrieved by the remote Bluetooth devices.
 *
 * @remarks The @a data must be released with free() by you.
 *
 * @param[out]   data	The manufacturer specific data
 * @param[out]   len	The length of @a data
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_set_advertising_manufacturer_data()
 */
int bt_adapter_get_advertising_manufacturer_data(char **data, int *len);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Sets the manufacturer specific data for advertising of local Bluetooth adapter.
 *
 * @param[in]   data	The manufacturer specific data of the Bluetooth device.
 * @param[in]   len	The length of @a data. The maximum length is 26(bytes).
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @post bt_adapter_advertising_manufacturer_data_changed_cb() will be invoked
 * if this function returns #BT_ERROR_NONE.
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_get_advertising_manufacturer_data()
 * @see bt_adapter_advertising_manufacturer_data_changed_cb()
 * @see bt_adapter_set_advertising_manufacturer_data_changed_cb()
 * @see bt_adapter_unset_advertising_manufacturer_data_changed_cb()
 */
int bt_adapter_set_advertising_manufacturer_data(char *data, int len);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Registers a callback function to be invoked
 * when the manufacturer specific data for advertising of Bluetooth adapter changes.
 *
 * @param[in]   callback	The callback function to invoke
 * @param[in]   user_data	The user data to be passed to the callback function
 *
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 * @post  bt_adapter_advertising_manufacturer_data_changed_cb() will be invoked.
 *
 * @see bt_initialize()
 * @see bt_adapter_unset_advertising_manufacturer_data_changed_cb()
 */
int bt_adapter_set_advertising_manufacturer_data_changed_cb(
		bt_adapter_advertising_manufacturer_data_changed_cb callback,
		void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief	Unregisters the callback function.
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 * @see bt_initialize()
 * @see bt_adapter_set_advertising_manufacturer_data_changed_cb()
 */
int bt_adapter_unset_advertising_manufacturer_data_changed_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Gets the manfucaturer specific data for LE scan response of local Bluetooth adapter.
 *
 * @details Use this function to get the manfucaturer specific data associated with Bluetooth
 * device, retrieved by the remote Bluetooth devices.
 *
 * @remarks The @a data must be released with free() by you.
 *
 * @param[out]   data	The manufacturer specific data
 * @param[out]   len	The length of @a data
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_set_scan_resp_manufacturer_data()
 */
int bt_adapter_get_scan_resp_manufacturer_data(char **data, int *len);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Sets the manufacturer specific data for LE scan response of local Bluetooth adapter.
 *
 * @param[in]   data	The manufacturer specific data of the Bluetooth device.
 * @param[in]   len	The length of @a data. The maximum length is 26(bytes).
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @post bt_adapter_scan_resp_manufacturer_data_changed_cb() will be invoked
 * if this function returns #BT_ERROR_NONE.
 *
 * @see bt_adapter_enable()
 * @see bt_adapter_get_scan_resp_manufacturer_data()
 * @see bt_adapter_scan_resp_manufacturer_data_changed_cb()
 * @see bt_adapter_set_scan_resp_manufacturer_data_changed_cb()
 * @see bt_adapter_unset_scan_resp_manufacturer_data_changed_cb()
 */
int bt_adapter_set_scan_resp_manufacturer_data(char *data, int len);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Registers a callback function to be invoked
 * when the manufacturer specific data for LE scan response of Bluetooth adapter changes.
 *
 * @param[in]   callback	The callback function to invoke
 * @param[in]   user_data	The user data to be passed to the callback function
 *
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 * @post  bt_adapter_scan_resp_manufacturer_data_changed_cb() will be invoked.
 *
 * @see bt_initialize()
 * @see bt_adapter_unset_scan_resp_manufacturer_data_changed_cb()
 */
int bt_adapter_set_scan_resp_manufacturer_data_changed_cb(
		bt_adapter_scan_resp_manufacturer_data_changed_cb callback,
		void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief	Unregisters the callback function.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 * @see bt_initialize()
 * @see bt_adapter_set_scan_resp_manufacturer_data_changed_cb()
 */
int bt_adapter_unset_scan_resp_manufacturer_data_changed_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Sets the scan parameters of local Bluetooth adapter.
 *
 * @param[in] interval	Scan interval.
 * @param[in] window	Scan window.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 */
int bt_adapter_set_scan_parameter(float interval, float window);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Sets the specific data for advertising of local Bluetooth adapter.
 *
 * @param[in]   data	The specific data of the Bluetooth device.
 * @param[in]   len	The length of @a data. The maximum length is 31(bytes).
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 * @post bt_adapter_advertising_manufacturer_data_changed_cb() will be invoked
 * if this function returns #BT_ERROR_NONE.
 *
 * @see bt_adapter_enable()
 */
int bt_adapter_set_headless_advertising_data(char *data, int len);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @Open Mobile Headless DB
 *
 * @param[in]  sdb Address of DB

 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BLUETOOTH_ERROR_INTERNAL  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 */
int bt_headless_open_db(void **sdb);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @Close Mobile Headless DB
 *
 * @param[in]  sdb Address of DB

 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BLUETOOTH_ERROR_INTERNAL  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 */
void  bt_mobileheadless_close_db(void **sdb);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @Add the BD address of Mobile in DB
 *
 * @param[in]   remote_address	BD Address of Mobile

 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 */
int bt_bluetooth_add_headless_mobile_in_DB(void *db, const char *remote_address);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @Add the BD address & Device Type of Mobile in DB
 *
 * @param[in]   remote_address	BD Address of Mobile

 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 */
int bt_bluetooth_add_headless_device_in_DB(void *db, const char *remote_address,const char *dev_type);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @Delete the BD address of Mobile in DB
 *
 * @param[in]   db	sqlitedb
 * @param[in]   remote_address	BD Address of Mobile

 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BLUETOOTH_ERROR_INTERNAL  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 */
int bt_bluetooth_delete_headless_mobile_in_DB(void *db, const char *remote_address);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @Get the BD address of Mobile in Present in DB
 *
 * @param[in]   db	sqlitedb
 * @param[in]   dev_list	 List of BD Addresses of Mobile Present in DB

 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BLUETOOTH_ERROR_INTERNAL  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 */
int bt_bluetooth_get_all_headless_mobile_in_DB(void *db, GSList **dev_list);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @Get the BD address and Dev type of Mobile in Present in DB
 *
 * @param[in]   db	sqlitedb
 * @param[in]   dev_list	 List of BD Addresses of Mobile Present in DB

 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BLUETOOTH_ERROR_INTERNAL  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 */
int bt_bluetooth_get_all_headless_device_in_DB(void *db, GSList **dev_list);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @Delete the All BD address of Mobile in DB
 *
 * @param[in]   db	sqlitedb
 * @param[in]   void

 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BLUETOOTH_ERROR_INTERNAL  Operation failed
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED with bt_adapter_enable().
 *
 * @see bt_adapter_enable()
 */
int bt_bluetooth_delete_all_headless_mobile_in_DB(void *db);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief set the dut configure mode
 *
 * @param[in] mode The confiture mode
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_dut_set_configure_mode(bool mode);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief send DUT mode data
 *
 * @param[in] opcode The opcode
 * @param[in] data The mode data
 * @param[in] length The data length
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_dut_sent_mode(unsigned short opcode, unsigned char *data, unsigned short length);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief dut wakeup test
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_dut_wakeup_test(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Set the callback for SmartRC throughput mode
 *
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_hid_set_smartrc_tput_mode_cb(bt_adapter_smartrc_tput_mode_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Unset the callback for SmartRC throughput mode
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_hid_unset_smartrc_tput_mode_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Start the SmartRC throughput mode
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_adapter_start_smartrc_tput_mode(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Stop the SmartRC throughput mode
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_adapter_stop_smartrc_tput_mode(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Get BT chip version.
 *
 * @param[out]   version	BT chip version
 *
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_adapter_get_chip_version(int *version);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Update the firmware
 *
 * @param[in]   type	BT chip type
 *
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_adapter_update_firmware(int chip_type);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Set BT app ready
 *
 * @param[in]   ready
 *
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_adapter_set_app_ready(bool ready);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  If BT chip update is required or not
 *
 * @param[out]   is_required
 *
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_adapter_is_required_update_firmware(bool *is_required);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Cancel BT firmware update
 *
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 *
 */
int bt_adapter_cancel_firmware_update(void);


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __TIZEN_NETWORK_BLUETOOTH_PRODUCT_H__
