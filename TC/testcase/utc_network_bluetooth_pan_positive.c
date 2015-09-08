/*
 * utc_network_bluetooth_pan_positive.c
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

static void utc_network_bluetooth_nap_activate_p(void);
static void utc_network_bluetooth_nap_deactivate_p(void);
static void utc_network_bluetooth_nap_disconnect_all_p(void);
static void utc_network_bluetooth_nap_set_connection_state_changed_p(void);
static void utc_network_bluetooth_nap_unset_connection_state_changed_p(void);
static void utc_network_bluetooth_panu_set_connection_state_changed_p(void);
static void utc_network_bluetooth_panu_unset_connection_state_changed_p(void);
static void utc_network_bluetooth_panu_connect_p(void);
static void utc_network_bluetooth_nap_disconnect_p(void);
static void utc_network_bluetooth_panu_disconnect_p(void);

void connection_state_changed_cb_for_nap_p(bool connected,
						const char *remote_address,
						const char *interface_name,
						void *user_data);
void connection_state_changed_cb_for_panu_p(int result,
						bool connected,
						const char *remote_address,
						bt_panu_service_type_e type,
						void *user_data);

void adapter_state_changed_cb_for_pan_p(int result,
						bt_adapter_state_e adapter_state,
						void *user_data);
gboolean timeout_func(gpointer data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_nap_activate_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_nap_set_connection_state_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_nap_unset_connection_state_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_nap_deactivate_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_nap_disconnect_all_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_panu_connect_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_panu_set_connection_state_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_panu_unset_connection_state_changed_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_nap_disconnect_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_panu_disconnect_p, POSITIVE_TC_IDX},
	{NULL, 0},
};


static void startup(void)
{
	int ret = BT_ERROR_NONE;
	int timeout_id = 0;

	/* start of TC */
	mainloop = g_main_loop_new(NULL, FALSE);

	bt_initialize();
	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_pan_p, "startup") != BT_ERROR_NONE) {
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
	tet_printf("Callback: Timeout.");
	g_main_loop_quit((GMainLoop *) data);
	return FALSE;
}

/**
 * @brief Callback funtions
*/

void adapter_state_changed_cb_for_pan_p(int result,
					bt_adapter_state_e adapter_state,
					void *user_data)
{

}

void connection_state_changed_cb_for_nap_p(bool connected,
					const char *remote_address,
					const char *interface_name,
					void *user_data)
{

}

void connection_state_changed_cb_for_panu_p(int result,
					bool connected,
					const char *remote_address,
					bt_panu_service_type_e type,
					void *user_data)
{

}

/**
 * @brief Positive test case of bt_nap_activate()
*/
static void utc_network_bluetooth_nap_activate_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_nap_activate();
	if (ret != BT_ERROR_ALREADY_DONE) {
		dts_check_eq("bt_nap_activate", ret, BT_ERROR_NONE, "bt_nap_activate() failed.");
	} else {
		dts_check_eq("bt_nap_activate", ret, BT_ERROR_ALREADY_DONE,
			"bt_nap_activate() failed.");
	}
}

/**
 * @brief Positive test case of bt_nap_deactivate()
*/
static void utc_network_bluetooth_nap_deactivate_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_nap_deactivate();
	dts_check_eq("bt_nap_deactivate", ret, BT_ERROR_NONE,
			"bt_nap_deactivate() failed.");

}

/**
 * @brief Positive test case of bt_nap_disconnect_all()
 */
static void utc_network_bluetooth_nap_disconnect_all_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_nap_disconnect_all();
	dts_check_eq("bt_nap_disconnect_all", ret, BT_ERROR_NONE,
			"bt_nap_disconnect_all() failed.");
}

/**
 * @brief Positive test case of bt_nap_set_connection_state_changed_cb()
 */
static void utc_network_bluetooth_nap_set_connection_state_changed_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_nap_set_connection_state_changed_cb(connection_state_changed_cb_for_nap_p, NULL);
	dts_check_eq("bt_nap_set_connection_state_changed_cb", ret, BT_ERROR_NONE,
			"bt_nap_set_connection_state_changed_cb() failed.");
}


/**
 * @brief Positive test case of bt_nap_unset_connection_state_changed_cb()
*/
static void utc_network_bluetooth_nap_unset_connection_state_changed_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_nap_unset_connection_state_changed_cb();
	dts_check_eq("bt_nap_unset_connection_state_changed_cb", ret, BT_ERROR_NONE,
			"bt_nap_unset_connection_state_changed_cb() failed.");
}


/**
 * @brief Positive test case of bt_panu_set_connection_state_changed_cb()
*/
static void utc_network_bluetooth_panu_set_connection_state_changed_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_panu_set_connection_state_changed_cb(connection_state_changed_cb_for_panu_p, NULL);
	dts_check_eq("bt_panu_set_connection_state_changed_cb", ret, BT_ERROR_NONE,
			"bt_panu_set_connection_state_changed_cb() failed.");

}


/**
 * @brief Positive test case of bt_panu_unset_connection_state_changed_cb()
*/
static void utc_network_bluetooth_panu_unset_connection_state_changed_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_panu_unset_connection_state_changed_cb();
	dts_check_eq("bt_panu_unset_connection_state_changed_cb", ret, BT_ERROR_NONE,
			"bt_panu_unset_connection_state_changed_cb() failed.");

}

/**
 * @brief Positive test case of bt_panu_connect()
*/
static void utc_network_bluetooth_panu_connect_p(void)
{
	int ret = BT_ERROR_NONE;
	const char *remote_address = "00:00:00:00:00:00";
	ret = bt_panu_connect(remote_address, BT_PANU_SERVICE_TYPE_NAP);
	dts_check_eq("bt_panu_connect", ret, BT_ERROR_NONE,
			"bt_panu_connect() failed.");

}

/**
 * @brief Positive test case of bt_nap_disconnect()
*/
static void utc_network_bluetooth_nap_disconnect_p(void)
{
	int ret = BT_ERROR_NONE;
	const char *remote_address = "00:00:00:00:00:00";
	ret = bt_nap_disconnect(remote_address);
	dts_check_eq("bt_nap_disconnect", ret, BT_ERROR_NONE,
			"bt_nap_disconnect() failed.");

}

/**
 * @brief Positive test case of bt_panu_disconnect()
*/
static void utc_network_bluetooth_panu_disconnect_p(void)
{
	int ret = BT_ERROR_NONE;
	const char *remote_address = "00:00:00:00:00:00";
	ret = bt_panu_disconnect(remote_address);
	dts_check_eq("bt_panu_disconnect", ret, BT_ERROR_NONE,
			"bt_panu_disconnect() failed.");

}
