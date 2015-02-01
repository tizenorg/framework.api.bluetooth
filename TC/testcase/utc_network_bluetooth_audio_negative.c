/*
 * utc_network_bluetooth_audio_negative.c
 *
 *  Created on: 19-Sep-2013
 *      Author: mrinal.m
 */

#include <tet_api.h>
#include <bluetooth.h>
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

static void utc_network_bluetooth_audio_connect_n(void);
static void utc_network_bluetooth_audio_disconnect_n(void);
static void utc_network_bluetooth_audio_set_connection_n(void);
static void utc_network_bluetooth_ag_get_speaker_gain_n(void);
static void utc_network_bluetooth_ag_is_nrec_enabled_n(void);
static void utc_network_bluetooth_ag_set_microphone_gain_changed_n(void);
static void utc_network_bluetooth_ag_set_speaker_gain_changed_n(void);
static void utc_network_bluetooth_ag_is_sco_opened_n(void);
static void utc_network_bluetooth_ag_set_sco_state_changed_n(void);
static void utc_network_bluetooth_ag_notify_call_event_n(void);
static void utc_network_bluetooth_ag_notify_call_list_n(void);
static void utc_network_bluetooth_ag_set_call_handling_event_cb_n(void);
static void utc_network_bluetooth_ag_set_multi_call_handling_event_cb_n(void);
static void utc_network_bluetooth_ag_set_vendor_cmd_cb_n(void);
static void utc_network_bluetooth_ag_unset_vendor_cmd_cb_n(void);
static void utc_network_bluetooth_ag_unset_multi_call_handling_event_cb_n(void);
static void utc_network_bluetooth_ag_unset_dtmf_transmitted_cb_n(void);
static void utc_network_bluetooth_ag_call_list_destroy_n(void);
static void utc_network_bluetooth_ag_call_list_reset_n(void);
static void utc_network_bluetooth_ag_call_list_add_n(void);
static void utc_network_bluetooth_ag_set_dtmf_transmitted_cb_n(void);

void adapter_state_changed_cb_for_audio_negative(int result,
						bt_adapter_state_e adapter_state,
						void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_audio_connect_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_audio_disconnect_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_audio_set_connection_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_get_speaker_gain_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_is_nrec_enabled_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_microphone_gain_changed_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_speaker_gain_changed_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_is_sco_opened_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_sco_state_changed_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_notify_call_event_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_notify_call_list_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_call_handling_event_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_multi_call_handling_event_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_vendor_cmd_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_dtmf_transmitted_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_call_list_destroy_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_call_list_reset_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_call_list_add_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_unset_vendor_cmd_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_unset_multi_call_handling_event_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_ag_unset_dtmf_transmitted_cb_n, NEGATIVE_TC_IDX},
	{NULL, 0},
};

static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();

	if (bt_audio_initialize() != BT_ERROR_NONE)
		tet_printf("bt_audio_initialize failed");

	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_audio_negative, "startup") != BT_ERROR_NONE) {
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
 * @brief Callback funtions
 */
void adapter_state_changed_cb_for_audio_negative(int result,
						bt_adapter_state_e adapter_state,
						void *user_data)
{
	tet_printf("Callback: bt_adapter_state_changed_cb was called.");
	if (user_data != NULL && !strcmp((char *)user_data, "startup")) {
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
 * @brief Negative test case of bt_audio_connect()
 */
static void utc_network_bluetooth_audio_connect_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_audio_connect(NULL, BT_AUDIO_PROFILE_TYPE_HSP_HFP);
	dts_check_eq("bt_audio_connect", ret,
			BT_ERROR_INVALID_PARAMETER, "bt_audio_connect() failed.");
}

/**
 * @brief Negative test case of bt_audio_disconnect()
 */
static void utc_network_bluetooth_audio_disconnect_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_audio_disconnect(NULL, BT_AUDIO_PROFILE_TYPE_A2DP);
	dts_check_eq("bt_audio_disconnect", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_audio_disconnect() failed.");
}

/**
 * @brief Negative test case of bt_audio_set_connection_state_changed_cb()
 */
static void utc_network_bluetooth_audio_set_connection_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_audio_set_connection_state_changed_cb(NULL, NULL);
	dts_check_eq("bt_audio_set_connection_state_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_audio_set_connection_state_changed_cb() failed.");
}

/**
 * @brief Negative test case of bt_ag_get_speaker_gain()
 */
static void utc_network_bluetooth_ag_get_speaker_gain_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_get_speaker_gain(NULL);
	dts_check_eq("bt_ag_get_speaker_gain", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_ag_get_speaker_gain() failed.");
}

/**
 * @brief Negative test case of bt_ag_is_nrec_enabled()
 */
static void utc_network_bluetooth_ag_is_nrec_enabled_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_is_nrec_enabled(NULL);
	dts_check_eq("bt_ag_is_nrec_enabled", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_ag_is_nrec_enabled() failed.");
}

/**
 * @brief Negative test case of bt_ag_set_microphone_gain_changed_cb()
 */
static void utc_network_bluetooth_ag_set_microphone_gain_changed_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_microphone_gain_changed_cb(NULL, NULL);
	dts_check_eq("bt_ag_set_microphone_gain_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_ag_set_microphone_gain_changed_cb() failed.");
}

