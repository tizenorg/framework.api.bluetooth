/*

Time-Profile version (TIP) 1.0
Current Time Service (CTS)1.1
Next DST Change Service (NDCS) 1.0
Reference Time Update Service (RTUS) 1.0

TIP, CTS, NDCS, RTUS are standard abbreviations.

*/


#include "bt_unit_test_gatt_time_profile.h"
#include <time.h>

extern bool gatt_debug;

bt_gatt_server_h tip_server = NULL;

static time_test_gatt_state_e test_cursor = BT_GATT_TEST_TIME_INITIALIZATION;

static int __time_profile_server_launch(void);
void __time_server_connection_state_changed_cb(int result, bool connected, const char *remote_address, void *user_data);
static int __initialize_tip_server();
static int __initialize_cts();
static int __initialize_ndcs();
static int __initialize_rtus();

int time_server_initialize(void)
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

	ret = bt_gatt_set_connection_state_changed_cb(__time_server_connection_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		goto fail;

	test_cursor = BT_GATT_TEST_TIME_INITIALIZED;
	return ret;
fail:
	DBG_PRT("Initiailization error returns  %d", ret);
	return ret;
}

void time_server_accept_input(char *buf)
{
	//ignore the other options for server
	if (test_cursor == BT_GATT_TEST_TIME_INITIALIZED) {
		if (__time_profile_server_launch() != BT_ERROR_NONE)
		test_cursor = BT_GATT_TEST_TIME_INITIALIZED;
	}
}

void time_server_signal_handler(int signno)
{
	bt_gatt_disconnect(remote_addr);
	bt_gatt_unset_connection_state_changed_cb();
}


gboolean time_server_key_event_cb(GIOChannel *chan,
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
			if (__time_profile_server_launch() != BT_ERROR_NONE) {
				test_cursor = BT_GATT_TEST_UNRECOVERABLE;
				USR_PRT("Time profile server launch failed");
			}
		}

	} else if (test_cursor == BT_GATT_TEST_TIME_INITIALIZED) {
		if (__time_profile_server_launch() != BT_ERROR_NONE) {
			test_cursor = BT_GATT_TEST_UNRECOVERABLE;
			USR_PRT("Time profile server launch failed");
		}
	} else if (test_cursor == BT_GATT_TEST_TIME_SERVER_INITIALIZED) {
		USR_PRT("Time profile server initialized. Ready to accept the connections");
	} else {
		USR_PRT("Default case: Server ready to accept the connection");
	}

	return TRUE;
}

void __time_server_connection_state_changed_cb(int result, bool connected, const char *remote_address, void *user_data)
{
	//shopuld we consider the case of "BT_GATT_TEST_UNRECOVERABLE" ??
		if (connected) {
			DBG_PRT("Remote device connected");
			g_strlcpy(remote_addr, remote_address, BT_ADDR_LENGTH);
		}
}

static int __time_profile_server_launch(void)
{
	int ret = BT_ERROR_NONE;
	test_cursor = BT_GATT_TEST_TIME_SERVER_INITIALIZING;
	DBG_PRT("__time_profile_server_launch");
	//Initialize the server
	ret = __initialize_tip_server();
	if(ret != BT_ERROR_NONE) {
		USR_PRT("Server  initialization failed");
		USR_PRT("Please reboot the Device");
		test_cursor = BT_GATT_TEST_UNRECOVERABLE;
		goto done;
	}
	DBG_PRT("__initialize_tip_server = %d", ret);
	//Current time service is mandatory service
	ret = __initialize_cts();
	if(ret != BT_ERROR_NONE) {
		USR_PRT("Current time service  initialization failed");
		USR_PRT("This is a mandatory service, So aborting initialization sequence");
		USR_PRT("Please reboot the Device");
		test_cursor = BT_GATT_TEST_UNRECOVERABLE;
		goto done; //if mandatory service can't be initilaized then don't proceed
	}
	DBG_PRT("__initialize_cts = %d", ret);
	goto done; //As of now multiple services registration is failing. So skipping,
	//Next DST Change Service is a optional service
	ret = __initialize_ndcs();
	if(ret != BT_ERROR_NONE) {
		USR_PRT("Current Time Service  initialization failed");
		USR_PRT("It is an optional service, so continue to host Time Profile");
	}
	DBG_PRT("__initialize_ndcs = %d", ret);
	//Reference Time Update service is optional service
	ret = __initialize_rtus();
	if(ret != BT_ERROR_NONE) {
		USR_PRT("Reference Time Update Service  initialization failed");
		USR_PRT("It is an optional service, so continue to host Time Profile");
	}
	DBG_PRT("__initialize_rtus = %d", ret);
	ret = BT_ERROR_NONE;
	test_cursor = BT_GATT_TEST_TIME_SERVER_INITIALIZED;
done:
	return ret;
}

