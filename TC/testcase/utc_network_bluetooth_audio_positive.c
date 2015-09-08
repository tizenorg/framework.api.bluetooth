/*
 * utc_network_bluetooth_audio_positive.c
 *
 *  Created on: 19-Sep-2013
 *      Author: mrinal.m
 */

#include <tet_api.h>
#include <bluetooth.h>
#include <glib.h>

#define CONFIG_FILE_PATH "/opt/home/capi-network-bluetooth/tetware.conf"

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

char *remote_address;
static GMainLoop *mainloop;

static void utc_network_bluetooth_audio_initialize_p(void);
static void utc_network_bluetooth_audio_deinitialize_p(void);
static void utc_network_bluetooth_audio_connect_p(void);
static void utc_network_bluetooth_audio_disconnect_p(void);
static void utc_network_bluetooth_audio_set_connection_p(void);
static void utc_network_bluetooth_audio_unset_connection_p(void);
static void utc_network_bluetooth_ag_notify_speaker_gain_p(void);
static void utc_network_bluetooth_ag_get_speaker_gain_p(void);
static void utc_network_bluetooth_ag_is_nrec_enabled_p(void);
static void utc_network_bluetooth_ag_set_microphone_gain_changed_p(void);
static void utc_network_bluetooth_ag_unset_microphone_gain_changed_p(void);
static void utc_network_bluetooth_ag_set_speaker_gain_changed_p(void);
static void utc_network_bluetooth_ag_unset_speaker_gain_changed_p(void);
static void utc_network_bluetooth_ag_open_sco_p(void);
static void utc_network_bluetooth_ag_close_sco_p(void);
static void utc_network_bluetooth_ag_is_sco_opened_p(void);
static void utc_network_bluetooth_ag_set_sco_state_changed_p(void);
static void utc_network_bluetooth_ag_unset_sco_state_changed_p(void);
static void utc_network_bluetooth_ag_notify_call_event_p(void);
static void utc_network_bluetooth_ag_notify_call_list_p(void);
static void utc_network_bluetooth_ag_notify_voice_recognition_state_p(void);
static void utc_network_bluetooth_ag_set_call_handling_event_cb_p(void);
static void utc_network_bluetooth_ag_unset_call_handling_event_cb_p(void);
static void utc_network_bluetooth_ag_set_multi_call_handling_event_cb_p(void);
static void utc_network_bluetooth_ag_unset_multi_call_handling_event_cb_p(void);
static void utc_network_bluetooth_ag_set_dtmf_transmitted_cb_p(void);
static void utc_network_bluetooth_ag_unset_dtmf_transmitted_cb_p(void);
static void utc_network_bluetooth_ag_set_vendor_cmd_cb_p(void);
static void utc_network_bluetooth_ag_unset_vendor_cmd_cb_p(void);
static void utc_network_bluetooth_ag_call_list_create_p(void);
static void utc_network_bluetooth_ag_call_list_destroy_p(void);
static void utc_network_bluetooth_ag_call_list_reset_p(void);
static void utc_network_bluetooth_ag_call_list_add_p(void);
static void utc_network_bluetooth_ag_ca2dp_set_content_protection_p(void);

void connection_state_changed_cb_for_audio_p(int result, bool connected,
					     const char *remote_address,
					     bt_audio_profile_type_e type,
					     void *user_data);
void dtmf_transmitted_cb_for_ag_p(const char *dtmf, void *user_data);
void multi_call_handling_event_cb_for_ag_p(bt_ag_multi_call_handling_event_e event, void *user_data);
void call_handling_event_cb_for_ag_p(bt_ag_call_handling_event_e event,
				     unsigned int call_id, void *user_data);
void sco_state_changed_cb_for_ag_p(int result, bool opened, void *user_data);
void speaker_gain_changed_cb_for_ag_p(int gain, void *user_data);
void microphone_gain_changed_cb_for_ag_p(int gain, void *user_data);
void adapter_state_changed_cb_for_audio_p(int result,
					  bt_adapter_state_e adapter_state,
					  void *user_data);

gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_audio_initialize_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_audio_set_connection_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_audio_connect_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_open_sco_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_microphone_gain_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_speaker_gain_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_notify_speaker_gain_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_get_speaker_gain_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_is_nrec_enabled_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_unset_microphone_gain_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_unset_speaker_gain_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_sco_state_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_vendor_cmd_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_unset_vendor_cmd_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_is_sco_opened_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_close_sco_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_notify_call_event_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_notify_call_list_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_notify_voice_recognition_state_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_call_handling_event_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_unset_call_handling_event_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_multi_call_handling_event_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_unset_multi_call_handling_event_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_set_dtmf_transmitted_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_unset_dtmf_transmitted_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_unset_sco_state_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_call_list_create_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_call_list_destroy_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_call_list_reset_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_call_list_add_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_ag_ca2dp_set_content_protection_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_audio_disconnect_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_audio_unset_connection_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_audio_deinitialize_p, POSITIVE_TC_IDX},
	{NULL, 0},
};

int get_value_from_file(void)
{
	FILE *fp;
	char *token;
	char buf[100];

	fp = fopen(CONFIG_FILE_PATH, "r");
	if (fp == NULL) {
		tet_printf("Default configuration is used\n");
		return -1;
	}
	while (fgets(buf, sizeof(buf), fp)) {
		if (buf[0] == '#' || buf[0] == '\n')
			continue;

		token = strrchr(buf, '\n');
		if (token == NULL) {
			tet_printf("g_conf is too long\n");
			break;
		}
		*token = '\0';

		token = strtok(buf, "=");
		if (token == NULL) {
			continue;
		}
		if (strcasecmp(token, "BT_ADDR_HEADSET") == 0) {
			token = strtok(NULL, "=");
			remote_address = strdup(token);
			fclose(fp);
			return 0;
		}
	}
	fclose(fp);
	return -1;
}

static void startup(void)
{
	bt_error_e ret = BT_ERROR_NONE;
	int timeout_id = 0;

	if(get_value_from_file() == -1) {
		tet_printf("Failed to read.");
	}

	/* start of TC */
	tet_printf("TC start.");
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();

	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_audio_p, "startup") != BT_ERROR_NONE) {
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
	tet_printf("[%s] Callback: Timeout.", __FUNCTION__);
	g_main_loop_quit((GMainLoop *) data);
	return FALSE;
}

/**
 * @brief Callback funtions
 */

void adapter_state_changed_cb_for_audio_p(int result,
					bt_adapter_state_e adapter_state,
					void *user_data)
{

}

void connection_state_changed_cb_for_audio_p(int result, bool connected,
						const char *remote_address,
						bt_audio_profile_type_e type,
						void *user_data)
{

}

void microphone_gain_changed_cb_for_ag_p(int gain, void *user_data)
{

}

void speaker_gain_changed_cb_for_ag_p(int gain, void *user_data)
{

}

void sco_state_changed_cb_for_ag_p(int result, bool opened, void *user_data)
{

}

void call_handling_event_cb_for_ag_p(bt_ag_call_handling_event_e event,
				unsigned int call_id, void *user_data)
{

}

void multi_call_handling_event_cb_for_ag_p(bt_ag_multi_call_handling_event_e event, void *user_data)
{

}

void dtmf_transmitted_cb_for_ag_p(const char *dtmf, void *user_data)
{

}

void vendor_cmd_event_cb_for_ag_p(char *cmd, void *user_data)
{
	tet_printf("Callback : vendor_cmd_cb was called.");
	tet_printf("Vendor Command[%s]", cmd);
}

/**
 * @brief Positive test case of bt_audio_initialize()
 */
static void utc_network_bluetooth_audio_initialize_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_audio_initialize();
	dts_check_eq("bt_audio_initialize", ret, BT_ERROR_NONE,
			"bt_audio_initialize() failed. %x", ret);

}

/**
 * @brief Positive test case of bt_audio_connect()
 */
static void utc_network_bluetooth_audio_connect_p(void)
{
	int ret = BT_ERROR_NONE;
	const char *remote_address = "00:23:78:AB:C4:20";
	ret = bt_audio_connect(remote_address, BT_AUDIO_PROFILE_TYPE_HSP_HFP);
	dts_check_eq("bt_audio_connect", ret, BT_ERROR_NONE,
			"bt_audio_connect() failed. %x", ret);
}

/**
 * @brief Positive test case of bt_audio_disconnect()
 */
