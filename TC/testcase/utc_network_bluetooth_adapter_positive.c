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
#include <time.h>

#define CONFIG_FILE_PATH "/opt/home/capi-network-bluetooth/tetware.conf"

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);
char *remote_address;
static GMainLoop *mainloop;
static bool callback_result = false;
bt_adapter_visibility_mode_e adapter_visibility_mode =
		BT_ADAPTER_VISIBILITY_MODE_LIMITED_DISCOVERABLE;
char adapter_name[128] = "dts_test";

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;

static void utc_network_bluetooth_initialize_p(void);
static void utc_network_bluetooth_deinitialize_p(void);
static void utc_network_bluetooth_adapter_set_state_changed_cb_p(void);
static void utc_network_bluetooth_adapter_set_name_changed_cb_p(void);
static void utc_network_bluetooth_adapter_set_visibility_mode_changed_cb_p(void);
static void utc_network_bluetooth_adapter_set_connectable_changed_cb_p(void);
static void utc_network_bluetooth_adapter_enable_p(void);
static void utc_network_bluetooth_adapter_get_state_p(void);
static void utc_network_bluetooth_adapter_get_address_p(void);
static void utc_network_bluetooth_adapter_set_name_p(void);
static void utc_network_bluetooth_adapter_get_name_p(void);
static void utc_network_bluetooth_adapter_set_visibility_p(void);
static void utc_network_bluetooth_adapter_get_visibility_p(void);
static void utc_network_bluetooth_adapter_add_white_list_p(void);
static void utc_network_bluetooth_adapter_remove_white_list_p(void);
static void utc_network_bluetooth_adapter_clear_white_list_p(void);
static void utc_network_bluetooth_adapter_set_connectable_p(void);
static void utc_network_bluetooth_adapter_get_connectable_p(void);
static void utc_network_bluetooth_adapter_disable_p(void);
static void utc_network_bluetooth_adapter_unset_state_changed_cb_p(void);
static void utc_network_bluetooth_adapter_unset_name_changed_cb_p(void);
static void utc_network_bluetooth_adapter_unset_visibility_mode_changed_cb_p(void);
static void utc_network_bluetooth_adapter_unset_connectable_changed_cb_p(void);
static void utc_network_bluetooth_adapter_set_le_state_changed_cb_p(void);
static void utc_network_bluetooth_adapter_le_enable_p(void);
static void utc_network_bluetooth_adapter_get_le_state_p(void);
static void utc_network_bluetooth_adapter_le_disable_p(void);
static void utc_network_bluetooth_adapter_unset_le_state_changed_cb_p(void);

void adapter_state_changed_cb_for_adapter_p(int result,
					    bt_adapter_state_e adapter_state,
					    void *user_data);
void adapter_state_changed_cb_for_adapter_p(int result,
					    bt_adapter_state_e adapter_state,
					    void *user_data);
void adapter_name_changed_cb_for_adapter_p(char *device_name, void *user_data);
void adapter_visibility_mode_changed_cb_for_adpater_p(int result,
						      bt_adapter_visibility_mode_e visibility_mode,
						      void *user_data);
void device_discovery_state_changed_cb_for_adapter_p(int result,
						     bt_adapter_device_discovery_state_e discovery_state,
						     bt_device_info_s *device_info,
						     void *user_data);
void adapter_connectable_changed_cb_for_adpater_p(int result,
							bool connectable, void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_initialize_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_enable_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_state_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_unset_state_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_state_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_address_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_name_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_name_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_unset_name_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_connectable_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_name_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_visibility_mode_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_unset_visibility_mode_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_visibility_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_visibility_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_connectable_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_connectable_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_unset_connectable_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_le_enable_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_add_white_list_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_remove_white_list_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_clear_white_list_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_le_state_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_unset_le_state_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_le_state_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_le_disable_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_adapter_disable_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_deinitialize_p, POSITIVE_TC_IDX},
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
		if (strcasecmp(token, "BT_ADDR_MOBILE") == 0) {
			token = strtok(NULL, "=");
			remote_address = strdup(token);
			fclose(fp);
			return 0;
		}
	}
	return -1;
}