/**
 * @brief Negative test case of bt_ag_set_speaker_gain_changed_cb()
 */
static void utc_network_bluetooth_ag_set_speaker_gain_changed_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_speaker_gain_changed_cb(NULL, NULL);
	dts_check_eq("bt_ag_set_speaker_gain_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_ag_set_speaker_gain_changed_cb() failed.");
}

/**
 * @brief Negative test case of bt_ag_is_sco_opened()
 */
static void utc_network_bluetooth_ag_is_sco_opened_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_is_sco_opened(NULL);
	dts_check_eq("bt_ag_is_sco_opened", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_ag_is_sco_opened() failed.");
}

/**
 * @brief Negative test case of bt_ag_set_sco_state_changed_cb()
 */
static void utc_network_bluetooth_ag_set_sco_state_changed_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_sco_state_changed_cb(NULL, NULL);
	dts_check_eq("bt_ag_set_sco_state_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_ag_set_sco_state_changed_cb() failed.");
}

/**
 * @brief Negative test case of bt_ag_notify_call_event()
 */
static void utc_network_bluetooth_ag_notify_call_event_n(void)
{
	int ret = BT_ERROR_NONE;
	unsigned int call_id = 0;
	ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_INCOMING, call_id, NULL);
	dts_check_eq("bt_ag_notify_call_event", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_ag_notify_call_event() failed.");
}

/**
 * @brief Negative test case of bt_ag_notify_call_list()
 */
static void utc_network_bluetooth_ag_notify_call_list_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_notify_call_list(NULL);
	dts_check_eq("bt_ag_notify_call_list", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_ag_notify_call_list() failed.");
}

/**
 * @brief Negative test case of bt_ag_set_call_handling_event_cb()
 */
static void utc_network_bluetooth_ag_set_call_handling_event_cb_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_call_handling_event_cb(NULL, NULL);
	dts_check_eq("bt_ag_set_call_handling_event_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_ag_set_call_handling_event_cb() failed.");
}

/**
 * @brief Negative test case of bt_ag_set_multi_call_handling_event_cb()
 */
static void utc_network_bluetooth_ag_set_multi_call_handling_event_cb_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_multi_call_handling_event_cb(NULL, NULL);
	dts_check_eq("bt_ag_set_multi_call_handling_event_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_ag_set_multi_call_handling_event_cb() failed.");
}

/**
 * @brief Negative test case of bt_ag_set_dtmf_transmitted_cb()
 */
static void utc_network_bluetooth_ag_set_dtmf_transmitted_cb_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_dtmf_transmitted_cb(NULL, NULL);
	dts_check_eq("bt_ag_set_dtmf_transmitted_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"bt_ag_set_dtmf_transmitted_cb() failed.");

}

/**
 * @brief Negative test case of bt_ag_unset_multi_call_handling_event_cb()
 */
static void utc_network_bluetooth_ag_unset_multi_call_handling_event_cb_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_deinitialize();
	ret = bt_ag_unset_multi_call_handling_event_cb();
	dts_check_eq("bt_ag_unset_multi_call_handling_event_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned when BT is not initialized.");
}

/**
 * @brief Negative test case of bt_ag_unset_dtmf_transmitted_cb()
 */
static void utc_network_bluetooth_ag_unset_dtmf_transmitted_cb_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_deinitialize();
	ret = bt_ag_unset_dtmf_transmitted_cb();
	dts_check_eq("bt_ag_unset_dtmf_transmitted_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned when BT is not initialized.");
}

/**
 * @brief Negative test case of bt_call_list_destroy()
 */
static void utc_network_bluetooth_ag_call_list_destroy_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_call_list_destroy(NULL);
	dts_check_eq("bt_call_list_destroy", ret, BT_ERROR_INVALID_PARAMETER,
			"bt_call_list_destroy() failed.");
}

/**
 * @brief Negative test case of bt_call_list_reset()
 */
static void utc_network_bluetooth_ag_call_list_reset_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_call_list_reset(NULL);
	dts_check_eq("bt_call_list_reset", ret, BT_ERROR_INVALID_PARAMETER,
			"bt_call_list_reset() failed.");
}

/**
 * @brief Negative test case of bt_call_list_add()
 */
static void utc_network_bluetooth_ag_call_list_add_n(void)
{
	int ret = BT_ERROR_NONE;
	unsigned int call_id = 0;
	const char *phone_number = "9663868998";

	ret = bt_call_list_add(NULL, call_id, BT_AG_CALL_STATE_INCOMING, phone_number);
	dts_check_eq("bt_call_list_add", ret, BT_ERROR_INVALID_PARAMETER,
			"bt_call_list_add() failed.");

}