static void utc_network_bluetooth_audio_disconnect_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_audio_disconnect(remote_address, BT_AUDIO_PROFILE_TYPE_HSP_HFP);
	dts_check_eq("bt_audio_disconnect", ret, BT_ERROR_NONE,
			"bt_audio_disconnect() failed. %x", ret);
}

/**
 * @brief Positive test case of bt_audio_set_connection_state_changed_cb()
 */
static void utc_network_bluetooth_audio_set_connection_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_audio_set_connection_state_changed_cb
		(connection_state_changed_cb_for_audio_p, NULL);
	dts_check_eq("bt_audio_set_connection_state_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_audio_set_connection_state_changed_cb() failed.");
}

/**
 * @brief Positive test case of bt_audio_unset_connection_state_changed_cb()
 */
static void utc_network_bluetooth_audio_unset_connection_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_audio_unset_connection_state_changed_cb();
	dts_check_eq("bt_audio_set_connection_state_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_audio_set_connection_state_changed_cb() failed.");
}

/**
 * @brief Positive test case of bt_ag_notify_speaker_gain()
 */
static void utc_network_bluetooth_ag_notify_speaker_gain_p(void)
{
	int ret = BT_ERROR_NONE;
	int gain = 10;
	ret = bt_ag_notify_speaker_gain(gain);
	dts_check_eq("bt_ag_notify_speaker_gain", ret, BT_ERROR_NONE,
			"bt_ag_notify_speaker_gain() failed.%x", ret);
}

/**
 * @brief Positive test case of bt_ag_get_speaker_gain()
 */
static void utc_network_bluetooth_ag_get_speaker_gain_p(void)
{
	int ret = BT_ERROR_NONE;
	int gain = 0;
	ret = bt_ag_get_speaker_gain(&gain);
	dts_check_eq("bt_ag_get_speaker_gain", ret, BT_ERROR_NONE,
			"bt_ag_get_speaker_gain() failed.");

}

/**
 * @brief Positive test case of bt_ag_is_nrec_enabled()
 */
static void utc_network_bluetooth_ag_is_nrec_enabled_p(void)
{
	int ret = BT_ERROR_NONE;
	bool enabled = 1;
	ret = bt_ag_is_nrec_enabled(&enabled);
	dts_check_eq("bt_ag_is_nrec_enabled", ret, BT_ERROR_NONE,
			"bt_ag_is_nrec_enabled() failed.");

}

/**
 * @brief Positive test case of bt_ag_set_microphone_gain_changed_cb()
 */
