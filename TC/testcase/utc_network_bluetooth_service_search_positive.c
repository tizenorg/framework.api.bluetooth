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

#define CONFIG_FILE_PATH "/opt/home/capi-network-bluetooth/tetware.conf"
enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);

void (*tet_startup) (void) = startup;
void (*tet_cleanup) (void) = cleanup;

char target_name[128] = "dts_test";
char *target_address;
bool callback_result = false;
bool discovery_result = false;
bool bonding_result = false;
static GMainLoop *mainloop;

static void utc_network_bluetooth_service_search_set_cb_p(void);
static void utc_network_bluetooth_service_search_p(void);
static void utc_network_bluetooth_service_search_cancel_p(void);
static void utc_network_bluetooth_service_search_unset_cb_p(void);

gboolean timeout_func(gpointer data);
void adapter_state_changed_cb_for_service_search_p(int result,
							bt_adapter_state_e adapter_state, void *user_data);
void device_discovery_state_changed_cb_for_service_search_p(int result,
						bt_adapter_device_discovery_state_e discovery_state,
		bt_adapter_device_discovery_info_s *discovery_info, void *user_data);
void service_searched_cb_for_service_search_p(int result,
							bt_device_sdp_info_s *sdp_info, void *user_data);
void device_bonded_cb_for_service_search_p(int result,
							bt_device_info_s *device_info, void *user_data);
void device_unbonded_cb(int result, char *remote_address, void *user_data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_service_search_set_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_service_search_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_service_search_cancel_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_service_search_unset_cb_p, POSITIVE_TC_IDX},
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
			target_address = strdup(token);
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
	callback_result = false;
	discovery_result = false;
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();

	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_service_search_p, "startup") != BT_ERROR_NONE) {
		tet_printf("bt_adapter_set_state_changed_cb() failed.");
	}

	if (bt_adapter_set_device_discovery_state_changed_cb
		(device_discovery_state_changed_cb_for_service_search_p, "startup") != BT_ERROR_NONE) {
		tet_printf("bt_adapter_set_device_discovery_state_changed_cb() failed.");
	}

	ret = bt_adapter_enable();
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_adapter_enable() succeeded.");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);

		tet_printf("BT was enabled. Device discovery will be started");
		if (bt_adapter_start_device_discovery() == BT_ERROR_NONE) {
			tet_printf("bt_adapter_start_device_discovery() succeeded.");
			timeout_id = g_timeout_add(60000, timeout_func, mainloop);
			g_main_loop_run(mainloop);
			g_source_remove(timeout_id);
		} else {
			tet_printf("bt_adapter_start_device_discovery() failed.");
		}
	} else if (ret == BT_ERROR_ALREADY_DONE) {
		tet_printf("BT was already enabled. Device discovery will be started");
		if (bt_adapter_start_device_discovery() == BT_ERROR_NONE) {
			tet_printf("bt_adapter_start_device_discovery() succeeded.");
			timeout_id = g_timeout_add(60000, timeout_func, mainloop);
			g_main_loop_run(mainloop);
			g_source_remove(timeout_id);
		} else {
			tet_printf("bt_adapter_start_device_discovery() failed.");
		}
	} else {
		tet_printf("DTS may fail because bt_adapter_enable() failed.");
	}

	if (bt_adapter_unset_state_changed_cb() != BT_ERROR_NONE) {
		tet_printf("bt_adapter_unset_state_changed_cb() failed.");
	}

	if (bt_adapter_unset_device_discovery_state_changed_cb() != BT_ERROR_NONE) {
		tet_printf("bt_adapter_unset_device_discovery_state_changed_cb() failed.");
	}

	tet_printf("TC start.");
}