static void startup(void)
{
	if(get_value_from_file() == -1) {
		tet_printf("Failed to read.");
	}

	/* start of TC */
	tet_printf("TC start.");
	mainloop = g_main_loop_new(NULL, FALSE);
}

static void cleanup(void)
{
	/* end of TC */
	tet_printf("TC end");
}

gboolean timeout_func(gpointer data)
{
	tet_printf("Callback: Timeout.");
	g_main_loop_quit((GMainLoop *)data);
	return FALSE;
}

/**
 * @brief Callback funtions
 */
void adapter_state_changed_cb_for_adapter_p(int result,
						bt_adapter_state_e adapter_state,
						void *user_data)
{
	if (user_data != NULL && !strcmp((char *)user_data, "enable")) {
		if (adapter_state == BT_ADAPTER_ENABLED && result == BT_ERROR_NONE) {
			tet_printf("Callback: BT was enabled.");
			callback_result = true;
		} else {
			tet_printf("Callback: BT was not enabled.");
		}

		if (mainloop)
			g_main_loop_quit(mainloop);
	} else if (user_data != NULL && !strcmp((char *)user_data, "disable")) {
		if (adapter_state == BT_ADAPTER_DISABLED && result == BT_ERROR_NONE) {
			tet_printf("Callback: BT was disabled.");
			callback_result = true;
		} else {
			tet_printf("Callback: BT was not disabled.");
		}

		if (mainloop) {
			g_main_loop_quit(mainloop);
		}
	}
}

void adapter_le_state_changed_cb_for_adapter_p(int result,
						bt_adapter_le_state_e adapter_le_state,
						void *user_data)
{
	if (user_data != NULL && !strcmp((char *)user_data, "enable")) {
		if (adapter_le_state == BT_ADAPTER_LE_ENABLED && result == BT_ERROR_NONE) {
			tet_printf("Callback: BT LE was enabled.");
			callback_result = true;
		} else {
			tet_printf("Callback: BT LE was not enabled.");
		}

		if (mainloop)
			g_main_loop_quit(mainloop);
	} else if (user_data != NULL && !strcmp((char *)user_data, "disable")) {
		if (adapter_le_state == BT_ADAPTER_LE_DISABLED && result == BT_ERROR_NONE) {
			tet_printf("Callback: BT LEwas disabled.");
			callback_result = true;
		} else {
			tet_printf("Callback: BT LE was not disabled.");
		}

		if (mainloop) {
			g_main_loop_quit(mainloop);
		}
	}
}

void device_discovery_state_changed_cb_for_adapter_p(int result,
				bt_adapter_device_discovery_state_e discovery_state,
				bt_device_info_s *device_info,
				void *user_data)
{
	tet_printf("bt_adapter_device_discovery_state_changed_cb was called");

	if (user_data != NULL && !strcmp((char *)user_data, "enable")) {
		if (discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FINISHED) {
			tet_printf("Callback: bt_adapter_device_discovery_state_changed_cb_for_adapter_p was called \
					and device discovery ends.");
			if (mainloop) {
				g_main_loop_quit(mainloop);
			}
		}
	}
}

void adapter_name_changed_cb_for_adapter_p(char *device_name, void *user_data)
{
	tet_printf("bt_adapter_name_changed_cb_for_adapter_p was called: %s",
			(char *)device_name);

	if (user_data != NULL && !strcmp((char *)user_data, "set_name")) {
		if (!strcmp(device_name, adapter_name))
			callback_result = true;
	}

	if (mainloop)
		g_main_loop_quit(mainloop);

}

void adapter_visibility_mode_changed_cb_for_adpater_p(int result,
					bt_adapter_visibility_mode_e visibility_mode,
					void *user_data)
{
	tet_printf("bt_adapter_visibility_mode_changed_cb was called: %d", visibility_mode);

	if (user_data != NULL && !strcmp((char *)user_data, "set_visibility")) {
		if (result == BT_ERROR_NONE) {
			if (visibility_mode == adapter_visibility_mode) {
				callback_result = true;
			} else {
				tet_printf("Callback: adapter_visibility_mode_changed_cb_for_adpater_p() was called but visibility mode is not same.");
				tet_printf("Callback: actual visibility mode : %d", visibility_mode);
				callback_result = false;
			}
		} else {
			tet_printf("\n Callback: adapter_visibility_mode_changed_cb_for_adpater_p() was called but failed.");
			callback_result = false;
		}
	}

	if (mainloop)
		g_main_loop_quit(mainloop);
}

