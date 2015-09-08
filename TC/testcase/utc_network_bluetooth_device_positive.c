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

#include <tet_api.h>
#include <bluetooth.h>
#include <stdlib.h>
#include <stdbool.h>
#include <glib.h>

#define CONFIG_FILE_PATH "/opt/home/capi-network-bluetooth/tetware.conf"

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

static GMainLoop *mainloop;
static bool callback_result = false;
char target_name[128] = "dts_test";
char *target_address;
bt_device_info_s *device_info = NULL;
bool discovery_result = false;

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

static void utc_network_bluetooth_device_set_bonded_cb_p(void);
static void utc_network_bluetooth_device_set_unbonded_cb_p(void);
static void utc_network_bluetooth_device_set_authorization_state_changed_cb_p(void);
static void utc_network_bluetooth_device_cancel_bonding_p(void);
static void utc_network_bluetooth_device_bond_p(void);
static void utc_network_bluetooth_device_foreach_bonded_device_p(void);
static void utc_network_bluetooth_device_set_alias_p(void);
static void utc_network_bluetooth_device_set_authorization_p(void);
static void utc_network_bluetooth_device_unbond_p(void);
static void utc_network_bluetooth_device_unset_bonded_cb_p(void);
static void utc_network_bluetooth_device_unset_unbonded_cb_p(void);
static void utc_network_bluetooth_device_unset_authorization_state_changed_cb_p(void);
static void utc_network_bluetooth_device_get_rssi_strength_p(void);
static void utc_network_bluetooth_device_disable_rssi_monitor_p(void);
static void utc_network_bluetooth_device_enable_rssi_monitor_p(void);
static void utc_network_bluetooth_device_unset_rssi_alert_cb_p(void);

gboolean timeout_func(gpointer data);
void adapter_state_changed_cb_for_device_p(int result,
							bt_adapter_state_e adapter_state,
							void *user_data);
void device_discovery_state_changed_cb_for_device_p(int result,
						bt_adapter_device_discovery_state_e discovery_state,
						bt_adapter_device_discovery_info_s *discovery_info,
						void *user_data);
void device_bonded_cb(int result, bt_device_info_s *device_info,
						void *user_data);
void device_unbonded_cb(int result, char *remote_address, void *user_data);
void device_authorization_state_changed_cb(bt_device_authorization_e authorization_state, \
		char *remote_address, void *user_data);
bool bonded_device_cb_for_device_p(bt_device_info_s *dev_info,
						void *user_data);
void device_get_rssi_strength_cb(char *bt_address,
						bt_device_connection_link_type_e link_type,
						int rssi_dbm, void *user_data);
static void device_rssi_enabled_cb(const char *address,
						bt_device_connection_link_type_e link_type,
						int rssi_enabled, void *user_data);
static void device_rssi_alert_cb(char *bt_address,
						bt_device_connection_link_type_e link_type,
						int rssi_alert_type, int rssi_alert_dbm, void *user_data);

struct tet_testlist tet_testlist[] = {
	{ utc_network_bluetooth_device_set_bonded_cb_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_bond_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_foreach_bonded_device_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_set_alias_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_set_authorization_state_changed_cb_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_set_authorization_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_unset_bonded_cb_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_unset_authorization_state_changed_cb_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_get_rssi_strength_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_enable_rssi_monitor_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_disable_rssi_monitor_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_unset_rssi_alert_cb_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_set_unbonded_cb_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_cancel_bonding_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_unbond_p, POSITIVE_TC_IDX },
	{ utc_network_bluetooth_device_unset_unbonded_cb_p, POSITIVE_TC_IDX },
	{ NULL, 0 },
};