static int __initialize_tip_server()
{
	int ret = BT_ERROR_NONE;
	
	ret = bt_gatt_server_initialize();
	if(ret != BT_ERROR_NONE) {
		DBG_PRT("Server Initialization failed");
		return ret;
	}

	ret = bt_gatt_server_create(&tip_server);
	if(ret != BT_ERROR_NONE) {
		DBG_PRT("Server Initialization failed");
		return ret;
	}
	return ret;
}

static int __initialize_cts()
{
	int ret = BT_ERROR_NONE;
	bt_gatt_h cts_service = NULL;
	char *cts_service_uuid = "1805"; // Current Time Service

//begining of current time characteristic declaration
	bt_gatt_h current_time_characteristic = NULL;
	char *current_time_char_uuid = "2a2b"; // Current Time characteristic
	int current_time_char_permissions = BT_GATT_PERMISSION_READ;
	int current_time_char_properties = BT_GATT_PROPERTY_NOTIFY | BT_GATT_PROPERTY_READ; // write is optional, it is ignored as of now.
	unsigned char current_time_char_value[10] = {0}; // 0 is not the proper default value. should check the Date Time + Day of Week spec

/*
	char value breakup
	todo : check whether the below characteristic value follow, LSO to MSO order from spec

	Exact Time 256 : Day Date Time + Fractions 256
						Day Date Time : Date Time + Day of Week
										Date Time : 7 bytes   (current_time_char_value[0] to current_time_char_value[6])
										Day of Week : 1 byte (current_time_char_value[7])
									total : 8 bytes
						Fractions 256 : 1 byte (current_time_char_value[8])
				Total bytes for Exact Time 256 = 9 bytes

	Adjust Reason : 1 byte (current_time_char_value[9])

	Total Bytes of this characteristic = 10 bytes
*/

	bt_gatt_h cccdesc = NULL;
	char *cccdesc_uuid = "2902";
	int cccdesc_permissions = BT_GATT_PERMISSION_READ | BT_GATT_PERMISSION_WRITE;
	char cccdesc_value[2] = {0, 0}; // Notification disabled by default
//end of current time characteristic declaration


//begining of Local time information characteristic declaration
	bt_gatt_h lti_characteristic = NULL;
	char *lti_char_uuid = "2a0f"; // Local Time Information
	int lti_char_permissions = BT_GATT_PERMISSION_READ;
	int lti_char_properties = BT_GATT_PROPERTY_READ; // write is optional, it is ignored as of now.
	char lti_char_value[2] = {0}; // 0 is not the proper default value. should check the Date Time + Day of Week spec

/*
	char value breakup
	todo : check whether the below characteristic value follow, LSO to MSO order from spec

	Time Zone : (lti_char_value[0])  sint8 (signed 8 bit integer) Minimu value -48, Maximum value 56

	DST offset : (lti_char_value[1]) uint8 (Unsigned 8 bit integer) Minimum value 0, Maximum value 8

*/

//end of Local time information characteristic declaration	

//begining of Reference time information characteristic declaration
	bt_gatt_h rti_characteristic = NULL;
	char *rti_char_uuid = "2a14"; // Reference Time Information
	int rti_char_permissions = BT_GATT_PERMISSION_READ;
	int rti_char_properties = BT_GATT_PROPERTY_READ;
	char rti_char_value[4] = {0}; // recheck the default value.

/*
	char value breakup
	todo : check whether the below characteristic value follow, LSO to MSO order from spec

	Source : Time source, 8bit, Minimum value 0, Maximum value 7. rti_char_value[0]

	Accuracy : Time Accuracy, uint8, Minimum Value 0 Maximum value 253. rti_char_value[1]

	Days Since Update: uint8, Minimum Value 0, Maximum value 254. rti_char_value[2]

	Hours since update: uint8, Minimum value 0, Maximum value 23. rti_char_value[3]

*/

//end of Reference time information characteristic declaration

	ret = bt_gatt_service_create(cts_service_uuid, BT_GATT_SERVICE_TYPE_PRIMARY, &cts_service);
	if (ret != BT_ERROR_NONE)
		goto fail;

//begining of current_time_characteristic
/* default value Initialization */
//Exact Time 256
	//Day Date Time
		//Date Time
			//Year 2015
			current_time_char_value[0] = 15;
			current_time_char_value[1] = 20;
			//Month september
			current_time_char_value[2] = 9;
			//Day 03
			current_time_char_value[3] = 3;
			//Hours 10
			current_time_char_value[4] = 10;
			//Minutes 10
			current_time_char_value[5] = 10;
			//seconds 10
			current_time_char_value[6] = 10;
		//Day of week
			current_time_char_value[7] = FRIDAY;
	//Fractions256
		current_time_char_value[8] = 0;
//Adjust Reason
	current_time_char_value[9] = 0; //It is initialized with "no reason". gatt_tip_current_time_adj_reason_mask_e should be used while notifying
	ret = bt_gatt_characteristic_create(current_time_char_uuid, current_time_char_permissions, current_time_char_properties,
												(char*)current_time_char_value, sizeof(current_time_char_value), &current_time_characteristic);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_gatt_service_add_characteristic(cts_service, current_time_characteristic);

	if (ret != BT_ERROR_NONE) {
		bt_gatt_destroy(current_time_characteristic);
		goto fail;
	}
	bt_gatt_server_set_read_value_requested_cb(current_time_characteristic, current_time_read_value_requested_cb, NULL);

	ret = bt_gatt_descriptor_create(cccdesc_uuid, cccdesc_permissions, cccdesc_value, sizeof(cccdesc_value), &cccdesc);

	if (ret != BT_ERROR_NONE) 
		goto fail;

	ret = bt_gatt_characteristic_add_descriptor(current_time_characteristic, cccdesc);

	if (ret != BT_ERROR_NONE) {
		bt_gatt_destroy(cccdesc);
		goto fail;
	}
//end of current_time_characteristic

//begining of Local time information characteristic
/* default value Initialization */
	//Time Zone : sint8, enum declaration is not done for the ease of development
	lti_char_value[0] = 16; //UTC+4:00
	//Daylight Saving Time
	lti_char_value[1] = DST_STANDARD_TIME;
	ret = bt_gatt_characteristic_create(lti_char_uuid, lti_char_permissions, lti_char_properties,
												lti_char_value, sizeof(lti_char_value), &lti_characteristic);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_gatt_service_add_characteristic(cts_service, lti_characteristic);

	if (ret != BT_ERROR_NONE) {
		bt_gatt_destroy(lti_characteristic);
		goto fail;
	}
	bt_gatt_server_set_read_value_requested_cb(lti_characteristic, lti_read_value_requested_cb, NULL);
	
//end of Local time information characteristic


//begining of Reference time information characteristic
/* default value Initialization */
	//source : Time source
	rti_char_value[0] = MANUAL;
	//Accuracy: can be 0 to 253, 254 = Accuracy out of range, 255 = Accuracy Unknown
	rti_char_value[1] = 0xff; //Accuracy Unknown
	//Days Since Update
	rti_char_value[2] = 0; //0 days since updated
	//Hours since Updated
	rti_char_value[3] = 0; //0 hours since updated
	ret = bt_gatt_characteristic_create(rti_char_uuid, rti_char_permissions, rti_char_properties,
												rti_char_value, sizeof(rti_char_value), &rti_characteristic);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_gatt_service_add_characteristic(cts_service, rti_characteristic);

	if (ret != BT_ERROR_NONE) {
		bt_gatt_destroy(rti_characteristic);
		goto fail;
	}
	bt_gatt_server_set_read_value_requested_cb(rti_characteristic, rti_read_value_requested_cb, NULL);
//end of Reference time information characteristic

	ret = bt_gatt_server_register_service(tip_server, cts_service);

	if (ret == BT_ERROR_NONE)
		return ret;
	
fail:
	bt_gatt_destroy(cts_service);
	return ret;
}