void adapter_connectable_changed_cb_for_adpater_p(int result, bool connectable,
					void *user_data)
{
	tet_printf("adapter_connectable_changed_cb_for_adapter_p was called.");
	if (result == BT_ERROR_NONE) {
		tet_printf("Callback: Connectable Changed");
		tet_printf("%s", connectable ? "Connectable" : "Non-connectable");
		callback_result = true;
	} else {
		tet_printf("Callback: adapter_connectable_changed_cb_for_adpater_p() was called but failed.");
		callback_result = false;
	}
	if (mainloop)
		g_main_loop_quit(mainloop);
}

/**
 * @brief Positive test case of bt_initialize()
 */
static void utc_network_bluetooth_initialize_p(void)
{
	int ret = bt_initialize();
	dts_check_eq("bt_initialize", ret, BT_ERROR_NONE,
			"bt_initialize() failed.");
}

/**
 * @brief Positive test case of bt_deinitialize()
 */
static void utc_network_bluetooth_deinitialize_p(void)
{
	int ret = bt_deinitialize();
	dts_check_eq("bt_deinitialize", ret,
			BT_ERROR_NONE, "bt_deinitialize() failed.");
}

/**
 * @brief Positive test case of bt_adapter_set_state_changed_cb()
 */
static void utc_network_bluetooth_adapter_set_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_adapter_p, NULL);
	dts_check_eq("bt_adapter_set_state_changed_cb", ret, BT_ERROR_NONE,
			"bt_adapter_set_state_changed_cb failed.");
}

/**
 * @brief Positive test case of bt_adapter_set_name_changed_cb()
 */
static void utc_network_bluetooth_adapter_set_name_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	ret = bt_adapter_set_name_changed_cb(adapter_name_changed_cb_for_adapter_p, NULL);
	if (ret == BT_ERROR_NONE) {
		timeout_id = g_timeout_add(10000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
	}
	dts_check_eq("bt_adapter_set_name_changed_cb", ret, BT_ERROR_NONE,
			"bt_adapter_set_name_changed_cb failed.");
}

/**
 * @brief Positive test case of bt_adapter_set_visibility_mode_changed_cb()
 */
static void utc_network_bluetooth_adapter_set_visibility_mode_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	ret = bt_adapter_set_visibility_mode_changed_cb(
			adapter_visibility_mode_changed_cb_for_adpater_p, NULL);
	if (ret == BT_ERROR_NONE) {
		timeout_id = g_timeout_add(10000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
	}
	dts_check_eq("bt_adapter_set_visibility_mode_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_adapter_set_visibility_mode_changed_cb failed.");
}

static void utc_network_bluetooth_adapter_set_connectable_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_set_connectable_changed_cb(
			adapter_connectable_changed_cb_for_adpater_p, NULL);
	dts_check_eq("bt_adapter_set_connectable_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_adapter_set_connectable_changed_cb failed.");
}

/**
 * @brief Positive test case of bt_adapter_enable()
 */
static void utc_network_bluetooth_adapter_enable_p(void)
{
	int timeout_id = 0;
	callback_result = false;
	bt_error_e ret = BT_ERROR_NONE;

	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_adapter_p, "enable") != BT_ERROR_NONE) {
		dts_fail("bt_adapter_enable",
			"bt_adapter_set_state_changed_cb() failed.");
	}

	ret = bt_adapter_enable();
	if (ret == BT_ERROR_NONE) {
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
	} else if (ret == BT_ERROR_ALREADY_DONE) {
		callback_result = true;
	} else {
		dts_fail("bt_adapter_enable", "bt_adapter_enable() failed.");
		bt_adapter_unset_state_changed_cb();
		return;
	}

	tet_printf("callback_result: %d", callback_result);
	bt_adapter_unset_state_changed_cb();
	dts_check_eq("bt_adapter_enable", callback_result, true,
		"BT was not enabled.");
}

