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


#ifndef __TIZEN_NETWORK_BLUETOOTH_INTERNAL_H__
#define __TIZEN_NETWORK_BLUETOOTH_INTERNAL_H__

#include <glib.h>
#include "bluetooth_type.h"
#include "bluetooth_type_internal.h"

#ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @file        bluetooth_internal.h
 */

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief Sets the manufacturer data of local Bluetooth adapter.
 * @since_tizen 2.3
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 *
 * @param[in]   data	The manufacturer specific data of the Bluetooth device.
 * @param[in]   len	The length of @a data.Maximaum length is 240 bytes.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 * @post bt_adapter_manufacturer_data_changed_cb() will be invoked
 * if this function returns #BT_ERROR_NONE.
 *
 * @see bt_adapter_manufacturer_data_changed_cb
 * @see bt_adapter_set_manufacturer_data_changed_cb()
 * @see bt_adapter_unset_manufacturer_data_changed_cb()
 */
int bt_adapter_set_manufacturer_data(char *data, int len);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Registers a callback function to be invoked
 * when the manufacturer data of Bluetooth adapter changes.
 * @since_tizen 2.3
 *
 * @param[in]   callback	The callback function to invoke
 * @param[in]   user_data	The user data to be passed to the callback function
 *
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 * @post  bt_adapter_manufacturer_data_changed_cb() will be invoked.
 *
 * @see bt_initialize()
 * @see bt_adapter_unset_manufacturer_data_changed_cb()
 */
int bt_adapter_set_manufacturer_data_changed_cb(
		bt_adapter_manufacturer_data_changed_cb callback,
		void *user_data);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief	Unregisters the callback function.
 * @since_tizen 2.3
 *
 * @return	0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 * @see bt_initialize()
 * @see bt_adapter_set_manufacturer_data_changed_cb()
 */
int bt_adapter_unset_manufacturer_data_changed_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Set Bluetooth LE scan mode
 * @since_tizen 2.3
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in]  scan_mode  The scan mode
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_NOT_ENABLED Adapter is not enabled
 * @retval #BT_ERROR_INVALID_PARAM Parameter is invalid
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 * @pre The Bluetooth service must be initialized with bt_initialize().
 * @see bt_initialize()
 */
int bt_adapter_le_set_scan_mode(bt_adapter_le_scan_mode_e scan_mode);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Creates scan filter to find only LE advertisement which has specific data.
 * @since_tizen 2.4
 *
 * @param[out] scan_filter The handle of scan filter
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 * @see bt_adapter_le_destroy_scan_filter()
 */
int bt_adapter_le_create_scan_filter(bt_scan_filter_h *scan_filter);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Destroys scan filter.
 * @since_tizen 2.4
 *
 * @param[in] scan_filter The handle of scan filter
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 * @see bt_adapter_le_create_scan_filter()
 */