static void utc_network_bluetooth_ag_set_microphone_gain_changed_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_microphone_gain_changed_cb(microphone_gain_changed_cb_for_ag_p, NULL);
	dts_check_eq("bt_ag_set_microphone_gain_changed_cb", ret, BT_ERROR_NONE,
			"bt_ag_set_microphone_gain_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_ag_unset_microphone_gain_changed_cb()
 */
static void utc_network_bluetooth_ag_unset_microphone_gain_changed_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_unset_microphone_gain_changed_cb();
	dts_check_eq("bt_ag_unset_microphone_gain_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_ag_unset_microphone_gain_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_ag_set_speaker_gain_changed_cb()
 */
static void utc_network_bluetooth_ag_set_speaker_gain_changed_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_speaker_gain_changed_cb(speaker_gain_changed_cb_for_ag_p, NULL);
	dts_check_eq("bt_ag_set_speaker_gain_changed_cb", ret, BT_ERROR_NONE,
			"bt_ag_set_speaker_gain_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_ag_unset_speaker_gain_changed_cb()
 */
static void utc_network_bluetooth_ag_unset_speaker_gain_changed_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_unset_speaker_gain_changed_cb();
	dts_check_eq("bt_ag_unset_speaker_gain_changed_cb", ret, BT_ERROR_NONE,
			"bt_ag_unset_speaker_gain_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_ag_open_sco()
 */
static void utc_network_bluetooth_ag_open_sco_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_open_sco();
	dts_check_eq("bt_ag_open_sco", ret, BT_ERROR_NONE,
			"bt_ag_open_sco() failed.");

}

/**
 * @brief Positive test case of bt_ag_close_sco()
 */
static void utc_network_bluetooth_ag_close_sco_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_close_sco();
	dts_check_eq("bt_ag_close_sco", ret, BT_ERROR_NONE,
			"bt_ag_close_sco() failed.");

}

/**
 * @brief Positive test case of bt_ag_is_sco_opened()
 */
static void utc_network_bluetooth_ag_is_sco_opened_p(void)
{
	int ret = BT_ERROR_NONE;
	bool opened = 1;
	ret = bt_ag_is_sco_opened(&opened);
	dts_check_eq("bt_ag_is_sco_opened", ret, BT_ERROR_NONE,
			"bt_ag_is_sco_opened() failed.");

}

/**
 * @brief Positive test case of bt_ag_set_sco_state_changed_cb()
 */
static void utc_network_bluetooth_ag_set_sco_state_changed_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_sco_state_changed_cb(sco_state_changed_cb_for_ag_p, NULL);
	dts_check_eq("bt_ag_set_sco_state_changed_cb", ret, BT_ERROR_NONE,
			"bt_ag_set_sco_state_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_ag_unset_sco_state_changed_cb()
 */
static void utc_network_bluetooth_ag_unset_sco_state_changed_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_unset_sco_state_changed_cb();
	dts_check_eq("bt_ag_unset_sco_state_changed_cb", ret, BT_ERROR_NONE,
			"bt_ag_unset_sco_state_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_ag_notify_call_event()
 */
static void utc_network_bluetooth_ag_notify_call_event_p(void)
{
	int ret = BT_ERROR_NONE;
	const char *phone_number = "9663868998";
	unsigned int call_id = 0;
	ret = bt_ag_notify_call_event(BT_AG_CALL_EVENT_INCOMING,
				call_id, phone_number);
	dts_check_eq("bt_ag_notify_call_event", ret, BT_ERROR_NONE,
				"bt_ag_notify_call_event() failed.");

}

/**
 * @brief Positive test case of bt_ag_notify_call_list()
 */
static void utc_network_bluetooth_ag_notify_call_list_p(void)
{
	int ret = BT_ERROR_NONE;
	bt_call_list_h handle;
	const char *phone_number = "9663868998";

	handle = g_malloc0(sizeof(bt_call_list_h));
	bt_call_list_add(handle, 1, BT_AG_CALL_STATE_ACTIVE, phone_number);
	ret = bt_ag_notify_call_list(handle);
	dts_check_eq("bt_ag_notify_call_list", ret, BT_ERROR_NONE,
				"bt_ag_notify_call_list() failed.");

}

/**
 * @brief Positive test case of bt_ag_notify_voice_recognition_state()
 */
static void utc_network_bluetooth_ag_notify_voice_recognition_state_p(void)
{
	int ret = BT_ERROR_NONE;
	bool state = 1;

	ret = bt_ag_notify_voice_recognition_state(state);
	dts_check_eq("bt_ag_notify_voice_recognition_state", ret, BT_ERROR_NONE,
				"bt_ag_notify_voice_recognition_state() failed.");

}

/**
 * @brief Positive test case of bt_ag_set_call_handling_event_cb()
 */
static void utc_network_bluetooth_ag_set_call_handling_event_cb_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_call_handling_event_cb(call_handling_event_cb_for_ag_p,
				NULL);
	dts_check_eq("bt_ag_set_call_handling_event_cb", ret, BT_ERROR_NONE,
				"bt_ag_set_call_handling_event_cb() failed.");

}

/**
 * @brief Positive test case of bt_ag_unset_call_handling_event_cb()
 */
static void utc_network_bluetooth_ag_unset_call_handling_event_cb_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_unset_call_handling_event_cb();
	dts_check_eq("bt_ag_unset_call_handling_event_cb", ret, BT_ERROR_NONE,
				"bt_ag_unset_call_handling_event_cb() failed.");

}

/**
 * @brief Positive test case of bt_ag_set_multi_call_handling_event_cb()
 */
static void utc_network_bluetooth_ag_set_multi_call_handling_event_cb_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_multi_call_handling_event_cb(multi_call_handling_event_cb_for_ag_p, NULL);
	dts_check_eq("bt_ag_set_multi_call_handling_event_cb", ret,
			BT_ERROR_NONE,
			"bt_ag_set_multi_call_handling_event_cb() failed.");

}

/**
 * @brief Positive test case of bt_ag_unset_multi_call_handling_event_cb()
 */
static void utc_network_bluetooth_ag_unset_multi_call_handling_event_cb_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_unset_multi_call_handling_event_cb();
	dts_check_eq("bt_ag_unset_multi_call_handling_event_cb", ret,
			BT_ERROR_NONE,
			"bt_ag_unset_multi_call_handling_event_cb() failed.");

}