/**
 * @brief Positive test case of bt_adapter_get_state()
 */
static void utc_network_bluetooth_adapter_get_state_p(void)
{
	bt_adapter_state_e adapter_state = BT_ADAPTER_DISABLED;
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_get_state(&adapter_state);
	if (ret == BT_ERROR_NONE && adapter_state == BT_ADAPTER_ENABLED) {
		dts_pass("bt_adapter_get_state",
			"bt_adapter_get_state() succeeded.");
	} else {
		dts_fail("bt_adapter_get_state",
			"bt_adapter_get_state() failed.");
	}
}

/**
 * @brief Positive test case of bt_adapter_get_address().
 */
static void utc_network_bluetooth_adapter_get_address_p(void)
{
	char *address = NULL;
	int ret = bt_adapter_get_address(&address);

	if (address != NULL)
		tet_printf("device address: %s", address);

	dts_check_eq("bt_adapter_get_address", ret, BT_ERROR_NONE,
			"bt_adapter_get_address() failed.");
}

/**
 * @brief Positive test case of bt_adapter_get_name().
 */
static void utc_network_bluetooth_adapter_get_name_p(void)
{
	char *name = NULL;
	int ret = bt_adapter_get_name(&name);

	if (name != NULL)
		tet_printf("device name : %s", name);

	memset(adapter_name, 0x00, sizeof(adapter_name));
	g_strlcpy(adapter_name, name, sizeof(adapter_name));

	dts_check_eq("bt_adapter_get_name", ret, BT_ERROR_NONE, \
			"bt_adapter_get_name() failed.");
}

/**
 * @brief Positive test case of bt_adapter_get_visibility().
 */
static void utc_network_bluetooth_adapter_get_visibility_p(void)
{
	bt_adapter_visibility_mode_e mode = BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE;
	int dur = 1;
	int ret = bt_adapter_get_visibility(&mode, &dur);

	tet_printf("visibility mode : %d", mode);
	if (mode == BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE) {
		adapter_visibility_mode = BT_ADAPTER_VISIBILITY_MODE_LIMITED_DISCOVERABLE;
	} else if (mode == BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE) {
		adapter_visibility_mode = BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE;
	} else
		adapter_visibility_mode = BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE;

	dts_check_eq("bt_adapter_get_visibility", ret, BT_ERROR_NONE,
			"bt_adapter_get_visibility() failed.");
}

/**
 * @brief Positive test case of bt_adapter_set_name().
 */
static void utc_network_bluetooth_adapter_set_name_p(void)
{
	int timeout_id = 0;
	char name[128] = "";

	snprintf(name, 128, "%s_1", adapter_name);
	strncpy(adapter_name, name, 128);

	if (bt_adapter_set_name_changed_cb(adapter_name_changed_cb_for_adapter_p, "set_name") != BT_ERROR_NONE) {
		dts_fail("bt_adapter_set_name",
			"bt_adapter_set_name_changed_cb() failed.");
	}

	if (bt_adapter_set_name(adapter_name) == BT_ERROR_NONE) {
		tet_printf("adapter_name_changed_cb_for_adapter_p() will be called");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		bt_adapter_unset_name_changed_cb();
		dts_check_eq("bt_adapter_set_name", callback_result, true,
			"The local adapter name was not set.");
	} else {
		dts_fail("bt_adapter_set_name", "bt_adapter_set_name() failed");
	}
}

/**
 * @brief Positive test case of bt_adapter_set_visibility().
 */
