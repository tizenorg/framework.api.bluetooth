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

/**
 * @file       bt_onoff.c
 * @brief      This is the source file for bt activation/deactivation.
 */

#include <stdio.h>
#include <glib.h>

#include "bluetooth.h"

#define PRT(format, args...) printf("%s:%d() "format, __FUNCTION__, __LINE__, ##args)
#define TC_PRT(format, args...) PRT(format"\n", ##args)

GMainLoop *main_loop = NULL;
static guint onoff_timer = 0;

static void __bt_adapter_state_changed_cb(int result, bt_adapter_state_e adapter_state, void *user_data)
{
	TC_PRT("state(%d), error(%d)", adapter_state, result);

	if (onoff_timer)
		g_source_remove(onoff_timer);
	onoff_timer = 0;

	if (result != BT_ERROR_NONE) {
		char *argv[] = {NULL};
		execv("all_log_dump.sh", argv);
	}

	if (main_loop)
		g_main_loop_quit(main_loop);
	else
		exit(0);
}

static gboolean __bt_onoff_timeout_cb(gpointer data)
{
	TC_PRT("Timed out");

	char *argv[] = {NULL};
	execv("all_log_dump.sh", argv);

	if (main_loop)
		g_main_loop_quit(main_loop);
	else
		exit(0);

	return FALSE;
}

int main(int argc, char* argv[])
{
	int ret;
	bt_adapter_state_e state;

	if(bt_initialize() != BT_ERROR_NONE)
	{
		TC_PRT("bt_initialize() failed.");
		return -1;
	}
	if (bt_adapter_set_state_changed_cb(__bt_adapter_state_changed_cb, NULL) != BT_ERROR_NONE)
	{
		TC_PRT("bt_adapter_set_state_changed_cb() failed.");
		goto DONE;
	}

	main_loop = g_main_loop_new(NULL, FALSE);

	bt_adapter_get_state(&state);
	ret = bt_adapter_get_state(&state);
	TC_PRT("state(%d), error(%d)", state, ret);

	onoff_timer = g_timeout_add(20000, (GSourceFunc)__bt_onoff_timeout_cb, NULL);

	if (argc <= 1) {
		if (state == BT_ADAPTER_DISABLED)
			ret = bt_adapter_enable();
		else
			ret = bt_adapter_disable();
		TC_PRT("bt_adapter_%s() error(%d)", state==BT_ADAPTER_DISABLED?"enable":"disable", ret);
	} else {
		if (argv[1][0] == '0') {
			if (state == BT_ADAPTER_DISABLED) {
				TC_PRT("Already disabled");
				goto DONE;
			} else {
				ret = bt_adapter_disable();
				TC_PRT("bt_adapter_disable() error(%d)", ret);
				if (ret != BT_ERROR_NONE)
					goto DONE;
			}
		} else {
			if (state == BT_ADAPTER_ENABLED) {
				TC_PRT("Already eabled");
				goto DONE;
			} else {
				ret = bt_adapter_enable();
				TC_PRT("bt_adapter_enable() error(%d)", ret);
				if (ret != BT_ERROR_NONE)
					goto DONE;
			}
		}
	}

	g_main_loop_run(main_loop);

DONE:
	if (onoff_timer)
		g_source_remove(onoff_timer);
	onoff_timer = 0;
	bt_adapter_unset_state_changed_cb();
	bt_deinitialize();

	return 0;
}
