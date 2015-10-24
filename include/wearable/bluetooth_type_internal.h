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


#ifndef __TIZEN_NETWORK_BLUETOOTH_TYPE_INTERNAL_H__
#define __TIZEN_NETWORK_BLUETOOTH_TYPE_INTERNAL_H__

#include <glib.h>

 #ifdef __cplusplus
extern "C"
{
#endif /* __cplusplus */

/**
 * @file bluetooth_type_internal.h
 */

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  definitions for PBAP fields.
 * @since_tizen 2.3
 */
#define BT_PBAP_FIELD_ALL		(0xFFFFFFFFFFFFFFFFULL)
#define BT_PBAP_FIELD_VERSION	(1ULL << 0)
#define BT_PBAP_FIELD_FN 		(1ULL << 1)
#define BT_PBAP_FIELD_N			(1ULL << 2)
#define BT_PBAP_FIELD_PHOTO		(1ULL << 3)
#define BT_PBAP_FIELD_BDAY		(1ULL << 4)
#define BT_PBAP_FIELD_ADR		(1ULL << 5)
#define BT_PBAP_FIELD_LABEL		(1ULL << 6)
#define BT_PBAP_FIELD_TEL		(1ULL << 7)
#define BT_PBAP_FIELD_EMAIL		(1ULL << 8)
#define BT_PBAP_FIELD_MAILER	(1ULL << 9)
#define BT_PBAP_FIELD_TZ		(1ULL << 10)
#define BT_PBAP_FIELD_GEO		(1ULL << 11)
#define BT_PBAP_FIELD_TITLE		(1ULL << 12)
#define BT_PBAP_FIELD_ROLE		(1ULL << 13)
#define BT_PBAP_FIELD_LOGO		(1ULL << 14)
#define BT_PBAP_FIELD_AGENT		(1ULL << 15)
#define BT_PBAP_FIELD_ORG		(1ULL << 16)
#define BT_PBAP_FIELD_NOTE		(1ULL << 17)
#define BT_PBAP_FIELD_REV		(1ULL << 18)
#define BT_PBAP_FIELD_SOUND		(1ULL << 19)
#define BT_PBAP_FIELD_URL		(1ULL << 20)
#define BT_PBAP_FIELD_UID		(1ULL << 21)
#define BT_PBAP_FIELD_KEY		(1ULL << 22)
#define BT_PBAP_FIELD_NICKNAME	(1ULL << 23)
#define BT_PBAP_FIELD_CATEGORIES	(1ULL << 24)
#define BT_PBAP_FIELD_PROID		(1ULL << 25)
#define BT_PBAP_FIELD_CLASS		(1ULL << 26)
#define BT_PBAP_FIELD_SORT_STRING	(1ULL << 27)
#define BT_PBAP_FIELD_X_IRMC_CALL_DATETIME		(1ULL << 28)

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief  Enumerations of the Bluetooth adapter le state.
 * @since_tizen 2.3.1
 */
typedef enum
{
	BT_ADAPTER_LE_DISABLED = 0x00, /**< Bluetooth le is disabled */
	BT_ADAPTER_LE_ENABLED, /**< Bluetooth le is enabled */
} bt_adapter_le_state_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief  Called when the Bluetooth adapter le state changes.
 * @since_tizen 2.3.1
 *
 * @param[in]   result  The result of the adapter state changing
 * @param[in]   adapter_le_state  The adapter le state to be changed
 * @param[in]   user_data  The user data passed from the callback registration function
 * @pre Either bt_adapter_le_enable() or bt_adapter_le_disable() will invoke this callback if you register this callback using bt_adapter_le_set_state_changed_cb().
 * @see bt_adapter_le_enable()
 * @see bt_adapter_le_disable()
 * @see bt_adapter_le_set_state_changed_cb()
 * @see bt_adapter_le_unset_state_changed_cb()
 */
typedef void (*bt_adapter_le_state_changed_cb)(int result, bt_adapter_le_state_e adapter_le_state, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_AG_MODULE
 * @brief  Enumerations for the call state
 * @since_tizen 2.3.1
 */
typedef enum {
    BT_AG_CALL_EVENT_IDLE = 0x00,  /**< Idle */
    BT_AG_CALL_EVENT_ANSWERED,  /**< Answered */
    BT_AG_CALL_EVENT_HELD,  /**< Held */
    BT_AG_CALL_EVENT_RETRIEVED,  /**< Retrieved */
    BT_AG_CALL_EVENT_DIALING,  /**< Dialing */
    BT_AG_CALL_EVENT_ALERTING,  /**< Alerting */
    BT_AG_CALL_EVENT_INCOMING,  /**< Incoming */
} bt_ag_call_event_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_AG_MODULE
 * @brief  Enumerations for the call state
 * @since_tizen 2.3.1
 */
typedef enum {
    BT_AG_CALL_STATE_IDLE = 0x00,  /**< Idle state */
    BT_AG_CALL_STATE_ACTIVE,  /**< Active state */
    BT_AG_CALL_STATE_HELD,  /**< Held state */
    BT_AG_CALL_STATE_DIALING,  /**< Dialing state */
    BT_AG_CALL_STATE_ALERTING,  /**< Alerting state */
    BT_AG_CALL_STATE_INCOMING,  /**< Incoming state */
    BT_AG_CALL_STATE_WAITING,  /**< Waiting for connected indication event after answering an incoming call*/
} bt_ag_call_state_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Called when the connectable state changes.
 * @since_tizen 2.3.1
 *
 * @param[in] result The result of the connectable state changing
 * @param[in] connectable The connectable to be changed
 * @param[in] user_data The user data passed from the callback registration function
 *
 * @pre This function will be invoked when the connectable state of local Bluetooth adapter changes
 * if you register this callback using bt_adapter_set_connectable_changed_cb().
 *
 * @see bt_adapter_set_connectable()
 * @see bt_adapter_set_connectable_changed_cb()
 * @see bt_adapter_unset_connectable_changed_cb()
 */
typedef void (*bt_adapter_connectable_changed_cb)
	(int result, bool connectable, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_OPP_SERVER_MODULE
 * @brief  Called when the push is requested.
 * @since_tizen 2.3.1
 *
 * @details You must call bt_opp_server_accept() if you want to accept.
 * Otherwise, you must call bt_opp_server_reject().
 * @param[in] file  The path of file to be pushed
 * @param[in] size The file size (bytes)
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_opp_server_initialize()
 */
typedef void (*bt_opp_server_push_requested_cb)(const char *file, int size, void *user_data);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief  Enumerations of the Bluetooth adapter le scan type.
 * @since_tizen 2.3
 */
typedef enum
{
	BT_ADAPTER_LE_PASSIVE_SCAN = 0x00,
	BT_ADAPTER_LE_ACTIVE_SCAN
} bt_adapter_le_scan_type_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief  Enumerations of the Bluetooth le scan mode.
 * @since_tizen 2.3
 */
typedef enum
{
	BT_ADAPTER_LE_SCAN_MODE_BALANCED,
	BT_ADAPTER_LE_SCAN_MODE_LOW_LATENCY,
	BT_ADAPTER_LE_SCAN_MODE_LOW_ENERGY
} bt_adapter_le_scan_mode_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Enumerations for the call handling event
 * @since_tizen 2.3
 */
typedef enum {
    BT_HF_CALL_HANDLING_EVENT_ANSWER = 0x00,  /**< Request to answer an incoming call */
    BT_HF_CALL_HANDLING_EVENT_RELEASE,  /**< Request to release a call */
    BT_HF_CALL_HANDLING_EVENT_REJECT,  /**< Request to reject an incoming call */
    BT_HF_CALL_HANDLING_EVENT_RING,  /**< Request of ringing call */
    BT_HF_CALL_HANDLING_EVENT_CALL_STARTED,  /**< Request of Call started */
    BT_HF_CALL_HANDLING_EVENT_CALL_ENDED,  /**< Request of Call Ended */
    BT_HF_CALL_HANDLING_EVENT_VOICE_RECOGNITION_ENABLED,  /**< Request of voice recognition enabled */
    BT_HF_CALL_HANDLING_EVENT_VOICE_RECOGNITION_DISABLED,  /**< Request of voice recognition disabled */
    BT_HF_CALL_HANDLING_EVENT_VENDOR_DEP_CMD,  /**< Request of Vendor command */
    BT_HF_CALL_HANDLING_EVENT_WAITING,  /**< Request to waiting a call */
    BT_HF_CALL_HANDLING_EVENT_HELD,  /**< Request to hold a call */
    BT_HF_CALL_HANDLING_EVENT_UNHELD,  /**< Request to unhold calls */
    BT_HF_CALL_HANDLING_EVENT_SWAPPED,  /**< Request to swap calls */
} bt_hf_call_handling_event_e;


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Enumerations for the multi call handling event
 * @since_tizen 2.3
 */
typedef enum {
    BT_HF_MULTI_CALL_HANDLING_EVENT_RELEASE_HELD_CALLS = 0x00,  /**< Request to release held calls */
    BT_HF_MULTI_CALL_HANDLING_EVENT_RELEASE_ACTIVE_CALLS,  /**< Request to release active calls */
    BT_HF_MULTI_CALL_HANDLING_EVENT_ACTIVATE_HELD_CALL,  /**< Request to put active calls into hold state and activate another (held or waiting) call */
    BT_HF_MULTI_CALL_HANDLING_EVENT_MERGE_CALLS,  /**< Request to add a held call to the conversation */
    BT_HF_MULTI_CALL_HANDLING_EVENT_EXPLICIT_CALL_TRANSFER,  /**< Request to let a user who has two calls to connect these two calls together and release its connections to both other parties */
} bt_hf_multi_call_handling_event_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Enumerations for the call state
 * @since_tizen 2.3
 */
typedef enum {
    BT_HF_CALL_EVENT_IDLE = 0x00,  /**< Idle */
    BT_HF_CALL_EVENT_ANSWER,  /**< Answered */
    BT_HF_CALL_EVENT_HOLD,  /**< Held */
    BT_HF_CALL_EVENT_RETRIEVE,  /**< Retrieved */
    BT_HF_CALL_EVENT_DIAL,  /**< Dialing */
    BT_HF_CALL_EVENT_ALERT,  /**< Alerting */
    BT_HF_CALL_EVENT_INCOMING,  /**< Incoming */
    BT_HF_CALL_EVENT_REDIAL,  /**< Redialling */
    BT_HF_CALL_EVENT_RELEASE_ALL_NONACTIVE_CALLS,  /**< Release all nonactive calls */
    BT_HF_CALL_EVENT_ACCEPT_AND_RELEASE,  /**< Accept and Release */
    BT_HF_CALL_EVENT_ACCEPT_AND_HOLD,  /**< Accept and Hold */
    BT_HF_CALL_EVENT_ADD_TO_CONVERSATION,  /**< Add to the conversation */
} bt_hf_call_event_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  Enumerations of Addressbook memory for PBAP.
 * @since_tizen 2.3
 */
typedef enum
{
	BT_PBAP_PHONE = 0x00, /**< Request for Addressbook from Phone*/
	BT_PBAP_SIM , /**< Request for Addressbook from SIM*/
} bt_pbap_addressbook_source_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  Enumerations of Folder type.
 * @since_tizen 2.3
 */
typedef enum
{
	BT_PBAP_PHONEBOOK = 0x00, /**< Request for Addressbook*/
	BT_PBAP_INCOMING , /**< Request for Incoming Calls*/
	BT_PBAP_OUTGOING , /**< Request for Outgoing Calls*/
	BT_PBAP_MISSED , /**< Request for Missed Calls*/
	BT_PBAP_COMBINED , /**< Request for Combined Calls*/
} bt_pbap_folder_type_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  Enumerations of Phonebook Search field.
 * @since_tizen 2.3
 */
typedef enum
{
	BT_PBAP_SEARCH_NAME = 0x00, /**< Request for Search by name (default)*/
	BT_PBAP_SEARCH_NUMBER, /**< Request for Search by phone number*/
	BT_PBAP_SEARCH_SOUND, /**< Request for Search by sound*/
} bt_pbap_search_field_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  Enumerations of vCard Formats.
 * @since_tizen 2.3
 */
typedef enum
{
	BT_PBAP_APP_PARAM_VCARD21 = 0x00, /**< vCard Format 2.1 (default)*/
	BT_PBAP_APP_PARAM_VCARD30, /**< vCard Format 3.0*/
} bt_pbap_filter_vcard_format_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  Enumerations of Sorting Orders.
 * @since_tizen 2.3
 */
typedef enum
{
	BT_PBAP_APP_PARAM_ORDER_INDEXED = 0x00, /**< Filter Order Indexed (default)*/
	BT_PBAP_APP_PARAM_ORDER_ALPHANUMERIC, /**< Filter Order Alphanumeric*/
	BT_PBAP_APP_PARAM_ORDER_PHONETIC, /**< Filter Order Phonetic*/
} bt_pbap_filter_sort_order_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Enumerations for the player control command
 * @since_tizen 2.3
 */
 typedef enum {
	BT_AVRCP_CONTROL_PLAY = 0x01,   /**< Play */
	BT_AVRCP_CONTROL_PAUSE,   /**< Pause */
	BT_AVRCP_CONTROL_STOP,   /**< Stop */
	BT_AVRCP_CONTROL_NEXT,   /**< Next Track*/
	BT_AVRCP_CONTROL_PREVIOUS,   /**< Previous track */
	BT_AVRCP_CONTROL_FAST_FORWARD,   /**< Fast Forward */
	BT_AVRCP_CONTROL_REWIND   /**< Rewind */
} bt_avrcp_player_command_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief Structure of Track metadata information.
 * @since_tizen 2.3
 *
 * @see #bt_class_s
 */
typedef struct {
	const char *title;
	const char *artist;
	const char *album;
	const char *genre;
	unsigned int total_tracks;
	unsigned int number;
	unsigned int duration;
} bt_avrcp_metadata_attributes_info_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Called when the connection state is changed.
 * @since_tizen 2.3
 * @param[in] connected  The state to be changed. @a true means connected state, Otherwise, @a false.
 * @param[in] remote_address  The remote address
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_avrcp_control_initialize()
 * @see bt_avrcp_control_deinitialize()
 */
typedef void (*bt_avrcp_control_connection_state_changed_cb) (bool connected, const char *remote_address, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Called when the Song position mode is changed by the remote target device.
 * @since_tizen 2.3
 *
 * @param[in] position The song position
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_avrcp_set_song_position_changed_cb()
 * @see bt_avrcp_unset_song_position_changed_cb()
 */
typedef void (*bt_avrcp_song_position_changed_cb) (unsigned int position, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Called when the Song metadata information is changed by the remote target device.
 * @since_tizen 2.3
 *
 * @param[in] position The song metadata information
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_avrcp_set_track_info_changed_cb()
 * @see bt_avrcp_unset_track_info_changed_cb()
 */
typedef void (*bt_avrcp_track_info_changed_cb) (bt_avrcp_metadata_attributes_info_s *track, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AVRCP_MODULE
 * @brief  Called when the Song Play status mode is changed by the remote target device.
 * @since_tizen 2.3
 *
 * @param[in] play_state The song play status
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_avrcp_set_play_status_changed_cb()
 * @see bt_avrcp_unset_play_status_changed_cb()
 */
typedef void (*bt_avrcp_play_status_changed_cb) (bt_avrcp_player_state_e play_state, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  HF Call status information.
 * @since_tizen 2.3
 */
typedef struct {
	char *number;  /**< Phone Number */
	int direction;  /**< Direction :Incoming(1), Outgoing(0) */
	int status;  /**< Call Status :Active(0), Held(1), Waiting(5), Dailing(2) */
	int multi_party;  /**< Multiparty/conf call: Yes(1), No(0) */
	int index;  /**< Call index/ID */
} bt_hf_call_status_info_s;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @brief  Called when the manufacturer dat changes.
 * @since_tizen 2.3
 *
 * @param[in]   data		The manufacurer data of the Bluetooth device to be changed
 * @param[in]   len			The length of @a data
 * @param[in]   user_data	The user data passed from the callback registration function
 * @pre This function will be invoked when the manufacturer data of Bluetooth adapter changes
 * if callback is registered using bt_adapter_set_manufacturer_data_changed_cb().
 * @see bt_adapter_set_manufacturer_data()
 * @see bt_adapter_set_manufacturer_data_changed_cb()
 * @see bt_adapter_unset_manufacturer_data_changed_cb()
 */
typedef void (*bt_adapter_manufacturer_data_changed_cb) (char *data,
		int len, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Called when the SCO(Synchronous Connection Oriented link) state is changed.
 * @since_tizen 2.3
 *
 * @details  This callback is called when the SCO state is changed.
 * When you call bt_ag_open_sco() or bt_ag_close_sco(), this callback is also called with error result even though these functions failed.
 * @param[in] result  The result of changing the connection state
 * @param[in] opened  The state to be changed: (@c true = opened, @c  false = not opened)
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_ag_set_sco_state_changed_cb()
 * @see bt_ag_unset_sco_state_changed_cb()
 * @see bt_ag_open_sco()
 * @see bt_ag_close_sco()
 */
typedef void (*bt_hf_sco_state_changed_cb) (int result, bool opened, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Called when a call handling event happened from Hands-Free.
 * @since_tizen 2.3
 *
 * @param[in] event  The call handling event happened from Hands-Free
 * @param[in] call_id  The call ID
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_ag_set_call_handling_event_cb()
 * @see bt_ag_unset_call_handling_event_cb()
 */
typedef void (*bt_hf_call_handling_event_cb) (bt_hf_call_handling_event_e event, char *phone_number, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Called when a multi call handling event happened from Hands-Free.
 * @since_tizen 2.3
 *
 * @param[in] event  The call handling event happened from Hands-Free
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_ag_set_multi_call_handling_event_cb()
 * @see bt_ag_unset_multi_call_handling_event_cb()
 */
typedef void (*bt_hf_multi_call_handling_event_cb) (bt_hf_multi_call_handling_event_e event, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Called when the speaker gain of the remote device is changed.
 * @since_tizen 2.3
 *
 * @param[in] gain The gain of speaker (0 ~ 15)
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_hf_set_speaker_gain_changed_cb()
 * @see bt_hf_unset_speaker_gain_changed_cb()
 */
typedef void (*bt_hf_speaker_gain_changed_cb) (int gain, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_AUDIO_HF_MODULE
 * @brief  Called when a call status updated event happened from Hands-Free.
 * @since_tizen 2.3
 * @remarks call_info_list has elements which consist of bt_hf_call_status_info_s
 * @remarks The @a call_info_list must be released with bt_hf_free_call_status_info_list() by you.
 *
 * @param[in] event  The call handling event happened from Hands-Free
 * @param[in] call_id  The call ID
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_hf_call_status_info_s
 * @see bt_hf_set_call_status_updated_event_cb()
 * @see bt_hf_unset_call_status_updated_event_cb()
 */
typedef void (*bt_hf_call_status_updated_event_cb) (GSList *call_info_list, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  Called when PBAP is Connected or Disconnected.
 * @since_tizen 2.3
 *
 * @param[in] remote_address Remote Device address
 * @param[in] pbap_enabled PBAP connection status (@c 1 = enabled, @c 0 = disabled)
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_pbap_connect()
 * @see bt_pbap_disconnect()
 */
typedef void (*bt_pbap_enabled_cb)(const char *remote_address,
		int pbap_enabled, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  Called when PBAP Phonebook Size is returned.
 * @since_tizen 2.3
 *
 * @param[in] remote_address Remote Device address
 * @param[in] size Size of Phonebook
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_pbap_get_phonebook_size()
 * @see bt_pbap_connect()
 * @see bt_pbap_disconnect()
 */
typedef void (*bt_pbap_phonebook_size_cb)(const char *remote_address,
		int size, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  Called when PBAP Phonebook Pull returns all contacts.
 * @since_tizen 2.3
 *
 * @param[in] remote_address Remote Device address
 * @param[in] vcf_file File in which vCards are saved
 * @param[in] status Status for successful Transfer (@c 0 = Unsuccessful, @c 1 = Successful)
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_pbap_get_phonebook()
 * @see bt_pbap_connect()
 * @see bt_pbap_disconnect()
 */
typedef void (*bt_pbap_phonebook_pull_cb)(const char *remote_address,
		char *vcf_file, int status, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  Called when PBAP List vCards returns the handles and Names.
 * @since_tizen 2.3
 *
 * @param[in] remote_address Remote Device address
 * @param[in] vcards List of vCards
 * @param[in] count Number of contacts in the list
 * @param[in] status Status for successful Transfer (@c 0 = Unsuccessful, @c 1 = Successful)
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_pbap_get_phonebook()
 * @see bt_pbap_connect()
 * @see bt_pbap_disconnect()
 */
typedef void (*bt_pbap_list_vcards_cb)(const char *remote_address,
		char **vcards, int count, int status, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  Called when PBAP Get vCard returns the contact.
 * @since_tizen 2.3
 *
 * @param[in] remote_address Remote Device address
 * @param[in] vcard Contact as vCard
 * @param[in] status Status for successful Transfer (@c 0 = Unsuccessful, @c 1 = Successful)
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_pbap_get_phonebook()
 * @see bt_pbap_connect()
 * @see bt_pbap_disconnect()
 */
typedef void (*bt_pbap_get_vcard_cb)(const char *remote_address,
		char *vcard, int status, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_PBAP_MODULE
 * @brief  Called when PBAP Phonebook Search returns the handles and Names.
 * @since_tizen 2.3
 *
 * @param[in] remote_address Remote Device address
 * @param[in] vcards List of vCards
 * @param[in] count Number of contacts in the list
 * @param[in] status Status for successful Transfer (@c 0 = Unsuccessful, @c 1 = Successful)
 * @param[in] user_data The user data passed from the callback registration function
 * @see bt_pbap_get_phonebook()
 * @see bt_pbap_connect()
 * @see bt_pbap_disconnect()
 */
typedef void (*bt_pbap_search_list_cb)(const char *remote_address,
		char **vcards, int count, int status, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Enumerations of the attribute's permission
 * @since_tizen 2.4
 */
typedef enum {
	BT_GATT_PERMISSION_READ = 0x01,
	BT_GATT_PERMISSION_WRITE = 0x02,
	BT_GATT_PERMISSION_ENCRYPTION = 0x04,
	BT_GATT_PERMISSION_AUTHENTICATION = 0x08,
	BT_GATT_PERMISSION_AUTHORIZATION = 0x10,
	BT_GATT_PERMISSION_NONE = 0x20,
} bt_gatt_permission_e;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief The handle to control Bluetooth LE scan filter
 * @since_tizen 2.4
 */
typedef void* bt_scan_filter_h;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_LE_MODULE
 * @brief The handle of a GATT server
 * @since_tizen 2.4
 */
typedef void* bt_gatt_server_h;

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Called when a value of a characteristic or descriptor's GATT handle has been changed
 * @since_tizen 2.4
 *
 * @remarks After this function is returned, a changed vlaue is automatically
 * applied to @a gatt_handle. Before that, @a gatt_handle has an old value.
 *
 * @param[in] remote_address The address of the remote device which requests a change
 * @param[in] server The GATT server handle
 * @param[in] gatt_handle The characteristic or descriptor's GATT handle which has an old value
 * @param[in] offset The requested offset from where the @a gatt_handle value will be updated
 * @param[in] value The new value
 * @param[in] len The length of @a value
 * @param[in] user_data The user data passed from the registration function
 *
 * @see bt_gatt_server_set_value_changed_cb()
 */
typedef void (*bt_gatt_server_value_changed_cb) (char *remote_address,
				bt_gatt_server_h server, bt_gatt_h gatt_handle,
				int offset, char *value, int len,
				void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Called when the remote device requests to read a value on a GATT server
 * @since_tizen 2.4
 *
 * @param[in] remote_address The address of the requesting remote device
 * @param[in] server The GATT server handle
 * @param[in] gatt_handle The characteristic or descriptor's GATT handle to be read
 * @param[in] request_id The identification of this request. It will be used to send a reponse.
 * @param[in] offset The requested offset from where the GATT handle's value is read
 * @param[in] user_data The user data passed from the registration function
 *
 * @see bt_gatt_server_set_read_value_requested_cb()
 * @see bt_gatt_server_send_response()
 */
typedef void (*bt_gatt_server_read_value_requested_cb) (char *remote_address,
		int request_id, bt_gatt_server_h server, bt_gatt_h gatt_handle,
		int offset, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Called when the remote device enables or disables the Notification/Indication for particular characteristics.
 * @since_tizen 2.4
 *
 * @param[in] server The GATT server handle
 * @param[in] gatt_handle The characteristic's GATT handle to be read
 * @param[in] user_data The user data passed from the registration function
 *
 * @see bt_gatt_server_set_read_value_requested_cb()
 */
typedef void (*bt_gatt_server_notification_state_change_cb) (bool notify,
			bt_gatt_server_h server, bt_gatt_h gatt_handle, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_GATT_MODULE
 * @brief  Called when the sending notification / indication is done
 * @since_tizen 2.4
 *
 * @remarks In case of an indication, once a confirmation is received from the remote device this callback will be called. \n
 * This callback will be called several times if there are two or more remote devices which enable a Client Characteristic Configuration Descriptor(CCCD). \n
 * For the last remote device, @a completed will be set as true.
 *
 * @param[in] result The result of a sending operation
 * @param[in] remote_address The address of the remote device
 * @param[in] server The GATT server handle
 * @param[in] characteristic The characteristic's GATT handle
 * @param[in] completed If this callback is for the last remote device which enables a CCCD, it will be true. Or it will be false.
 * @param[in] user_data The user data passed from the requesting function
 *
 * @see bt_gatt_server_notify()
 */
typedef void (*bt_gatt_server_notification_sent_cb) (int result,
		char *remote_address, bt_gatt_server_h server,
		bt_gatt_h characteristic, bool completed, void *user_data);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_DEVICE_MODULE
 * @brief Device LE connection update structure.
 * @since_tizen 2.3.1
 */
typedef struct
{
       unsigned int interval_min;   /**< Minimum value for the connection event interval (msec) */
       unsigned int interval_max;   /**< Maximum value for the connection event interval (msec) */
       unsigned int latency;   /**< Slave latency (msec) */
       unsigned int time_out;   /**< Supervision timeout (msec) */
} bt_le_conn_update_s;

/**
 * @}
 */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif // __TIZEN_NETWORK_BLUETOOTH_TYPE_INTERNAL_H__
