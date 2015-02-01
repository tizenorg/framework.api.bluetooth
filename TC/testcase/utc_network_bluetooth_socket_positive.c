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

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

const char target_name[128] = "dts_test";
const char *rfcomm_test_uuid_spp = "00001101-0000-1000-8000-00805F9B34FB";
char *target_address;
int socket_fd = 0;
bool callback_result = false;
bool discovery_result = false;
bool bonding_result = false;
bool is_connecting = false;
static GMainLoop *mainloop;

static void utc_network_bluetooth_socket_set_data_received_cb_p(void);
static void utc_network_bluetooth_socket_set_connection_state_changed_cb_p(void);

static void utc_network_bluetooth_socket_create_rfcomm_p(void);
static void utc_network_bluetooth_socket_destroy_rfcomm_p(void);
static void utc_network_bluetooth_socket_listen_and_accept_rfcomm_p(void);
static void utc_network_bluetooth_socket_connect_rfcomm_p(void);
static void utc_network_bluetooth_socket_disconnect_rfcomm_p(void);
static void utc_network_bluetooth_socket_send_data_p(void);
static void utc_network_bluetooth_socket_unset_data_received_cb_p(void);
static void utc_network_bluetooth_socket_unset_connection_state_changed_cb_p(void);

gboolean timeout_func(gpointer data);
void adapter_state_changed_cb_for_socket_p(int result,
						bt_adapter_state_e adapter_state, void *user_data);
void device_discovery_state_changed_cb_for_socket_p(int result,
						bt_adapter_device_discovery_state_e discovery_state,
		bt_adapter_device_discovery_info_s *discovery_info, void *user_data);
void device_bonded_cb_for_socket_p(int result, bt_device_info_s *device_info,
						void *user_data);
void socket_data_received_cb_for_socket_p(bt_socket_received_data_s *data,
						void *user_data);
void socket_connection_state_changed_cb_for_socket_p(int result,
						bt_socket_connection_state_e connection_state,
						bt_socket_connection_s *connection, void *user_data);
void device_unbonded_cb(int result, char *remote_address, void *user_data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_socket_create_rfcomm_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_socket_set_connection_state_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_socket_listen_and_accept_rfcomm_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_socket_set_data_received_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_socket_connect_rfcomm_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_socket_send_data_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_socket_disconnect_rfcomm_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_socket_destroy_rfcomm_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_socket_unset_data_received_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_socket_unset_connection_state_changed_cb_p, POSITIVE_TC_IDX},
	{NULL, 0},
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
	tet_printf("TC start.");
	callback_result = false;
	discovery_result = false;
	bonding_result = false;
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();

	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_socket_p, "startup") != BT_ERROR_NONE) {
		tet_printf("bt_adapter_set_state_changed_cb() failed.");
	}

	if (bt_adapter_set_device_discovery_state_changed_cb(device_discovery_state_changed_cb_for_socket_p, "startup") != BT_ERROR_NONE) {
		tet_printf("bt_adapter_set_device_discovery_state_changed_cb() failed.");
	}

	if (bt_device_set_bond_created_cb(device_bonded_cb_for_socket_p, "startup") != BT_ERROR_NONE) {
		tet_printf("bt_device_set_bond_created_cb() failed.");
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

	if (discovery_result == true && bt_device_create_bond(target_address) == BT_ERROR_NONE) {
		tet_printf("bt_device_create_bond() succeeded.");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
	} else {
		tet_printf("bt_device_create_bond() failed.");
	}

	if (bt_adapter_unset_state_changed_cb() != BT_ERROR_NONE) {
		tet_printf("bt_adapter_unset_state_changed_cb() failed.");
	}

	if (bt_adapter_unset_device_discovery_state_changed_cb() != BT_ERROR_NONE) {
		tet_printf("bt_adapter_unset_device_discovery_state_changed_cb() failed.");
	}

	if (bt_device_unset_bond_created_cb() != BT_ERROR_NONE) {
		tet_printf("bt_device_unset_bond_created_cb() failed.");
	}

}

