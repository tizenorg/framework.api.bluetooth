/*
 * utc_network_bluetooth_gatt_positive.c
 *
 *  Created on: 23-Sep-2013
 *      Author: shagun.garg
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

char *remote_address;
static GMainLoop *mainloop;

static void utc_network_bluetooth_gatt_foreach_primary_services_p(void);
static void utc_network_bluetooth_gatt_discover_characteristics_p(void);

static void utc_network_bluetooth_gatt_get_service_uuid_p(void);
static void utc_network_bluetooth_gatt_foreach_included_services_p(void);
static void utc_network_bluetooth_gatt_set_characteristic_changed_cb_p(void);
static void utc_network_bluetooth_gatt_unset_characteristic_changed_cb_p(void);
static void utc_network_bluetooth_gatt_get_characteristic_declaration_p(void);
static void utc_network_bluetooth_gatt_set_characteristic_value_p(void);
static void utc_network_bluetooth_gatt_clone_attribute_handle_p(void);
static void utc_network_bluetooth_gatt_destroy_attribute_handle_p(void);
static void utc_network_bluetooth_gatt_connect_p(void);
static void utc_network_bluetooth_gatt_disconnect_p(void);
static void utc_network_bluetooth_set_gatt_connection_state_changed_cb_p(void);
static void utc_network_bluetooth_unset_gatt_connection_state_changed_cb_p(void);

gboolean timeout_func(gpointer data);
void adapter_state_changed_cb_for_gatt_p(int result, bt_adapter_state_e adapter_state,
					void *user_data);
void adapter_le_state_changed_cb_for_gatt_p(int result, bt_adapter_le_state_e adapter_state,
					void *user_data);

bool primary_service_cb_for_gatt_p(bt_gatt_attribute_h service, void *user_data);
bool included_service_cb_for_gatt_p(bt_gatt_attribute_h service, void *user_data);
bool characteristics_discovered_cb_for_gatt_p(int result, int index, int total, bt_gatt_attribute_h characteristic,
					void *user_data);
void characteristics_changed_cb_for_gatt_p(bt_gatt_attribute_h characteristic, unsigned char *value, int value_length,
					void *user_data);
void gatt_connection_state_changed_cb(int result, bool connected,
					const char *remote_address, void *user_data);

struct tet_testlist tet_testlist[] = {
	{utc_network_bluetooth_gatt_foreach_primary_services_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_gatt_discover_characteristics_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_gatt_get_service_uuid_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_gatt_foreach_included_services_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_gatt_set_characteristic_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_set_gatt_connection_state_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_gatt_connect_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_gatt_disconnect_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_unset_gatt_connection_state_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_gatt_unset_characteristic_changed_cb_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_gatt_get_characteristic_declaration_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_gatt_set_characteristic_value_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_gatt_clone_attribute_handle_p, POSITIVE_TC_IDX},
	{utc_network_bluetooth_gatt_destroy_attribute_handle_p, POSITIVE_TC_IDX},
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
		if (strcasecmp(token, "BT_ADDR_LE") == 0) {
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

	if (bt_adapter_set_state_changed_cb(adapter_state_changed_cb_for_gatt_p, "startup") != BT_ERROR_NONE) {
		tet_printf("DTS may fail because bt_adapter_set_state_changed_cb() failed");
	}

	if (bt_adapter_le_set_state_changed_cb(adapter_le_state_changed_cb_for_gatt_p, "enable") != BT_ERROR_NONE) {
		dts_fail("bt_adapter_le_enable",
			"bt_adapter_le_set_state_changed_cb() failed.");
	}

	tet_printf("bt_adapter_enable() was called.");
	ret = bt_adapter_enable();
	if (ret == BT_ERROR_NONE) {
		tet_printf("bt_adapter_enable() succeeded.");
		tet_printf("bt_adapter_le_enable() was called.");
		ret = bt_adapter_le_enable();
		if (ret == BT_ERROR_NONE) {
			tet_printf("bt_adapter_le_enable() succeeded.");
			timeout_id = g_timeout_add(60000, timeout_func, mainloop);
			g_main_loop_run(mainloop);
			g_source_remove(timeout_id);
		}
		else {
			tet_printf("DTS may fail because bt_adapter_le_enable failed");
			bt_adapter_le_unset_state_changed_cb();
		}
	} else if (ret == BT_ERROR_NOT_ENABLED) {
		tet_printf("Bluetooth adapter is not enabled.");
	} else {
		tet_printf("DTS may fail because bt_adapter_enable failed");
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



void adapter_state_changed_cb_for_gatt_p(int result, bt_adapter_state_e adapter_state, void* user_data)
{

}
void adapter_le_state_changed_cb_for_gatt_p(int result, bt_adapter_le_state_e adapter_le__state, void* user_data)
{

}

bool primary_service_cb_for_gatt_p(bt_gatt_attribute_h service, void *user_data)
{
	return false;
}

bool characteristics_discovered_cb_for_gatt_p(int result, int index, int total, bt_gatt_attribute_h characteristic,
				void *user_data)
{
	return false;
}

bool included_service_cb_for_gatt_p(bt_gatt_attribute_h service, void *user_data)
{
	return false;
}

void characteristics_changed_cb_for_gatt_p(bt_gatt_attribute_h characteristic, unsigned char *value, int value_length,
				void *user_data)
{

}

void gatt_connection_state_changed_cb(int result, bool connected,
					const char *remote_address, void *user_data)
{
	tet_printf("bt_gatt_connection_state_changed_cb : %d", result);
	if (connected)
		tet_printf("LE Connected");
	else
		tet_printf("LE Disconnected");
}

/**
 * @brief Positive test case of bt_gatt_foreach_primary_services()
 */
