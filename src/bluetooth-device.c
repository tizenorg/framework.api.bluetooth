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

#include <glib.h>
#include <dlog.h>
#include <stdio.h>
#include <bluetooth-api.h>

#include "bluetooth.h"
#include "bluetooth_internal.h"
#include "bluetooth_private.h"

int bt_device_create_bond(const char *device_address)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	error_code = _bt_get_error_code(bluetooth_bond_device(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_device_create_bond_by_type(const char *device_address,
			bt_device_connection_link_type_e conn_type)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	error_code = _bt_get_error_code(bluetooth_bond_device_by_type(&addr_hex, conn_type));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_device_cancel_bonding(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_cancel_bonding());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_device_destroy_bond(const char *device_address)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	error_code = _bt_get_error_code(bluetooth_unbond_device(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_device_set_alias(const char *device_address, const char *alias)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);
	BT_CHECK_INPUT_PARAMETER(alias);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	error_code = _bt_get_error_code(bluetooth_set_alias(&addr_hex, alias));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_device_set_authorization(const char *device_address, bt_device_authorization_e authorization)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	gboolean trusted = FALSE;
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	if (authorization == BT_DEVICE_AUTHORIZED)
		trusted = TRUE;

	error_code = _bt_get_error_code(bluetooth_authorize_device(&addr_hex, trusted));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_device_start_service_search(const char *device_address)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int ret = 0;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	ret = _bt_get_error_code(bluetooth_search_service(&addr_hex));

	/* In service search, BT_ERROR_SERVICE_SEARCH_FAILED is returned instead of BT_ERROR_OPERATION_FAILED. */
	if (ret == BT_ERROR_OPERATION_FAILED)
		ret = BT_ERROR_SERVICE_SEARCH_FAILED;

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_device_cancel_service_search(void)
{
	int ret = 0;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	ret = _bt_get_error_code(bluetooth_cancel_service_search());
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_device_foreach_connected_profiles(const char *remote_address, bt_device_connected_profile callback, void *user_data)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	gboolean is_connected = FALSE;
	int ret;
	int i = 0;
	int profiles[] = {BT_PROFILE_RFCOMM, BT_PROFILE_A2DP,
			BT_PROFILE_HSP, BT_PROFILE_HID, BT_PROFILE_NAP,
			BT_PROFILE_AG, BT_PROFILE_GATT, BT_PROFILE_NAP_SERVER,
			0};

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	while (profiles[i] != 0) {
		ret = _bt_get_error_code(bluetooth_is_device_connected(&addr_hex,
								profiles[i],
								&is_connected));
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			return ret;
		}

		if (is_connected == TRUE) {
			if (!callback(profiles[i], user_data))
				break;
		}

		i++;
	}

	return BT_ERROR_NONE;
}

int bt_device_is_profile_connected(const char *remote_address, bt_profile_e bt_profile,
				bool *connected_status)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	int ret;
	gboolean is_connected = FALSE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	ret = _bt_get_error_code(bluetooth_is_device_connected(&addr_hex, bt_profile,
								&is_connected));
	*connected_status = is_connected ? true : false;

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	return ret;

}

int bt_device_set_bond_created_cb(bt_device_bond_created_cb callback, void *user_data)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_BOND_CREATED, callback, user_data);

	BT_DBG("+");

	return BT_ERROR_NONE;
}

int bt_device_set_bond_destroyed_cb(bt_device_bond_destroyed_cb callback, void *user_data)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_BOND_DESTROYED, callback, user_data);

	BT_DBG("+");

	return BT_ERROR_NONE;
}

int bt_device_set_authorization_changed_cb(bt_device_authorization_changed_cb callback, void *user_data)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_AUTHORIZATION_CHANGED, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_device_set_service_searched_cb(bt_device_service_searched_cb callback, void *user_data)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_SERVICE_SEARCHED, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_device_get_connection_state(const char *remote_address,
		bt_device_connection_link_type_e link_type, bool *connected)
{
	int ret;
	bluetooth_device_address_t addr_hex = { {0,} };
	bluetooth_connected_link_t connected_link = BLUETOOTH_CONNECTED_LINK_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(connected);

	if (link_type != BT_DEVICE_CONNECTION_LINK_BREDR &&
	    link_type != BT_DEVICE_CONNECTION_LINK_LE)
		return BT_ERROR_INVALID_PARAMETER;

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	ret = _bt_get_error_code(bluetooth_get_connected_link_type(&addr_hex, &connected_link));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to run function",
				_bt_convert_error_to_string(ret), ret);
		return ret;
	}

	if (connected_link == BLUETOOTH_CONNECTED_LINK_BREDR_LE) {
		*connected = true;
		return BT_ERROR_NONE;
	}

	if (link_type == BT_DEVICE_CONNECTION_LINK_BREDR) {
		if (connected_link == BLUETOOTH_CONNECTED_LINK_BREDR)
			*connected = true;
		else
			*connected = false;
	} else if (link_type == BT_DEVICE_CONNECTION_LINK_LE) {
		if (connected_link == BLUETOOTH_CONNECTED_LINK_LE)
			*connected = true;
		else
			*connected = false;
	}

	return BT_ERROR_NONE;
}

