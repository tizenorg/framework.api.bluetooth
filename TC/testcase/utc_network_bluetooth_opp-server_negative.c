/*
 *
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
const char *dest = "/tmp";

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

static void utc_network_bluetooth_opp_server_initialize_n(void);
static void utc_network_bluetooth_opp_server_initialize_by_connection_request_n(void);
static void utc_network_bluetooth_opp_server_accept_n(void);
static void utc_network_bluetooth_opp_server_set_destination_n(void);

void adapter_state_changed_cb_for_opp_server_n(int result,
							bt_adapter_state_e adapter_state, void *user_data);
void transfer_progress_cb_for_opp_server(const char *file, long long size,
												int percent, void *user_data);
void transfer_finished_cb_for_opp_server(int result, const char *file,
											long long size, void *user_data);
void push_requested_cb_for_opp_server(const char *file, int size, void *user_data);

gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_opp_server_initialize_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_opp_server_initialize_by_connection_request_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_opp_server_set_destination_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_opp_server_accept_n, NEGATIVE_TC_IDX},
	{NULL, 0},
};

static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_opp_server_n, "startup") != BT_ERROR_NONE) {
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
	g_main_loop_quit((GMainLoop *)data);
	return FALSE;
}

void push_requested_cb_for_opp_server(const char *file, int size,
											void *user_data)
{

}

void transfer_progress_cb_for_opp_server(const char *file, long long size,
											int percent, void *user_data)
{

}

void transfer_finished_cb_for_opp_server(int result, const char *file,
											long long size, void *user_data)
{

}

/**
 * @brief Callback funtions
 */
void adapter_state_changed_cb_for_opp_server_n(int result,
							bt_adapter_state_e adapter_state, void *user_data)
{
	tet_printf("Callback: bt_adapter_state_changed_cb was called.");
	if ((user_data != NULL) && !strcmp((char *)user_data, "startup")) {
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

static void utc_network_bluetooth_opp_server_initialize_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_server_initialize(NULL, NULL, NULL);
	dts_check_eq("bt_opp_server_initialize", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when destination parameter is NULL");
}


static void utc_network_bluetooth_opp_server_accept_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_opp_server_accept(transfer_progress_cb_for_opp_server,
						transfer_finished_cb_for_opp_server, NULL, NULL, NULL);
	dts_check_eq("bt_opp_server_accept", ret, BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_NOT_INITIALIZED must be returned.");

}


static void utc_network_bluetooth_opp_server_set_destination_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_server_initialize(dest, push_requested_cb_for_opp_server_p,
			NULL);

	ret = bt_opp_server_set_destination(NULL);
	dts_check_eq("bt_opp_server_set_destination", ret,
	BT_ERROR_INVALID_PARAMETER,
	"BT_ERROR_INVALID_PARAMETER must be returned destination parameter is NULL");
}

static void utc_network_bluetooth_opp_server_initialize_by_connection_request_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_server_initialize_by_connection_request(NULL, NULL, NULL);
	dts_check_eq("bt_opp_server_initialize_by_connection_request", ret,
	BT_ERROR_INVALID_PARAMETER,
	"BT_ERROR_INVALID_PARAMETER must be returned destination parameter is NULL");

}
