#include "bt_unit_test_gatt_hr_profile.h"

extern bool gatt_debug;

static hr_test_gatt_state_e test_cursor = BT_GATT_TEST_HR_INITIALIZATION;
static bool input_valid = false;
static bt_gatt_h hr_control_point_handle = NULL;

static void __hr_client_scan_result_cb(int result,
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

void __hr_client_search_and_connect()
{
	int ret;
	ret = bt_adapter_le_start_scan(__hr_client_scan_result_cb, NULL);
	if(ret != BT_ERROR_NONE)
		test_cursor = BT_GATT_TEST_UNRECOVERABLE;
}

void __hr_client_initiate_connection()
{
	int ret;
	USR_PRT("Make sure the remote device is turned on and visible");
	USR_PRT("Please wait.. while we connect the device");

	test_cursor = BT_GATT_TEST_TABLE_CONNECTING;

	ret = bt_gatt_connect(remote_addr, false);
	if(ret != BT_ERROR_NONE)
		__hr_client_search_and_connect();
}

static void __hr_client_reset_energy_expended_cb(int result, bt_gatt_h request_handle,
		void *user_data)
{
	if (result != BT_ERROR_NONE) {
		USR_PRT("Reset of Energy expended value failed");
		DBG_PRT("result code %d", result);
		return;
	}

	USR_PRT("Reset of Energy expended value is complete");
	return;
}

void __hr_client_reset_energy_expended()
{
	if (hr_control_point_handle) {
		char reset = 0x01;
		USR_PRT("Attempting to reset the energy expended");
		bt_gatt_characteristic_set_write_type(hr_control_point_handle,
				BT_GATT_WRITE_TYPE_WRITE);
		bt_gatt_set_value(hr_control_point_handle, &reset, sizeof(reset));
		bt_gatt_client_write_value(hr_control_point_handle, __hr_client_reset_energy_expended_cb, NULL);
	} else
		USR_PRT("Heart rate control point is not supported");
}
void __hr_client_measurement_cb(bt_gatt_h chr, char *value, int len,
		void *user_data)
{
	int value_cursor = 0;
	char flags;
	DBG_PRT("len [%d]", len);
	if(len < 1) {
		DBG_PRT("invalid notification");
	}
	flags = value[0];
	value_cursor++;

	if((flags & HR_VALUE_FORMAT_MASK)) {
		USR_PRT("Heart Rate Measurement Value %x %x", value[value_cursor], value[value_cursor+1]);
		value_cursor += 2;
	}else {
		USR_PRT("Heart Rate Measurement Value %x", value[value_cursor]);
		value_cursor++;
	}

	switch(flags & HR_SENSOR_CONTACT_STATUS_BIT_MASK) {
	case 0x00:
		USR_PRT("Sensor Contact feature is not supported in the current connection");
		break;
	case 0x02:
		USR_PRT("Sensor Contact feature is not supported in the current connection");
		break;
	case 0x04:
		USR_PRT("Sensor Contact feature is supported, but contact is not detected ");
		break;
	case 0x06:
		USR_PRT("Sensor Contact feature is supported and contact is detected");
		break;
	default:
		USR_PRT("unreachable code");
	}
	if(flags & HR_ENERGY_EXPENDED_BIT_MASK) {
		USR_PRT("Energy Expended %x %x KJ", value[value_cursor], value[value_cursor + 1]);
		if((value[value_cursor] == 0xff) && (value[value_cursor + 1] == 0xff))
			__hr_client_reset_energy_expended();
		value_cursor += 2;
	}

	if(flags & HR_RR_INTERVAL_BIT_MASK) {
		while(value_cursor <= (len - 2)) {
		USR_PRT("RR Interval %x %x", value[value_cursor], value[value_cursor+1]);
		value_cursor += 2;
		}
	}

}

void __hr_client_print_sensor_loc(bt_gatt_h h)
{
	int ret;
	char *value = NULL;
	int len = 0;

	ret = bt_gatt_get_value(h, &value, &len);
	if (ret != BT_ERROR_NONE) {
		DBG_PRT("bt_gatt_get_value is failed : %d", ret);
		return;
	}

	if(len > 1) {
		DBG_PRT("invalid sensor location value");
		return;
	}

	switch(value[0]) {
	case 0:
		USR_PRT("location : other");
		break;
	case 1:
		USR_PRT("location : Chest");
		break;
	case 2:
		USR_PRT("location : Wrist");
		break;
	case 3:
		USR_PRT("location : Finger");
		break;
	case 4:
		USR_PRT("location : Hand");
		break;
	case 5:
		USR_PRT("location : Ear Lobe");
		break;
	case 6:
		USR_PRT("location : Foot");
		break;
	default:
		USR_PRT("Invalid Location value.");
	}
	g_free(value);
}

void __hr_client_body_sensor_loc_cb(int result, bt_gatt_h gatt_handle, void *data)
{

	DBG_PRT("Read %s", result == BT_ERROR_NONE ? "Success" : "Fail");

	if (result != BT_ERROR_NONE)
		return;

	__hr_client_print_sensor_loc(gatt_handle);

	return;
}
void __hr_client_dev_inf_read_complete_cb(int result, bt_gatt_h gatt_handle, void *data)
{
	int ret;
	char *str = NULL;
	char *value = NULL;
	int len = 0;
	bool hex_format = false;
	int i;

	if (result != BT_ERROR_NONE) {
		USR_PRT("error occured in reading");
		DBG_PRT("error number =%d", result);
		return;
	}

	ret = bt_gatt_get_value(gatt_handle, &value, &len);
	if (ret != BT_ERROR_NONE) {
		USR_PRT("error occured in reading");
		DBG_PRT("error number =%d", result);
		return;
	}

	for (i = 0; i < len; i++)
		if (isalnum(value[i]) == 0) {
			hex_format = true;
			break;
		}

	if (hex_format) {
		str = g_malloc0(len * 5 + 1);
			if (str) {
			for (i = 0; i < len; i++) {
				if (i > 0)
					str[i * 5 - 1] = ' ';

				snprintf(&str[i * 5], 5, "0x%02X", value[i]);
			}
		}
	} else {
		str = g_malloc0(len + 1);
		if (str)
			memcpy(str, value, len);
	}

	USR_PRT("Value : %s", str);

	g_free(str);
	g_free(value);
	return;
}

bool __hr_client_dev_inf_service_cb(int total, int index, bt_gatt_h chr_handle, void *data)
{
	char *uuid = NULL;
	char *str = NULL;

	bt_gatt_get_uuid(chr_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	if(!strcasecmp(uuid, manufacture_name_string)){
		USR_PRT("Reading Manufacturer name. Printed data is in Hexa decimal");
	} else if(!strcasecmp(uuid, model_number_string)){
		DBG_PRT("Reading Model Number. Printed data is in Hexa decimal");
	} else if(!strcasecmp(uuid, serial_number_string)){
		USR_PRT("Serial Number. Printed data is in Hexa decimaal");
	} else 	if(!strcasecmp(uuid, firmware_revision_string)){
		USR_PRT("Firmware Revision. Printed data is in Hexa decimal");
	} else if(!strcasecmp(uuid, hardware_revision_string)){
		USR_PRT("Hardware Rivision. Printed data is in Hexa decimal");
	} else if(!strcasecmp(uuid, software_revision_string)){
		USR_PRT("Software revision. Printed data is in Hexa decimaal");
	} else if(!strcasecmp(uuid, system_id)){
		USR_PRT("System id. Printed data is in Hexa decimal");
		USR_PRT("this is a 8 byte value.");
		USR_PRT("The first 40 bits is Manufacturer Identifier.");
		USR_PRT("Remaining 24 bits is Organizationally Unique Identifier.");
	} else if(!strcasecmp(uuid, regulatory_certification_data_list)){
		USR_PRT("IEEE 11073-20601 Regulatory Certification Data List");
		USR_PRT("Printed data is a opaque data structure, application to make sense");
	} else if(!strcasecmp(uuid, PnP_id)){
		USR_PRT("PnP id. Printed data is in Hexa decimal");
		USR_PRT("First byte is Vendor ID Source: 0X01 means SIG assigned, 0x02 means USB Implementer Forum");
		USR_PRT("Next 2 byte is Vendor ID");
		USR_PRT("Next 2 byte is Product ID");
		USR_PRT("Next 2 byte is Product version");
	}

	bt_gatt_client_read_value(chr_handle, __hr_client_dev_inf_read_complete_cb, NULL);

	g_free(str);
	g_free(uuid);

	return true;
}

bool __hr_client_hr_service_cb(int total, int index, bt_gatt_h chr_handle, void *data)
{
	char *uuid = NULL;
	char *str = NULL;

	bt_gatt_get_uuid(chr_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &str);

	if(!strcasecmp(uuid, heart_rate_measurement_uuid)){
		DBG_PRT("Enabling notification for %s characteristic" ,str ? str : "Unknown");
		if(bt_gatt_client_set_characteristic_value_changed_cb(chr_handle,
			__hr_client_measurement_cb, NULL) != BT_ERROR_NONE)
			DBG_PRT("registerinig Notification failed");
	} else if(!strcasecmp(uuid, body_sensor_location)){
		DBG_PRT("Reading Body sensor location");
		bt_gatt_client_read_value(chr_handle, __hr_client_body_sensor_loc_cb, NULL);
	} else if(!strcasecmp(uuid, heart_rate_control_point)){
		USR_PRT("Energy Expended feature is supported");
		if(hr_control_point_handle)
			g_free(hr_control_point_handle);
		hr_control_point_handle = g_strdup(chr_handle);
	}
	g_free(str);
	g_free(uuid);

	return true;
}

bool __hr_client_monitor_svc_cb(int total, int index, bt_gatt_h svc_handle, void *data)
{
	char *svc_str = NULL;
	char *uuid = NULL;

	bt_gatt_get_uuid(svc_handle, &uuid);
	bt_gatt_get_uuid_specification_name(uuid, &svc_str);
	DBG_PRT("%s service (uuid = %s)", svc_str ? svc_str : uuid, uuid);

	if(!strcasecmp(uuid, heart_rate_service_uuid)){
		DBG_PRT("%s service", svc_str ? svc_str : uuid);
		bt_gatt_service_foreach_characteristics(svc_handle,
				__hr_client_hr_service_cb, NULL);
	} else if(!strcasecmp(uuid, device_information_service_uuid)){
		DBG_PRT("%s service", svc_str ? svc_str : uuid);
		bt_gatt_service_foreach_characteristics(svc_handle,
				__hr_client_dev_inf_service_cb, NULL);
	}
	g_free(svc_str);
	g_free(uuid);

	return true;
}
void __hr_client_connection_state_changed_cb(int result, bool connected, const char *remote_address, void *user_data)
{
	if(!strcmp(remote_address, remote_addr)) {
		if (connected) {
			DBG_PRT("Connected to the remote device");
			test_cursor = BT_GATT_TEST_TABLE_SERVICE;
			if (gatt_client) {
				bt_gatt_client_destroy(gatt_client);
				gatt_client = NULL;
				if(hr_control_point_handle)
					g_free(hr_control_point_handle);
			}
			if(bt_gatt_client_create(remote_addr, &gatt_client) != BT_ERROR_NONE) {
				test_cursor = BT_GATT_TEST_UNRECOVERABLE;
				DBG_PRT("GATT_Client creation failed");
				return;
			}
			if(bt_gatt_client_foreach_services(gatt_client,
					__hr_client_monitor_svc_cb, NULL) != BT_ERROR_NONE) {
				test_cursor = BT_GATT_TEST_UNRECOVERABLE;
				return;
			}
			test_cursor = BT_GATT_TEST_TABLE_MONITOR_SERVICES;
		} else {
			USR_PRT("Connection to the remote device is removed");
			USR_PRT("Attempting to re-connect to the device");
			USR_PRT("Make sure the remote device is turned on and in range");
			test_cursor = BT_GATT_TEST_TABLE_CONNECTING;
			bt_gatt_connect(remote_addr, true);
		}
	}

}

int __hr_client_validate_input_bd_addr (char *bd_addr)
{
	/*have to design better check for validity check of the input*/
	if (bd_addr == NULL || strlen(bd_addr) < 16) {
		input_valid = false;
		return BT_ERROR_INVALID_PARAMETER;
	}
	input_valid = true;
	return BT_ERROR_NONE;
}

void hr_client_accept_input(char *buf)
{
	if (__hr_client_validate_input_bd_addr(buf) != BT_ERROR_NONE) {
		USR_PRT("invalid bluetooth address");
	} else {
		memcpy (remote_addr, buf, 17);
		remote_addr[17] = 0;
		if (test_cursor == BT_GATT_TEST_HR_INITIALIZED)
			__hr_client_initiate_connection();
	}

}

int hr_client_initialize(void)
{
	int ret = BT_ERROR_NONE;
	test_cursor = BT_GATT_TEST_HR_INITIALIZATION;

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

	ret = bt_gatt_set_connection_state_changed_cb(__hr_client_connection_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE)
		goto fail;

	test_cursor = BT_GATT_TEST_HR_INITIALIZED;
	return ret;
fail:
	DBG_PRT("Initiailization error returns  %d", ret);
	return ret;
}

void hr_client_signal_handler(int signno)
{
	bt_gatt_disconnect(remote_addr);
	bt_gatt_unset_connection_state_changed_cb();
}

gboolean hr_client_key_event_cb(GIOChannel *chan,
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
	} else if (test_cursor == BT_GATT_TEST_HR_INITIALIZATION) {
		if(hr_client_initialize() != BT_ERROR_NONE) {
			USR_PRT("Initialization failed please reboot");
		} else {
			DBG_PRT("Initialization is successfull");
			if(!input_valid) {
				USR_PRT("In-put remote BD-ADDR in the below format");
				USR_PRT("00:11:22:33:44:55");
				test_cursor = BT_GATT_TEST_TABLE_SET_ADDRESS;
			} else {
				__hr_client_initiate_connection();
			}
		}
	} else if (test_cursor == BT_GATT_TEST_HR_INITIALIZED) {
		if(!input_valid) {
			USR_PRT("In-put remote BD-ADDR in the below format");
			USR_PRT("00:11:22:33:44:55");
			test_cursor = BT_GATT_TEST_TABLE_SET_ADDRESS;
		} else {
			__hr_client_initiate_connection();
		}
	} else if (test_cursor == BT_GATT_TEST_TABLE_SET_ADDRESS) {
		if (__hr_client_validate_input_bd_addr(buf) != BT_ERROR_NONE) {
			USR_PRT("invalid bluetooth address");
			USR_PRT("In-put remote BD-ADDR in the below format");
			USR_PRT("00:11:22:33:44:55");
			test_cursor = BT_GATT_TEST_TABLE_SET_ADDRESS;
		} else {
			memcpy (remote_addr, buf, 17);
			remote_addr[17] = 0;
			__hr_client_initiate_connection();
		}
	} else if (test_cursor == BT_GATT_TEST_TABLE_CONNECT) {
		__hr_client_initiate_connection();
		USR_PRT("Please wait.. while we connect the device");
	} else if (test_cursor == BT_GATT_TEST_TABLE_CONNECTING) {
		USR_PRT("Please wait.. while we connect the device");
	} else if (test_cursor == BT_GATT_TEST_TABLE_SERVICE) {
		USR_PRT("Quering supported services..");
	} else if (test_cursor == BT_GATT_TEST_TABLE_MONITOR_SERVICES) {
	//	DBG_PRT("TODO");
	}

	return TRUE;
}

