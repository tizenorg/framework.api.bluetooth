/*
 * utc_network_bluetooth_pan_negative.c
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

static void utc_network_bluetooth_nap_set_connection_state_changed_n(void);
static void utc_network_bluetooth_panu_set_connection_state_changed_n(void);
static void utc_network_bluetooth_panu_connect_n(void);
static void utc_network_bluetooth_nap_disconnect_n(void);
static void utc_network_bluetooth_panu_disconnect_n(void);

void adapter_state_changed_cb_for_pan_n(int result,
						bt_adapter_state_e adapter_state, void *user_data);
gboolean timeout_func(gpointer data);


struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_nap_set_connection_state_changed_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_panu_set_connection_state_changed_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_panu_connect_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_nap_disconnect_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_panu_disconnect_n, NEGATIVE_TC_IDX},
	{NULL, 0},
};

static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_pan_n, "startup") != BT_ERROR_NONE) {
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




/**
 * @brief Negative test case of bt_nap_set_connection_state_changed_cb()
 */

void adapter_state_changed_cb_for_pan_n(int result,
					bt_adapter_state_e adapter_state,
					void *user_data)
{

}

static void utc_network_bluetooth_nap_set_connection_state_changed_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_nap_set_connection_state_changed_cb(NULL, NULL);
	dts_check_eq("bt_nap_set_connection_state_changed_cb", ret,
		     BT_ERROR_INVALID_PARAMETER,
		     "bt_nap_set_connection_state_changed_cb() failed.");

}

/**
 * @brief Negative test case of bt_panu_set_connection_state_changed_cb()
 */
static void utc_network_bluetooth_panu_set_connection_state_changed_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_panu_set_connection_state_changed_cb(NULL, NULL);
	dts_check_eq("bt_panu_set_connection_state_changed_cb", ret,
		     BT_ERROR_INVALID_PARAMETER,
		     "bt_panu_set_connection_state_changed_cb() failed.");

}

/**
 * @brief Negative test case of bt_panu_connect()
 */
static void utc_network_bluetooth_panu_connect_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_panu_connect(NULL, BT_PANU_SERVICE_TYPE_NAP);
	dts_check_eq("bt_panu_connect", ret, BT_ERROR_INVALID_PARAMETER,
			"bt_panu_connect() failed.");

}

/**
 * @brief Negative test case of bt_nap_disconnect()
 */
static void utc_network_bluetooth_nap_disconnect_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_nap_disconnect(NULL);
	dts_check_eq("bt_nap_disconnect", ret, BT_ERROR_INVALID_PARAMETER,
			"bt_nap_disconnect() failed.");

}
/**
 * @brief Negative test case of bt_panu_disconnect()
 */
static void utc_network_bluetooth_panu_disconnect_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_panu_disconnect(NULL);
	dts_check_eq("bt_panu_disconnect", ret, BT_ERROR_INVALID_PARAMETER,
			"bt_panu_disconnect() failed.");

}
