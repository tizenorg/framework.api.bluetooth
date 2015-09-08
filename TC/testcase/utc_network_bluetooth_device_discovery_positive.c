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

static GMainLoop *mainloop;
static bool callback_result = false;

static void startup(void);
static void cleanup(void);

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

static void utc_network_bluetooth_device_discovery_get_status_p(void);
static void utc_network_bluetooth_device_discovery_set_state_changed_cb_p(void);
static void utc_network_bluetooth_device_discovery_start_p(void);
static void utc_network_bluetooth_device_discovery_unset_state_changed_cb_p(void);
static void utc_network_bluetooth_device_discovery_cancel_p(void);

gboolean timeout_func(gpointer data);
void adapter_state_changed_cb_for_discovery_p(int result,
						bt_adapter_state_e adapter_state,
						void *user_data);
void device_discovery_state_changed_cb_for_discovery_p(int result,
						bt_adapter_device_discovery_state_e discovery_state,
						bt_adapter_device_discovery_info_s *discovery_info,
						void *user_data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_device_discovery_get_status_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_device_discovery_set_state_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_device_discovery_start_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_device_discovery_unset_state_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_device_discovery_cancel_p, POSITIVE_TC_IDX},
	{NULL, 0},
};

static void startup(void)
{
	bt_error_e ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();

	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_discovery_p, "startup") != BT_ERROR_NONE) {
		tet_printf("DTS may fail because bt_adapter_set_state_changed_cb() failed.");
	}

	ret = bt_adapter_enable();
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_adapter_enable() succeeded.");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
	} else if (ret == BT_ERROR_ALREADY_DONE) {
		tet_printf("BT was already enabled.");
	} else {
		tet_printf("DTS may fail because bt_adapter_enable() failed.");
	}

	if (bt_adapter_unset_state_changed_cb() != BT_ERROR_NONE) {
		tet_printf
			("bt_adapter_unset_device_discovery_state_changed_cb() failed.");
	}

	tet_printf("TC start");
}

static void cleanup(void)
{
	/* end of TC */
	tet_printf("TC end");
	bt_deinitialize();
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
void adapter_state_changed_cb_for_discovery_p(int result,
						bt_adapter_state_e adapter_state,
						void *user_data)
{
	if (user_data != NULL && !strcmp((char *)user_data, "startup")) {
		if (adapter_state == BT_ADAPTER_ENABLED &&
				result == BT_ERROR_NONE) {
			tet_printf("Callback: BT was enabled.");
		} else {
			tet_printf("Callback: bt_adapter_state_changed_cb() was \
					called but bt_adapter_enable() failed.");
		}

		if (mainloop) {
			g_main_loop_quit(mainloop);
		}
	}
}

void device_discovery_state_changed_cb_for_discovery_p(int result,
					bt_adapter_device_discovery_state_e discovery_state,
					bt_adapter_device_discovery_info_s *discovery_info,
					void *user_data)
{
	tet_printf("Callback: device_discovery_state_changed_cb_for_discovery_p is called");

	if (discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FOUND) {
		tet_printf("Callback: Devices were found");
		tet_printf("Callback: Device name is %s", discovery_info->remote_name);
		tet_printf("Callback: Service class: %x", discovery_info->bt_class.major_service_class_mask);
		if (user_data != NULL && !strcmp((char *)user_data, "start")) {
			callback_result = true;
			if (mainloop) {
				g_main_loop_quit(mainloop);
			}
		}
	} else if (discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FINISHED) {
		tet_printf("Callback: Device discovery finished");
		if (user_data != NULL) {
			if (!strcmp((char *)user_data, "start") && callback_result == false) {
				tet_printf("You need a discoverable device");
				tet_printf("utc_network_bluetooth_device_discovery_start_p and \
						utc_network_bluetooth_device_discovery_cancel_p will fail");
			} else if (!strcmp((char *)user_data, "cancel")) {
				if (result == BT_ERROR_CANCELLED) {
					callback_result = true;
				}
			}
		}
		if (mainloop) {
			g_main_loop_quit(mainloop);
		}
	}
}

/**
 * @brief Positive test case of bt_adapter_is_discovering()
 */
static void utc_network_bluetooth_device_discovery_get_status_p(void)
{
	bt_error_e ret = BT_ERROR_NONE;
	bool status = false;

	ret = bt_adapter_is_discovering(&status);
	dts_check_eq("bt_adapter_is_discovering", ret, BT_ERROR_NONE,
			"bt_adapter_is_discovering() failed.");
}

/**
 * @brief Positive test case of bt_adapter_set_device_discovery_state_changed_cb()
 */
static void utc_network_bluetooth_device_discovery_set_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_set_device_discovery_state_changed_cb(device_discovery_state_changed_cb_for_discovery_p, "start");
	dts_check_eq("bt_adapter_set_device_discovery_state_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_adapter_set_device_discovery_state_changed_cb() failed.");
}

/**
 * @brief Positive test case of bt_adapter_start_device_discovery()
 */
static void utc_network_bluetooth_device_discovery_start_p(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	callback_result = false;

	ret = bt_adapter_start_device_discovery();
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_adapter_device_discovery_state_changed_cb_for_discovery_p() will be called.");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		dts_check_eq("bt_adapter_start_device_discovery",
				callback_result, true,
				"Callback was called but failed.");
	} else {
		dts_fail("bt_adapter_start_device_discovery",
				"bt_adapter_start_device_discovery() failed.");
	}
}

/**
 * @brief Positive test case of bt_adapter_unset_device_discovery_state_changed_cb()
 */
static void utc_network_bluetooth_device_discovery_unset_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_unset_device_discovery_state_changed_cb();
	dts_check_eq("bt_adapter_unset_device_discovery_state_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_device_discovery_uset_state_changed_cb() failed");
}

/**
 * @brief Positive test case of bt_adapter_stop_device_discovery()
 */
static void utc_network_bluetooth_device_discovery_cancel_p(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	callback_result = false;

	if (bt_adapter_set_device_discovery_state_changed_cb
		(device_discovery_state_changed_cb_for_discovery_p, "cancel") != BT_ERROR_NONE) {
		dts_fail("bt_adapter_stop_device_discovery",
			"bt_adapter_set_device_discovery_state_changed_cb() failed");
	}

	ret = bt_adapter_stop_device_discovery();
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_adapter_device_discovery_state_changed_cb_for_discovery_p() will be called");   
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		bt_adapter_unset_device_discovery_state_changed_cb();
		dts_check_eq("bt_adapter_stop_device_discovery", callback_result, true,
					"Callback was called but failed");
	} else {
		dts_fail("bt_adapter_stop_device_discovery",
				"bt_adapter_stop_device_discovery() failed");
	}

}

