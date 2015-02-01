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

static void utc_network_bluetooth_opp_client_add_file_n(void);
static void utc_network_bluetooth_opp_client_push_files_n(void);

void adapter_state_changed_cb_for_opp_client_n(int result,
							bt_adapter_state_e adapter_state, void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_opp_client_add_file_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_opp_client_push_files_n, NEGATIVE_TC_IDX},
	{NULL, 0},
};


static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	bt_opp_client_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_opp_client_n, "startup") != BT_ERROR_NONE) {
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

void adapter_state_changed_cb_for_opp_client_n(int result,
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


static void utc_network_bluetooth_opp_client_add_file_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_client_add_file(NULL);
	dts_check_eq("bt_opp_client_add_file", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when file parameter is NULL");
}


static void utc_network_bluetooth_opp_client_push_files_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_client_push_files(NULL, NULL, NULL, NULL, NULL);
	dts_check_eq("bt_opp_client_push_files", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when parameters are NULL");
}