static void cleanup(void)
{
	/* end of TC */
	int timeout_id = 0;

	/* Destroy a bond */
	bt_device_set_bond_destroyed_cb(device_unbonded_cb, NULL);
	if (bt_device_destroy_bond(target_address) == BT_ERROR_NONE) {
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
	}

	bt_deinitialize();
	tet_printf("TC end.");
}

gboolean timeout_func(gpointer data)
{
	tet_printf("[%s] Callback: Timeout.", __FUNCTION__);
	g_main_loop_quit((GMainLoop *) data);
	return FALSE;
}

/**
 * @brief Callback funtions
 */
void device_unbonded_cb(int result, char *remote_address, void *user_data)
{
	tet_printf("Callback: bt_device_bond_destroyed_cb() was called.");

	if (mainloop) {
		g_main_loop_quit(mainloop);
	}

}

void adapter_state_changed_cb_for_socket_p(int result,
					bt_adapter_state_e adapter_state, void *user_data)
{
	if (user_data != NULL && !strcmp((char *)user_data, "startup")) {
		if (adapter_state == BT_ADAPTER_ENABLED && result == BT_ERROR_NONE) {
			tet_printf("Callback: BT was enabled.");
			bt_adapter_start_device_discovery();
		} else {
			tet_printf("Callback: BT was not enabled. DTS will be started but DTS may fail.");
			if (mainloop) {
				g_main_loop_quit(mainloop);
			}
		}
	}
}

void device_discovery_state_changed_cb_for_socket_p(int result,
			bt_adapter_device_discovery_state_e discovery_state,
			bt_adapter_device_discovery_info_s *discovery_info, void *user_data)
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
				tet_printf("Callback: device address: %s", discovery_info->remote_address);
				if (bt_adapter_stop_device_discovery() == BT_ERROR_NONE) {
					tet_printf("Callback: Device discovery will be stopped");
				}
			}
		}
	}
}

void device_bonded_cb_for_socket_p(int result, bt_device_info_s *device_info, void *user_data)
{
	if ((user_data != NULL) && !strcmp((char *)user_data, "startup")) {
		if ((device_info->remote_address != NULL) &&
				!strcmp(device_info->remote_address, target_address)) {
			if (result == BT_ERROR_NONE) {
				tet_printf("Callback: Bonding succeeded");
				bonding_result = true;
			} else {
				tet_printf("Callback: Bonding failed");
			}
		}
	}

	if (mainloop) {
		g_main_loop_quit(mainloop);
	}
}

void socket_data_received_cb_for_socket_p(bt_socket_received_data_s *data,
					void *user_data)
{
}

void socket_connection_state_changed_cb_for_socket_p(int result,
				bt_socket_connection_state_e connection_state,
				bt_socket_connection_s *connection, void *user_data)
{
	tet_printf("Callback: bt_socket_connection_state_changed_cb() was called");
	if (connection_state == BT_SOCKET_CONNECTED)
		tet_printf("Callback: Connected");
	else
		tet_printf("Callback: Disconnected");

	if (connection_state == BT_SOCKET_CONNECTED) {
		tet_printf("Callback: [BT_SOCKET_CONNECTED] result = %d", result);
		if (result == BT_ERROR_NONE && is_connecting == true) {
			callback_result = true;
			socket_fd = connection->socket_fd;
			tet_printf("Callback: socket_fd = %d", socket_fd);

			if (mainloop) {
				g_main_loop_quit(mainloop);
			}
		}
	} else if (connection_state == BT_SOCKET_DISCONNECTED) {
		tet_printf("Callback: [BT_SOCKET_DISCONNECTED] result = %d", result);
		if (result == BT_ERROR_NONE && is_connecting == false) {
			callback_result = true;
			socket_fd = connection->socket_fd;
			tet_printf("Callback: socket_fd = %d", socket_fd);

			if (mainloop) {
				g_main_loop_quit(mainloop);
			}
		}
	}
}

/**
 * @brief Positive test case of bt_socket_set_data_received_cb()
 */
