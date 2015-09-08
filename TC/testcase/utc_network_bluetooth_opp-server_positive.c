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
const char *dest = "/tmp";

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

static void utc_network_bluetooth_opp_server_initialize_p(void);
static void utc_network_bluetooth_opp_server_initialize_by_connection_request_p(void);
static void utc_network_bluetooth_opp_server_deinitialize_p(void);
static void utc_network_bluetooth_opp_server_accept_p(void);
static void utc_network_bluetooth_opp_server_reject_p(void);
static void utc_network_bluetooth_opp_server_set_destination_p(void);

void push_requested_cb_for_opp_server_p(const char *file, int size, void *user_data);
void connection_requested_cb_for_opp_server_p(const char *remote_address,
															void *user_data);
void adapter_state_changed_cb_for_opp_server_p(int result,
							bt_adapter_state_e adapter_state, void *user_data);
void transfer_finished_cb_for_opp_server_p(int result, const char *file,
											long long size, void *user_data);
void transfer_progress_cb_for_opp_server_p(int result, const char *file,
											long long size, void *user_data);


gboolean timeout_func(gpointer data);


struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_opp_server_initialize_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_opp_server_initialize_by_connection_request_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_opp_server_set_destination_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_opp_server_accept_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_opp_server_reject_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_opp_server_deinitialize_p, POSITIVE_TC_IDX},
	{NULL, 0},
};

static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_opp_server_p, "startup") != BT_ERROR_NONE) {
		tet_printf("DTS may fail because bt_adapter_set_state_changed_cb() failed");
	}

	tet_printf("bt_adapter_enable() was called.");
			ret = bt_adapter_enable();
			if (ret == BT_ERROR_NONE) {
				tet_printf("bt_adapter_enable() succeeded.");
				timeout_id = g_timeout_add(60000, timeout_func, mainloop);
				g_main_loop_run(mainloop);
				g_source_remove(timeout_id);
			} else if (ret != BT_ERROR_ALREADY_DONE) {
				tet_printf("DTS may fail because bt_adapter_disable() failed");
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


void adapter_state_changed_cb_for_opp_server_p(int result,
							bt_adapter_state_e adapter_state, void *user_data)
{

}


void push_requested_cb_for_opp_server_p(const char *file, int size,
															void *user_data)
{

}

void connection_requested_cb_for_opp_server_p(const char *remote_address,
															void *user_data)
{

}

void transfer_progress_cb_for_opp_server_p(int result, const char *file,
											long long size, void *user_data)
{

}

void transfer_finished_cb_for_opp_server_p(int result, const char *file,
											long long size, void *user_data)
{

}

static void utc_network_bluetooth_opp_server_initialize_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_server_initialize(dest, push_requested_cb_for_opp_server_p,
									NULL);

	if (ret != BT_ERROR_RESOURCE_BUSY) {
	dts_check_eq("bt_opp_server_initialize", ret, BT_ERROR_NONE,
						"bt_opp_server_initialize() failed");
	} else {
		dts_check_eq("bt_opp_server_initialize", ret, BT_ERROR_RESOURCE_BUSY,
				"bt_opp_server_initialize() failed");
	}


}

static void utc_network_bluetooth_opp_server_deinitialize_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_server_deinitialize();
	dts_check_eq("bt_opp_server_deinitialize", ret, BT_ERROR_NONE,
			"bt_opp_server_deinitialize() failed.");
}

static void utc_network_bluetooth_opp_server_accept_p(void)
{
	int ret = BT_ERROR_NONE;
	int tid = 0;
	const char *name = "abc";
	ret = bt_opp_server_accept(transfer_progress_cb_for_opp_server_p,
			transfer_finished_cb_for_opp_server_p, name, NULL, &tid);
	dts_check_eq("bt_opp_server_accept", ret,
			BT_ERROR_NONE, "bt_opp_server_accept() failed.");

}

static void utc_network_bluetooth_opp_server_reject_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_server_reject();
	dts_check_eq("bt_opp_server_reject", ret,
			BT_ERROR_NONE, "bt_opp_server_reject() failed.");
}

static void utc_network_bluetooth_opp_server_set_destination_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_server_set_destination(dest);
	dts_check_eq("bt_opp_server_set_destination", ret,
			BT_ERROR_NONE, "bt_opp_server_destination() failed");
}

static void utc_network_bluetooth_opp_server_initialize_by_connection_request_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_server_initialize_by_connection_request(dest,
				connection_requested_cb_for_opp_server_p, NULL);
	dts_check_eq("bt_opp_server_initialize_by_connection_request", ret,
				BT_ERROR_NONE,
				"bt_opp_server_initialize_by_connection_request() failed");

}