int get_value_from_file(void)
{
	FILE *fp;
	char *token;
	char buf[100];

	fp = fopen(CONFIG_FILE_PATH, "r");
	if (fp == NULL) {
		tet_printf("Default configuration is used\n");
		return -1;
	}
	while (fgets(buf, sizeof(buf), fp)) {
		if (buf[0] == '#' || buf[0] == '\n')
			continue;

		token = strrchr(buf, '\n');
		if (token == NULL) {
			tet_printf("g_conf is too long\n");
			break;
		}
		*token = '\0';

		token = strtok(buf, "=");
		if (token == NULL) {
				continue;
		}
		if (strcasecmp(token, "BT_ADDR_MOBILE") == 0) {
			token = strtok(NULL, "=");
			target_address = strdup(token);
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	return -1;
}

static void startup(void)
{
	bt_error_e ret = BT_ERROR_NONE;
	int timeout_id = 0;

	if(get_value_from_file() == -1) {
		tet_printf("Failed to read.");
	}

	/* start of TC */
	callback_result = false;
	discovery_result = false;
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();

	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_device_p, "startup") != BT_ERROR_NONE) {
		tet_printf("bt_adapter_set_state_changed_cb() failed.");
	}

	if (bt_adapter_set_device_discovery_state_changed_cb(device_discovery_state_changed_cb_for_device_p, "startup") != BT_ERROR_NONE) {
		tet_printf("bt_adapter_set_device_discovery_state_changed_cb() failed.");
	}

	ret = bt_adapter_enable();
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_adapter_enable() succeeded.");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);

		tet_printf("BT was enabled. Device discovery will be started");
		if (bt_adapter_start_device_discovery() == BT_ERROR_NONE) {
			tet_printf("bt_adapter_start_device_discovery() succeeded.");
			timeout_id = g_timeout_add(60000, timeout_func, mainloop);
			g_main_loop_run(mainloop);
			g_source_remove(timeout_id);
		} else {
			tet_printf("bt_adapter_start_device_discovery() failed.");
		}
	} else if (ret == BT_ERROR_ALREADY_DONE) {
		tet_printf("BT was already enabled. Device discovery will be started");
		if (bt_adapter_start_device_discovery() == BT_ERROR_NONE) {
			tet_printf("bt_adapter_start_device_discovery() succeeded.");
			timeout_id = g_timeout_add(60000, timeout_func, mainloop);
			g_main_loop_run(mainloop);
			g_source_remove(timeout_id);
		} else {
			tet_printf("bt_adapter_start_device_discovery() failed.");
		}
	} else {
		tet_printf("DTS may fail because bt_adapter_enable() failed.");
	}

	if (bt_adapter_unset_state_changed_cb() != BT_ERROR_NONE) {
		tet_printf("bt_adapter_unset_state_changed_cb() failed.");
	}

	if (bt_adapter_unset_device_discovery_state_changed_cb() !=
				BT_ERROR_NONE) {
				tet_printf("bt_adapter_unset_device_discovery_state_changed_cb() failed.");
	}

	tet_printf("TC start.");
}

static void cleanup(void)
{
	/* end of TC */
	if (device_info != NULL)
		free(device_info);

	bt_deinitialize();
	tet_printf("TC end");
}

gboolean timeout_func(gpointer data)
{
	tet_printf("[%s] Callback: Timeout.", __FUNCTION__);
	g_main_loop_quit((GMainLoop *) data);
	return FALSE;
}

void adapter_state_changed_cb_for_device_p(int result,
						bt_adapter_state_e adapter_state,
						void *user_data)
{
	tet_printf("Callback: bt_adapter_state_changed_cb() was called");
	if (user_data != NULL && !strcmp((char *)user_data, "startup")) {
		if (adapter_state == BT_ADAPTER_ENABLED && result == BT_ERROR_NONE) {
			tet_printf("Callback: BT was enabled.");
		} else {
			tet_printf("Callback: BT was not enabled. DTS will be started but DTS may fail.");
		}

		if (mainloop) {
			g_main_loop_quit(mainloop);
		}
	}
}

void device_discovery_state_changed_cb_for_device_p(int result,
						bt_adapter_device_discovery_state_e discovery_state,
						bt_adapter_device_discovery_info_s *discovery_info,
						void *user_data)
{
	tet_printf("Callback: bt_adapter_device_discovery_state_changed_cb() was called");
	if (user_data != NULL && !strcmp((char *)user_data, "startup")) {
		if (discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FINISHED) {
			tet_printf("Callback: Device discovery finished. DTS will be started.");
			if (mainloop) {
				g_main_loop_quit(mainloop);
			}
		} else if (discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FOUND) {
			tet_printf("Callback: Devices were founded (%s)", discovery_info->remote_name);
			if (discovery_info->remote_name != NULL && !strcmp(discovery_info->remote_name, target_name)) {
				discovery_result = true;
				tet_printf("Callback: dts_test device was found.");
				strncpy(target_address, discovery_info->remote_address, 18);
				tet_printf("Callback: device address: [%s]", discovery_info->remote_address);
				if (bt_adapter_stop_device_discovery() == BT_ERROR_NONE) {
					tet_printf("Callback: Device discovery will be stopped");
				}
			}
		}
	}
}