static int __initialize_ndcs()
{
	int ret = BT_ERROR_NONE;
	bt_gatt_h ndcs_service = NULL;
	char *ndcs_service_uuid = "1807"; // Next DST Change Service

//begining of Time with DST characteristic declaration
	bt_gatt_h twdst_characteristic = NULL;
	char *twdst_char_uuid = "2a11"; // Time with DST
	int twdst_char_permissions = BT_GATT_PERMISSION_READ;
	int twdst_char_properties = BT_GATT_PROPERTY_READ;
	char twdst_char_value[8] = {0};
/*
	char value breakup
	todo : check whether the below characteristic value follow, LSO to MSO order from spec
	Date Time:	twdst_char_value[0] to twdst_char_value[6]
				twdst_char_value[0] and twdst_char_value[1]= Year. Minimum value 1582, Maximum value 9999
				twdst_char_value[2] = Month. Minimum value 0, Maximum value 12
				twdst_char_value[3] = Day. Minimum value 1, Maximum value 31
				twdst_char_value[4] = Hours. Minimum value 0, Maximum value 23
				twdst_char_value[5] = Minutes. Minimum value 0, Maximum value 59
				twdst_char_value[6] = Seconds. Minimum value 0, Maximum value 59
				
	DST Offset:	twdst_char_value[7]
				twdst_char_value[7] = DST offset. Minimum value 0, Maximum value 8
*/
//end of Time with DST characteristic declaration

/* default value Initialization, Hard-coded. Later change this to retrieve from system information */
	//	Year 2015
	twdst_char_value[0] = 15;
	twdst_char_value[1] = 20;
	//	Month September
	twdst_char_value[2] = 9; //enumaration Declaration is not required, because it is generic and common
	//	Day 03
	twdst_char_value[3] = 3;
	//	Hours 10 
	twdst_char_value[4] = 10;
	//	Minutes 10
	twdst_char_value[5] = 10;
	//	Seconds
	twdst_char_value[6] = 10;
	//	DST Offset
	twdst_char_value[7] = DST_STANDARD_TIME;
	ret = bt_gatt_service_create(ndcs_service_uuid, BT_GATT_SERVICE_TYPE_PRIMARY, &ndcs_service);
	if (ret != BT_ERROR_NONE)
		goto fail;

//begining of Time With DST characteristic
	ret = bt_gatt_characteristic_create(twdst_char_uuid, twdst_char_permissions, twdst_char_properties,
												twdst_char_value, sizeof(twdst_char_value), &twdst_characteristic);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_gatt_service_add_characteristic(ndcs_service, twdst_characteristic);

	if (ret != BT_ERROR_NONE) {
		bt_gatt_destroy(twdst_characteristic);
		goto fail;
	}
	bt_gatt_server_set_read_value_requested_cb(twdst_characteristic, twdst_read_value_requested_cb, NULL);
//end of Time With DST characteristic

	ret = bt_gatt_server_register_service(tip_server, ndcs_service);

	if (ret == BT_ERROR_NONE)
		return ret;

fail:
	bt_gatt_destroy(ndcs_service);
	return ret;

}


