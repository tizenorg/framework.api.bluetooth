/*
 * utc_network_bluetooth_common_negative.c
 *
 *  Created on: 20-Sep-2013
 *      Author: mrinal.m
 */

#include "bluetooth.h"
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
void adapter_state_changed_cb_for_common_n(int result,
					   bt_adapter_state_e adapter_state,
					   void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{NULL, 0},
};

static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_common_n, "startup") != BT_ERROR_NONE) {
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

void adapter_state_changed_cb_for_common_n(int result,
						bt_adapter_state_e adapter_state,
						void *user_data)
{

}