void device_bonded_cb(int result, bt_device_info_s * device_info,
				void *user_data)
{
	int i = 0;

	tet_printf("Callback: bt_device_bond_created_cb() was called.");
	if (result == BT_ERROR_TIMED_OUT)
		tet_printf("result is BT_ERROR_TIMEOUT");
	else if (result == BT_ERROR_CANCELLED)
		tet_printf("result is BT_ERROR_CANCELLED");
	else if (result == BT_ERROR_AUTH_FAILED)
		tet_printf("result is BT_ERROR_AUTH_FAILED");
	else if (result == BT_ERROR_OPERATION_FAILED)
		tet_printf("result is BT_ERROR_OPERATION_FAILED");
	else if (result == BT_ERROR_AUTH_REJECTED)
		tet_printf("result is BT_ERROR_AUTH_REJECTED");
	else if (result == BT_ERROR_REMOTE_DEVICE_NOT_FOUND)
		tet_printf("result is BT_ERROR_REMOTE_DEVICE_NOT_FOUND");
	else if (result == BT_ERROR_NONE)
		tet_printf("result is BT_ERROR_NONE");
	else
		tet_printf("result is unknown: %s",
				_bt_convert_error_to_string(result));

	/* in case of bt_device_cancel_bonding() */
	if (user_data != NULL && !strcmp((char *)user_data, "cancel")) {
		if (result == BT_ERROR_CANCELLED) {
			tet_printf("Callback: Cancellation of bonding succeeded");
			callback_result = true;
		} else {
			tet_printf("Callback: Cancellation of bonding failed");
		}
	} /* in case of bt_device_create_bond() */
	else if (user_data != NULL && !strcmp((char *)user_data, "bond")) {
		if (device_info == NULL) {
			tet_printf("Callback data is NULL");

			if (result == BT_ERROR_CANCELLED)
				tet_printf("No user input in the peer device");

			if (mainloop)
				g_main_loop_quit(mainloop);

			return;
		}

		if (device_info->remote_name != NULL && !strcmp(device_info->remote_name, target_name)) {
			if (result == BT_ERROR_NONE) {
				tet_printf("Callback: Bonding succeeded");
				tet_printf("Callback: serivce count : %d", device_info->service_count);
				callback_result = true;
				for (i = 0; i < device_info->service_count; i++) {
					tet_printf("Callback: service list[%d] - %s", i + 1, device_info->service_uuid[i]);
				}
			} else {
				tet_printf("Callback: Bonding failed");
			}
		}
	}

	if (mainloop) {
		g_main_loop_quit(mainloop);
	}
}

void device_unbonded_cb(int result, char *remote_address, void *user_data)
{
	tet_printf("Callback: bt_device_bond_destroyed_cb() was called.");
	tet_printf("Callback: target address");
	if (target_address != NULL) {
		tet_printf("device address: %s", target_address);
	} else
		tet_printf("Callback: target address is NULL!");

	tet_printf("Callback: address of unbonded device");
	if (remote_address != NULL) {
		tet_printf("device address: %s", remote_address);
	} else
		tet_printf("Callback: address of unbonded device is NULL!");

	if ((remote_address != NULL) && \
			!strcmp(remote_address, target_address)) {
		if (result == BT_ERROR_NONE) {
			tet_printf("Callback: Unbonding succeeded");
			callback_result = true;
		} else {
			tet_printf("Callback: Unbonding failed");
		}
	}

	if (mainloop) {
		g_main_loop_quit(mainloop);
	}

}

void device_authorization_state_changed_cb(bt_device_authorization_e authorization_state, \
		char *address, void *user_data)
{
	if (authorization_state == BT_DEVICE_AUTHORIZED) {
		if (address != NULL && !strcmp(address, target_address)) {
			callback_result = true;
			if (mainloop) {
				g_main_loop_quit(mainloop);
			}
		}
	} else {
		if (address != NULL && !strcmp(address, target_address)) {
			callback_result = true;
			if (mainloop) {
				g_main_loop_quit(mainloop);
			}
		}
	}
}

void device_get_rssi_strength_cb(char *bt_address,
						bt_device_connection_link_type_e link_type,
						int rssi_dbm, void *user_data)
{
	tet_printf("Callback:Rssi_strength_cb is called.");
	tet_printf("RSSI Strength: [Address %s][Link Type %d][RSSI dBm %d]",
			bt_address, link_type, rssi_dbm);
}

static void device_rssi_enabled_cb(const char *address,
		bt_device_connection_link_type_e link_type,
		int rssi_enabled, void *user_data)
{
	tet_printf("Callback:Rssi_enabled_cb is called.");
	tet_printf("RSSI Enabled: %s %d %d", address, link_type, rssi_enabled);
}

static void device_rssi_alert_cb(char *bt_address,
		bt_device_connection_link_type_e link_type,
		int rssi_alert_type, int rssi_alert_dbm, void *user_data)
{
	tet_printf("Callback:rssi_alert_cb is called.");
	tet_printf("RSSI Alert: [Address:%s LinkType:%d][RSSI Alert Type:%d dBm:%d]",
			bt_address, link_type, rssi_alert_type, rssi_alert_dbm);
}