static int __initialize_rtus()
{
	int ret = BT_ERROR_NONE;
	bt_gatt_h rtus_service = NULL;
	char *rtus_service_uuid = "1806"; // Reference Time Update Service
	
//begining of Time Update Control Point characteristic declaration
	bt_gatt_h tucp_characteristic = NULL;
	char *tucp_char_uuid = "2a16"; // Time Update Control Point.
	int tucp_char_permissions = BT_GATT_PERMISSION_WRITE;
	int tucp_char_properties = BT_GATT_PROPERTY_WRITE_WITHOUT_RESPONSE;
	char tucp_char_value[1] = {0};
/*
	char value breakup

	Time Update Control Point : (tucp_char_value[0])  uint8 (unsigned 8 bit integer) Minimu value 1, Maximum value 2

*/

//end of Time Update Control Point characteristic declaration

//begining of Time Update State characteristic declaration
	bt_gatt_h tus_characteristic = NULL;
	char *tus_char_uuid = "2a17"; // Time Update Control Point.
	int tus_char_permissions = BT_GATT_PERMISSION_READ;
	int tus_char_properties = BT_GATT_PROPERTY_READ;
	char tus_char_value[2] = {0};
/*
	char value breakup

	Current state : (tus_char_value[0])  uint8 (unsigned 8 bit integer) Minimu value 0, Maximum value 1

	Result : (tus_char_value[1])  uint8 (unsigned 8 bit integer) Minimu value 0, Maximum value 4
*/
//end of Time Update State characteristic declaration

	ret = bt_gatt_service_create(rtus_service_uuid, BT_GATT_SERVICE_TYPE_PRIMARY, &rtus_service);
	if (ret != BT_ERROR_NONE)
		goto fail;
	
//begining of Time With DST characteristic
/* default value Initialization, */
	tucp_char_value[0] = TUCP_RESERVED;
	ret = bt_gatt_characteristic_create(tucp_char_uuid, tucp_char_permissions, tucp_char_properties,
												tucp_char_value, sizeof(tucp_char_value), &tucp_characteristic);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_gatt_service_add_characteristic(rtus_service, tucp_characteristic);

	if (ret != BT_ERROR_NONE) {
		bt_gatt_destroy(tucp_characteristic);
		goto fail;
	}
	bt_gatt_server_set_value_changed_cb(tucp_characteristic, tucp_value_changed_cb, NULL);
//end of Time With DST characteristic

//begining of Time Update State characteristic
/* default value Initialization, */
	tus_char_value[0] = TUS_CURRENT_STATE_IDLE;
	tus_char_value[1] = TUS_RESULT_SUCCESSFULL;
	ret = bt_gatt_characteristic_create(tus_char_uuid, tus_char_permissions, tus_char_properties,
												tus_char_value, sizeof(tus_char_value), &tus_characteristic);
	if (ret != BT_ERROR_NONE)
		goto fail;

	ret = bt_gatt_service_add_characteristic(rtus_service, tus_characteristic);

	if (ret != BT_ERROR_NONE) {
		bt_gatt_destroy(tus_characteristic);
		goto fail;
	}
	bt_gatt_server_set_read_value_requested_cb(tus_characteristic, tus_read_value_requested_cb, NULL);
//end of Time Update State characteristic

	ret = bt_gatt_server_register_service(tip_server, rtus_service);

	if (ret == BT_ERROR_NONE)
		return ret;

fail:
	bt_gatt_destroy(rtus_service);
	return ret;

}