static void cleanup(void)
{
	/* end of TC */
	int timeout_id = 0;

	/* Destroy a bond */
	bt_device_set_bond_destroyed_cb(device_unbonded_cb, NULL);
	if (bt_device_destroy_bond(target_address) == BT_ERROR_NONE) {
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
	}

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
void device_unbonded_cb(int result, char *remote_address, void *user_data)
{
	tet_printf("Callback: bt_device_bond_destroyed_cb() was called.");

	if (mainloop) {
		g_main_loop_quit(mainloop);
	}

}

void adapter_state_changed_cb_for_service_search_p(int result,
						bt_adapter_state_e adapter_state, void *user_data)
{
	tet_printf("Callback: bt_adapter_state_changed_cb() was called");
	if (user_data != NULL && !strcmp((char *)user_data, "startup")) {
		if (adapter_state == BT_ADAPTER_ENABLED && result == BT_ERROR_NONE) {
			tet_printf("Callback: BT was enabled.");
		} else {
			tet_printf("Callback: BT was not enabled. DTS will be started but DTS may fail.");
		}

		if (mainloop) {
			g_main_loop_quit(mainloop);
		}
	}
}

void device_discovery_state_changed_cb_for_service_search_p(int result,
						bt_adapter_device_discovery_state_e discovery_state,
		bt_adapter_device_discovery_info_s *discovery_info, void *user_data)
{
	tet_printf("Callback: bt_adapter_device_discovery_state_changed_cb() was called");
	if (user_data != NULL && !strcmp((char *)user_data, "startup")) {
		if (discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FINISHED) {
			tet_printf("Callback: Device discovery finished. DTS will be started.");
			if (mainloop) {
				g_main_loop_quit(mainloop);
			}
		} else if (discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FOUND) {
			tet_printf("Callback: Devices were founded (%s)",
				   discovery_info->remote_name);
			if (discovery_info->remote_name != NULL && !strcmp(discovery_info->remote_name, target_name)) {
				discovery_result = true;
				tet_printf("Callback: dts_test device was found.");
				strncpy(target_address, discovery_info->remote_address, 18);
				tet_printf("Callback: device address: %s", discovery_info->remote_address);
				if (bt_adapter_stop_device_discovery() == BT_ERROR_NONE) {
					tet_printf("Callback: Device discovery will be stopped");
				}
			}
		}
	}
}

void service_searched_cb_for_service_search_p(int result,
							bt_device_sdp_info_s *sdp_info,
							void *user_data)
{
	int i = 0;

	tet_printf("Callback: bt_device_service_searched_cb() was called.");
	if (result == BT_ERROR_NONE) {
		if (sdp_info != NULL && !strcmp(sdp_info->remote_address, target_address)) {
			tet_printf("Callback: Service search succeeded.");
			tet_printf("Callback: Service count : %d", sdp_info->service_count);
			for (i = 0; i < sdp_info->service_count; i++) {
				tet_printf("Callback: uuid[%d] - %s", i + 1, sdp_info->service_uuid[i]);
			}

			callback_result = true;
		}
	} else if (result == BT_ERROR_CANCELLED) {
		if (sdp_info != NULL && !strcmp(sdp_info->remote_address, target_address)) {
			tet_printf("Callback: Calcellation of service search succeeded.");
			callback_result = true;
		}
	} else {
		tet_printf("Callback: Failed.");
	}

	if (mainloop) {
		g_main_loop_quit(mainloop);
	}
}

void device_bonded_cb_for_service_search_p(int result,
					bt_device_info_s *device_info, void *user_data)
{
	if (user_data != NULL && !strcmp((char *)user_data, "startup")) {
		if (device_info->remote_address != NULL && !strcmp(device_info->remote_address, target_address)) {
			if (result == BT_ERROR_NONE) {
				tet_printf("Callback: Bonding succeeded");
				bonding_result = true;
			} else {
				tet_printf("Callback: Bonding failed");
			}
		}
	}

	if (mainloop) {
		g_main_loop_quit(mainloop);
	}
}

/**
 * @brief Positive test case of bt_device_set_service_searched_cb()
 */
static void utc_network_bluetooth_service_search_set_cb_p(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	ret = bt_device_set_service_searched_cb(service_searched_cb_for_service_search_p, NULL);
	if (ret == BT_ERROR_NONE) {
		if (bt_device_set_bond_created_cb(device_bonded_cb_for_service_search_p, "startup") != BT_ERROR_NONE) {
			tet_printf("bt_device_set_bond_created_cb() failed.");
		}

		if (discovery_result == true && bt_device_create_bond(target_address) == BT_ERROR_NONE) {
			tet_printf("bt_device_create_bond() succeeded.");
			timeout_id = g_timeout_add(60000, timeout_func, mainloop);
			g_main_loop_run(mainloop);
			g_source_remove(timeout_id);
		} else {
			tet_printf("bt_device_create_bond() failed.");
		}
	}

	dts_check_eq("bt_device_set_service_searched_cb", ret, BT_ERROR_NONE,
		"bt_device_set_service_searched_cb() failed.");
}

/**
 * @brief Positive test case of bt_device_start_service_search()
 */
static void utc_network_bluetooth_service_search_p(void)
{
	int timeout_id = 0;
	int ret = BT_ERROR_NONE;

	if (discovery_result != true) {
		tet_printf("bt_device_start_service_search failed.");
	}

	callback_result = false;
	ret = bt_device_start_service_search(target_address);
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_device_start_service_search() succeeded.");
		tet_printf("service_searched_cb_for_service_search_p() will be called.");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		dts_check_eq("bt_device_start_service_search", ret, BT_ERROR_NONE,
												"bt_device_start_service_search() failed.");
	} else {
		dts_fail("bt_device_start_service_search",
				"bt_device_start_service_search() failed.");
	}
}

/**
 * @brief Positive test case of bt_device_cancel_service_search()
 */
static void utc_network_bluetooth_service_search_cancel_p(void)
{
	int timeout_id = 0;
	int ret = BT_ERROR_NONE;

	if (discovery_result != true) {
		tet_printf("t_device_cancel_service_search failed.");
	}

	callback_result = false;
	if (bt_device_start_service_search(target_address) != BT_ERROR_NONE) {
		tet_printf("t_device_start_service_search failed.");

	} else {
		tet_printf("bt_device_cancel_service_search",
				"bt_device_start_service_search() succeeded.");
	}

    ret = bt_device_cancel_service_search();
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_device_cancel_service_search() succeeded.");
		tet_printf("service_searched_cb_for_service_search_p() will be called.");
		timeout_id = g_timeout_add(60000, timeout_func, mainloop);
		g_main_loop_run(mainloop);
		g_source_remove(timeout_id);
		dts_check_eq("bt_device_cancel_service_search", ret, BT_ERROR_NONE,
				"bt_device_cancel_service_search() failed.");

	} else {
		dts_fail("bt_device_cancel_service_search",
				"bt_device_cancel_service_search() failed.");
	}
}

/**
 * @brief Positive test case of bt_device_unset_service_searched_cb()
 */
static void utc_network_bluetooth_service_search_unset_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_unset_service_searched_cb();
	dts_check_eq("bt_device_unset_service_searched_cb", ret, BT_ERROR_NONE,
		"bt_device_set_service_searched_cb() failed.");
}