int bt_adapter_le_destroy_scan_filter(bt_scan_filter_h scan_filter);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Adds the device address to filter advertisements
 * @since_tizen 2.4
 *
 * @param[in] scan_filter The handle of scan filter
 * @param[in] address The device address to filter advertisements
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_scan_filter_set_device_address(bt_scan_filter_h scan_filter, const char *address);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Adds the device name to filter advertisements
 * @since_tizen 2.4
 *
 * @param[in] scan_filter The handle of scan filter
 * @param[in] name The device name to filter advertisements
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED	Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_scan_filter_set_device_name(bt_scan_filter_h scan_filter, const char *name);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Adds the service UUID to filter advertisements
 * @since_tizen 2.4
 *
 * @param[in] scan_filter The handle of scan filter
 * @param[in] uuid The service UUID to filter advertisements
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED	Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_scan_filter_set_service_uuid(bt_scan_filter_h scan_filter, const char *uuid);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Adds the service uuid and the mask to filter advertisements by partial data
 * @since_tizen 2.4
 *
 * @remarks the length of mask msut be the same with the length of service uuid.
 *
 * @param[in] scan_filter The handle of scan filter
 * @param[in] uuid The service UUID to filter advertisements
 * @param[in] mask The mask to filter advertisements
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED	Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_scan_filter_set_service_uuid_with_mask(bt_scan_filter_h scan_filter,
			const char *uuid, const char *mask);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Adds the service solicitation UUID to filter advertisements
 * @since_tizen 2.4
 *
 * @param[in] scan_filter The handle of scan filter
 * @param[in] uuid The service solicitation UUID to filter advertisements
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED	Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_scan_filter_set_service_solicitation_uuid(bt_scan_filter_h scan_filter, const char *uuid);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Adds the service solicitation uuid and the mask to filter advertisements by partial data
 * @since_tizen 2.4
 *
 * @remarks the length of mask msut be the same with the length of service solicitation uuid.
 *
 * @param[in] scan_filter The handle of scan filter
 * @param[in] uuid The service solicitation UUID to filter advertisements
 * @param[in] mask The mask to filter advertisements
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED	Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_scan_filter_set_service_solicitation_uuid_with_mask(bt_scan_filter_h scan_filter,
			const char *uuid, const char *mask);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Adds the service data to filter advertisements
 * @since_tizen 2.4
 *
 * @param[in] scan_filter The handle of scan filter
 * @param[in] uuid The service UUID to filter advertisements
 * @param[in] data The service data to filter advertisements
 * @param[in] data_len The length of the service data
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED	Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_scan_filter_set_service_data(bt_scan_filter_h scan_filter,
			const char *uuid, const char *data, unsigned int data_len);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Adds the service data and the mask to filter advertisements by partial data
 * @since_tizen 2.4
 *
 * @remarks the length of mask msut be the same with the length of service data.
 *
 * @param[in] scan_filter The handle of scan filter
 * @param[in] uuid The service UUID to filter advertisements
 * @param[in] data The service data to filter advertisements
 * @param[in] data_len The length of the service data
 * @param[in] mask The mask to filter advertisements
 * @param[in] mask_len The length of the mask to be set.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED	Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_scan_filter_set_service_data_with_mask(bt_scan_filter_h scan_filter,
			const char *uuid, const char *data, unsigned int data_len, const char *mask, unsigned int mask_len);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Adds the manufacturer data to filter advertisements
 * @since_tizen 2.4
 *
 * @param[in] scan_filter The handle of scan filter
 * @param[in] manufacturer_id The manufacturer ID (0x0000 ~ 0xFFFF)
 * @param[in] data The manufacturer data (byte array)
 * @param[in] data_len The length of manufacturer data
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED	Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_scan_filter_set_manufacturer_data(bt_scan_filter_h scan_filter,
			int manufacturer_id, const char *data, unsigned int data_len);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Adds the manufacturer data and the mask to filter advertisements by partial data
 * @since_tizen 2.4
 *
 * @remarks the length of mask msut be the same with the length of service uuid.
 *
 * @param[in] scan_filter The handle of scan filter
 * @param[in] manufacturer_id The manufacturer ID (0x0000 ~ 0xFFFF)
 * @param[in] data The manufacturer data (byte array)
 * @param[in] data_len The length of manufacturer data
 * @param[in] mask The mask to filter advertisements
 * @param[in] mask_len The length of the mask to be set.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER	Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED	Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_scan_filter_set_manufacturer_data_with_mask(bt_scan_filter_h scan_filter,
			int manufacturer_id, const char *data, unsigned int data_len, const char *mask, unsigned int mask_len);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Registers the scan filter to use for scanning
 * @since_tizen 2.4
 *
 * @remarks Several scan filters can be registered. And the specific advertisements, satisfy the one of scan filters, will be found.
 *
 * @param[in] scan_filter The handle of scan filter
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOW_IN_PROGRESS  Scan is in progress
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_register_scan_filter(bt_scan_filter_h scan_filter);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Unregisters the scan filter to be registered
 * @since_tizen 2.4
 *
 * @param[in] scan_filter The handle of scan filter
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOW_IN_PROGRESS  Scan is in progress
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_unregister_scan_filter(bt_scan_filter_h scan_filter);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Unregisters all scan filters to be registered
 * @since_tizen 2.4
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOW_IN_PROGRESS  Scan is in progress
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 */
int bt_adapter_le_unregister_all_scan_filters(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief Set advertising filter policy to use white list
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/bluetooth
 *
 * @param[in] advertiser The handle of advertiser
 * @param[in] filter_policy The filter policy of advertising
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_NOW_IN_PROGRESS  Operation is now in progress
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized with bt_initialize().
 *
 * @see bt_adapter_le_start_advertising_new()
 */
int bt_adapter_le_set_advertising_filter_policy(bt_advertiser_h advertiser, bt_adapter_le_advertising_filter_policy_e filter_policy);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief Gets a connection state
 * @since_tizen 2.4
 *
 * @param[in] remote_address The address of the remote Bluetooth device
 * @param[in] link_type The link type to get a connection state
 * @param[out] connected The connection state
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED   Operation failed
 */
int bt_device_get_connection_state(const char *remote_address, bt_device_connection_link_type_e link_type, bool *connected);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_SOCKET_MODULE
 * @brief Registers a rfcomm server socket with a specific UUID. Activation by dbus is possible when the profile is connected.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 *
 * @remarks A socket can be destroyed by bt_socket_destroy_rfcomm_ex().
 * Application should call this API to receive a connection event when launched again by dbus.
 *
 * @param[in] uuid The UUID of service to provide
 * @param[in] bus_name bus_name of the application which is provided in service file.
 * @param[in] object_path dbus of the application
 * @return 0 on success, otherwise a negative error value.
 *
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_NOW_IN_PROGRESS  Already registered
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 *
 * @see bt_socket_listen_and_accept_rfcomm_ex()
 * @see bt_socket_destroy_rfcomm_ex()
 */
int bt_socket_create_rfcomm_ex(const char *uuid, const char *bus_name, const char *object_path);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_SOCKET_MODULE
 * @brief Removes the rfcomm server socket which was created using bt_socket_create_rfcomm_ex().
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @remarks If callback function bt_socket_connection_state_changed_cb() is set and the remote Bluetooth device is connected,
 * then bt_socket_connection_state_changed_cb() will be called when this function is finished successfully.
 *
 * @param[in] uuid The UUID (which was created using bt_socket_create_rfcomm()) to destroy
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The socket must be created with bt_socket_create_rfcomm_ex().
 * @post If callback function bt_socket_connection_state_changed_cb() is set and the remote Bluetooth device is connected,
 * then bt_socket_connection_state_changed_cb() will be called.
 * @see bt_socket_create_rfcomm_ex()
 * @see bt_socket_connection_state_changed_cb()
 * @see bt_socket_set_connection_state_changed_cb()
 * @see bt_socket_unset_connection_state_changed_cb()
 */
int bt_socket_destroy_rfcomm_ex(const char *uuid);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_SOCKET_MODULE
 * @brief Starts listening on passed rfcomm socket and accepts connection requests. Activation by dbus is possible when the profile is connected.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @details Pop-up is shown automatically when a RFCOMM connection is requested.
 * bt_socket_connection_state_changed_cb() will be called with
 * #BT_SOCKET_CONNECTED if you click "yes" and connection is finished successfully.
 * @param[in] uuid The UUID of service to provide
 * @param[in] max_pending_connections The maximum number of pending connections
 * @param[in] bus_name bus_name of the application which is provided in service file.
 * @param[in] object_path dbus of the application
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The socket must be created with bt_socket_create_rfcomm_ex().
 * @post If callback function bt_socket_connection_state_changed_cb() is set,
 * then bt_socket_connection_state_changed_cb() will be called when the remote Bluetooth device is connected.
 * @see bt_socket_create_rfcomm_ex()
 * @see bt_socket_connection_state_changed_cb()
 * @see bt_socket_set_connection_state_changed_cb()
 * @see bt_socket_unset_connection_state_changed_cb()
 */
int bt_socket_listen_and_accept_rfcomm_ex(const char *uuid, int max_pending_connections, const char* bus_name, const char *object_path);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_AG_MODULE
 * @brief Checks whether the remoted device is wbs (Wide Band Speech) mode or not.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[out] wbs_mode The wbs status: (@c true = wide band speech, @c  false = narrow band speech)
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The remote device is connected by bt_audio_connect() with #BT_AUDIO_PROFILE_TYPE_HSP_HFP service.
 * @see bt_audio_connect()
 */
int bt_ag_is_wbs_mode(bool *wbs_mode);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_AG_MODULE
 * @brief Gets the HF(Hands-Free) profile connected status for AG role.
 * @since_tizen 2.4
 * @param[out] connected the connected status: (@c true = connected , @c  false = not connected )
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED	Operation failed
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 */
int bt_ag_is_connected(bool *connected);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Registers a callback function that will be invoked when the SCO(Synchronous Connection Oriented link) state is changed.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_ag_sco_state_changed_cb()
 * @see bt_ag_unset_sco_state_changed_cb()
 */
int bt_hf_set_sco_state_changed_cb(bt_hf_sco_state_changed_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Unregisters a callback function that will be invoked when the SCO(Synchronous Connection Oriented link) state is changed.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_ag_sco_state_changed_cb()
 * @see bt_ag_set_sco_state_changed_cb()
 */
int bt_hf_unset_sco_state_changed_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Registers a callback function that will be invoked when a call handling event happened from Hands-Free.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_ag_call_handling_event_cb()
 * @see bt_ag_unset_call_handling_event_cb()
 */
int bt_hf_set_call_handling_event_cb(bt_hf_call_handling_event_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Unregisters a callback function that will be invoked when a call handling event happened from Hands-Free.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_ag_call_handling_event_cb()
 * @see bt_ag_set_call_handling_event_cb()
 */
int bt_hf_unset_call_handling_event_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Registers a callback function that will be invoked when a multi call handling event happened from Hands-Free.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_ag_multi_call_handling_event_cb()
 * @see bt_ag_unset_multi_call_handling_event_cb()
 */
int bt_hf_set_multi_call_handling_event_cb(bt_hf_multi_call_handling_event_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Unregisters a callback function that will be invoked when a multi call handling event happened from Hands-Free.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_ag_multi_call_handling_event_cb()
 * @see bt_ag_set_multi_call_handling_event_cb()
 */
int bt_hf_unset_multi_call_handling_event_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief Initializes the Bluetooth HF profile related with audio.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @remarks This function must be called before Bluetooth profiles related with audio starts.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth service must be initialized with bt_initialize().
 * @see bt_hf_deinitialize()
 */
int bt_hf_initialize(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief Deinitializes the Bluetooth HF profile related with audio.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 */
int bt_hf_deinitialize(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief Notifies the call event to the remote bluetooth device.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 * @remarks Before notifying #BT_HF_CALL_EVENT_ANSWERED or #BT_HF_CALL_EVENT_DIALING, you should open SCO(Synchronous Connection Oriented link)
 * if Bluetooth Hands-Free need SCO connection.
 * @param[in] event  The call event
 * @param[in] call_id  The call ID
 * @param[in] phone_number  The phone number. You must set this value in case of #BT_HF_CALL_EVENT_DIALING and #BT_HF_CALL_EVENT_INCOMING.
 * In other cases, this value can be NULL.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio device must be connected with bt_audio_connect().
 * @see bt_audio_connect()
 */
int bt_hf_notify_call_event(bt_hf_call_event_e event, char *phone_number);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Notifies the speaker gain to the remote device.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 * @details This function sends a signal to the remote device. This signal has the gain value.
 * @a gain is represented on a scale from 0 to 15. This value is absolute value relating to a particular volume level.
 * When the speaker gain of remote device is changed to the requested gain, bt_audio_speaker_gain_changed_cb() will be called.
 * @param[in] gain The gain of speaker (0 ~ 15)
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The remote device is connected by bt_audio_connect() with #BT_AUDIO_PROFILE_TYPE_HSP_HFP service.
 * @see bt_hf_get_speaker_gain()
 * @see bt_hf_set_speaker_gain_changed_cb()
 * @see bt_hf_unset_speaker_gain_changed_cb()
 */
int bt_hf_notify_speaker_gain(int gain);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Registers a callback function that will be invoked when the speaker gain of the remote device is changed.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @details This function let you know the change of the speaker gain of the remote device.
 * @a gain is represented on a scale from 0 to 15. This value is absolute value relating to a particular volume level.
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_hf_unset_speaker_gain_changed_cb()
 */
int bt_hf_set_speaker_gain_changed_cb(bt_hf_speaker_gain_changed_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Unregisters a callback function that will be invoked when the speaker gain of the remote device is changed.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_hf_set_speaker_gain_changed_cb()
 */
int bt_hf_unset_speaker_gain_changed_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief Notifies the state of voice recognition.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 * @param[in] state  The state of voice recognition: (@c true = enabled, @c  false = disabled)
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio device must be connected with bt_audio_connect().
 * @see bt_audio_connect()
 */
int bt_hf_notify_voice_recognition_state(bool state);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Registers a callback function that will be invoked when a call status event happened from Hands-Free.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_hf_unset_call_status_updated_event_cb()
 */
int bt_hf_set_call_status_updated_event_cb(bt_hf_call_status_updated_event_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Unregisters a callback function that will be invoked when a call status event happened from Hands-Free.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_hf_set_call_status_updated_event_cb()
 */
int bt_hf_unset_call_status_updated_event_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief Closes an opened SCO(Synchronous Connection Oriented link), synchronously.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 */
int bt_hf_close_sco(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief Sends the DTMF(Dual Tone Multi Frequency).
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 * @param[in] dtmf  The DTMF to send
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio device must be connected with bt_audio_connect().
 * @see bt_audio_connect()
 */
int bt_hf_send_dtmf(char *dtmf);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief Gets the HF(Hands-Free) profile connected status for HF role.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @param[out] connected the connected status: (@c true = connected , @c  false = not connected )
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED	Operation failed
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 */
int bt_hf_is_connected(bool *connected);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief Checks whether an opened SCO(Synchronous Connection Oriented link) exists or not.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 *
 * @param[out] opened The SCO status: (@c true = opened, @c  false = not opened)
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED Operation failed
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth audio device must be connected with bt_audio_connect().
 * @see bt_audio_connect()
 */
int bt_hf_is_sco_opened(bool *opened);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief Gets the codec ID.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @remarks The @a codec_id must be released with free() by you.
 *
 * @param[out] codec_id The codec ID
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED Operation failed
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth audio device must be connected with bt_audio_connect().
 * @see bt_audio_connect()
 */
int bt_hf_get_codec_id(unsigned int *codec_id);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief Gets the call status information list.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @remarks The @a call_list must be released with bt_hf_free_call_status_info_list() by you.
 * @remarks call_info_list has elements which consist of bt_hf_call_status_info_s
 *
 * @param[out] call_list The call status information list
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED Operation failed
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth audio device must be connected with bt_audio_connect().
 * @see bt_hf_call_status_info_s
 * @see bt_audio_connect()
 */
int bt_hf_get_call_status_info_list(GSList **call_list);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief Frees the call status information list.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 *
 * @param[in] call_list The call status information list
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_hf_get_call_status_info_list()
 */
int bt_hf_free_call_status_info_list(GSList *call_list);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief Initializes the Bluetooth PBAP Client.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @details Initializes the Bluetooth PBAP Client.
 *  This initialization is necessary to call other PBAP client APIs.
 *
 * @remarks This function must be called to initiatize Bluetooth PBAP client. You must free all resources of the Bluetooth service
 * by calling bt_pbap_deinit() if Bluetooth PBAP Client is no longer needed.
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 *
 * @see  bt_pbap_deinit()
 * @see  bt_pbap_connect()
 * @see  bt_pbap_disconnect()
 * @see  bt_pbap_get_phonebook_size()
 * @see  bt_pbap_get_phonebook()
 * @see  bt_pbap_get_list()
 * @see  bt_pbap_pull_vcard()
 * @see  bt_pbap_phonebook_search()
 */
int bt_pbap_init(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief De-initializes the Bluetooth PBAP Client.
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @details De-Initializes the Bluetooth PBAP Client.
 *  This deinitialization must be done to free resource when PBAP client is not longer needed.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 *	PBAP client must be initialized with bt_pbap_init()
 * @see  bt_pbap_deinit()
 * @see  bt_pbap_connect()
 * @see  bt_pbap_disconnect()
 * @see  bt_pbap_get_phonebook_size()
 * @see  bt_pbap_get_phonebook()
 * @see  bt_pbap_get_list()
 * @see  bt_pbap_pull_vcard()
 * @see  bt_pbap_phonebook_search()
 */
int bt_pbap_deinit(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief To connect to PBAP server
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 *
 * @param[in] address The other device's address
 * @param[in] Callback to be called when PBAP session is Connected.
 * @param[in] user_data Data to be passed to PBAP enabled/disabled callback.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Adapter is not enabled
 * @retval #BLUETOOTH_ERROR_ALREADY_CONNECT PBAP client already connected
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 *	PBAP client must be initialized with bt_pbap_init()
 * @see bt_pbap_disconnect()
 */
int bt_pbap_connect(const char *address, bt_pbap_enabled_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief To disconnect from PBAP server
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 *
 * @param[in] address The other device's address
 * @param[in] Callback to be called when PBAP session is disconnected.
 * @param[in] user_data Data to be passed to PBAP enabled/disabled callback.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Adapter is not enabled
 * @retval #BLUETOOTH_ERROR_NOT_CONNECTED PBAP Client is not connected
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 * 	PBAP client must be initialized with bt_pbap_init()
 *	PBAP connection must be created with bt_pbap_connect().
 *
 * @see bt_pbap_connect()
 */
int bt_pbap_disconnect(const char *address, bt_pbap_enabled_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief To get size of phonebook from PBAP server
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 *
 * @param[in] address The other device's address
 * @param[in] source Source of Phonebook (Phone/SIM)
 * @param[in] type Type of folder
 * @param[in] Callback to be called when PBAP Phonebook Size is Returned.
 * @param[in] user_data Data to be passed to the PBAP Phonebook Size callback.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Adapter is not enabled
 * @retval #BLUETOOTH_ERROR_NOT_CONNECTED PBAP Client is not connected
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 *	PBAP client must be initialized with bt_pbap_init()
 *	PBAP connection must be created with bt_pbap_connect().
 *
 * @see bt_pbap_connect()
 */
int bt_pbap_get_phonebook_size(const char *address, bt_pbap_addressbook_source_e source,
		bt_pbap_folder_type_e type, bt_pbap_phonebook_size_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief To get contacts/call logs as vCards from PBAP server
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 *
 * @param[in] address The other device's address
 * @param[in] source Source of Phonebook (Phone/SIM)
 * @param[in] type Type of folder
 * @param[in] format Format of vCard
 * @param[in] order Specifies which field shall be used to sort vCards.
 * @param[in] offset vCards to be excluded from beginning.
 * @param[in] maxlistcount Maximum number of vCards to be fetched
 * @param[in] fields Fields of vCard to be fetched.
 * @param[in] Callback to be called when PBAP Phonebook is Pulled.
 * @param[in] user_data Data to be passed to the PBAP Phonebook Pull callback.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Adapter is not enabled
 * @retval #BLUETOOTH_ERROR_NOT_CONNECTED PBAP Client is not connected
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 *	PBAP client must be initialized with bt_pbap_init()
 *	PBAP connection must be created with bt_pbap_connect().
 *
 * @see bt_pbap_connect()
 */
int bt_pbap_get_phonebook(const char *address, bt_pbap_addressbook_source_e source,
		bt_pbap_folder_type_e type, bt_pbap_filter_vcard_format_e format,
		bt_pbap_filter_sort_order_e order, unsigned short offset,
		unsigned short maxlistcount, long long unsigned fields,
		bt_pbap_phonebook_pull_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief To get Contact or Call List from PBAP server
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 *
 * @param[in] address The other device's address
 * @param[in] source Source of Phonebook (Phone/SIM)
 * @param[in] type Type of folder
 * @param[in] order Specifies which field shall be used to sort vCards.
 * @param[in] offset vCards to be excluded from beginning.
 * @param[in] maxlistcount Maximum number of vCards to be fetched
 * @param[in] Callback to be called when PBAP List is returned.
 * @param[in] user_data Data to be passed to the PBAP Phonebook Pull callback.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Adapter is not enabled
 * @retval #BLUETOOTH_ERROR_NOT_CONNECTED PBAP Client is not connected
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 *	PBAP client must be initialized with bt_pbap_init()
 *	PBAP connection must be created with bt_pbap_connect().
 *
 * @see bt_pbap_connect()
 */
int bt_pbap_get_list(const char *address, bt_pbap_addressbook_source_e source,
		bt_pbap_folder_type_e type, bt_pbap_filter_sort_order_e order,
		unsigned short offset, unsigned short maxlistcount,
		bt_pbap_list_vcards_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief To get a contact/call log from PBAP server
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 *
 * @param[in] address The other device's address
 * @param[in] source Source of Phonebook (Phone/SIM)
 * @param[in] type Type of folder
 * @param[in] index Handle of vCard to be fetched
 * @param[in] format Format of vCard
 * @param[in] fields Fields of vCard to be fetched.
 * @param[in] Callback to be called when PBAP Phonebook is Pulled.
 * @param[in] user_data Data to be passed to the PBAP Phonebook Pull callback.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Adapter is not enabled
 * @retval #BLUETOOTH_ERROR_NOT_CONNECTED PBAP Client is not connected
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 *	PBAP client must be initialized with bt_pbap_init()
 *	PBAP connection must be created with bt_pbap_connect().
 *
 * @see bt_pbap_connect()
 */
int bt_pbap_pull_vcard(const char *address, bt_pbap_addressbook_source_e source,
		bt_pbap_folder_type_e type, int index,
		bt_pbap_filter_vcard_format_e format, long long unsigned fields,
		bt_pbap_get_vcard_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief To get Contact or Call List from PBAP server
 * @since_tizen @if MOBILE @elseif WEARABLE 2.3.1 @endif
 * @privlevel platform
 * @privilege http://tizen.org/privilege/bluetooth.admin
 *
 * @param[in] address The other device's address
 * @param[in] source Source of Phonebook (Phone/SIM)
 * @param[in] type Type of folder
 * @param[in] search_attribute field to be search
 * @param[in] search_value pattern to be searched for
 * @param[in] order Specifies which field shall be used to sort vCards.
 * @param[in] offset vCards to be excluded from beginning.
 * @param[in] maxlistcount Maximum number of vCards to be fetched
 * @param[in] Callback to be called when PBAP List is returned.
 * @param[in] user_data Data to be passed to the PBAP Phonebook Pull callback.
 *
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Adapter is not enabled
 * @retval #BLUETOOTH_ERROR_NOT_CONNECTED PBAP Client is not connected
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 *	PBAP client must be initialized with bt_pbap_init()
 *	PBAP connection must be created with bt_pbap_connect().
 *
 * @see bt_pbap_connect()
 */
int bt_pbap_phonebook_search(const char *address,
		bt_pbap_addressbook_source_e source, bt_pbap_folder_type_e type,
		bt_pbap_search_field_e search_attribute, const char *search_value,
		bt_pbap_filter_sort_order_e order,
		unsigned short offset, unsigned short maxlistcount,
		bt_pbap_search_list_cb callback, void *user_data);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief Gets the specification name from the UUID
 * @since_tizen 2.4
 *
 * @remarks @a name must be released with free() by you.
 *
 * @param[in] uuid The UUID
 * @param[out] name The specification name which defined from www.bluetooth.org
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_get_uuid()
 */
int bt_gatt_get_uuid_specification_name(const char *uuid, char **name);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief Destroys the GATT handle
 * @since_tizen 2.4
 *
 * @param[in] gatt_handle The handle of service, characteristic or descriptor
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_service_create()
 * @see bt_gatt_characteristic_create()
 * @see bt_gatt_descriptor_create()
 */
int bt_gatt_destroy(bt_gatt_h gatt_handle);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Gets the permissions which a characteristic or descriptor's GATT handle has
 * @since_tizen 2.4
 *
 * @param[in] gatt_handle The handle of a characteristic or descriptor
 * @param[out] permissions The permissions which a characteristic or descriptor's GATT handle has.
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_characteristic_create()
 * @see bt_gatt_descriptor_create()
 * @see bt_gatt_permission_e
 */
int bt_gatt_get_permissions(bt_gatt_h gatt_handle, int *permissions);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Updates the permissions which a characteristic or descriptor's GATT handle has
 * @since_tizen 2.4
 *
 * @param[in] gatt_handle The handle of a characteristic or descriptor
 * @param[in] permissions The permissions to be updated
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_characteristic_create()
 * @see bt_gatt_descriptor_create()
 * @see bt_gatt_permission_e
 */
int bt_gatt_set_permissions(bt_gatt_h gatt_handle, int permissions);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Creates the GATT service
 * @since_tizen 2.4
 *
 * @param[in] uuid The UUID of the service
 * @param[in] type The type of the service
 * @param[out] service The GATT handle of the created service
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_destroy()
 */
int bt_gatt_service_create(const char *uuid, bt_gatt_service_type_e type,
			   bt_gatt_h *service);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Adds a characteristic to a specified service
 * @since_tizen 2.4
 *
 * @param[in] service The service's GATT handle
 * @param[in] characteristic The characteristic's GATT handle to be added
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_service_create()
 * @see bt_gatt_characteristic_create()
 */
int bt_gatt_service_add_characteristic(bt_gatt_h service,
					bt_gatt_h characteristic);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Adds a service to a specified service as included service
 * @since_tizen 2.4
 *
 * @param[in] service The service's GATT handle
 * @param[in] included_service The service's GATT handle to be added as included service
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_service_create()
 */
int bt_gatt_service_add_included_service(bt_gatt_h service,
					bt_gatt_h included_service);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Gets the GATT server handle to which the specified service belongs
 * @since_tizen 2.4
 *
 * @param[in] service The service's GATT handle
 * @param[out] server The GATT server handle to which @a service belongs
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_server_create()
 * @see bt_gatt_service_create()
 * @see bt_gatt_server_register_service()
 */
int bt_gatt_service_get_server(bt_gatt_h service, bt_gatt_server_h *server);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Creates the GATT characteristic
 * @since_tizen 2.4
 *
 * @param[in] uuid The UUID of the characteristic
 * @param[in] permissions the permissions of the characteristic
 * @param[in] properties The properties of the characteristic
 * @param[in] Value The value(byte stream) of the characteristic
 * @param[in] value_length The length of @a value
 * @param[out] characteristic The GATT handle of the created characteristic
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_destroy()
 */
int bt_gatt_characteristic_create(const char *uuid, int permissions,
				int properties, const char *value, int value_length,
				bt_gatt_h *characteristic);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Adds a descriptor to a specified characteristic
 * @since_tizen 2.4
 *
 * @param[in] characteristic The GATT handle of the characteristic
 * @param[in] descriptor The descriptor's GATT handle to be added
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_characteristic_create()
 * @see bt_gatt_descriptor_create()
 */
int bt_gatt_characteristic_add_descriptor(bt_gatt_h characteristic,
					bt_gatt_h descriptor);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Creates the GATT characteristic descriptor
 * @since_tizen 2.4
 *
 * @param[in] uuid The UUID of the descriptor
 * @param[in] permissions The permissions of the descriptor
 * @param[in] value The value(byte stream) associated with the descriptor
 * @param[in] value_length The length of @a value
 * @param[out] descriptor The GATT handle of the created characteristic descriptor
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_destroy()
 */
int bt_gatt_descriptor_create(const char *uuid, int permissions,
				const char *value, int value_length,
				bt_gatt_h *descriptor);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief Initializes the GATT Server
 * @since_tizen 2.4
 *
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre	The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 * @see	bt_gatt_server_deinitialize()
 */
int bt_gatt_server_initialize(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief DeInitializes the GATT server
 * @since_tizen 2.4
 *
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE	Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre	The state of local Bluetooth must be #BT_ADAPTER_ENABLED.
 * @see	bt_gatt_server_initialize()
 */
int bt_gatt_server_deinitialize(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Creates the GATT server's handle
 * @since_tizen 2.4
 *
 * @param[out] server The GATT server's handle
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OUT_OF_MEMORY  Out of memory
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_server_destroy()
 */
int bt_gatt_server_create(bt_gatt_server_h *server);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Destroys the GATT server's handle
 * @since_tizen 2.4
 *
 * @remarks All registered services to GATT server are unregistered
 *
 * @param[in] server The GATT server's handle
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_server_create()
 */
int bt_gatt_server_destroy(bt_gatt_server_h server);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Registers a callback function to be invoked when a read request for a specified characteristic or descriptor is issued from a remote device(GATT client)
 * @since_tizen 2.4
 *
 * @param[in] gatt_handle The GATT handle of a characteristic or descriptor
 * @param[in] callback The callback to be invoked
 * @param[in] user_data The user data to be passed to @a callback function
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_server_read_value_requested_cb()
 * @see bt_gatt_server_send_response()
 */
int bt_gatt_server_set_read_value_requested_cb(bt_gatt_h gatt_handle,
				bt_gatt_server_read_value_requested_cb callback,
				void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Registers a callback function to be invoked when a value of a characteristic or descriptor has been changed by a remote device(GATT client)'s request
 * @since_tizen 2.4
 *
 * @param[in] gatt_handle The GATT handle of a characteristic or descriptor
 * @param[in] callback The callback to be invoked
 * @param[in] user_data The user data to be passed to @a callback function
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_server_value_changed_cb()
 */
int bt_gatt_server_set_value_changed_cb(bt_gatt_h gatt_handle,
				bt_gatt_server_value_changed_cb callback,
				void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Registers a specified service to the specified GATT server that the local device is hosting
 * @since_tizen 2.4
 * @privlevel public
 * @privilege %http://tizen.org/privilege/bluetooth
 *
 * @param[in] server The GATT server that local device is hosting
 * @param[in] service The service, which needs to be registered in @a server
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_ALREADY_DONE Operation is already done
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_server_create()
 * @see bt_gatt_service_create()
 * @see bt_gatt_server_unregister_service()
 */
int bt_gatt_server_register_service(bt_gatt_server_h server, bt_gatt_h service);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Unregisters a specified service from the specified GATT server that the local device is hosting
 * @since_tizen 2.4
 *
 * @param[in] server The GATT server that local device is hosting
 * @param[in] service The service, which needs to be unregistered from @a server
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_SERVICE_NOT_FOUND  Service not found
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_server_register_service()
 */
int bt_gatt_server_unregister_service(bt_gatt_server_h server, bt_gatt_h service);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Unregisters all services from the specified GATT server that the local device is hosting
 * @since_tizen 2.4
 *
 * @param[in] server The GATT server that local device is hosting
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_server_register_service()
 */
int bt_gatt_server_unregister_all_services(bt_gatt_server_h server);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Sends a response to the remote device as a result of a read request
 * @since_tizen 2.4
 *
 * @remarks Until this function is called, a read request is not finished.
 *
 * @param[in] request_id The identification of a read request
 * @param[in] offset The offset from where a value is read
 * @param[in] value The value to be sent. It will be sent from @a offset. If it is NULL, a requested GATT handle's value will be sent from @a offset.
 * @param[in] value_length Value Length
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_server_read_value_requested_cb()
 */
int bt_gatt_server_send_response(int request_id,
			int offset, char *value, int value_length);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Notifys a value change of the characteristic to the remote devices which enable a Client Characteristic Configuration Descriptor
 * @since_tizen 2.4
 *
 * @param[in] characteristic The characteristic which has a changed value
 * @param[in] need_confirm If it is true, an indication is sent. If it is false, a notification is sent.
 * @param[in] callback The function to be invoked on each remote device when a sending operation is done
 * @param[in] user_data The user data to be passed to @a callback function
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_server_notification_sent_cb()
 */
int bt_gatt_server_notify(bt_gatt_h characteristic, bool need_confirm,
		bt_gatt_server_notification_sent_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Gets a service's GATT handle which has specific UUID
 * @since_tizen 2.4
 *
 * @remark The returned GATT handle must not be freed by application.
 * It will be freed when an associated server is destroyed by bt_gatt_server_destroy().
 * If there are multiple services which have same UUID, only the first matched one will be returned.
 *
 * @param[in] server The GATT server's handle
 * @param[in] uuid The serivce's GATT handle which has this UUID will be returned if it exists
 * @param[out] service The service's GATT handle which has @a uuid if it exists
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NO_DATA  No data available
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 */
int bt_gatt_server_get_service(bt_gatt_server_h server, const char *uuid,
				bt_gatt_h *service);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Invokes @a callback function on each service that belongs to the specified GATT server
 * @since_tizen 2.4
 *
 * @param[in] server The GATT server's handle
 * @param[in] callback The function to be invoked on each service
 * @param[in] user_data The user data to be passed to @a callback function
 * @return  0 on success, otherwise a negative error value
 * @retval #BT_ERROR_NONE Successful
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @see bt_gatt_foreach_cb()
 */
int bt_gatt_server_foreach_services(bt_gatt_server_h server,
		bt_gatt_foreach_cb callback, void *user_data);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Registers a callback function that will be invoked when the song position is changed by the remote target device.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @see bt_initialize()
 * @see bt_avrcp_unset_song_position_changed_cb()
 */
int bt_avrcp_set_song_position_changed_cb(bt_avrcp_song_position_changed_cb callback, void *user_data);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Unregisters a callback function that will be invoked when the song position is changed by the remote target device.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @see bt_initialize()
 * @see bt_avrcp_set_song_position_changed_cb()
 */
int bt_avrcp_unset_song_position_changed_cb(void);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Registers a callback function that will be invoked when the Play status is changed by the remote target device.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @see bt_initialize()
 * @see bt_avrcp_unset_play_status_changed_cb()
 */
int bt_avrcp_set_play_status_changed_cb(bt_avrcp_play_status_changed_cb callback, void *user_data);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Unregisters a callback function that will be invoked when the Play status is changed by the remote target device.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @see bt_initialize()
 * @see bt_avrcp_unset_play_status_changed_cb()
 */
int bt_avrcp_unset_play_status_changed_cb(void);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Registers a callback function that will be invoked when the track metadata is changed by the remote target device.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @see bt_initialize()
 * @see bt_avrcp_unset_track_info_changed_cb()
 */
int bt_avrcp_set_track_info_changed_cb(bt_avrcp_track_info_changed_cb callback, void *user_data);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Unregisters a callback function that will be invoked when the track metadata is changed by the remote target device.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth service must be initialized by bt_initialize().
 * @see bt_initialize()
 * @see bt_avrcp_set_track_info_changed_cb()
 */
int bt_avrcp_unset_track_info_changed_cb(void);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Registers a callback function that will be invoked when the A2DP Source connection state is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @param[in] callback The callback function to register
 * @param[in] user_data The user data to be passed to the callback function
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_audio_connection_state_changed_cb()
 */
int bt_a2dp_source_audio_set_connection_state_changed_cb(bt_audio_connection_state_changed_cb callback, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_MODULE
 * @brief  Unregisters a callback function that will be invoked when the A2DP Source connection state is changed.
 * @since_tizen @if MOBILE 2.3 @elseif WEARABLE 2.3.1 @endif
 * @return   0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The Bluetooth audio service must be initialized with bt_audio_initialize().
 * @see bt_audio_initialize()
 * @see bt_audio_connection_state_changed_cb()
 * @see bt_audio_set_connection_state_changed_cb()
 */
int bt_a2dp_source_audio_unset_connection_state_changed_cb(void);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief Initializes the Bluetooth AVRCP(Audio/Video Remote Control Profile) service.
 * @since_tizen 2.4
 * @remarks This function must be called before Bluetooth AVRCP service. \n
 * You must free all resources of the this service by calling bt_avrcp_control_deinitialize()
 * if Bluetooth AVRCP service is no longer needed.
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] callback The callback function called when the connection state is changed
 * @param[in] user_data The user data to be passed to the callback function
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth service must be initialized with bt_initialize().
 * @see bt_initialize()
 * @see bt_avrcp_control_deinitialize()
 */
int bt_avrcp_control_initialize(bt_avrcp_control_connection_state_changed_cb callback, void *user_data);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief Deinitializes the Bluetooth AVRCP(Audio/Video Remote Control Profile) service.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_avrcp_control_initialize().
 * @see bt_avrcp_control_initialize()
 */
int bt_avrcp_control_deinitialize(void);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief Connects the Bluetooth AVRCP(Audio/Video Remote Control Profile) service.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_avrcp_control_initialize().
 * @see bt_avrcp_control_initialize()
 */
int bt_avrcp_control_connect(const char *remote_address);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief Disconnects the Bluetooth AVRCP(Audio/Video Remote Control Profile) service.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @return 0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_avrcp_control_initialize().
 * @see bt_avrcp_control_initialize()
 * @see bt_avrcp_control_connect()
 */
int bt_avrcp_control_disconnect(const char *remote_address);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief Handle the Bluetooth AVRCP(Audio/Video Remote Control Profile) commands.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] AVRCP Commands Play, Pause, Next, Rewind.
 * @return 0 on success, otherwise a negative error value.
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 * @pre The Bluetooth audio service must be initialized with bt_avrcp_control_initialize().
 * @see bt_avrcp_control_initialize()
 * @see bt_avrcp_control_connect()
 */
int bt_avrcp_control_send_player_command(bt_avrcp_player_command_e cmd);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  send the equalizer change to the remote device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] state To set the equalizer state ON/OFF
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The remote device must be connected.
 * @see bt_avrcp_target_connection_state_changed_cb()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_set_equalizer_state(bt_avrcp_equalizer_state_e state);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  get the the equalize state of the remote device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] equalizer To get the equalizer value ON/OFF
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The remote device must be connected.
 * @see bt_avrcp_target_connection_state_changed_cb()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_get_equalizer_state(bt_avrcp_equalizer_state_e *state);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  send the repeat change to the remote device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] Mode To set the repeat mode OFF, SINGLE, ALL
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The remote device must be connected.
 * @see bt_avrcp_target_connection_state_changed_cb()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_set_repeat_mode(bt_avrcp_repeat_mode_e mode);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  get the repeat state of the remote device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] Mode To get the repeat mode OFF, SINGLE, ALL
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The remote device must be connected.
 * @see bt_avrcp_target_connection_state_changed_cb()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_get_repeat_mode(bt_avrcp_repeat_mode_e *mode);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  send the shuffle change to the remote device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] Mode To set the shuffle mode OFF, GROUP, ALL
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The remote device must be connected.
 * @see bt_avrcp_target_connection_state_changed_cb()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_set_shuffle_mode(bt_avrcp_shuffle_mode_e mode);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  get the shuffle state of the remote device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] Mode To get the shuffle mode OFF, GROUP, ALL
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The remote device must be connected.
 * @see bt_avrcp_target_connection_state_changed_cb()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_get_shuffle_mode(bt_avrcp_shuffle_mode_e *mode);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  send the scan mode change to the remote device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] Mode To set the scan mode OFF, GROUP, ALL
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The remote device must be connected.
 * @see bt_avrcp_target_connection_state_changed_cb()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_set_scan_mode(bt_avrcp_scan_mode_e mode);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  get the scan mode of the remote device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] Mode To get the scan mode OFF, GROUP, ALL
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The remote device must be connected.
 * @see bt_avrcp_target_connection_state_changed_cb()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_get_scan_mode(bt_avrcp_scan_mode_e *mode);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  get the song position vale of the remote device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] Mode To get the position value in milliseconds
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The remote device must be connected.
 * @see bt_avrcp_target_connection_state_changed_cb()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_get_position(unsigned int *position);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  get the play status of the remote device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] Mode To get the play status PLAYING, STOPPED...
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The remote device must be connected.
 * @see bt_avrcp_target_connection_state_changed_cb()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_get_play_status(bt_avrcp_player_state_e *status);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  get the metadata of the remote device.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] track To get the metadata of track played in remote device.
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @retval #BT_ERROR_NOT_ENABLED  Not enabled
 * @retval #BT_ERROR_OPERATION_FAILED  Operation failed
 * @retval #BT_ERROR_REMOTE_DEVICE_NOT_CONNECTED  Remote device is not connected
 * @retval #BT_ERROR_PERMISSION_DENIED  Permission denied
 * @retval #BT_ERROR_NOT_SUPPORTED  Not supported
 *
 * @pre The remote device must be connected.
 * @see bt_avrcp_target_connection_state_changed_cb()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_get_track_info(bt_avrcp_metadata_attributes_info_s **track);

/**
 * @internal
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  to free the metadata  staructure.
 * @since_tizen 2.4
 * @privlevel platform
 * @privilege %http://tizen.org/privilege/bluetooth.admin
 * @param[in] track To free the memory of metadata structure.
 * @return  0 on success, otherwise a negative error value.
 * @retval #BT_ERROR_NONE  Successful
 * @retval #BT_ERROR_NOT_INITIALIZED  Not initialized
 * @retval #BT_ERROR_INVALID_PARAMETER  Invalid parameter
 * @pre track should point to valid metadata address.
 * @see bt_avrcp_control_get_track_info()
 * @see bt_avrcp_target_initialize()
 */
int bt_avrcp_control_free_track_info(bt_avrcp_metadata_attributes_info_s *track);

/**
 * @}
 */


#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __TIZEN_NETWORK_BLUETOOTH_INTERNAL_H__
