/*
 * utc_network_bluetooth_gatt_negative.c
 *
 *  Created on: 23-Sep-2013
 *      Author: shagun.garg
 */

#include <tet_api.h>
#include <bluetooth.h>
#include <stdlib.h>
#include <stdbool.h>
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

static void utc_network_bluetooth_gatt_foreach_primary_services_n(void);
static void utc_network_bluetooth_gatt_discover_characteristics_n(void);
static void utc_network_bluetooth_gatt_get_service_uuid_n(void);
static void utc_network_bluetooth_gatt_foreach_included_services_n(void);
static void utc_network_bluetooth_gatt_set_characteristic_changed_cb_n(void);
static void utc_network_bluetooth_gatt_unset_characteristic_changed_cb_n(void);
static void utc_network_bluetooth_gatt_get_characteristic_declaration_n(void);
static void utc_network_bluetooth_gatt_set_characteristic_value_n(void);
static void utc_network_bluetooth_gatt_clone_attribute_handle_n(void);
static void utc_network_bluetooth_gatt_destroy_attribute_handle_n(void);
static void utc_network_bluetooth_gatt_connect_n(void);
static void utc_network_bluetooth_gatt_disconnect_n(void);
static void utc_network_bluetooth_set_gatt_connection_state_changed_cb_n(void);
static void utc_network_bluetooth_unset_gatt_connection_state_changed_cb_n(void);

void adapter_state_changed_cb_for_gatt_n(int result,
				bt_adapter_state_e adapter_state, void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_gatt_foreach_primary_services_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_gatt_discover_characteristics_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_gatt_get_service_uuid_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_gatt_foreach_included_services_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_gatt_set_characteristic_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_set_gatt_connection_state_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_gatt_connect_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_gatt_disconnect_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_gatt_get_characteristic_declaration_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_gatt_set_characteristic_value_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_gatt_clone_attribute_handle_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_gatt_destroy_attribute_handle_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_unset_gatt_connection_state_changed_cb_n, NEGATIVE_TC_IDX},
	{utc_network_bluetooth_gatt_unset_characteristic_changed_cb_n, NEGATIVE_TC_IDX},
	{NULL, 0},
};

static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_gatt_n, "startup") != BT_ERROR_NONE) {
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
void adapter_state_changed_cb_for_gatt_n(int result,
					bt_adapter_state_e adapter_state, void *user_data)
{
	tet_printf("Callback: bt_adapter_state_changed_cb was called.");
	if ((user_data != NULL) && !strcmp((char *)user_data, "startup")) {
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
 * @brief Negative test case of bt_gatt_foreach_primary_services()
 */
static void utc_network_bluetooth_gatt_foreach_primary_services_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_foreach_primary_services(NULL, NULL, NULL);
	dts_check_eq("bt_gatt_foreach_primary_services", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when remote address and callback parameters are NULL.");
}

/**
 * @brief Negative test case of bt_gatt_discover_characteristics()
 */
static void utc_network_bluetooth_gatt_discover_characteristics_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_discover_characteristics(NULL, NULL, NULL);
	dts_check_eq("bt_gatt_discover_characteristics", ret,
		BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when service and callback parameters are NULL.");
}

/**
 * @brief Negative test case of bt_gatt_get_service_uuid()
 */
static void utc_network_bluetooth_gatt_get_service_uuid_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_gatt_get_service_uuid(NULL, NULL);
	dts_check_eq("bt_gatt_get_service_uuid", ret, BT_ERROR_INVALID_PARAMETER,
		"BT_ERROR_INVALID_PARAMETER must be returned when service parameter is NULL.");
}

/**
 * @brief Negative test case of bt_gatt_foreach_included_services()
 */
static void utc_network_bluetooth_gatt_foreach_included_services_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_foreach_included_services(NULL, NULL, NULL);
	dts_check_eq("bt_socket_unset_connection_state_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when service and callback parameters are NULL.");
}

/**
 * @brief Negative test case of bt_gatt_set_characteristic_changed_cb()
 */
static void utc_network_bluetooth_gatt_set_characteristic_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_set_characteristic_changed_cb(NULL, NULL);
	dts_check_eq("bt_gatt_set_characteristic_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when service and callback parameters are NULL.");
}

/**
 * @brief Negative test case of bt_gatt_unset_characteristic_changed_cb()
 */
static void utc_network_bluetooth_gatt_unset_characteristic_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_unset_characteristic_changed_cb();
	dts_check_eq("bt_gatt_unset_characteristic_changed_cb", ret,
			BT_ERROR_NOT_INITIALIZED,
			"BT_ERROR_NOT_INITIALIZED must be returned when BT is not initialized.");
}


/**
 * @brief Negative test case of bt_gatt_get_characteristic_declaration()
 */
static void utc_network_bluetooth_gatt_get_characteristic_declaration_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_get_characteristic_declaration(NULL, NULL, NULL, 0);
	dts_check_eq("bt_gatt_get_characteristic_declaration", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when characteristic parameter is NULL.");
}

/**
 * @brief Negative test case of bt_gatt_set_characteristic_value()
 */
static void utc_network_bluetooth_gatt_set_characteristic_value_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_set_characteristic_value(NULL, NULL, -1);
	dts_check_eq("bt_socket_connect_rfcomm", ret, BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when characteristic and value parameters are NULL.");
}

/**
 * @brief Negative test case of bt_gatt_clone_attribute_handle()
 */
static void utc_network_bluetooth_gatt_clone_attribute_handle_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_clone_attribute_handle(NULL, NULL);
	dts_check_eq("bt_gatt_clone_attribute_handle", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when origin parameter is NULL.");
}

/**
 * @brief Negative test case of bt_gatt_destroy_attribute_handle()
 */
static void utc_network_bluetooth_gatt_destroy_attribute_handle_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_destroy_attribute_handle(NULL);
	dts_check_eq("bt_gatt_destroy_attribute_handle", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when handle parameter is NULL.");
}

static void utc_network_bluetooth_gatt_connect_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_gatt_connect(NULL, false);
	dts_check_eq("bt_gatt_connect", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when address is NULL.");

}

static void utc_network_bluetooth_gatt_disconnect_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_gatt_disconnect(NULL);
	dts_check_eq("bt_gatt_connect", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when address is NULL.");

}

static void utc_network_bluetooth_set_gatt_connection_state_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_set_connection_state_changed_cb(NULL, NULL);
	dts_check_eq("bt_gatt_set_connection_state_changed_cb", ret,
			BT_ERROR_INVALID_PARAMETER,
			"BT_ERROR_INVALID_PARAMETER must be returned when address is NULL.");
}

static void utc_network_bluetooth_unset_gatt_connection_state_changed_cb_n(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_deinitialize();
	ret = bt_gatt_unset_connection_state_changed_cb();
	dts_check_eq("bt_gatt_unset_connection_state_changed_cb", ret,
		BT_ERROR_NOT_INITIALIZED,
		"BT_ERROR_NOT_INITIALIZED must be returned when BT is not initialized.");
}