/**
 * @brief Positive test case of bt_device_set_bond_created_cb()
 */
static void utc_network_bluetooth_device_set_bonded_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	/*  Normal operation test */
	ret = bt_device_set_bond_created_cb(device_bonded_cb, "cancel");
	dts_check_eq("bt_device_set_bond_created_cb", ret, BT_ERROR_NONE,
			"bt_device_set_bond_created_cb() failed");
}

/**
 * @brief Positive test case of bt_device_set_bond_destroyed_cb()
 */
static void utc_network_bluetooth_device_set_unbonded_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	/*  Normal operation test */
	ret = bt_device_set_bond_destroyed_cb(device_unbonded_cb, NULL);
	dts_check_eq("bt_device_set_bond_destroyed_cb", ret, BT_ERROR_NONE,
		"bt_device_set_bond_destroyed_cb() failed");
}

/**
 * @brief Positive test case of bt_device_set_authorization_changed_cb()
 */
static void
utc_network_bluetooth_device_set_authorization_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	/*  Normal operation test */
	ret = bt_device_set_authorization_changed_cb(device_authorization_state_changed_cb, NULL);
	dts_check_eq("bt_device_set_authorization_changed_cb", ret,
				BT_ERROR_NONE,
				"bt_device_set_authorization_changed_cb() failed");
}

/**
 * @brief Positive test case of bt_device_cancel_bonding()
 */
static void utc_network_bluetooth_device_cancel_bonding_p(void)
{
	int ret = BT_ERROR_NONE;
	callback_result = false;
	int timeout_id = 0;

	if (bt_device_set_bond_created_cb(device_bonded_cb, "cancel") !=
			BT_ERROR_NONE) {
			dts_fail("bt_device_cancel_bonding",
			"bt_device_set_bond_created_cb() failed");
	}

	/* bond device */
	ret = bt_device_create_bond(target_address);
	if (ret != BT_ERROR_NONE) {
		dts_fail("bt_device_cancel_bonding",
			"bt_device_create_bond failed");
	}
	tet_printf("bt_device_create_bond() was called");

	/* cancel bonding */
	ret = bt_device_cancel_bonding();
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_device_cancel_bonding() will be called");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		bt_device_unset_bond_created_cb();
		dts_check_eq("bt_device_cancel_bonding", callback_result, true,
				"Failed");
	} else {
		bt_device_unset_bond_created_cb();
		dts_fail("bt_device_cancel_bonding",
			"bt_device_cancel_bonding() failed");
	}
}

/**
 * @brief Positive test case of bt_device_unset_bond_created_cb()
 */
static void utc_network_bluetooth_device_unset_bonded_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	/*  Normal operation test */
	ret = bt_device_unset_bond_created_cb();
	dts_check_eq("bt_device_unset_bond_created_cb", ret, BT_ERROR_NONE,
				"bt_device_unset_bond_created_cb() failed");
}

/**
 * @brief Positive test case of bt_device_create_bond()
 */
static void utc_network_bluetooth_device_bond_p(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	callback_result = false;

	/* bond device */
	ret = bt_device_set_bond_created_cb(device_bonded_cb, "bond");
	if (ret != BT_ERROR_NONE) {
		tet_printf("bt_device_set_bond_created_cb was called");
	}

	ret = bt_device_create_bond(target_address);
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_device_bond_created_cb() will be called");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		bt_device_unset_bond_created_cb();
		dts_check_eq("bt_device_create_bond", ret, BT_ERROR_NONE,
								"bt_device_create_bond() failed.");
	} else {
		bt_device_unset_bond_created_cb();
		dts_fail("bt_device_create_bond",
			"bt_device_create_bond() failed");
	}
}


