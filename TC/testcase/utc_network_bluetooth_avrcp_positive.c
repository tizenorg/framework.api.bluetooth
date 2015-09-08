/*
 * utc_network_bluetooth_avrcp_positive.c
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

void target_connection_state_changed_cb_for_avrcp_p(bool connected,
						const char *remote_address,
						void *user_data);
void equalizer_state_changed_cb_for_avrcp_p(bt_avrcp_equalizer_state_e equalizer,
						void *user_data);
void repeat_mode_changed_cb_for_avrcp_p(bt_avrcp_repeat_mode_e repeat,
						void *user_data);
void shuffle_mode_changed_cb_for_avrcp_p(bt_avrcp_shuffle_mode_e shuffle,
						void *user_data);
void scan_mode_changed_cb_for_avrcp_p(bt_avrcp_scan_mode_e scan,
						void *user_data);

static void utc_network_bluetooth_avrcp_targe_initialize_p(void);
static void utc_network_bluetooth_avrcp_target_deinitialize_p(void);
static void utc_network_bluetooth_avrcp_target_notify_equalizer_state_p(void);
static void utc_network_bluetooth_avrcp_target_notify_repeat_mode_p(void);
static void utc_network_bluetooth_avrcp_target_notify_shuffle_mode_p(void);
static void utc_network_bluetooth_avrcp_target_notify_scan_mode_p(void);
static void utc_network_bluetooth_avrcp_target_notify_player_state_p(void);
static void utc_network_bluetooth_avrcp_target_notify_position_p(void);
static void utc_network_bluetooth_avrcp_target_notify_track_p(void);
static void utc_network_bluetooth_avrcp_set_equalizer_state_changed_p(void);
static void utc_network_bluetooth_avrcp_unset_equalizer_state_changed_p(void);
static void utc_network_bluetooth_avrcp_set_repeat_mode_changed_p(void);
static void utc_network_bluetooth_avrcp_unset_repeat_mode_changed_p(void);
static void utc_network_bluetooth_avrcp_set_shuffle_mode_changed_p(void);
static void utc_network_bluetooth_avrcp_unset_shuffle_mode_changed_p(void);
static void utc_network_bluetooth_avrcp_set_scan_mode_changed_p(void);
static void utc_network_bluetooth_avrcp_unset_scan_mode_changed_p(void);

void adapter_state_changed_cb_for_avrcp_p(int result,
					bt_adapter_state_e adapter_state,
					void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_avrcp_targe_initialize_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_target_notify_equalizer_state_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_target_notify_repeat_mode_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_target_notify_shuffle_mode_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_target_notify_scan_mode_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_target_notify_player_state_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_target_notify_position_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_target_notify_track_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_set_equalizer_state_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_unset_equalizer_state_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_set_repeat_mode_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_unset_repeat_mode_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_set_shuffle_mode_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_unset_shuffle_mode_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_set_scan_mode_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_unset_scan_mode_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_avrcp_target_deinitialize_p, POSITIVE_TC_IDX},
	{NULL, 0},
};

static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_avrcp_p, "startup") != BT_ERROR_NONE) {
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

/**
 * @brief Callback funtions
 */

void adapter_state_changed_cb_for_avrcp_p(int result,
					bt_adapter_state_e adapter_state,
					void *user_data)
{

}

void target_connection_state_changed_cb_for_avrcp_p(bool connected,
					const char *remote_address,
					void *user_data)
{

}

void equalizer_state_changed_cb_for_avrcp_p(
					bt_avrcp_equalizer_state_e equalizer,
					void *user_data)
{

}

void repeat_mode_changed_cb_for_avrcp_p(bt_avrcp_repeat_mode_e repeat,
					void *user_data)
{

}

void shuffle_mode_changed_cb_for_avrcp_p(bt_avrcp_shuffle_mode_e shuffle,
					void *user_data)
{

}

void scan_mode_changed_cb_for_avrcp_p(bt_avrcp_scan_mode_e scan,
					void *user_data)
{

}

/**
 * @brief Positive test case of bt_avrcp_target_initialize()
 */
static void utc_network_bluetooth_avrcp_targe_initialize_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_target_initialize(
								target_connection_state_changed_cb_for_avrcp_p,
								NULL);
	dts_check_eq("bt_avrcp_target_initialize", ret, BT_ERROR_NONE,
			"bt_avrcp_target_initialize() failed.");

}

/**
 * @brief Positive test case of bt_avrcp_target_deinitialize()
 */
static void utc_network_bluetooth_avrcp_target_deinitialize_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_target_deinitialize();
	dts_check_eq("bt_avrcp_target_deinitialize", ret, BT_ERROR_NONE,
			"bt_avrcp_target_deinitialize() failed.");

}

/**
 * @brief Positive test case of bt_avrcp_target_notify_equalizer_state()
 */
static void utc_network_bluetooth_avrcp_target_notify_equalizer_state_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_target_notify_equalizer_state(BT_AVRCP_EQUALIZER_STATE_ON);
	dts_check_eq("bt_avrcp_target_notify_equalizer_state", ret,
			BT_ERROR_NONE,
			"bt_avrcp_target_notify_equalizer_state() failed.");

}

/**
 * @brief Positive test case of bt_avrcp_target_notify_repeat_mode()
 */
static void utc_network_bluetooth_avrcp_target_notify_repeat_mode_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_target_notify_repeat_mode(BT_AVRCP_REPEAT_MODE_ALL_TRACK);
	dts_check_eq("bt_avrcp_target_notify_repeat_mode", ret, BT_ERROR_NONE,
			"bt_avrcp_target_notify_repeat_mode() failed.");

}

