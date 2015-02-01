/*
 * utc_network_bluetooth_avrcp_negative.c
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

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

const char target_name[128] = "dts_test";
char target_address[18] = "";

static GMainLoop *mainloop;

static void utc_network_bluetooth_audio_initialize_n(void);
static void utc_network_bluetooth_avrcp_set_equalizer_state_changed_n(void);
static void utc_network_bluetooth_avrcp_set_repeat_mode_changed_n(void);
static void utc_network_bluetooth_avrcp_set_shuffle_mode_changed_n(void);
static void utc_network_bluetooth_avrcp_set_scan_mode_changed_n(void);


void adapter_state_changed_cb_for_avrcp_n(int result,
					bt_adapter_state_e adapter_state,
					void *user_data);
gboolean timeout_func(gpointer data);


struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_audio_initialize_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_set_equalizer_state_changed_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_set_repeat_mode_changed_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_set_shuffle_mode_changed_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_set_scan_mode_changed_n, NEGATIVE_TC_IDX},
	{NULL, 0},
};


static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_avrcp_n, "startup") != BT_ERROR_NONE) {
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
 * @brief Negative test case of bt_avrcp_target_initialize()
 */

void adapter_state_changed_cb_for_avrcp_n(int result,
					  bt_adapter_state_e adapter_state,
					  void *user_data)
{

}

static void utc_network_bluetooth_audio_initialize_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_target_initialize(NULL, NULL);
	dts_check_eq("bt_avrcp_target_initialize", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_avrcp_target_initialize() failed.");

}

/**
 * @brief Negative test case of bt_avrcp_set_equalizer_state_changed_cb()
 */
static void utc_network_bluetooth_avrcp_set_equalizer_state_changed_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_set_equalizer_state_changed_cb(NULL, NULL);
	dts_check_eq("bt_avrcp_set_equalizer_state_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_avrcp_set_equalizer_state_changed_cb() failed.");

}

/**
 * @brief Negative test case of bt_avrcp_set_repeat_mode_changed_cb()
 */
static void utc_network_bluetooth_avrcp_set_repeat_mode_changed_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_set_repeat_mode_changed_cb(NULL, NULL);
	dts_check_eq("bt_avrcp_set_repeat_mode_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_avrcp_set_repeat_mode_changed_cb() failed.");

}

/**
 * @brief Negative test case of bt_avrcp_set_shuffle_mode_changed_cb()
 */
static void utc_network_bluetooth_avrcp_set_shuffle_mode_changed_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_set_shuffle_mode_changed_cb(NULL, NULL);
	dts_check_eq("bt_avrcp_set_shuffle_mode_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_avrcp_set_shuffle_mode_changed_cb() failed.");

}

/**
 * @brief Negative test case of bt_avrcp_set_scan_mode_changed_cb()
 */
static void utc_network_bluetooth_avrcp_set_scan_mode_changed_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_set_scan_mode_changed_cb(NULL, NULL);
	dts_check_eq("bt_avrcp_set_scan_mode_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_avrcp_set_scan_mode_changed_cb() failed.");

}