void current_time_read_value_requested_cb(char *remote_address, int request_id, bt_gatt_server_h server,
				bt_gatt_h gatt_handle, int offset, void *user_data)
{
	DBG_PRT("+");
//user_data is set to NULL while registering the callback.
//Since there should be only one instance of service, there will be only one instance of characteristic and corresponding value.
//So, check and correct the characteristic value. (value will be same for all the connected clients)

	struct timespec tp;
	struct tm tm;
	unsigned char current_time_char_value[10] = {0}; // 0 is not the proper default value. should check the Date Time + Day of Week spec
	int year =0;

	if (clock_gettime(CLOCK_REALTIME, &tp) == -1) {
		DBG_PRT("clock_gettime error");
		goto done;
	}

	if (localtime_r(&tp.tv_sec, &tm) == NULL) {
		DBG_PRT("localtime_r error");
		goto done;
	}

	DBG_PRT("tm.tm_year = %d ", tm.tm_year);
	DBG_PRT("tm.tm_mon = %d ", tm.tm_mon);
	DBG_PRT("tm.tm_mday = %d ", tm.tm_mday);
	DBG_PRT("tm.tm_hour = %d ", tm.tm_hour);
	DBG_PRT("tm.tm_min = %d ", tm.tm_min);
	DBG_PRT("tm.tm_sec = %d ", tm.tm_sec);
	DBG_PRT("tm.tm_wday = %d ", tm.tm_wday);

	year = 1900 + tm.tm_year;
	//Year 2015
	current_time_char_value[0] = ((year >> 0) & 0xff);
	current_time_char_value[1] = ((year >> 8) & 0xff);
	//Month september
	current_time_char_value[2] = tm.tm_mon;
	//Day 03
	current_time_char_value[3] = tm.tm_mday;
	//Hours 10
	current_time_char_value[4] = tm.tm_hour;
	//Minutes 10
	current_time_char_value[5] = tm.tm_min;
	//seconds 10
	current_time_char_value[6] = tm.tm_sec;
	//Day of week
	current_time_char_value[7] = tm.tm_wday;
	//Fractions256
	current_time_char_value[8] = 0;
//Adjust Reason
	current_time_char_value[9] = 0; //set to "no reason". gatt_tip_current_time_adj_reason_mask_e should be used while notifying
done:
	bt_gatt_server_send_response(request_id, offset, (char*)current_time_char_value, sizeof(current_time_char_value) - offset);
}