int bt_device_set_connection_state_changed_cb(bt_device_connection_state_changed_cb callback, void *user_data)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_DEVICE_CONNECTION_STATUS, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_device_unset_bond_created_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_BOND_CREATED);

	BT_DBG("+");

	return BT_ERROR_NONE;
}

int bt_device_unset_bond_destroyed_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_BOND_DESTROYED);

	BT_DBG("+");

	return BT_ERROR_NONE;
}

int bt_device_unset_authorization_changed_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_AUTHORIZATION_CHANGED);
	return BT_ERROR_NONE;
}

int bt_device_unset_service_searched_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_SERVICE_SEARCHED);
	return BT_ERROR_NONE;
}

int bt_device_unset_connection_state_changed_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_DEVICE_CONNECTION_STATUS);
	return BT_ERROR_NONE;
}

int bt_device_le_conn_update(const char *device_address,
            const bt_le_conn_update_s *parameters)
{
	bluetooth_device_address_t addr_hex = { {0,} };
	bluetooth_le_connection_param_t param = { 0 };
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_address);
	BT_CHECK_INPUT_PARAMETER(parameters);

	_bt_convert_address_to_hex(&addr_hex, device_address);
	param.interval_min = parameters->interval_min;
	param.interval_max = parameters->interval_max;
	param.latency = parameters->latency;
	param.timeout = parameters->time_out;

	ret = _bt_get_error_code(bluetooth_le_conn_update(&addr_hex, &param));

	if (BT_ERROR_NONE != ret) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_device_get_service_mask_from_uuid_list(char **uuids,
				      int no_of_service,
				      bt_service_class_t *service_mask_list)
{
	int i = 0;
	unsigned int service = 0;
	char **parts = NULL;
	bt_service_class_t service_mask = 0;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(uuids);
	BT_CHECK_INPUT_PARAMETER(service_mask_list);
	BT_CHECK_INIT_STATUS();

	BT_DBG("Get service mask from uuid list");
	BT_DBG("no_of_service = %d", no_of_service);

	for (i = 0; i < no_of_service; i++) {
		parts = g_strsplit(uuids[i], "-", -1);

		if (parts == NULL || parts[0] == NULL) {
			g_strfreev(parts);
			continue;
		}

		service = g_ascii_strtoull(parts[0], NULL, 16);
		g_strfreev(parts);

		switch (service) {
		case BLUETOOTH_SPP_PROFILE_UUID:
			service_mask |= BT_SC_SPP_SERVICE_MASK;
			break;

		case BLUETOOTH_LAP_PROFILE_UUID:
			service_mask |= BT_SC_LAP_SERVICE_MASK;
			break;

		case BLUETOOTH_DUN_PROFILE_UUID:
			service_mask |= BT_SC_DUN_SERVICE_MASK;
			break;

		case BLUETOOTH_OBEX_IR_MC_SYNC_SERVICE_UUID:
			service_mask |= BT_SC_SYNC_SERVICE_MASK;
			break;

		case BLUETOOTH_OBEX_OBJECT_PUSH_SERVICE_UUID:
			service_mask |= BT_SC_OPP_SERVICE_MASK;
			break;

		case BLUETOOTH_OBEX_FILE_TRANSFER_UUID:
			service_mask |= BT_SC_FTP_SERVICE_MASK;
			break;

		case BLUETOOTH_HS_PROFILE_UUID:
			service_mask |= BT_SC_HSP_SERVICE_MASK;
			break;

		case BLUETOOTH_CTP_PROFILE_UUID:
			service_mask |= BT_SC_CTP_SERVICE_MASK;
			break;

		case BLUETOOTH_AUDIO_SOURCE_UUID:
			service_mask |= BT_SC_A2DP_SOURCE_SERVICE_MASK;
			break;

		case BLUETOOTH_AUDIO_SINK_UUID:
			service_mask |= BT_SC_A2DP_SERVICE_MASK;
			break;

		case BLUETOOTH_VIDEO_SOURCE_UUID:
			service_mask |= BT_SC_NONE;
			break;

		case BLUETOOTH_VIDEO_SINK_UUID:
			service_mask |= BT_SC_NONE;
			break;

		case BLUETOOTH_AV_REMOTE_CONTROL_TARGET_UUID:
			service_mask |= BT_SC_NONE;
			break;

		case BLUETOOTH_AV_REMOTE_CONTROL_UUID:
			service_mask |= BT_SC_AVRCP_SERVICE_MASK;
			break;

		case BLUETOOTH_ICP_PROFILE_UUID:
			service_mask |= BT_SC_ICP_SERVICE_MASK;
			break;

		case BLUETOOTH_FAX_PROFILE_UUID:
			service_mask |= BT_SC_FAX_SERVICE_MASK;
			break;

		case BLUETOOTH_HEADSET_AG_SERVICE_UUID:
			service_mask |= BT_SC_NONE; /* BT_SC_HSP_SERVICE_MASK */
			break;

		case BLUETOOTH_PAN_PANU_PROFILE_UUID:
			service_mask |= BT_SC_PANU_SERVICE_MASK;
			break;

		case BLUETOOTH_PAN_NAP_PROFILE_UUID:
			service_mask |= BT_SC_NAP_SERVICE_MASK;
			break;

		case BLUETOOTH_PAN_GN_PROFILE_UUID:
			service_mask |= BT_SC_GN_SERVICE_MASK;
			break;

		case BLUETOOTH_REFERENCE_PRINTING:
			service_mask |= BT_SC_NONE;
			break;

		case BLUETOOTH_OBEX_IMAGING_UUID:
			service_mask |= BT_SC_NONE;
			break;

		case BLUETOOTH_OBEX_IMAGING_RESPONDER_UUID:
			service_mask |= BT_SC_BIP_SERVICE_MASK;
			break;

		case BLUETOOTH_HF_PROFILE_UUID:
			service_mask |= BT_SC_HFP_SERVICE_MASK;
			break;

		case BLUETOOTH_HFG_PROFILE_UUID:
			service_mask |= BT_SC_NONE;
			break;

		case BLUETOOTH_DIRECT_PRINTING_REFERENCE_OBJ_UUID:
			service_mask |= BT_SC_NONE;
			break;

		case BLUETOOTH_BASIC_PRINTING:
			service_mask |= BT_SC_NONE;
			break;

		case BLUETOOTH_HID_PROFILE_UUID:
			service_mask |= BT_SC_HID_SERVICE_MASK;
			break;

		case BLUETOOTH_SIM_ACCESS_PROFILE_UUID:
			service_mask |= BT_SC_SAP_SERVICE_MASK;
			break;

		case BLUETOOTH_OBEX_PBA_PROFILE_UUID:
			service_mask |= BT_SC_PBAP_SERVICE_MASK;
			break;

		case BLUETOOTH_OBEX_BPPS_PROFILE_UUID:
			service_mask |= BT_SC_BPP_SERVICE_MASK;
			break;

		case BLUETOOTH_PNP_INFORMATION_UUID:
			service_mask |= BT_SC_NONE;
			break;

		case BLUETOOTH_OBEX_PRINTING_STATUS_UUID:
			service_mask |= BT_SC_BPP_SERVICE_MASK;
			break;

		case BLUETOOTH_HCR_PROFILE_UUID:
			service_mask |= BT_SC_NONE;
			break;

		case BLUETOOTH_OBEX_SYNCML_TRANSFER_UUID:
			service_mask |= BT_SC_NONE;
			break;

		default:
			break;
		}

	}

	*service_mask_list = service_mask;
	BT_DBG("service_mask = %x, service_mask_list = %x", service_mask,
	       service_mask_list);

	return BT_ERROR_NONE;
}

