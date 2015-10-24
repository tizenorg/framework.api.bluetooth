#include "bt_unit_test_gatt.h"

#define BT_ADDR_LENGTH 18

static char remote_addr[BT_ADDR_LENGTH] = "00:11:22:33:44:55";

typedef enum
{
	BT_GATT_TEST_UNRECOVERABLE = 1,
	BT_GATT_TEST_TIME_INITIALIZATION,
	BT_GATT_TEST_TIME_INITIALIZED,
	BT_GATT_TEST_TIME_SERVER_INITIALIZING,
	BT_GATT_TEST_TIME_SERVER_INITIALIZED,
	BT_GATT_TEST_TIME_CLIENT_SET_ADDRESS,
	BT_GATT_TEST_TIME_CLIENT_CONNECT,
	BT_GATT_TEST_TIME_CLIENT_CONNECTING,
	BT_GATT_TEST_TIME_CLIENT_BONDING,
	BT_GATT_TEST_TIME_CLIENT_BROWSE_SERVICES,
	BT_GATT_TEST_TIME_CLIENT_MONITOR_SERVICES,
	BT_GATT_TEST_TIME_CLIENT_READ_VALUES,
	BT_GATT_TEST_TIME_CLIENT_READ_ONGOING,
	BT_UNIT_TEST_TABLE_FINISH = 0xFF,
} time_test_gatt_state_e;

typedef enum
{
	DST_STANDARD_TIME = 0x00, // +0h
	DST_HALF_AN_HOUR_DAYLIGHT_TIME = 0x02, //+0.5h
	DST_DAYLIGHT_TIME = 0x04, //+1h
	DST_DOUBLE_DAYLIGHT_TIME = 0x08, //+2h
	DST_UNKNOWN = 0xff, //unknown
}gatt_tip_dst_offset_e;

typedef enum
{
	TUCP_RESERVED = 0x00,
	TUCP_GET_REFERENCE_UPDATE = 0x01,
	TUCP_CANCEL_REFERENCE_UPDATE = 0x02,
}gatt_tip_tucp_e;

typedef enum
{
	TUS_CURRENT_STATE_IDLE = 0x00,
	TUS_CURRENTSTATE_UPDATE_PENDING = 0x01,
}gatt_tip_tus_current_state_e;

typedef enum
{
	TUS_RESULT_SUCCESSFULL = 0x00,
	TUS_RESULT_CANCELLED = 0x01,
	TUS_RESULT_NO_CONNECTION_TO_REFERENCE = 0x02,
	TUS_RESULT_REFERENCE_RESP_WITH_ERROR = 0x03,
	TUS_RESULT_TIMEOUT = 0x04,
	TUS_RESULT_UPDATE_NOT_ATTEMPTED_AFTER_RESET = 0x05,
}gatt_tip_tus_result_e;

typedef enum
{
	DAY_UNKNOWN = 0x00,
	MONDAY = 0x01,
	TUESDAY = 0x02,
	WEDNESDAY = 0x03,
	THURSDAY = 0x04,
	FRIDAY = 0x05,
	SATURDAY = 0x06,
	SUNDAY = 0x07,
}gatt_tip_dow_e;

typedef enum
{
	MANUAL_TIME_UPDATE = 0x01,
	EXTERNAL_REFERENCE_TIME_UPDATE = 0x02,
	CHANGE_OF_TIME_ZONE = 0x04,
	CHANGE_OF_DST = 0x08,
}gatt_tip_current_time_adj_reason_mask_e;

typedef enum
{
	SOURCE_UNKNOWN = 0x00,
	NETWORK_TIME_PROTOCOL = 0x01,
	GPS = 0x02,
	RADIO_TIME_SINGAL = 0x03,
	MANUAL = 0x04,
	ATOMIC_CLOCK = 0x05,
	CELLULAR_NETWORK = 0x06,
}gatt_tip_time_source_e;


typedef void (*time_client_char_read_interpretter) (bt_gatt_h characteristic, char *value, int length);


typedef struct {
	bt_gatt_h char_handle;
	time_client_char_read_interpretter read_cb;
} time_client_read_list_s;

void current_time_read_value_requested_cb(char *remote_address, int request_id, bt_gatt_server_h server,
				bt_gatt_h gatt_handle, int offset, void *user_data);
void lti_read_value_requested_cb(char *remote_address, int request_id, bt_gatt_server_h server,
				bt_gatt_h gatt_handle, int offset, void *user_data);
void rti_read_value_requested_cb(char *remote_address, int request_id, bt_gatt_server_h server,
				bt_gatt_h gatt_handle, int offset, void *user_data);
void twdst_read_value_requested_cb(char *remote_address, int request_id, bt_gatt_server_h server,
				bt_gatt_h gatt_handle, int offset, void *user_data);
void tus_read_value_requested_cb(char *remote_address, int request_id, bt_gatt_server_h server,
				bt_gatt_h gatt_handle, int offset, void *user_data);

void tucp_value_changed_cb(char *remote_address, bt_gatt_server_h server, bt_gatt_h gatt_handle, int offset, char *value, int len,
				void *user_data);
