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

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

static GMainLoop *mainloop;

void(*tet_startup)(void) = startup;
void(*tet_cleanup)(void) = cleanup;

char *remote_address;

static void utc_network_bluetooth_opp_client_initialize_p(void);
static void utc_network_bluetooth_opp_client_deinitialize_p(void);
static void utc_network_bluetooth_opp_client_add_file_p(void);
static void utc_network_bluetooth_opp_client_clear_files_p(void);
static void utc_network_bluetooth_opp_client_push_files_p(void);
static void utc_network_bluetooth_opp_client_cancel_push_p(void);

void push_responded_cb_for_opp_client_p(int result,
						const char *remote_address, void *user_data);
void push_progress_cb_for_opp_client_p(const char *file,
						long long size, int percent, void *user_data);
void push_finished_cb_for_opp_client_p(int result,
						const char *remote_address, void *user_data);
void adapter_state_changed_cb_for_opp_client_p(int result,
						bt_adapter_state_e adapter_state, void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_opp_client_initialize_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_opp_client_add_file_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_opp_client_push_files_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_opp_client_cancel_push_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_opp_client_clear_files_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_opp_client_deinitialize_p, POSITIVE_TC_IDX},
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
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	if(get_value_from_file() == -1) {
		tet_printf("Failed to read.");
	}

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_opp_client_p, "startup") != BT_ERROR_NONE) {
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

void adapter_state_changed_cb_for_opp_client_p(int result,
							bt_adapter_state_e adapter_state, void *user_data)
{
	if ((user_data != NULL) && !strcmp((char *)user_data, "startup")) {
		if (adapter_state == BT_ADAPTER_ENABLED && result == BT_ERROR_NONE) {
			tet_printf("Callback: BT was enabled.");
			bt_adapter_start_device_discovery();
		} else {
			tet_printf("Callback: BT was not enabled. DTS will be started but DTS may fail.");
			if (mainloop) {
				g_main_loop_quit(mainloop);
			}
		}
	}
}

void push_responded_cb_for_opp_client_p(int result,
								const char *remote_address, void *user_data)
{

}


void push_progress_cb_for_opp_client_p(const char *file, long long size,
												int percent, void *user_data)
{

}


void push_finished_cb_for_opp_client_p(int result,
		const char *remote_address, void *user_data)
{

}


static void utc_network_bluetooth_opp_client_initialize_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_client_initialize();
	dts_check_eq("bt_opp_client_initialize", ret,
			BT_ERROR_NONE, "bt_opp_client_initialize() failed.");
}

static void utc_network_bluetooth_opp_client_deinitialize_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_client_deinitialize();
	dts_check_eq("bt_opp_client_deinitialize", ret,
			BT_ERROR_NONE, "bt_opp_client_deinitialize() failed.");

}

static void utc_network_bluetooth_opp_client_add_file_p(void)
{
	int ret = BT_ERROR_NONE;
	int fd = 0;
	const char *file = "/tmp/a.txt";

	if (access("/tmp/a.txt", F_OK) < 0) {
		fd = open("/tmp/a.txt", O_RDWR|O_CREAT,0666);
		write(fd, "hey", 3);
	}

	ret = bt_opp_client_add_file(file);
	dts_check_eq("bt_opp_client_add_file", ret,
			BT_ERROR_NONE, "bt_opp_client_add_file() failed");
}


static void utc_network_bluetooth_opp_client_clear_files_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_client_clear_files();
	dts_check_eq("bt_opp_client_clear_files", ret,
			BT_ERROR_NONE, "bt_opp_client_clear_files() failed.");

}


static void utc_network_bluetooth_opp_client_push_files_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_opp_client_push_files(remote_address, push_responded_cb_for_opp_client_p,
	push_progress_cb_for_opp_client_p, push_finished_cb_for_opp_client_p, NULL);

	dts_check_eq("bt_opp_client_push_files", ret,
				BT_ERROR_NONE, "bt_opp_client_push_files() failed");
}

static void utc_network_bluetooth_opp_client_cancel_push_p(void)
{
	int ret = BT_ERROR_NONE;

		ret = bt_opp_client_cancel_push();
		dts_check_eq("bt_opp_client_cancel_push", ret,
						BT_ERROR_NONE, "bt_opp_client_cancel_push() failed.");
}
