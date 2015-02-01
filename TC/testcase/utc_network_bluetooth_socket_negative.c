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

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

static GMainLoop *mainloop;

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

static void utc_network_bluetooth_socket_set_data_received_cb_n(void);
static void utc_network_bluetooth_socket_set_connection_state_changed_cb_n(void);
static void utc_network_bluetooth_socket_unset_data_received_cb_n(void);
static void utc_network_bluetooth_socket_unset_connection_state_changed_cb_n(void);
static void utc_network_bluetooth_socket_create_rfcomm_n(void);
static void utc_network_bluetooth_socket_destroy_rfcomm_n(void);
static void utc_network_bluetooth_socket_listen_and_accept_rfcomm_n(void);
static void utc_network_bluetooth_socket_connect_rfcomm_n(void);
static void utc_network_bluetooth_socket_disconnect_rfcomm_n(void);
static void utc_network_bluetooth_socket_send_data_n(void);

void adapter_state_changed_cb_for_device_negative(int result,
					bt_adapter_state_e adapter_state, void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_socket_create_rfcomm_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_socket_set_connection_state_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_socket_listen_and_accept_rfcomm_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_socket_set_data_received_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_socket_connect_rfcomm_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_socket_send_data_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_socket_disconnect_rfcomm_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_socket_destroy_rfcomm_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_socket_unset_data_received_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_socket_unset_connection_state_changed_cb_n, NEGATIVE_TC_IDX},
	{NULL, 0},
};

static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_device_negative, "startup") != BT_ERROR_NONE) {
		tet_printf("DTS may fail because bt_adapter_set_state_changed_cb() failed");
	}

	tet_printf("bt_adapter_disable() was called.");
	ret = bt_adapter_disable();
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_adapter_disable() succeeded.");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
	} else if (ret == BT_ERROR_NOT_ENABLED) {
		tet_printf("Bluetooth adapter is not enabled.");
	} else {
		tet_printf("DTS may fail because bt_adapter_disable() failed");
	}

	if (bt_adapter_unset_state_changed_cb() != BT_ERROR_NONE) {
		tet_printf("bt_adapter_set_state_changed_cb() failed.");
	}

	tet_printf("TC start");
}

static void cleanup(void)
{
	/* end of TC */
	bt_deinitialize();
	tet_printf("TC end.");
}

gboolean timeout_func(gpointer data)
{
	tet_printf("Callback: Timeout.");
	g_main_loop_quit((GMainLoop *) data);
	return FALSE;
}

/**
 * @brief Callback funtions
 */
void adapter_state_changed_cb_for_device_negative(int result,
							bt_adapter_state_e adapter_state,
							void *user_data)
{
	tet_printf("Callback: bt_adapter_state_changed_cb was called.");
	if (user_data != NULL && !strcmp((char *)user_data, "startup")) {
		if (adapter_state == BT_ADAPTER_DISABLED && result == BT_ERROR_NONE) {
			tet_printf("Callback: BT was disabled. DTS will be started.");
		} else {
			tet_printf("Callback: BT was not disabled. DTS will be started but DTS may fail.");
		}

		if (mainloop) {
			g_main_loop_quit(mainloop);
		}
	}
}



/**
 * @brief Negative test case of bt_socket_set_data_received_cb()
 */
static void utc_network_bluetooth_socket_set_data_received_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_set_data_received_cb(NULL, NULL);
	dts_check_eq("bt_socket_set_data_received_cb", ret,
				BT_ERROR_INVALID_PARAMETER,
				"BT_ERROR_INVALID_PARAMETER must be returned when callback parameter is NULL");
}

/**
 * @brief Negative test case of bt_socket_set_connection_state_changed_cb()
 */
static void utc_network_bluetooth_socket_set_connection_state_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_set_connection_state_changed_cb(NULL, NULL);
	dts_check_eq("bt_socket_set_connection_state_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when callback parameter is NULL");
}

/**
 * @brief Negative test case of bt_socket_unset_data_received_cb()
 */
static void utc_network_bluetooth_socket_unset_data_received_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	if (bt_deinitialize() != BT_ERROR_NONE) {
		dts_fail("bt_socket_unset_data_received_cb",
				"bt_deinitialize() failed.");
	}

	ret = bt_socket_unset_data_received_cb();
	dts_check_eq("bt_socket_unset_data_received_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned when BT service is not initialized.");
}

/**
 * @brief Negative test case of bt_socket_unset_connection_state_changed_cb()
 */
static void utc_network_bluetooth_socket_unset_connection_state_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_unset_connection_state_changed_cb();
	dts_check_eq("bt_socket_unset_connection_state_changed_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned when BT service is not initialized.");
}

/**
 * @brief Negative test case of bt_socket_create_rfcomm()
 */
static void utc_network_bluetooth_socket_create_rfcomm_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_create_rfcomm(NULL, NULL);
	dts_check_eq("bt_socket_create_rfcomm", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when port_uuid parameter is NULL");
}

/**
 * @brief Negative test case of bt_socket_destroy_rfcomm()
 */
static void utc_network_bluetooth_socket_destroy_rfcomm_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_destroy_rfcomm(NULL);
	dts_check_eq("bt_socket_listen_and_accept_rfcomm", ret,
			BT_ERROR_NOT_ENABLED,
		"BT_ERROR_INVALID_PARAMETER must be returned when socket_fd is below 0");
}


/**
 * @brief Negative test case of bt_socket_listen_and_accept_rfcomm()
 */
static void utc_network_bluetooth_socket_listen_and_accept_rfcomm_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_listen_and_accept_rfcomm(1, 1);
	dts_check_eq("bt_socket_listen_and_accept_rfcomm", ret,
			BT_ERROR_NOT_ENABLED,
			"BT_ERROR_NOT_ENABLED must be returned when BT is not enabled");
}

/**
 * @brief Negative test case of bt_socket_connect_rfcomm()
 */
static void utc_network_bluetooth_socket_connect_rfcomm_n(void)
{
	int ret = BT_ERROR_NONE;
	char *address = "00:02:79:AC:7D:FD";
	char *uuid = "";

	ret = bt_socket_connect_rfcomm(address, uuid);
	dts_check_eq("bt_socket_connect_rfcomm", ret, BT_ERROR_NOT_ENABLED,
			"BT_ERROR_NOT_ENABLED must be returned when BT is not enabled");
}

/**
 * @brief Negative test case of bt_socket_disconnect_rfcomm()
 */
static void utc_network_bluetooth_socket_disconnect_rfcomm_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_socket_disconnect_rfcomm(-1);
	dts_check_eq("bt_socket_disconnect_rfcomm", ret, BT_ERROR_NOT_ENABLED,
			"BT_ERROR_NOT_ENABLED must be returned when BT is not enabled");
}

/**
 * @brief Negative test case of bt_socket_send_data()
 */
static void utc_network_bluetooth_socket_send_data_n(void)
{
	int ret = BT_ERROR_NONE;
	char *data = "dts_test";

	ret = bt_socket_send_data(1, data, sizeof(data));
	dts_check_eq("bt_socket_send_data", ret, BT_ERROR_NOT_ENABLED,
			"BT_ERROR_NOT_ENABLED must be returned when BT is not enabled");
}