int bt_passkey_reply(char *passkey, bool authentication_reply)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(passkey);
	int error_code = BT_ERROR_NONE;
	error_code = _bt_get_error_code(bluetooth_passkey_reply(passkey, authentication_reply));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_passkey_confirmation_reply(bool confirmation_reply)
{
	BT_CHECK_INIT_STATUS();

	int error_code = BT_ERROR_NONE;
	error_code = _bt_get_error_code(bluetooth_passkey_confirmation_reply(confirmation_reply));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}
int bt_device_le_set_data_length(const char *remote_address,
		unsigned int max_tx_Octets,  unsigned int max_tx_Time)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	bluetooth_device_address_t addr_hex = {{0,}};

	//Range for host suggested txtime is 0x001B-0x00FB  and
	// txocets is 0x0148- 0x0848 as per BT 4.2 spec
	if ((max_tx_Octets < 0x001B || max_tx_Octets > 0x00FB)
		&& (max_tx_Time < 0x0148 || max_tx_Time > 0x0848)) {
		return BT_ERROR_INVALID_PARAMETER;
	}

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	ret = _bt_get_error_code(bluetooth_le_set_data_length(
					&addr_hex,
					max_tx_Octets,
					max_tx_Time));

	if (ret != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);

	return ret;
}

int bt_device_le_set_data_length_change_cb(
	_bt_le_set_data_length_changed_cb callback, void *user_data)
{
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_LE_DATA_LENGTH_CHANGED, callback, user_data);

	return BT_ERROR_NONE;
}
