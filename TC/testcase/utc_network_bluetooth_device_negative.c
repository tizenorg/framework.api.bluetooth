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
gboolean timeout_func(gpointer data);

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

static void utc_network_bluetooth_device_set_bonded_cb_n(void);
static void utc_network_bluetooth_device_set_unbonded_cb_n(void);
static void utc_network_bluetooth_device_set_authorization_state_changed_cb_n(void);
static void utc_network_bluetooth_device_cancel_bonding_n(void);
static void utc_network_bluetooth_device_bond_n(void);
static void utc_network_bluetooth_device_foreach_bonded_device_n(void);
static void utc_network_bluetooth_device_set_alias_n(void);
static void utc_network_bluetooth_device_set_authorization_n(void);
static void utc_network_bluetooth_device_unbond_n(void);
static void utc_network_bluetooth_device_unset_bonded_cb_n(void);
static void utc_network_bluetooth_device_unset_unbonded_cb_n(void);
static void utc_network_bluetooth_device_unset_authorization_state_changed_cb_n(void);
static void utc_network_bluetooth_device_get_rssi_strength_n(void);
static void utc_network_bluetooth_device_disable_rssi_monitor_n(void);
static void utc_network_bluetooth_device_enable_rssi_monitor_n(void);
static void utc_network_bluetooth_device_unset_rssi_alert_cb_n(void);

void adapter_state_changed_cb_for_device_negative(int result,
						bt_adapter_state_e adapter_state,
						void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_device_set_bonded_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_device_set_unbonded_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_device_set_authorization_state_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_device_bond_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_device_cancel_bonding_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_device_unbond_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_device_foreach_bonded_device_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_device_set_alias_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_device_set_authorization_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_device_get_rssi_strength_n, NEGATIVE_TC_IDX },
	{utc_network_bluetooth_device_enable_rssi_monitor_n, NEGATIVE_TC_IDX },
	{utc_network_bluetooth_device_disable_rssi_monitor_n, NEGATIVE_TC_IDX },
	{utc_network_bluetooth_device_unset_rssi_alert_cb_n, NEGATIVE_TC_IDX },
	{utc_network_bluetooth_device_unset_bonded_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_device_unset_unbonded_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_device_unset_authorization_state_changed_cb_n, NEGATIVE_TC_IDX},
	{NULL, 0},
};

gboolean timeout_func(gpointer data)
{
	tet_printf("Callback: Timeout.");
	g_main_loop_quit((GMainLoop *) data);
	return FALSE;
}

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

/**
 * @brief Callback funtions
 */
void adapter_state_changed_cb_for_device_negative(int result,
						bt_adapter_state_e adapter_state,
						void *user_data)
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

/**
 * @brief Negative test case of bt_device_set_bond_created_cb()
 */
static void utc_network_bluetooth_device_set_bonded_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_set_bond_created_cb(NULL, NULL);
	dts_check_eq("bt_device_set_bond_created_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when callback parameter is NULL");
}

/**
 * @brief Negative test case of bt_device_set_bond_destroyed_cb()
 */
static void utc_network_bluetooth_device_set_unbonded_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_set_bond_destroyed_cb(NULL, NULL);
	dts_check_eq("bt_device_set_bond_destroyed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when callback parameter is NULL");
}

/**
 * @brief Negative test case of bt_device_set_authorization_changed_cb()
 */
static void
utc_network_bluetooth_device_set_authorization_state_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_set_authorization_changed_cb(NULL, NULL);
	dts_check_eq("bt_device_set_authorization_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when callback parameter is NULL");
}

/**
 * @brief Negative test case of bt_device_unset_bond_created_cb()
 */
static void utc_network_bluetooth_device_unset_bonded_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_deinitialize();

	ret = bt_device_unset_bond_created_cb();
	dts_check_eq("bt_device_unset_bond_created_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned when BT is not initialized.");
}

	/**
	 * @brief Negative test case of bt_device_unset_bond_destroyed_cb()
	 */
static void utc_network_bluetooth_device_unset_unbonded_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_unset_bond_destroyed_cb();
	dts_check_eq("bt_device_unset_bond_destroyed_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned when BT service is not initialized.");
}

/**
 * @brief Negative test case of bt_device_unset_authorization_changed_cb()
 */
static void
utc_network_bluetooth_device_unset_authorization_state_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_unset_authorization_changed_cb();
	dts_check_eq("bt_device_unset_authorization_changed_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned when BT service is not initialized.");
}

	/**
	 * @brief Negative test case of bt_device_create_bond()
	 */
static void utc_network_bluetooth_device_bond_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_create_bond(NULL);
	dts_check_eq("bt_device_create_bond", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_NOT_INITIALIZED must be returned when BT service is not initialized.");
}

	/**
	 * @brief Negative test case of bt_device_cancel_bonding()
	 */
static void utc_network_bluetooth_device_cancel_bonding_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_cancel_bonding();
	dts_check_eq("bt_device_cancel_bonding", ret, BT_ERROR_NOT_ENABLED,
		"BT_ERROR_NOT_ENABLED must be returned when BT is not enabled");
}

/**
 * @brief Negative test case of bt_device_destroy_bond()
 */
static void utc_network_bluetooth_device_unbond_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_destroy_bond(NULL);
	dts_check_eq("bt_device_destroy_bond", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL");
}

/**
 * @brief Negative test case of bt_adapter_foreach_bonded_device()
 */
static void utc_network_bluetooth_device_foreach_bonded_device_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_foreach_bonded_device(NULL, NULL);
	dts_check_eq("bt_adapter_foreach_bonded_device", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL");
}

/**
 * @brief Negative test case of bt_device_set_alias()
 */
static void utc_network_bluetooth_device_set_alias_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_set_alias(NULL, "dts_alias");
	dts_check_eq("bt_device_set_alias", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when address parameter is NULL");
}

/**
 * @brief Negative test case of bt_device_set_authorization()
 */
static void utc_network_bluetooth_device_set_authorization_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_set_authorization(NULL, BT_DEVICE_AUTHORIZED);
	dts_check_eq("bt_device_set_authorization", ret,
		BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when address parameter is NULL");
}
