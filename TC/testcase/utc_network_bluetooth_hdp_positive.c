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

void(*tet_startup)(void) = startup;
void(*tet_cleanup)(void) = cleanup;

const char *appid = "/org/bluez/health_app_2";
const char *remote_adr = "00:22:58:07:77:BB";

static void utc_network_bluetooth_hdp_register_sink_app_p(void);
static void utc_network_bluetooth_hdp_unregister_sink_app_p(void);
static void utc_network_bluetooth_hdp_connect_to_source_p(void);
static void utc_network_bluetooth_hdp_disconnect_p(void);
static void utc_network_bluetooth_hdp_send_data_p(void);
static void utc_network_bluetooth_hdp_set_connection_state_changed_cb_p(void);
static void utc_network_bluetooth_hdp_unset_connection_state_changed_cb_p(void);
static void utc_network_bluetooth_hdp_set_data_received_cb_p(void);
static void utc_network_bluetooth_hdp_unset_data_received_cb_p(void);

void data_received_cb_for_hdp_p(unsigned int channel, const char *data,
					unsigned int size, void *user_data);
void connected_cb_for_hdp_p(int result, const char *remote_address,
					const char *app_id,
					bt_hdp_channel_type_e type, unsigned int channel, void *user_data);
void disconnected_cb_for_hdp_p(int result, const char *remote_address,
					unsigned int channel, void *user_data);



void adapter_state_changed_cb_for_hdp_p(int result,
							bt_adapter_state_e adapter_state, void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_hdp_register_sink_app_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_hdp_set_connection_state_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_hdp_set_data_received_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_hdp_connect_to_source_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_hdp_send_data_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_hdp_disconnect_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_hdp_unset_connection_state_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_hdp_unset_data_received_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_hdp_unregister_sink_app_p, POSITIVE_TC_IDX},
	{NULL, 0},
};


static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_hdp_p, "startup") != BT_ERROR_NONE) {
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
	g_main_loop_quit((GMainLoop *) data);
	return FALSE;
}

void adapter_state_changed_cb_for_hdp_p(int result,
					bt_adapter_state_e adapter_state, void *user_data)
{

}

void data_received_cb_for_hdp_p(unsigned int channel, const char *data,
					unsigned int size, void *user_data)
{

}

void connected_cb_for_hdp_p(int result, const char *remote_address,
					const char *app_id,
					bt_hdp_channel_type_e type, unsigned int channel, void *user_data)
{

}

void disconnected_cb_for_hdp_p(int result, const char *remote_address,
					unsigned int channel, void *user_data)
{

}

static void utc_network_bluetooth_hdp_set_data_received_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_hdp_set_data_received_cb(data_received_cb_for_hdp_p, NULL);
	dts_check_eq("bt_hdp_set_data_received_cb", ret, BT_ERROR_NONE,
					"bt_hdp_set_data_received_cb() failed.");
}


static void utc_network_bluetooth_hdp_unset_data_received_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_hdp_unset_data_received_cb();
	dts_check_eq("bt_hdp_unset_data_received_cb", ret, BT_ERROR_NONE,
					"bt_hdp_unset_data_received_cb() failed.");
}

static void utc_network_bluetooth_hdp_set_connection_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_hdp_set_connection_state_changed_cb(connected_cb_for_hdp_p,
				disconnected_cb_for_hdp_p, NULL);
	dts_check_eq("bt_hdp_set_connection_state_changed_cb", ret, BT_ERROR_NONE,
				"bt_hdp_set_connection_state_changed_cb() failed.");
}

static void utc_network_bluetooth_hdp_unset_connection_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_hdp_unset_connection_state_changed_cb();
	dts_check_eq("bt_hdp_unset_connection_state_changed_cb", ret, BT_ERROR_NONE,
		"bt_hdp_unset_connection_state_changed_cb() failed.");
}

static void utc_network_bluetooth_hdp_send_data_p(void)
{
	int ret = BT_ERROR_NONE;
	char *dts_test = "dts_test";

	ret = bt_hdp_send_data(1, "dts_test", sizeof(dts_test));
	dts_check_eq("bt_hdp_send_data", ret, BT_ERROR_NONE,
			"bt_hdp_send_data() failed.");
}

static void utc_network_bluetooth_hdp_disconnect_p(void)
{
	int ret = BT_ERROR_NONE;
	char *remote_adr = "00:22:58:07:77:BB";

	ret = bt_hdp_disconnect(remote_adr, 0);
	dts_check_eq("bt_hdp_disconnect", ret, BT_ERROR_NONE,
					"bt_hdp_disconnect() failed.");
}

static void utc_network_bluetooth_hdp_connect_to_source_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_hdp_connect_to_source(remote_adr, appid);
	dts_check_eq("bt_hdp_connect_to_source", ret, BT_ERROR_NONE,
				"bt_hdp_connect_to_source() failed.");

}

static void utc_network_bluetooth_hdp_unregister_sink_app_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_hdp_unregister_sink_app(appid);
	dts_check_eq("bt_hdp_unregister_sink_app", ret, BT_ERROR_NONE,
					"bt_hdp_unregister_sink_app() failed.");

}

static void utc_network_bluetooth_hdp_register_sink_app_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_hdp_register_sink_app(1, &appid);
	dts_check_eq("bt_hdp_register_sink_app", ret, BT_ERROR_NONE,
					"bt_hdp_register_sink_app() failed.");

}