static void utc_network_bluetooth_adapter_set_visibility_p(void)
{
	callback_result = false;
	int timeout = 0;
	int timeout_id = 0;

	if (bt_adapter_set_visibility_mode_changed_cb(adapter_visibility_mode_changed_cb_for_adpater_p, "set_visibility") != BT_ERROR_NONE) {
		dts_fail("bt_adapter_set_visibility",
			"bt_adapter_set_visibility_mode_changed_cb() failed.");
	}

	tet_printf("set visibility mode: %d", adapter_visibility_mode);
	if (adapter_visibility_mode == BT_ADAPTER_VISIBILITY_MODE_LIMITED_DISCOVERABLE) {
		timeout = 90;
		tet_printf("set timeout: %d", timeout);
	}

	if (bt_adapter_set_visibility(adapter_visibility_mode, timeout) == BT_ERROR_NONE) {
		tet_printf("bt_adapter_visibility_changed_cb() will be called");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		bt_adapter_unset_visibility_mode_changed_cb();
		dts_check_eq("bt_adapter_set_visibility", callback_result, true,
			"The visibility of BT was not set.");
	} else {
		dts_fail("bt_adapter_set_visibility",
			"bt_adapter_set_visibility() failed");
	}
}

/**
 * @brief Positive test case of bt_adapter_le_add_white_list
 */
static void utc_network_bluetooth_adapter_add_white_list_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_adapter_le_add_white_list(remote_address);
	dts_check_eq("bt_adapter_le_add_white_list", ret, BT_ERROR_NONE,
		"bt_adapter_le_add_white_list() failed.");
}

/**
 * @brief Positive test case of bt_adapter_le_clear_white_list
 */
static void utc_network_bluetooth_adapter_clear_white_list_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_adapter_le_clear_white_list();
	dts_check_eq("bt_adapter_le_clear_white_list", ret, BT_ERROR_NONE,
		"bt_adapter_le_clear_white_list() failed.");
}

/**
 * @brief Positive test case of bt_adapter_le_remove_white_list
 */
static void utc_network_bluetooth_adapter_remove_white_list_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_adapter_le_remove_white_list(remote_address);
	dts_check_eq("bt_adapter_le_remove_white_list", ret, BT_ERROR_NONE,
		"bt_adapter_le_remove_white_list() failed.");
}

/**
 * @brief Positive test case of bt_adapter_get_connectable
 */
static void utc_network_bluetooth_adapter_get_connectable_p(void)
{
	int ret = BT_ERROR_NONE;
	bool connectable = false;
	ret = bt_adapter_get_connectable(&connectable);
	dts_check_eq("bt_adapter_get_connectable", ret, BT_ERROR_NONE,
		"bt_adapter_get_connectable() failed.");
}

/**
 * @brief Positive test case of bt_adapter_set_connectable
 */
static void utc_network_bluetooth_adapter_set_connectable_p(void)
{
	int ret = BT_ERROR_NONE;
	bool connectable = false;
	int timeout_id = 0;
	callback_result = false;

	ret = bt_adapter_get_connectable(&connectable);
	ret = bt_adapter_set_connectable(!connectable);
	if (ret == BT_ERROR_NONE) {
		tet_printf("adapter_connectable_changed_cb will be called");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		dts_check_eq("bt_adapter_set_connectable", ret, BT_ERROR_NONE,
				"bt_adapter_set_connectable() failed.");
	}
}

/**
 * @brief Positive test case of bt_adapter_disable()
 */
static void utc_network_bluetooth_adapter_disable_p(void)
{
	callback_result = false;
	int timeout_id = 0;

	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_adapter_p, "disable") != BT_ERROR_NONE) {
		dts_fail("bt_adapter_disable",
			"bt_adapter_set_state_changed_cb() failed.");
	}

	if (bt_adapter_disable() == BT_ERROR_NONE) {
		tet_printf("bt_adapter_state_changed_cb() will be called");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		bt_adapter_unset_state_changed_cb();
		dts_check_eq("bt_adapter_disable", callback_result, true,
			"BT was not disabled");
	} else {
		dts_fail("bt_adapter_disable", "bt_adapter_disable() failed");
	}
}

/**
 * @brief Positive test case of bt_adapter_unset_state_changed_cb()
 */
static void utc_network_bluetooth_adapter_unset_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_unset_state_changed_cb();
	dts_check_eq("bt_adapter_unset_state_changed_cb", ret, BT_ERROR_NONE,
			"bt_adapter_unset_state_changed_cb() failed.");
}

/**
 * @brief Positive test case of bt_adapter_unset_name_changed_cb()
 */
