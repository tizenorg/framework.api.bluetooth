
#include "bt_unit_test_gatt.h"

bt_gatt_client_h gatt_client = NULL;
char remote_addr[18] = "00:11:22:33:44:55";

/* Mandatory Services for HR profile*/

//Heart Rate service begin
char *heart_rate_service_uuid = "0000180D-0000-1000-8000-00805F9B34FB";
/*Mandatory Characteristics for this service, in HR profile*/
char *heart_rate_measurement_uuid = "00002A37-0000-1000-8000-00805F9B34FB";
char *body_sensor_location = "00002A38-0000-1000-8000-00805F9B34FB";
/*optional characteristics for this service, in HR profile */
char *heart_rate_control_point= "00002A39-0000-1000-8000-00805F9B34FB";
//Heart Rate service end

//Device Information Service begin
char *device_information_service_uuid = "0000180A-0000-1000-8000-00805F9B34FB";
/*Mandatory Characteristics for this service, in HR profile*/
char *manufacture_name_string = "00002A29-0000-1000-8000-00805F9B34FB";
/*optional characteristics for this service, in HR profile */
char *model_number_string = "00002A24-0000-1000-8000-00805F9B34FB";
char *serial_number_string = "00002A25-0000-1000-8000-00805F9B34FB";
char *firmware_revision_string = "00002A26-0000-1000-8000-00805F9B34FB";
char *hardware_revision_string = "00002A27-0000-1000-8000-00805F9B34FB";
char *software_revision_string = "00002A28-0000-1000-8000-00805F9B34FB";
char *system_id = "00002A23-0000-1000-8000-00805F9B34FB";
char *regulatory_certification_data_list = "00002A2A-0000-1000-8000-00805F9B34FB";
char *PnP_id = "00002A50-0000-1000-8000-00805F9B34FB";
//Device Information Service end

/* Mandatory Services for HR profile end */


/* Optional services for HR profile */

// there are no optional services for this profile

/* Optional services for HR profile end */


typedef enum
{
	HR_VALUE_FORMAT_MASK = 0x01,
	HR_SENSOR_CONTACT_STATUS_BIT_MASK = 0x06,
	HR_ENERGY_EXPENDED_BIT_MASK = 0x08,
	HR_RR_INTERVAL_BIT_MASK = 0x10,
}gatt_hr_measurement_property_e;


typedef enum
{
	BT_GATT_TEST_UNRECOVERABLE = 1,
	BT_GATT_TEST_INITIALIZATION,
	BT_GATT_TEST_HR_INITIALIZATION,
	BT_GATT_TEST_HR_INITIALIZED,
	BT_GATT_TEST_TABLE_SET_ADDRESS,
	BT_GATT_TEST_TABLE_CONNECT,
	BT_GATT_TEST_TABLE_CONNECTING,
	BT_GATT_TEST_TABLE_SERVICE,
	BT_GATT_TEST_TABLE_MONITOR_SERVICES,
	BT_UNIT_TEST_TABLE_FINISH = 0xFF,
} hr_test_gatt_state_e;
