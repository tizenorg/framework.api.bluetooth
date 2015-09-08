/*
 * utc_network_bluetooth_common_positive.c
 *
 *  Created on: 20-Sep-2013
 *      Author: mrinal.m
 */

#include <bluetooth.h>
#include <tet_api.h>
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

void adapter_state_changed_cb_for_common_p(int result,
					bt_adapter_state_e adapter_state,
					void *user_data);
gboolean timeout_func(gpointer data);

static void utc_network_bluetooth_bt_initialize_p(void);
static void utc_network_bluetooth_bt_deinitialize_p(void);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_bt_initialize_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_bt_deinitialize_p, POSITIVE_TC_IDX},
	{NULL, 0},
};

static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_common_p, "startup") != BT_ERROR_NONE) {
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

void adapter_state_changed_cb_for_common_p(int result,
					bt_adapter_state_e adapter_state,
					void *user_data)
{

}

/**
 * @brief Positive test case of bt_initialize()
 */
static void utc_network_bluetooth_bt_initialize_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_initialize();
	dts_check_eq("bt_initialize", ret, BT_ERROR_NONE,
			"bt_initialize() failed.");

}

/**
 * @brief Positive test case of bt_deinitialize()
 */
static void utc_network_bluetooth_bt_deinitialize_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_deinitialize();
	dts_check_eq("bt_deinitialize", ret, BT_ERROR_NONE,
			"bt_deinitialize() failed.");

}