static void utc_network_bluetooth_adapter_unset_name_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_unset_name_changed_cb();
	dts_check_eq("bt_adapter_unset_name_changed_cb", ret, BT_ERROR_NONE,
			"bt_adapter_unset_name_changed_cb() failed.");
}

/**
 * @brief Positive test case of bt_adapter_unset_visibility_mode_changed_cb()
 */
static void utc_network_bluetooth_adapter_unset_visibility_mode_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_unset_visibility_mode_changed_cb();
	dts_check_eq("bt_adapter_unset_visibility_mode_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_adapter_unset_visibility_mode_changed_cb() failed.");
}

/**
 * @brief Positive test case of bt_adapter_unset_connectable_changed_cb()
 */
static void utc_network_bluetooth_adapter_unset_connectable_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_unset_connectable_changed_cb();
	dts_check_eq("bt_adapter_unset_connectable_changed_cb", ret,
			BT_ERROR_NONE,
			"bt_adapter_unset_connectable_changed_cb() failed.");
}

/**
 * @brief Positive test case of bt_adapter_le_set_state_changed_cb()
 */
static void utc_network_bluetooth_adapter_set_le_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_le_set_state_changed_cb(
			adapter_le_state_changed_cb_for_adapter_p, NULL);
	dts_check_eq("bt_adapter_le_set_state_changed_cb", ret, BT_ERROR_NONE,
			"bt_adapter_set_state_le_state_changed_cb failed.");
}

static void utc_network_bluetooth_adapter_le_enable_p(void)
{
	int timeout_id = 0;
	callback_result = false;
	bt_error_e ret = BT_ERROR_NONE;

	if (bt_adapter_le_set_state_changed_cb(adapter_le_state_changed_cb_for_adapter_p, "enable") != BT_ERROR_NONE) {
		dts_fail("bt_adapter_le_enable",
			"bt_adapter_le_set_state_changed_cb() failed.");
	}

	ret = bt_adapter_le_enable();
	if (ret == BT_ERROR_NONE) {
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
	} else if (ret == BT_ERROR_ALREADY_DONE) {
		callback_result = true;
	} else {
		dts_fail("bt_adapter_le_enable", "bt_adapter_le_enable() failed.");
		bt_adapter_le_unset_state_changed_cb();
		return;
	}

	tet_printf("callback_result: %d", callback_result);
	bt_adapter_le_unset_state_changed_cb();
	dts_check_eq("bt_adapter_le_enable", callback_result, true,
		"BT LE was not enabled.");
}

static void utc_network_bluetooth_adapter_le_disable_p(void)
{
	callback_result = false;
	int timeout_id = 0;

	if (bt_adapter_le_set_state_changed_cb(adapter_le_state_changed_cb_for_adapter_p, "disable") != BT_ERROR_NONE) {
		dts_fail("bt_adapter_le_disable",
			"bt_adapter_le_set_state_changed_cb() failed.");
	}

	if (bt_adapter_le_disable() == BT_ERROR_NONE) {
		tet_printf("bt_adapter_le_state_changed_cb() will be called");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		bt_adapter_le_unset_state_changed_cb();
		dts_check_eq("bt_adapter_le_disable", callback_result, true,
			"BT LE was not disabled");
	} else {
		dts_fail("bt_adapter_le_disable", "bt_adapter_le_disable() failed");
	}
}

static void utc_network_bluetooth_adapter_get_le_state_p(void)
{
	bt_adapter_le_state_e adapter_le_state = BT_ADAPTER_LE_DISABLED;
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_le_get_state(&adapter_le_state);
	if (ret == BT_ERROR_NONE && adapter_le_state == BT_ADAPTER_LE_ENABLED) {
		dts_pass("bt_adapter_le_get_state",
			"bt_adapter_le_get_state() succeeded.");
	} else {
		dts_fail("bt_adapter_le_get_state",
			"bt_adapter_le_get_state() failed.");
	}
}

static void utc_network_bluetooth_adapter_unset_le_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_le_unset_state_changed_cb();
	dts_check_eq("bt_adapter_le_unset_state_changed_cb", ret, BT_ERROR_NONE,
			"bt_adapter_unset_state_le_changed_cb() failed.");
}