/**
 * @brief Positive test case of bt_ag_set_dtmf_transmitted_cb()
 */
static void utc_network_bluetooth_ag_set_dtmf_transmitted_cb_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_set_dtmf_transmitted_cb(dtmf_transmitted_cb_for_ag_p, NULL);
	dts_check_eq("bt_ag_set_dtmf_transmitted_cb", ret, BT_ERROR_NONE,
			"bt_ag_set_dtmf_transmitted_cb() failed.");

}

/**
 * @brief Positive test case of bt_ag_unset_dtmf_transmitted_cb()
 */
static void utc_network_bluetooth_ag_unset_dtmf_transmitted_cb_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_ag_unset_dtmf_transmitted_cb();
	dts_check_eq("bt_ag_unset_dtmf_transmitted_cb", ret, BT_ERROR_NONE,
			"bt_ag_unset_dtmf_transmitted_cb() failed.");

}

/**
 * @brief Positive test case of bt_call_list_create()
 */
static void utc_network_bluetooth_ag_call_list_create_p(void)
{
	int ret = BT_ERROR_NONE;
	bt_call_list_h handle;
	const char *phone_number = "9663868998";

	ret = bt_call_list_create(&handle);
	ret = bt_call_list_add(handle, 1, BT_AG_CALL_STATE_ACTIVE, phone_number);

	dts_check_eq("bt_call_list_create", ret, BT_ERROR_NONE,
			"bt_call_list_create() failed.");
}

/**
 * @brief Positive test case of bt_call_list_destroy()
 */
static void utc_network_bluetooth_ag_call_list_destroy_p(void)
{
	int ret = BT_ERROR_NONE;
	bt_call_list_h handle;
	const char *phone_number = "9663868998";

	handle = g_malloc0(sizeof(bt_call_list_h));
	bt_call_list_add(handle, 1, BT_AG_CALL_STATE_ACTIVE, phone_number);
	ret = bt_call_list_destroy(handle);
	dts_check_eq("bt_call_list_destroy", ret, BT_ERROR_NONE,
			"bt_call_list_destroy() failed.");
}

/**
 * @brief Positive test case of bt_call_list_reset()
 */
static void utc_network_bluetooth_ag_call_list_reset_p(void)
{
	int ret = BT_ERROR_NONE;
	bt_call_list_h handle;
	const char *phone_number = "9663868998";

	handle = g_malloc0(sizeof(bt_call_list_h));
	bt_call_list_add(handle, 1, BT_AG_CALL_STATE_ACTIVE, phone_number);
	ret = bt_call_list_reset(handle);
	dts_check_eq("bt_call_list_reset", ret, BT_ERROR_NONE,
			"bt_call_list_reset() failed.");
}

/**
 * @brief Positive test case of bt_call_list_add()
 */
static void utc_network_bluetooth_ag_call_list_add_p(void)
{
	int ret = BT_ERROR_NONE;
	unsigned int call_id = 0;
	bt_call_list_h handle;
	const char *phone_number = "9663868998";

	handle = g_malloc0(sizeof(bt_call_list_h));
	bt_call_list_add(handle, 1, BT_AG_CALL_STATE_ACTIVE, phone_number);
	ret = bt_call_list_add(handle, call_id, BT_AG_CALL_STATE_INCOMING, phone_number);
	dts_check_eq("bt_call_list_add", ret, BT_ERROR_NONE,
			"bt_call_list_add() failed.");
}

/**
 * @brief Positive test case of bt_audio_deinitialize()
 */
static void utc_network_bluetooth_audio_deinitialize_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_audio_deinitialize();
	dts_check_eq("bt_audio_deinitialize", ret, BT_ERROR_NONE,
			"bt_audio_deinitialize() failed.");
}