static void utc_network_bluetooth_gatt_foreach_primary_services_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_foreach_primary_services(remote_address, primary_service_cb_for_gatt_p, NULL);
	dts_check_eq("bt_gatt_foreach_primary_services", ret, BT_ERROR_NONE,
		"bt_gatt_foreach_primary_services() failed.");
}


/**
 * @brief Positive test case of bt_gatt_discover_characteristics()
 */
static void utc_network_bluetooth_gatt_discover_characteristics_p(void)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_attribute_h gatt_serv;
	gatt_serv = g_malloc0(sizeof(bt_gatt_attribute_h));

	ret = bt_gatt_discover_characteristics(gatt_serv, characteristics_discovered_cb_for_gatt_p, NULL);
	dts_check_eq("bt_gatt_discover_characteristics", ret, BT_ERROR_NONE, "bt_gatt_discover_characteristics() failed.");
}


/**
 * @brief Positive test case of bt_gatt_get_service_uuid()
 */
static void utc_network_bluetooth_gatt_get_service_uuid_p(void)
{
	int ret = BT_ERROR_NONE;
	char *uid = "dts_uid";
	bt_gatt_attribute_h gatt_serv;
	gatt_serv = g_malloc0(sizeof(bt_gatt_attribute_h));

	ret = bt_gatt_get_service_uuid(gatt_serv, &uid);
	dts_check_eq("bt_gatt_get_service_uuid", ret, BT_ERROR_NONE,
		"bt_gatt_get_service_uuid() failed.");
}

/**
 * @brief Positive test case of bt_gatt_foreach_included_services()
 */
static void utc_network_bluetooth_gatt_foreach_included_services_p(void)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_attribute_h gatt_serv;
	gatt_serv = g_malloc0(sizeof(bt_gatt_attribute_h));
	ret = bt_gatt_foreach_included_services(gatt_serv, included_service_cb_for_gatt_p, NULL);
	dts_check_eq("bt_gatt_foreach_included_services", ret, BT_ERROR_NONE,
		"bt_gatt_foreach_included_services() failed.");
}


/**
 * @brief Positive test case of bt_gatt_set_characteristic_changed_cb()
 */
static void utc_network_bluetooth_gatt_set_characteristic_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_set_characteristic_changed_cb(characteristics_changed_cb_for_gatt_p, NULL);
	dts_check_eq("bt_gatt_set_characteristic_changed_cb", ret, BT_ERROR_NONE,
		"bt_gatt_set_characteristic_changed_cb() failed.");
}


