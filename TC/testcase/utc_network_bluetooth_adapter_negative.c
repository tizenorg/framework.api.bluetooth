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

enum {
	POSITIVE_TC_IDX = 0x01,
	NEGATIVE_TC_IDX,
};

static void startup(void);
static void cleanup(void);
bt_adapter_visibility_mode_e adapter_visibility_mode =
		BT_ADAPTER_VISIBILITY_MODE_LIMITED_DISCOVERABLE;
char adapter_name[128] = "dts_test";

void (*tet_startup)(void) = startup;
void (*tet_cleanup)(void) = cleanup;


static void utc_network_bluetooth_deinitialize_n(void);
static void utc_network_bluetooth_adapter_set_state_changed_cb_n(void);
static void utc_network_bluetooth_adapter_set_name_changed_cb_n(void);
static void utc_network_bluetooth_adapter_set_visibility_mode_changed_cb_n(void);
static void utc_network_bluetooth_adapter_set_connectable_changed_cb_n(void);
static void utc_network_bluetooth_adapter_enable_n(void);
static void utc_network_bluetooth_adapter_get_state_n(void);
static void utc_network_bluetooth_adapter_get_address_n(void);
static void utc_network_bluetooth_adapter_set_name_n(void);
static void utc_network_bluetooth_adapter_get_name_n(void);
static void utc_network_bluetooth_adapter_set_visibility_n(void);
static void utc_network_bluetooth_adapter_get_visibility_n(void);
static void utc_network_bluetooth_adapter_add_white_list_n(void);
static void utc_network_bluetooth_adapter_clear_white_list_n(void);
static void utc_network_bluetooth_adapter_remove_white_list_n(void);
static void utc_network_bluetooth_adapter_set_connectable_n(void);
static void utc_network_bluetooth_adapter_get_connectable_n(void);
static void utc_network_bluetooth_adapter_disable_n(void);
static void utc_network_bluetooth_adapter_unset_state_changed_cb_n(void);
static void utc_network_bluetooth_adapter_unset_name_changed_cb_n(void);
static void utc_network_bluetooth_adapter_unset_visibility_mode_changed_cb_n(void);
static void utc_network_bluetooth_adapter_unset_connectable_changed_cb_n(void);
static void utc_network_bluetooth_adapter_set_le_state_changed_cb_n(void);
static void utc_network_bluetooth_adapter_unset_le_state_changed_cb_n(void);
static void utc_network_bluetooth_adapter_le_enable_n(void);
static void utc_network_bluetooth_adapter_get_le_state_n(void);
static void utc_network_bluetooth_adapter_le_disable_n(void);

void adapter_state_changed_cb_for_adapter_n(int result,
					    bt_adapter_state_e adapter_state,
					    void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_adapter_disable_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_state_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_name_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_visibility_mode_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_connectable_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_state_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_address_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_name_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_name_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_visibility_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_visibility_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_le_disable_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_add_white_list_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_remove_white_list_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_clear_white_list_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_connectable_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_deinitialize_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_connectable_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_enable_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_unset_connectable_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_unset_state_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_unset_name_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_unset_visibility_mode_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_set_le_state_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_get_le_state_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_le_enable_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_adapter_unset_le_state_changed_cb_n, NEGATIVE_TC_IDX},
	{NULL, 0},
};

static void startup(void)
{
	/* start of TC */
	tet_printf("TC start.");
	bt_initialize();
}

static void cleanup(void)
{
	/* end of TC */
	tet_printf("TC end");
}

/**
 * @brief Negative test case of bt_deinitialize()
 */
static void utc_network_bluetooth_deinitialize_n(void)
{
	int ret = bt_deinitialize();

	if (ret == BT_ERROR_NONE) {
		ret = bt_deinitialize();
		dts_check_eq("bt_deinitialize", ret, BT_ERROR_NOT_INITIALIZED,
				"BT_ERROR_NOT_INITIALIZED must be returned.");
	} else {
		dts_fail("bt_deinitialize", "bt_deinitialize() failed.");
	}
}

