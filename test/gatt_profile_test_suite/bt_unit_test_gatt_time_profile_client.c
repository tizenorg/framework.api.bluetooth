/*

Time-Profile version (TIP) 1.0
Current Time Service (CTS)1.1
Next DST Change Service (NDCS) 1.0
Reference Time Update Service (RTUS) 1.0

TIP, CTS, NDCS, RTUS are standard abbreviations.

*/


#include "bt_unit_test_gatt_time_profile.h"

extern bool gatt_debug;

static bool input_valid = false;
static bool notification_possible = false;

bt_gatt_client_h time_gatt_client = NULL;

static time_test_gatt_state_e test_cursor = BT_GATT_TEST_TIME_INITIALIZATION;

char *cts_service_uuid = "00001805-0000-1000-8000-00805F9B34FB"; // Current Time Service
char *current_time_char_uuid = "00002a2b-0000-1000-8000-00805F9B34FB"; // Current Time characteristic
char *lti_char_uuid = "00002a0f-0000-1000-8000-00805F9B34FB"; // Local Time Information
char *rti_char_uuid = "00002a14-0000-1000-8000-00805F9B34FB"; // Reference Time Information


char *ndcs_service_uuid = "00001807-0000-1000-8000-00805F9B34FB"; // Next DST Change Service
char *twdst_char_uuid = "00002a11-0000-1000-8000-00805F9B34FB"; // Time with DST


char *rtus_service_uuid = "00001806-0000-1000-8000-00805F9B34FB"; // Reference Time Update Service
char *tucp_char_uuid = "00002a16-0000-1000-8000-00805F9B34FB"; // Time Update Control Point
char *tus_char_uuid = "00002a17-0000-1000-8000-00805F9B34FB"; // Time Update Control Point.

//handles collection,
static bt_gatt_h time_cti_handle = NULL;
static bt_gatt_h time_lti_handle = NULL;
static bt_gatt_h time_rti_handle = NULL;
static bt_gatt_h time_twdst_handle = NULL;
static bt_gatt_h time_tucp_handle = NULL;
static bt_gatt_h time_tus_handle = NULL;


GSList *read_all_list = NULL;

static int __time_client_validate_input_bd_addr (char *bd_addr);
static void __time_client_scan_result_cb(int result, bt_adapter_le_device_scan_result_info_s *info, void *user_data);
static void __time_client_search_and_connect();
static void __time_client_initiate_connection();
static void __time_client_connection_state_changed_cb(int result, bool connected, const char *remote_address, void *user_data);
static bool __time_client_monitor_svc_cb(int total, int index, bt_gatt_h svc_handle, void *data);
bool __time_client_cts_service_cb(int total, int index, bt_gatt_h chr_handle, void *data);
bool __time_client_ndcs_service_cb(int total, int index, bt_gatt_h chr_handle, void *data);
bool __time_client_rtus_service_cb(int total, int index, bt_gatt_h chr_handle, void *data);
void __time_client_cts_notify_cb(bt_gatt_h chr, char *value, int len, void *user_data);
void __time_client_read_all();
void __cti_read_cb(bt_gatt_h characteristic, char *value, int length);
void __lti_read_cb(bt_gatt_h characteristic, char *value, int length);
void __rti_read_cb(bt_gatt_h characteristic, char *value, int length);
void __ndcs_read_cb(bt_gatt_h characteristic, char *value, int length);