/**
 * @brief Positive test case of bt_gatt_unset_characteristic_changed_cb()
 */
static void utc_network_bluetooth_gatt_unset_characteristic_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_unset_characteristic_changed_cb();
	dts_check_eq("bt_gatt_unset_characteristic_changed_cb", ret, BT_ERROR_NONE,
			"bt_gatt_unset_characteristic_changed_cb() failed.");

}


/**
 * @brief Positive test case of bt_gatt_get_characteristic_declaration()
 */
static void utc_network_bluetooth_gatt_get_characteristic_declaration_p(void)
{
	int ret = BT_ERROR_NONE;
	char **uid = "dts_uid";
	unsigned char **val = "dts_value";
	int l = 1;
	bt_gatt_attribute_h gatt_charac;
	gatt_charac = g_malloc0(sizeof(bt_gatt_attribute_h));
	ret = bt_gatt_get_characteristic_declaration(gatt_charac, uid, val, &l);
	dts_check_eq("bt_gatt_get_characteristic_declaration", ret, BT_ERROR_NONE,
			"gatt_get_characteristic_declaration() failed.");
}


/**
 * @brief Positive test case of bt_gatt_set_characteristic_value()
 */
static void utc_network_bluetooth_gatt_set_characteristic_value_p(void)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_attribute_h gatt_charac;
	gatt_charac = g_malloc0(sizeof(bt_gatt_attribute_h));
	const unsigned char *gatt_value = "dts_value";
	ret = bt_gatt_set_characteristic_value(gatt_charac, gatt_value, strlen("dts_value"));
	dts_check_eq("bt_gatt_set_characteristic_value", ret, BT_ERROR_NONE,
			"bt_gatt_set_characteristic_value() failed.");
}


/**
 * @brief Positive test case of bt_gatt_clone_attribute_handle()
 */
static void utc_network_bluetooth_gatt_clone_attribute_handle_p(void)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_attribute_h gatt_origin;
	bt_gatt_attribute_h gatt_clone;
	gatt_origin = g_malloc0(sizeof(bt_gatt_attribute_h));
	gatt_clone = g_malloc0(sizeof(bt_gatt_attribute_h));

	ret = bt_gatt_clone_attribute_handle(&gatt_clone, gatt_origin);
	dts_check_eq("bt_gatt_clone_attribute_handle", ret, BT_ERROR_NONE,
		"bt_gatt_clone_attribute_handle() failed.");
}


/**
 * @brief Positive test case of bt_gatt_destroy_attribute_handle()
 */
static void utc_network_bluetooth_gatt_destroy_attribute_handle_p(void)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_attribute_h gatt_handle;
	gatt_handle = g_malloc0(sizeof(bt_gatt_attribute_h));

	ret = bt_gatt_destroy_attribute_handle(gatt_handle);
	dts_check_eq("bt_gatt_destroy_attribute_handle", ret, BT_ERROR_NONE,
		"bt_gatt_destroy_attribute_handle() failed.");
}

static void utc_network_bluetooth_gatt_connect_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_connect(remote_address, false);
	dts_check_eq("bt_gatt_connect", ret, BT_ERROR_NONE,
		"bt_gatt_connect() failed.");

}

static void utc_network_bluetooth_gatt_disconnect_p(void)
{
	int ret = BT_ERROR_NONE;
	ret = bt_gatt_disconnect(remote_address);
	dts_check_eq("bt_gatt_connect", ret, BT_ERROR_NONE,
		"bt_gatt_connect() failed.");

}

static void utc_network_bluetooth_set_gatt_connection_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_set_connection_state_changed_cb(gatt_connection_state_changed_cb, NULL);
	dts_check_eq("bt_gatt_set_connection_state_changed_cb", ret, BT_ERROR_NONE,
		"bt_gatt_set_connection_state_changed_cb() failed.");
}

static void utc_network_bluetooth_unset_gatt_connection_state_changed_cb_p(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_gatt_unset_connection_state_changed_cb();
	dts_check_eq("bt_gatt_unset_connection_state_changed_cb", ret, BT_ERROR_NONE,
		"bt_gatt_unset_connection_state_changed_cb() failed.");
}