bool bonded_device_cb_for_device_p(bt_device_info_s *dev_info, void *user_data)
{
	int i = 0;
	device_info = (bt_device_info_s *) malloc(sizeof(bt_device_info_s));
	if (device_info == NULL) {
		tet_printf("Callback: Memory allocation failed");
	} else if (dev_info != NULL) {
		device_info->remote_address = strdup(dev_info->remote_address);
		tet_printf("device address : %s", device_info->remote_address);
		device_info->remote_name = strdup(dev_info->remote_name);
		tet_printf("device name : %s", device_info->remote_name);
		device_info->bt_class.major_device_class = dev_info->bt_class.major_device_class;
		tet_printf("major clasee : %2.2X", device_info->bt_class.major_device_class);
		device_info->bt_class.minor_device_class = dev_info->bt_class.minor_device_class;
		tet_printf("minor clasee : %2.2X", device_info->bt_class.minor_device_class);
		device_info->bt_class.major_service_class_mask = dev_info->bt_class.major_service_class_mask;
		tet_printf("service clasee : %2.2X", device_info->bt_class.major_service_class_mask);
		device_info->service_count = dev_info->service_count;
		tet_printf("service count : %d", device_info->service_count);
		if (device_info->service_count > 0) {
			device_info->service_uuid = (char **)malloc(sizeof(char *) * device_info->service_count);
			if (device_info->service_uuid == NULL) {
				tet_printf("Failed to allocate memory");
			} else {
				if (dev_info->service_uuid == NULL) {
					tet_printf("service_list is NULL");
				} else {
					tet_printf("Copy service list");
					for (i = 0; i < device_info->service_count; i++) {
						device_info->service_uuid[i] = strdup(dev_info->service_uuid[i]);
						tet_printf("Callback: service list[%d] - %s", i + 1, device_info->service_uuid[i]);
					}
				}
			}
		} else
			device_info->service_uuid = NULL;

		device_info->is_bonded = dev_info->is_bonded;
		tet_printf("is_bonded : %d", device_info->is_bonded);
		device_info->is_connected = dev_info->is_connected;
		tet_printf("is_connected : %d", device_info->is_connected);
		tet_printf("Callback: device_info was selected.");
	}

	return false;
}

/**
 * @brief Positive test case of bt_adapter_foreach_bonded_device()
 */
static void utc_network_bluetooth_device_foreach_bonded_device_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_foreach_bonded_device(bonded_device_cb_for_device_p, NULL);
	if (ret == BT_ERROR_NONE) {
		dts_pass("bt_adapter_foreach_bonded_device",
			"bt_adapter_foreach_bonded_device pass");
	} else {
		dts_fail("bt_adapter_foreach_bonded_device",
			"bt_adapter_foreach_bonded_device() failed");
	}
}

/**
 * @brief Positive test case of bt_device_set_alias()
 */
static void utc_network_bluetooth_device_set_alias_p(void)
{
	int ret = BT_ERROR_NONE;

	if (device_info == NULL) {
		tet_printf("No device to alias");
		dts_fail("bt_device_set_alias", "bt_device_set_alias() failed");
		return;
	}

	ret = bt_device_set_alias(device_info->remote_address, "dts_alias");
	dts_check_eq("bt_device_set_alias", ret, BT_ERROR_NONE,
			"bt_device_set_alias() failed");
}

/**
 * @brief Positive test case of bt_device_set_authorization()
 */
static void utc_network_bluetooth_device_set_authorization_p(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	callback_result = false;

	ret = bt_device_set_authorization(target_address, BT_DEVICE_AUTHORIZED);
	if (ret == BT_ERROR_NONE) {
		tet_printf("device_authorization_state_changed_cb will be called");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		dts_check_eq("bt_device_set_authorization", ret, BT_ERROR_NONE,
												"bt_device_set_authorization() failed.");
		callback_result = false;
	} else {
		dts_fail("bt_device_set_authorization",
			"bt_device_set_authorization() failed");
	}
}

/**
 * @brief Positive test case of bt_device_destroy_bond()
 */
static void utc_network_bluetooth_device_unbond_p(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;
	callback_result = false;

	tet_printf("bt_device_destroy_bond() will start");
	ret = bt_device_destroy_bond(target_address);
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_device_unbond_cb() will be called");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		dts_check_eq("bt_device_destroy_bond", ret, BT_ERROR_NONE,
										"bt_device_destroy_bond() failed.");
	} else {
		dts_fail("bt_device_destroy_bond",
			"bt_device_destroy_bond() failed");
	}
}

/**
 * @brief Positive test case of bt_device_unset_bond_destroyed_cb()
 */
static void utc_network_bluetooth_device_unset_unbonded_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	/*  Normal operation test */
	ret = bt_device_unset_bond_destroyed_cb();
	dts_check_eq("bt_device_unset_bond_destroyed_cb", ret, BT_ERROR_NONE,
		     "bt_device_unset_bond_destroyed_cb() failed");
}

/**
 * @brief Positive test case of bt_device_unset_authorization_changed_cb()
 */
static void utc_network_bluetooth_device_unset_authorization_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	/*  Normal operation test */
	ret = bt_device_unset_authorization_changed_cb();
	dts_check_eq("bt_device_unset_authorization_changed_cb", ret,
				BT_ERROR_NONE,
				"bt_device_unset_authorization_changed_cb() failed");
}