int time_client_initialize(void)
{
	int ret = BT_ERROR_NONE;
	test_cursor = BT_GATT_TEST_TIME_INITIALIZATION;

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

	ret = bt_gatt_set_connection_state_changed_cb(__time_client_connection_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		goto fail;

	test_cursor = BT_GATT_TEST_TIME_INITIALIZED;
	return ret;
fail:
	DBG_PRT("Initiailization error returns  %d", ret);
	return ret;
}

void time_client_accept_input(char *buf)
{
	if (__time_client_validate_input_bd_addr(buf) != BT_ERROR_NONE) {
		USR_PRT("invalid bluetooth address");
	} else {
		memcpy (remote_addr, buf, 17);
		remote_addr[17] = 0;
		if (test_cursor == BT_GATT_TEST_TIME_INITIALIZED)
			__time_client_initiate_connection();
	}
}

void time_client_signal_handler(int signno)
{
	bt_gatt_disconnect(remote_addr);
	bt_gatt_unset_connection_state_changed_cb();
}


gboolean time_client_key_event_cb(GIOChannel *chan,
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

	if (test_cursor == BT_GATT_TEST_UNRECOVERABLE) {
		USR_PRT("Unrecoverable error, please reboot");
	} else if (test_cursor == BT_GATT_TEST_TIME_INITIALIZATION) {
		if(time_server_initialize() != BT_ERROR_NONE) {
			USR_PRT("Initialization failed please reboot");
		} else {
			DBG_PRT("Initialization is successfull");
			if(!input_valid) {
				USR_PRT("In-put remote BD-ADDR in the below format");
				USR_PRT("00:11:22:33:44:55");
				test_cursor = BT_GATT_TEST_TIME_CLIENT_SET_ADDRESS;
			} else {
				__time_client_initiate_connection();
			}			
		}

	} else if (test_cursor == BT_GATT_TEST_TIME_INITIALIZED) {
		if(!input_valid) {
			USR_PRT("In-put remote BD-ADDR in the below format");
			USR_PRT("00:11:22:33:44:55");
			test_cursor = BT_GATT_TEST_TIME_CLIENT_SET_ADDRESS;
		} else {
			__time_client_initiate_connection();
		}
	} else if (test_cursor == BT_GATT_TEST_TIME_CLIENT_SET_ADDRESS) {
		if (__time_client_validate_input_bd_addr(buf) != BT_ERROR_NONE) {
			USR_PRT("invalid bluetooth address");
			USR_PRT("In-put remote BD-ADDR in the below format");
			USR_PRT("00:11:22:33:44:55");
			test_cursor = BT_GATT_TEST_TIME_CLIENT_SET_ADDRESS;
		} else {
			memcpy (remote_addr, buf, 17);
			remote_addr[17] = 0;
			__time_client_initiate_connection();
		}
	} else if (test_cursor == BT_GATT_TEST_TIME_CLIENT_CONNECT) {
		__time_client_initiate_connection();
		USR_PRT("Please wait.. while we connect the device");
	} else if (test_cursor == BT_GATT_TEST_TIME_CLIENT_CONNECTING) {
		USR_PRT("Please wait.. while we connect the device");
	} else if (test_cursor == BT_GATT_TEST_TIME_CLIENT_BONDING) {
		USR_PRT("Please wait.. Bonding is ongoing. If asked for pairing confirmation, take action");
	} else if (test_cursor == BT_GATT_TEST_TIME_CLIENT_BROWSE_SERVICES) {
		USR_PRT("Quering supported services..");
	} else if (test_cursor == BT_GATT_TEST_TIME_CLIENT_MONITOR_SERVICES) {
		USR_PRT("Press enter to refresh the Values");
		test_cursor = BT_GATT_TEST_TIME_CLIENT_READ_VALUES;
	} else if (test_cursor == BT_GATT_TEST_TIME_CLIENT_READ_VALUES) {
		DBG_PRT("Reading values, please wait");
		__time_client_read_all();
		test_cursor = BT_GATT_TEST_TIME_CLIENT_READ_ONGOING;
	} else if (test_cursor == BT_GATT_TEST_TIME_CLIENT_READ_ONGOING) {
		DBG_PRT("Reading values, please wait");
	}

	return TRUE;
}

static int __time_client_validate_input_bd_addr (char *bd_addr)
{
	/*have to design better check for validity check of the input*/
	if (bd_addr == NULL || strlen(bd_addr) < 16) {
		input_valid = false;
		return BT_ERROR_INVALID_PARAMETER;
	}
	input_valid = true;
	return BT_ERROR_NONE;
}

static void __time_client_scan_result_cb(int result,
	bt_adapter_le_device_scan_result_info_s *info, void *user_data)
{
	if (info == NULL) {
		DBG_PRT("No discovery_info!");
		return;
	}
	if(!strcmp(info->remote_address, remote_addr)) {
		bt_adapter_le_stop_scan();
		if(bt_gatt_connect(remote_addr, false) != BT_ERROR_NONE)
			test_cursor = BT_GATT_TEST_UNRECOVERABLE;
	}
}
static void __time_client_search_and_connect()
{
	int ret;
	ret = bt_adapter_le_start_scan(__time_client_scan_result_cb, NULL);
	if(ret != BT_ERROR_NONE)
		test_cursor = BT_GATT_TEST_UNRECOVERABLE;
}

static void __time_client_initiate_connection()
{
	int ret;
	USR_PRT("Make sure the remote device is turned on and visible");
	USR_PRT("Please wait.. while we connect the device");

	test_cursor = BT_GATT_TEST_TIME_CLIENT_CONNECTING;

	ret = bt_gatt_connect(remote_addr, false);
	if(ret != BT_ERROR_NONE)
		__time_client_search_and_connect();
}

void __time_client_bond_created_cb(int result, bt_device_info_s *device_info, void *user_data)
{
	if(result == BT_ERROR_NONE)
	{
		DBG_PRT("bond Creation successfull.");

		DBG_PRT("Callback: is_bonded - %d.", device_info->is_bonded);
		DBG_PRT("Callback: is_connected - %d.", device_info->is_connected);
		notification_possible = true;
		
	} else {
		DBG_PRT("bond creation failed.");
		notification_possible = false;
	}
	test_cursor = BT_GATT_TEST_TIME_CLIENT_BROWSE_SERVICES;
	if (time_gatt_client) {
		bt_gatt_client_destroy(time_gatt_client);
		time_gatt_client = NULL;
	}
	if(bt_gatt_client_create(remote_addr, &time_gatt_client) != BT_ERROR_NONE) {
		test_cursor = BT_GATT_TEST_UNRECOVERABLE;
		DBG_PRT("GATT_Client creation failed");
		return;
	}
	if(bt_gatt_client_foreach_services(time_gatt_client,
			__time_client_monitor_svc_cb, NULL) != BT_ERROR_NONE) {
		test_cursor = BT_GATT_TEST_UNRECOVERABLE;
		return;
	}
	test_cursor = BT_GATT_TEST_TIME_CLIENT_MONITOR_SERVICES;
}


static void __time_client_connection_state_changed_cb(int result, bool connected, const char *remote_address, void *user_data)
{
	int ret = BT_ERROR_NONE;
	if (test_cursor == BT_GATT_TEST_UNRECOVERABLE)
		return;
	
	if(!strcmp(remote_address, remote_addr)) {
		if (connected) {
			DBG_PRT("Connected to the remote device");
			bt_device_info_s *device_info = NULL;

			ret = bt_adapter_get_bonded_device_info(remote_addr,
								&device_info);
			if(ret == BT_ERROR_NONE) {
				if (device_info) {
					DBG_PRT("address: %s", device_info->remote_address);
					DBG_PRT("bonded status: %d", device_info->is_bonded);
					if(device_info->is_bonded == false) {
						 ret = bt_device_create_bond_by_type(remote_addr, BT_DEVICE_CONNECTION_LINK_LE);
						 if(ret == BT_ERROR_NONE) {
						 	bt_device_set_bond_created_cb(__time_client_bond_created_cb, NULL);
						 	bt_adapter_free_device_info(device_info);
							test_cursor = BT_GATT_TEST_TIME_CLIENT_BONDING;
							return;
						}
					}
				}
			}
			else if(ret == BT_ERROR_REMOTE_DEVICE_NOT_BONDED) {
				ret = bt_device_create_bond_by_type(remote_addr, BT_DEVICE_CONNECTION_LINK_LE);
				 if(ret == BT_ERROR_NONE) {
				 	bt_device_set_bond_created_cb(__time_client_bond_created_cb, NULL);
				 	bt_adapter_free_device_info(device_info);
					test_cursor = BT_GATT_TEST_TIME_CLIENT_BONDING;
					return;
				}
			}
			bt_adapter_free_device_info(device_info);
			test_cursor = BT_GATT_TEST_TIME_CLIENT_BROWSE_SERVICES;
			if (time_gatt_client) {
				bt_gatt_client_destroy(time_gatt_client);
				time_gatt_client = NULL;
			}
			if(bt_gatt_client_create(remote_addr, &time_gatt_client) != BT_ERROR_NONE) {
				test_cursor = BT_GATT_TEST_UNRECOVERABLE;
				DBG_PRT("GATT_Client creation failed");
				return;
			}
			if(bt_gatt_client_foreach_services(time_gatt_client,
					__time_client_monitor_svc_cb, NULL) != BT_ERROR_NONE) {
				test_cursor = BT_GATT_TEST_UNRECOVERABLE;
				return;
			}
			test_cursor = BT_GATT_TEST_TIME_CLIENT_MONITOR_SERVICES;
		} else {
			USR_PRT("Connection to the remote device is removed");
			USR_PRT("Attempting to re-connect to the device");
			USR_PRT("Make sure the remote device is turned on and in range");
			test_cursor = BT_GATT_TEST_TIME_CLIENT_CONNECTING;
			notification_possible = false;
			if (time_gatt_client)
				bt_gatt_client_destroy(time_gatt_client);
			bt_gatt_connect(remote_addr, true);
		}
	}

}

static bool __time_client_monitor_svc_cb(int total, int index, bt_gatt_h svc_handle, void *data)
{
	char *svc_str = NULL;
	char *uuid = NULL;

	bt_gatt_get_uuid(svc_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &svc_str);
	DBG_PRT("%s (uuid = %s)", svc_str ? svc_str : uuid, uuid);

	if(!strcasecmp(uuid, cts_service_uuid)){
		DBG_PRT("%s service", svc_str ? svc_str : uuid);
		bt_gatt_service_foreach_characteristics(svc_handle,
				__time_client_cts_service_cb, NULL);
	} else if(!strcasecmp(uuid, ndcs_service_uuid)){
		DBG_PRT("%s service", svc_str ? svc_str : uuid);
		bt_gatt_service_foreach_characteristics(svc_handle,
				__time_client_ndcs_service_cb, NULL);
	} else if(!strcasecmp(uuid, rtus_service_uuid)){
		DBG_PRT("%s service", svc_str ? svc_str : uuid);
		bt_gatt_service_foreach_characteristics(svc_handle,
				__time_client_rtus_service_cb, NULL);
	}
	
	g_free(svc_str);
	g_free(uuid);

	return true;
}

bool __time_client_cts_service_cb(int total, int index, bt_gatt_h chr_handle, void *data)
{
	char *uuid = NULL;
	char *str = NULL;
	int ret;

	bt_gatt_get_uuid(chr_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	if(!strcasecmp(uuid, current_time_char_uuid)){
		USR_PRT("CurrentTime Information feature is supported");
		if(time_cti_handle)
			g_free(time_cti_handle);
		time_cti_handle = chr_handle;
		if(notification_possible) {
			ret = bt_gatt_client_set_characteristic_value_changed_cb(chr_handle,
						__time_client_cts_notify_cb, NULL);
			if(ret != BT_ERROR_NONE)
				DBG_PRT("registerinig Notification failed, error = %d", ret);
			if(ret == BT_ERROR_PERMISSION_DENIED)
				DBG_PRT("Probably pairing information doesn't exist in either devices");	
		}
	} else if(!strcasecmp(uuid, lti_char_uuid)){
		USR_PRT("Local Time Information feature is supported");
		if(time_lti_handle)
			g_free(time_lti_handle);
		time_lti_handle = chr_handle;
	} else if(!strcasecmp(uuid, rti_char_uuid)){
		USR_PRT("Reference Time Information feature is supported");
		if(time_rti_handle)
			g_free(time_rti_handle);
		time_rti_handle = chr_handle;
	}
	g_free(str);
	g_free(uuid);

	return true;
}


bool __time_client_ndcs_service_cb(int total, int index, bt_gatt_h chr_handle, void *data)
{
	char *uuid = NULL;
	char *str = NULL;

	bt_gatt_get_uuid(chr_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	if(!strcasecmp(uuid, twdst_char_uuid)){
		USR_PRT("TWDST feature is supported");
		if(time_twdst_handle)
			g_free(time_twdst_handle);
		time_twdst_handle = chr_handle;
	}
	
	g_free(str);
	g_free(uuid);

	return true;
}

bool __time_client_rtus_service_cb(int total, int index, bt_gatt_h chr_handle, void *data)
{
	char *uuid = NULL;
	char *str = NULL;

	bt_gatt_get_uuid(chr_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	if(!strcasecmp(uuid, tucp_char_uuid)){
		USR_PRT("CurrentTime Information feature is supported");
		if(time_tucp_handle)
			g_free(time_tucp_handle);
		time_tucp_handle = chr_handle;
	} else if(!strcasecmp(uuid, tus_char_uuid)){
		USR_PRT("Local Time Information feature is supported");
		if(time_tus_handle)
			g_free(time_tus_handle);
		time_tus_handle = chr_handle;
	}

	g_free(str);
	g_free(uuid);

	return true;
}

void __time_client_cts_notify_cb(bt_gatt_h chr, char *value, int len,
		void *user_data)
{
	DBG_PRT("+");
}

void __time_client_read_all_cb(int result, bt_gatt_h gatt_handle, void *data)
{
	int ret = BT_ERROR_NONE;
	char *value = NULL;
	int len = 0;
	time_client_char_read_interpretter read_cb = (time_client_char_read_interpretter)data;
	DBG_PRT("Read %s", result == BT_ERROR_NONE ? "Success" : "Fail");

	if (result == BT_ERROR_NONE) {
		ret = bt_gatt_get_value(gatt_handle, &value, &len);
		if (ret == BT_ERROR_NONE) {
			read_cb(gatt_handle, value, len);
		}
	}

	if(read_all_list) {
		time_client_read_list_s *read_list = (time_client_read_list_s *)read_all_list->data;
		bt_gatt_client_read_value(read_list->char_handle, __time_client_read_all_cb, read_list->read_cb);
		 read_all_list = g_slist_remove(read_all_list, read_list);
	}
	else {
		test_cursor = BT_GATT_TEST_TIME_CLIENT_MONITOR_SERVICES;
	}
}
void __time_client_read_all()
{
	if(read_all_list)
		g_slist_free(read_all_list);
	read_all_list = NULL;

	if(time_cti_handle) {
		time_client_read_list_s *cti_read = NULL;
		cti_read = g_malloc0(sizeof(time_client_read_list_s));
		if (cti_read) {
			cti_read->char_handle = time_cti_handle;
			cti_read->read_cb = __cti_read_cb;
			read_all_list = g_slist_append(read_all_list, cti_read);
		}
	}
	if(time_lti_handle) {
		time_client_read_list_s *lti_read = NULL;
		lti_read = g_malloc0(sizeof(time_client_read_list_s));
		if (lti_read) {
			lti_read->char_handle = time_lti_handle;
			lti_read->read_cb = __lti_read_cb;
			read_all_list = g_slist_append(read_all_list, lti_read);
		}
	}
	if(time_rti_handle) {
		time_client_read_list_s *rti_read = NULL;
		rti_read = g_malloc0(sizeof(time_client_read_list_s));
		if (rti_read) {
			rti_read->char_handle = time_rti_handle;
			rti_read->read_cb = __rti_read_cb;
			read_all_list = g_slist_append(read_all_list, rti_read);
		}
	}
	if(time_twdst_handle) {
		time_client_read_list_s *twdst_read = NULL;
		twdst_read = g_malloc0(sizeof(time_client_read_list_s));
		if (twdst_read) {
			twdst_read->char_handle = time_twdst_handle;
			twdst_read->read_cb = __ndcs_read_cb;
			read_all_list = g_slist_append(read_all_list, twdst_read);
		}
	}

	if(read_all_list) {
		time_client_read_list_s *read_list = (time_client_read_list_s *)read_all_list->data;
		bt_gatt_client_read_value(read_list->char_handle, __time_client_read_all_cb, read_list->read_cb);
		 read_all_list = g_slist_remove(read_all_list, read_list);
	}
	else {
		test_cursor = BT_GATT_TEST_TIME_CLIENT_MONITOR_SERVICES;
	}
}

void __cti_read_cb(bt_gatt_h characteristic, char *value, int length)
{
	int year = 0;
	
	if(length != 10) {
		USR_PRT("invalid value received from remote device"); 
		return;
	}
	DBG_PRT("value[1] = %d, value[0] = %d", value[1], value[0]);
	year = (value[1] << 8) + value[0];
	USR_PRT("\n\t\tCURRENT TIME INFORMATION (YYYY/MM/DD  HH:MM:SS");
	USR_PRT("\t%d/%d/%d  %d:%d:%d", year, value[2], value[3], value[4], value[5], value[6]);
	switch(value[7]) {
	case DAY_UNKNOWN:
		USR_PRT("\tDay of the week : Unknown");
		break;
	case MONDAY:
		USR_PRT("\tDay of the week : MONDAY");
		break;
	case TUESDAY:
		USR_PRT("\tDay of the week : TUESDAY");
		break;
	case WEDNESDAY:
		USR_PRT("\tDay of the week : WEDNESDAY");
		break;
	case THURSDAY:
		USR_PRT("\tDay of the week : THURSDAY");
		break;
	case FRIDAY:
		USR_PRT("\tDay of the week : FRIDAY");
		break;
	case SATURDAY:
		USR_PRT("\tDay of the week : SATURDAY");
		break;
	case SUNDAY:
		USR_PRT("\tDay of the week : SUNDAY");
		break;
	}
	USR_PRT("\tfractions256 = %d", value[8]);
	USR_PRT("\tAdjust reason= %d", value[9]);
}

void __lti_read_cb(bt_gatt_h characteristic, char *value, int length)
{
	if(length != 2) {
		USR_PRT("invalid value received from remote device"); 
		return;
	}
	USR_PRT("\n\t\tLocal Time Information");
	USR_PRT("\tTime Zone %d", value[0]);
	USR_PRT("\tDST = %d", value[1]);
}

void __rti_read_cb(bt_gatt_h characteristic, char *value, int length)
{
	if(length != 4) {
		USR_PRT("invalid value received from remote device"); 
		return;
	}
	USR_PRT("\n\t\tReference Time Information");
	USR_PRT("\tTime source %d", value[0]);
	USR_PRT("\tAccuracy = %d", value[1]);
	USR_PRT("\tDays since last update = %d", value[2]);
	USR_PRT("\tHours since last update = %d", value[3]);
}

void __ndcs_read_cb(bt_gatt_h characteristic, char *value, int length)
{
	int year = 0;
	if(length != 8) {
		USR_PRT("invalid value received from remote device"); 
		return;
	}
	year = (value[1] << 8) + value[0];
	USR_PRT("\n\t\tTime with DST  Information");
	USR_PRT("\t%d/%d/%d  %d:%d:%d", year, value[2], value[3], value[4], value[5], value[6]);
	USR_PRT("\tDST offset %d", value[7]);	
}