void lti_read_value_requested_cb(char *remote_address, int request_id, bt_gatt_server_h server,
				bt_gatt_h gatt_handle, int offset, void *user_data)
{
	DBG_PRT("+");
	char value[2] = {0, };
//user_data is set to NULL while registering the callback.
//Since there should be only one instance of service, there will be only one instance of characteristic and corresponding value.
//So, check and correct the characteristic value. (value will be same for all the connected clients)

	tzset();

	/* Convert POSIX "timezone" (seconds West of GMT) to Time Profile
	 * format (offset from UTC in number of 15 minutes increments). */
	value[0] = (signed char) (-1 * timezone / (60 * 15));

	/* FIXME: POSIX "daylight" variable only indicates whether there
	 * is DST for the local time or not. The offset is unknown. */
	value[1] = daylight ? 0xff : 0x00;

	DBG_PRT("Time Zone = %u", value[0]);
	DBG_PRT("DayLight = %d", value[1]);
	
	bt_gatt_server_send_response(request_id, offset, value, sizeof(value) - offset);
}

void rti_read_value_requested_cb(char *remote_address, int request_id, bt_gatt_server_h server,
				bt_gatt_h gatt_handle, int offset, void *user_data)
{
	DBG_PRT("+");
//user_data is set to NULL while registering the callback.
//Since there should be only one instance of service, there will be only one instance of characteristic and corresponding value.
//So, check and correct the characteristic value. (value will be same for all the connected clients)
}

void twdst_read_value_requested_cb(char *remote_address, int request_id, bt_gatt_server_h server,
				bt_gatt_h gatt_handle, int offset, void *user_data)
{
	DBG_PRT("+");
//user_data is set to NULL while registering the callback.
//Since there should be only one instance of service, there will be only one instance of characteristic and corresponding value.
//So, check and correct the characteristic value. (value will be same for all the connected clients)
	struct timespec tp;
	struct tm tm;
	char twdst_char_value[8] = {0};
	int year =0;

	if (clock_gettime(CLOCK_REALTIME, &tp) == -1) {
		DBG_PRT("clock_gettime error");
		goto done;
	}

	if (localtime_r(&tp.tv_sec, &tm) == NULL) {
		DBG_PRT("localtime_r error");
		goto done;
	}

	DBG_PRT("tm.tm_year = %d ", tm.tm_year);
	DBG_PRT("tm.tm_mon = %d ", tm.tm_mon);
	DBG_PRT("tm.tm_mday = %d ", tm.tm_mday);
	DBG_PRT("tm.tm_hour = %d ", tm.tm_hour);
	DBG_PRT("tm.tm_min = %d ", tm.tm_min);
	DBG_PRT("tm.tm_sec = %d ", tm.tm_sec);

	year = 1900 + tm.tm_year;
	//Year 2015
	twdst_char_value[0] = ((year >> 0) & 0xff);
	twdst_char_value[1] = ((year >> 8) & 0xff);
	//Month september
	twdst_char_value[2] = tm.tm_mon;
	//Day 03
	twdst_char_value[3] = tm.tm_mday;
	//Hours 10
	twdst_char_value[4] = tm.tm_hour;
	//Minutes 10
	twdst_char_value[5] = tm.tm_min;
	//seconds 10
	twdst_char_value[6] = tm.tm_sec;

	tzset();

	/* FIXME: POSIX "daylight" variable only indicates whether there
	 * is DST for the local time or not. The offset is unknown. */
	twdst_char_value[7] = daylight ? 0xff : 0x00;
done:
	bt_gatt_server_send_response(request_id, offset, (char*)twdst_char_value, sizeof(twdst_char_value) - offset);
		
}

void tus_read_value_requested_cb(char *remote_address, int request_id, bt_gatt_server_h server,
				bt_gatt_h gatt_handle, int offset, void *user_data)
{
	DBG_PRT("+");
//user_data is set to NULL while registering the callback.
//Since there should be only one instance of service, there will be only one instance of characteristic and corresponding value.
//So, check and correct the characteristic value. (value will be same for all the connected clients)
}

void tucp_value_changed_cb(char *remote_address, bt_gatt_server_h server, bt_gatt_h gatt_handle, int offset,
			char *value, int len, void *user_data)
{
	DBG_PRT("+");
//user_data is set to NULL while registering the callback.
//take action on the written value.
}