/**
 * @brief Negative test case of bt_adapter_disable()
 */
static void utc_network_bluetooth_adapter_disable_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_disable();
	dts_check_eq("bt_adapter_disable", ret, BT_ERROR_NOT_ENABLED,
				"BT_ERROR_NOT_ENABLED must be returned when BT is disabled");
}

/**
 * @brief Negative test case of bt_adapter_set_state_changed_cb()
 */
static void utc_network_bluetooth_adapter_set_state_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_set_state_changed_cb(NULL, NULL);
	dts_check_eq("bt_adapter_set_state_changed_cb", ret,
				BT_ERROR_INVALID_PARAMETER,
				"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

/**
 * @brief Negative test case of bt_adapter_set_name_changed_cb()
 */
static void utc_network_bluetooth_adapter_set_name_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_set_name_changed_cb(NULL, NULL);
	dts_check_eq("bt_adapter_set_name_changed_cb", ret,
				BT_ERROR_INVALID_PARAMETER,
				"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

/**
 * @brief Negative test case of bt_adapter_set_visibility_mode_changed_cb()
 */
static void utc_network_bluetooth_adapter_set_visibility_mode_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_set_visibility_mode_changed_cb(NULL, NULL);
	dts_check_eq("bt_adapter_set_visibility_mode_changed_cb", ret,
				BT_ERROR_INVALID_PARAMETER,
				"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

/**
 * @brief Negative test case of bt_adapter_set_connectable_changed_cb()
 */
static void utc_network_bluetooth_adapter_set_connectable_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_set_connectable_changed_cb(NULL, NULL);
	dts_check_eq("bt_adapter_set_connectable_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

/**
 * @brief Negative test case of bt_adapter_get_state()
 */
static void utc_network_bluetooth_adapter_get_state_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_get_state(NULL);
	dts_check_eq("bt_adapter_get_state", ret, BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

/**
 * @brief Negative test case of bt_adapter_get_address().
 */
static void utc_network_bluetooth_adapter_get_address_n(void)
{
	char *address = NULL;
	int ret = bt_adapter_get_address(&address);

	dts_check_eq("bt_adapter_get_address", ret, BT_ERROR_NOT_ENABLED,
			"BT_ERROR_NOT_ENABLED must be returned when BT is not enabled.");
}

/**
 * @brief Negative test case of bt_adapter_get_name().
 */
static void utc_network_bluetooth_adapter_get_name_n(void)
{
	char *name = NULL;
	int ret = bt_adapter_get_name(&name);

	dts_check_eq("bt_adapter_get_name", ret, BT_ERROR_NOT_ENABLED,
			"BT_ERROR_NOT_ENABLED must be returned when BT is not enabled.");
}

/**
 * @brief Negative test case of bt_adapter_get_visibility().
 */
static void utc_network_bluetooth_adapter_get_visibility_n(void)
{
	int dur = 10;
	int ret = bt_adapter_get_visibility(NULL, &dur);

	dts_check_eq("bt_adapter_get_visibility", ret, BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

/**
 * @brief Negative test case of bt_adapter_set_name().
 */
static void utc_network_bluetooth_adapter_set_name_n(void)
{
	int ret = bt_adapter_set_name(NULL);
	dts_check_eq("bt_adapter_set_name", ret, BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

/**
 * @brief Negative test case of bt_adapter_set_visibility().
 */
static void utc_network_bluetooth_adapter_set_visibility_n(void)
{
	int ret = bt_adapter_set_visibility(adapter_visibility_mode, 180);

	dts_check_eq("bt_adapter_set_visibility", ret, BT_ERROR_NOT_ENABLED,
			"BT_ERROR_NOT_ENABLED must be returned when BT is not enabled.");
}

/**
 * @brief Negative test case of bt_adapter_le_add_white_list
 */
static void utc_network_bluetooth_adapter_add_white_list_n(void)
{
	int ret = bt_adapter_le_add_white_list(NULL);
	dts_check_eq("bt_adapter_le_add_white_list", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

/**
 * @brief Negative test case of bt_adapter_clear_white_list
 */
static void utc_network_bluetooth_adapter_clear_white_list_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_adapter_clear_white_list();
	dts_check_eq("bt_adapter_clear_white_list", ret, BT_ERROR_NOT_ENABLED,
		"BT_ERROR_NOT_ENABLED must be returned when BT is not enabled.");
}

/**
 * @brief Negative test case of bt_adapter_le_add_white_list
 */
static void utc_network_bluetooth_adapter_remove_white_list_n(void)
{
	int ret = bt_adapter_le_remove_white_list(NULL);
	dts_check_eq("bt_adapter_le_remove_white_list", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

/**
 * @brief Negative test case of bt_adapter_get_connectable
 */
static void utc_network_bluetooth_adapter_get_connectable_n(void)
{
	int ret = bt_adapter_get_connectable(NULL);
	dts_check_eq("bt_adapter_get_connectable", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

/**
 * @brief Negative test case of bt_adapter_set_connectable
 */
static void utc_network_bluetooth_adapter_set_connectable_n(void)
{
	int ret = bt_adapter_set_connectable(NULL);
	dts_check_eq("bt_adapter_set_connectable", ret, BT_ERROR_NOT_INITIALIZED,
		"BT_ERROR_NOT_INITIALIZED must be returned when BT is not initialized.");
}

/**
 * @brief Negative test case of bt_adapter_unset_state_changed_cb()
 */
static void utc_network_bluetooth_adapter_unset_state_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_unset_state_changed_cb();
	dts_check_eq("bt_adapter_unset_state_changed_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned when BT service is not initialized.");
}

/**
 * @brief Negative test case of bt_adapter_unset_name_changed_cb()
 */
static void utc_network_bluetooth_adapter_unset_name_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_unset_name_changed_cb();
	dts_check_eq("bt_adapter_unset_name_changed_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned when BT service is not initialized.");
}

/**
 * @brief Negative test case of bt_adapter_unset_visibility_mode_changed_cb()
 */
static void
utc_network_bluetooth_adapter_unset_visibility_mode_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_unset_visibility_mode_changed_cb();
	dts_check_eq("bt_adapter_unset_visibility_mode_changed_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned when BT service is not initialized.");
}

/**
 * @brief Negative test case of bt_adapter_enable()
 */
static void utc_network_bluetooth_adapter_enable_n(void)
{
	bt_error_e ret = bt_adapter_enable();
	dts_check_eq("bt_adapter_enable", ret, BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned.");
}

/**
 * @brief Negative test case of bt_adapter_unset_connectable_changed_cb()
 */
static void utc_network_bluetooth_adapter_unset_connectable_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_unset_connectable_changed_cb();
	dts_check_eq("bt_adapter_unset_connectable_changed_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"bt_adapter_unset_connectable_changed_cb() failed.");
}

/**
 * @brief Negative test case of bt_adapter_le_set_state_changed_cb()
 */
static void utc_network_bluetooth_adapter_set_le_state_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_le_set_state_changed_cb(NULL, NULL);
	dts_check_eq("bt_adapter_le_set_state_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

static void utc_network_bluetooth_adapter_le_enable_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_le_enable();
	dts_check_eq("bt_adapter_le_enable", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned.");
}

static void utc_network_bluetooth_adapter_le_disable_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_le_disable();
	dts_check_eq("bt_adapter_disable", ret, BT_ERROR_NOT_ENABLED,
			"BT_ERROR_NOT_ENABLED must be returned when BT is disabled");
}

static void utc_network_bluetooth_adapter_get_le_state_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_le_get_state(NULL);
	dts_check_eq("bt_adapter_get_state", ret, BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when parameter is NULL.");
}

static void utc_network_bluetooth_adapter_unset_le_state_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_adapter_le_unset_state_changed_cb();
	dts_check_eq("bt_adapter_unset_state_changed_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned");
}
