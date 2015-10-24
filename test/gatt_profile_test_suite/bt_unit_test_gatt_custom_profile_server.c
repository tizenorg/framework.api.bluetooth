#include "bt_unit_test_gatt_custom_profile_server.h"


#define service_uuid "F2819041-529C-11E3-A67F-0002A5d5C51C" // Android Unlock Gear
#define characteristic_uuid "F2819043-529C-11E3-A67F-0002A5d5C51C"

typedef struct {
	bt_gatt_h svc;
	bt_gatt_h chr;
	bt_gatt_h desc;
} gatt_handle_t;

extern bool gatt_debug;

static custom_test_gatt_state_e test_cursor = BT_GATT_TEST_CUSTOM_INITIALIZATION;
static gatt_handle_t custom_h;
static bt_gatt_server_h server = NULL;


void __custom_server_connection_state_changed_cb(int result, bool connected, const char *remote_address, void *user_data)
{
	if (connected) {
		DBG_PRT("Remote device connected [%s]", remote_address);
		g_strlcpy(remote_addr, remote_address, BT_ADDR_LENGTH);
	}
}

static void __custom_server_advertising_state_changed_cb(int result,
		bt_advertiser_h advertiser, bt_adapter_le_advertising_state_e adv_state, void *user_data)
{
	DBG_PRT("Result : %d", result);
	DBG_PRT("Advertiser : %p", advertiser);
	DBG_PRT("Advertising %s [%d]", adv_state == BT_ADAPTER_LE_ADVERTISING_STARTED ?
				"started" : "stopped", adv_state);
}

static int __custom_server_start_advertising(void)
{
	int ret;
	bt_advertiser_h advertiser = NULL;

	DBG_PRT("Start advertising");

	ret = bt_adapter_le_create_advertiser(&advertiser);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_adapter_le_add_advertising_service_uuid(advertiser,
			BT_ADAPTER_LE_PACKET_ADVERTISING, service_uuid);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_adapter_le_set_advertising_device_name(advertiser,
	BT_ADAPTER_LE_PACKET_SCAN_RESPONSE, true);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_adapter_le_start_advertising_new(advertiser,
				__custom_server_advertising_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		goto fail;

	return ret;

fail:
	DBG_PRT("Advertising failed [0x%04x]", ret);
	return ret;
}

static int __custom_server_register_gatt(void)
{
	int ret;
	bt_gatt_h service = NULL;
	bt_gatt_h characteristic = NULL;
	char char_value[4] = {10, 20, 30, 40};
	int value_length = 4;
	int permissions = BT_GATT_PERMISSION_READ | BT_GATT_PERMISSION_WRITE;
	int properties = BT_GATT_PROPERTY_READ | BT_GATT_PROPERTY_WRITE |
					BT_GATT_PROPERTY_WRITE_WITHOUT_RESPONSE;

	DBG_PRT("Register gatt server");

	ret = bt_gatt_server_initialize();
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_gatt_server_create(&server);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_gatt_service_create(service_uuid, BT_GATT_SERVICE_TYPE_PRIMARY, &service);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_gatt_characteristic_create(characteristic_uuid, permissions, properties,
										char_value, value_length, &characteristic);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_gatt_service_add_characteristic(service, characteristic);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_gatt_server_register_service(server, service);
	if (ret != BT_ERROR_NONE)
		goto fail;

	custom_h.svc = service;
	custom_h.chr = characteristic;

	return ret;

fail:
	DBG_PRT("Register gatt server failed [0x%04x]", ret);
	return ret;
}

static int __custom_profile_server_launch(void)
{
	int ret = BT_ERROR_NONE;
	test_cursor = BT_GATT_TEST_CUSTOM_SERVER_INITIALIZING;

	DBG_PRT("Initialize gatt server");

	ret = __custom_server_register_gatt();
	if (ret != BT_ERROR_NONE)
		return ret;

	ret = __custom_server_start_advertising();
	if (ret != BT_ERROR_NONE)
		return ret;

	test_cursor = BT_GATT_TEST_CUSTOM_SERVER_INITIALIZED;
	return ret;
}

int custom_server_initialize(void)
{
	int ret = BT_ERROR_NONE;
	test_cursor = BT_GATT_TEST_CUSTOM_INITIALIZATION;

	ret = bt_initialize();
	if (ret == BT_ERROR_NONE || ret == BT_ERROR_ALREADY_DONE)
		DBG_PRT("Initialized");
	else
		goto fail;

	ret = bt_adapter_enable();
	if (ret == BT_ERROR_NONE || ret == BT_ERROR_ALREADY_DONE)
		DBG_PRT("Adapter enabled");
	else
		goto fail;

	ret = bt_gatt_set_connection_state_changed_cb(__custom_server_connection_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		goto fail;

	test_cursor = BT_GATT_TEST_CUSTOM_INITIALIZED;
	return ret;

fail:
	DBG_PRT("Initiailization error returns  %d", ret);
	return ret;
}

void custom_server_accept_input(char *buf)
{
//	DBG_PRT("test_cursor %d", test_cursor);

	//ignore the other options for server
	if (test_cursor == BT_GATT_TEST_CUSTOM_INITIALIZED) {
		if (__custom_profile_server_launch() != BT_ERROR_NONE)
			test_cursor = BT_GATT_TEST_UNRECOVERABLE;
	}
}

void custom_server_signal_handler(int signno)
{
	DBG_PRT("Terminate custom gatt server");

	bt_gatt_disconnect(remote_addr);
	bt_gatt_unset_connection_state_changed_cb();
}

gboolean custom_server_key_event_cb(GIOChannel *chan,
				GIOCondition cond,
				gpointer data)
{
	char buf[BUFFER_LEN] = { 0 };
	unsigned int len = 0;

	if (g_io_channel_read_chars(chan, buf, sizeof(buf),
				&len, NULL) == G_IO_STATUS_ERROR) {
		DBG_PRT("IO Channel read error");
		return FALSE;
	}

//	DBG_PRT("test_cursor %d", test_cursor);

	if (test_cursor == BT_GATT_TEST_UNRECOVERABLE) {
		USR_PRT("Unrecoverable error, please reboot");
	} else if (test_cursor == BT_GATT_TEST_CUSTOM_INITIALIZATION) {
		if(custom_server_initialize() != BT_ERROR_NONE) {
			USR_PRT("Initialization failed please reboot");
		} else {
			DBG_PRT("Initialization is successfull");
			if (__custom_profile_server_launch() != BT_ERROR_NONE) {
				test_cursor = BT_GATT_TEST_UNRECOVERABLE;
				USR_PRT("Custom profile server launch failed");
			}
		}
	} else if (test_cursor == BT_GATT_TEST_CUSTOM_INITIALIZED) {
		if (__custom_profile_server_launch() != BT_ERROR_NONE) {
			test_cursor = BT_GATT_TEST_UNRECOVERABLE;
			USR_PRT("Custom profile server launch failed");
		}
	} else if (test_cursor == BT_GATT_TEST_CUSTOM_SERVER_INITIALIZED) {
		USR_PRT("Custom profile server initialized. Ready to accept the connections");
	}

	return TRUE;
}