static void utc_network_bluetooth_socket_set_data_received_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_set_data_received_cb(socket_data_received_cb_for_socket_p, NULL);
	dts_check_eq("bt_socket_set_data_received_cb", ret, BT_ERROR_NONE,
				"bt_socket_set_data_received_cb() failed.");
}

/**
 * @brief Positive test case of bt_socket_set_connection_state_changed_cb()
 */
static void utc_network_bluetooth_socket_set_connection_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_set_connection_state_changed_cb(socket_connection_state_changed_cb_for_socket_p, NULL);
	dts_check_eq("bt_socket_set_connection_state_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_socket_set_connection_state_changed_cb() failed.");
}

/**
 * @brief Positive test case of bt_socket_create_rfcomm()
 */
static void utc_network_bluetooth_socket_create_rfcomm_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_create_rfcomm(rfcomm_test_uuid_spp, &socket_fd);
	dts_check_eq("bt_socket_create_rfcomm", ret, BT_ERROR_NONE,
			"bt_socket_create_rfcomm() failed.");
}

/**
 * @brief Positive test case of bt_socket_listen_and_accept_rfcomm()
 */
static void utc_network_bluetooth_socket_listen_and_accept_rfcomm_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_listen_and_accept_rfcomm(socket_fd, 1);
	dts_check_eq("bt_socket_listen_and_accept_rfcomm", ret, BT_ERROR_NONE,
			"bt_socket_listen_and_accept_rfcomm() failed.");
}

/**
 * @brief Positive test case of bt_socket_destroy_rfcomm()
 */
static void utc_network_bluetooth_socket_destroy_rfcomm_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_destroy_rfcomm(socket_fd);
	dts_check_eq("bt_socket_destroy_rfcomm", ret, BT_ERROR_NONE,
			"bt_socket_destroy_rfcomm() failed.");
}

/**
 * @brief Positive test case of bt_socket_connect_rfcomm()
 */
static void utc_network_bluetooth_socket_connect_rfcomm_p(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	is_connecting = true;
	callback_result = false;
	ret = bt_socket_connect_rfcomm(target_address, rfcomm_test_uuid_spp);
	if (ret == BT_ERROR_NONE) {
		dts_check_eq("bt_socket_connect_rfcomm", ret, BT_ERROR_NONE,
						"bt_socket_connect_rfcomm() failed.");
		tet_printf("bt_socket_connect_rfcomm() succeeded.");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
	} else {
		dts_fail("bt_socket_connect_rfcomm",
				"bt_socket_connect_rfcomm() failed");
	}
}

/**
 * @brief Positive test case of bt_socket_disconnect_rfcomm()
 */
static void utc_network_bluetooth_socket_disconnect_rfcomm_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_disconnect_rfcomm(socket_fd);
	dts_check_eq("bt_socket_disconnect_rfcomm", ret, BT_ERROR_NONE,
				"bt_socket_disconnect_rfcomm() failed.");
}

/**
 * @brief Positive test case of bt_socket_send_data()
 */
static void utc_network_bluetooth_socket_send_data_p(void)
{
	int ret = BT_ERROR_NONE;
	char *dts_test = "dts_test";
	int socket_fd = 1;
	ret = bt_socket_send_data(socket_fd, "dts_test", sizeof(dts_test));
	dts_check_eq("bt_socket_send_data", ret, BT_ERROR_NONE,
				"bt_socket_send_data() failed.");
}

/**
 * @brief Positive test case of bt_socket_unset_data_received_cb()
 */
static void utc_network_bluetooth_socket_unset_data_received_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_unset_data_received_cb();
	dts_check_eq("bt_socket_unset_data_received_cb", ret, BT_ERROR_NONE,
				"bt_socket_unset_data_received_cb() failed.");
}

/**
 * @brief Positive test case of bt_socket_unset_connection_state_changed_cb()
 */
static void utc_network_bluetooth_socket_unset_connection_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_unset_connection_state_changed_cb();
	dts_check_eq("bt_socket_unset_connection_state_changed_cb", ret,
				BT_ERROR_NONE,
				"bt_socket_unset_connection_state_changed_cb() failed.");
}