/**
 * @brief Positive test case of bt_avrcp_target_notify_shuffle_mode()
 */
static void utc_network_bluetooth_avrcp_target_notify_shuffle_mode_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_target_notify_shuffle_mode(BT_AVRCP_SHUFFLE_MODE_ALL_TRACK);
	dts_check_eq("bt_avrcp_target_notify_shuffle_mode", ret, BT_ERROR_NONE,
			"bt_avrcp_target_notify_shuffle_mode() failed.");

}

/**
 * @brief Positive test case of bt_avrcp_target_notify_scan_mode()
 */
static void utc_network_bluetooth_avrcp_target_notify_scan_mode_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_target_notify_scan_mode(BT_AVRCP_SCAN_MODE_ALL_TRACK);
	dts_check_eq("bt_avrcp_target_notify_scan_mode", ret, BT_ERROR_NONE,
			"bt_avrcp_target_notify_scan_mode() failed.");
}

/**
 * @brief Positive test case of bt_avrcp_target_notify_player_state()
 */
static void utc_network_bluetooth_avrcp_target_notify_player_state_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_target_notify_player_state(BT_AVRCP_PLAYER_STATE_PLAYING);
	dts_check_eq("bt_avrcp_target_notify_player_state", ret, BT_ERROR_NONE,
			"bt_avrcp_target_notify_player_state() failed.");
}

/**
 * @brief Positive test case of bt_avrcp_target_notify_position()
 */
static void utc_network_bluetooth_avrcp_target_notify_position_p(void)
{
	int ret = BT_ERROR_NONE;
	unsigned int position = 0;
	ret = bt_avrcp_target_notify_position(position);
	dts_check_eq("bt_avrcp_target_notify_position", ret, BT_ERROR_NONE,
			"bt_avrcp_target_notify_position() failed.");
}

/**
 * @brief Positive test case of bt_avrcp_target_notify_track()
 */
static void utc_network_bluetooth_avrcp_target_notify_track_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_target_notify_track("title", "artist", "album", "genre", 5, 20, 2);
	dts_check_eq("bt_avrcp_target_notify_track", ret, BT_ERROR_NONE,
			"bt_avrcp_target_notify_track() failed.");
}

/**
 * @brief Positive test case of bt_avrcp_set_equalizer_state_changed_cb()
 */
static void utc_network_bluetooth_avrcp_set_equalizer_state_changed_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_set_equalizer_state_changed_cb(
									equalizer_state_changed_cb_for_avrcp_p,
									NULL);
	dts_check_eq("bt_avrcp_set_equalizer_state_changed_cb", ret, BT_ERROR_NONE,
			"bt_avrcp_set_equalizer_state_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_avrcp_unset_equalizer_state_changed_cb()
 */
static void utc_network_bluetooth_avrcp_unset_equalizer_state_changed_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_unset_equalizer_state_changed_cb();
	dts_check_eq("bt_avrcp_unset_equalizer_state_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_avrcp_unset_equalizer_state_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_avrcp_set_repeat_mode_changed_cb()
 */
static void utc_network_bluetooth_avrcp_set_repeat_mode_changed_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_set_repeat_mode_changed_cb(repeat_mode_changed_cb_for_avrcp_p, NULL);
	dts_check_eq("bt_avrcp_set_repeat_mode_changed_cb", ret, BT_ERROR_NONE,
			"bt_avrcp_set_repeat_mode_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_avrcp_unset_repeat_mode_changed_cb()
 */
static void utc_network_bluetooth_avrcp_unset_repeat_mode_changed_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_unset_repeat_mode_changed_cb();
	dts_check_eq("bt_avrcp_unset_repeat_mode_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_avrcp_unset_repeat_mode_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_avrcp_set_shuffle_mode_changed_cb()
 */
static void utc_network_bluetooth_avrcp_set_shuffle_mode_changed_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_set_shuffle_mode_changed_cb(
										shuffle_mode_changed_cb_for_avrcp_p,
										NULL);
	dts_check_eq("bt_avrcp_set_shuffle_mode_changed_cb", ret, BT_ERROR_NONE,
			"bt_avrcp_set_shuffle_mode_changed_cb() failed.");
}

/**
 * @brief Positive test case of bt_avrcp_unset_shuffle_mode_changed_cb()
 */
static void utc_network_bluetooth_avrcp_unset_shuffle_mode_changed_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_unset_shuffle_mode_changed_cb();
	dts_check_eq("bt_avrcp_unset_shuffle_mode_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_avrcp_unset_shuffle_mode_changed_cb() failed.");
}

/**
 * @brief Positive test case of bt_avrcp_set_scan_mode_changed_cb()
 */
static void utc_network_bluetooth_avrcp_set_scan_mode_changed_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_set_scan_mode_changed_cb(scan_mode_changed_cb_for_avrcp_p,
					NULL);
	dts_check_eq("bt_avrcp_set_scan_mode_changed_cb", ret, BT_ERROR_NONE,
					"bt_avrcp_set_scan_mode_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_avrcp_unset_scan_mode_changed_cb()
 */
static void utc_network_bluetooth_avrcp_unset_scan_mode_changed_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_avrcp_unset_scan_mode_changed_cb();
	dts_check_eq("bt_avrcp_unset_scan_mode_changed_cb", ret, BT_ERROR_NONE,
			"bt_avrcp_unset_scan_mode_changed_cb() failed.");

}

