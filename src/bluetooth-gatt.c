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

#include <stdbool.h>
#include <math.h>
#include <dlog.h>
#include <bluetooth-api.h>

#include "bluetooth.h"
#include "bluetooth_private.h"
#include "bluetooth_internal.h"
#include "bluetooth_type_internal.h"

#define BT_ADDR_STR_LEN		17

static GSList *gatt_client_list = NULL;

static GSList *gatt_server_list = NULL;
static bool is_gatt_server_initialized = false;

#ifdef TIZEN_GATT_DISABLE
#define BT_CHECK_GATT_SUPPORT() \
		{ \
			BT_CHECK_BT_SUPPORT(); \
			LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
			return BT_ERROR_NOT_SUPPORTED; \
		}
#else
#define BT_CHECK_GATT_SUPPORT()
#endif

#define BT_CHECK_GATT_SERVER_INIT_STATUS() \
	if (__bt_check_gatt_server_init_status() == BT_ERROR_NOT_INITIALIZED) \
	{ \
		LOGE("[%s] NOT_INITIALIZED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_INITIALIZED); \
		return BT_ERROR_NOT_INITIALIZED; \
	}

int __bt_check_gatt_server_init_status(void)
{
	if (is_gatt_server_initialized != true) {
		BT_ERR("NOT_INITIALIZED(0x%08x)", BT_ERROR_NOT_INITIALIZED);
		return BT_ERROR_NOT_INITIALIZED;
	}

	return BT_ERROR_NONE;
}

static char *__convert_uuid_to_uuid128(const char *uuid)
{
	int len;
	char *uuid128;

	len = strlen(uuid);

	switch (len) {
	case 4:		/* UUID 16bits */
		uuid128 = g_strdup_printf("0000%s-0000-1000-8000-00805F9B34FB", uuid);
		break;

	case 8:		/* UUID 32bits */
		uuid128 = g_strdup_printf("%s-0000-1000-8000-00805F9B34FB", uuid);
		break;

	case 36:	/* UUID 128bits */
		uuid128 = strdup(uuid);
		break;

	default:
		return NULL;
	}

	return uuid128;
}

static int __get_gatt_handle_by_uuid(GSList *list, const char *uuid,
		bt_gatt_h *gatt_handle)
{
	GSList *l;
	char *uuid128_a;
	char *uuid128_b;

	uuid128_a = __convert_uuid_to_uuid128(uuid);
	if (uuid128_a == NULL) {
		BT_ERR("Wrong type of uuid : %s", uuid);
		return BT_ERROR_INVALID_PARAMETER;
	}

	for (l = list; l; l = g_slist_next(l)) {
		bt_gatt_common_s *common = (bt_gatt_common_s *)l->data;

		uuid128_b = __convert_uuid_to_uuid128(common->uuid);
		if (g_ascii_strcasecmp(uuid128_a, uuid128_b) == 0) {
			g_free(uuid128_b);
			break;
		}

		g_free(uuid128_b);
	}

	if (!l)
		return BT_ERROR_NO_DATA;

	*gatt_handle = l->data;

	return BT_ERROR_NONE;
}

const GSList* _bt_gatt_get_client_list(void)
{
	return gatt_client_list;
}

const GSList* _bt_gatt_get_server_list(void)
{
	return gatt_server_list;
}

#ifdef BT_ENABLE_LEGACY_GATT_CLIENT
bool _bt_gatt_is_legacy_client_mode(void)
{
	if (gatt_client_list) {
		BT_INFO("New GATT Client APIs");
		return false;
	}

	BT_INFO("Legacy GATT Client APIs");
	return true;
}

int bt_gatt_foreach_primary_services(const char *remote_address,
				bt_gatt_primary_service_cb callback,
				void *user_data)
{
	int i;
	int ret;
	bool foreach_call = true;
	bluetooth_device_address_t addr_hex = { {0,} };
	bt_gatt_handle_info_t *prim_svc;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(callback);

	prim_svc = g_new0(bt_gatt_handle_info_t, 1);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	ret = _bt_get_error_code(bluetooth_gatt_get_primary_services(&addr_hex,
								prim_svc));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to run function",
					_bt_convert_error_to_string(ret), ret);
		g_free(prim_svc);
		return ret;
	}

	for (i = 0; i < prim_svc->count; i++) {
		if (prim_svc->handle[i] == NULL)
			continue;

		BT_INFO("handle: %s", prim_svc->handle[i]);

		if (foreach_call == true &&
		    !callback((bt_gatt_attribute_h)prim_svc->handle[i], user_data)) {
			foreach_call = false;
		}

		/* Application should clone the handle using API in callback */
		/* bt_gatt_clone_attribute_handle */
		g_free(prim_svc->handle[i]);
	}

	g_free(prim_svc->handle);
	g_free(prim_svc);

	return ret;
}

int bt_gatt_discover_characteristics(bt_gatt_attribute_h service,
				bt_gatt_characteristics_discovered_cb callback,
				void *user_data)
{
	int ret;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(service);
	BT_CHECK_INPUT_PARAMETER(callback);

	ret = _bt_get_error_code(bluetooth_gatt_discover_service_characteristics((const char *)service));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	} else {
		_bt_set_cb(BT_EVENT_GATT_CLIENT_CHARACTERISTIC_DISCOVERED_LEGACY, callback, user_data);
	}

	return ret;
}

int bt_gatt_get_service_uuid(bt_gatt_attribute_h service, char **uuid)
{
	int i;
	int ret;
	bt_gatt_service_property_t property;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(service);

	memset(&property, 0x00, sizeof(bt_gatt_service_property_t));

	ret = _bt_get_error_code(bluetooth_gatt_get_service_property((const char *)service, &property));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	} else {
		*uuid = g_strdup(property.uuid);

		if (property.include_handles.count != 0 && property.include_handles.handle) {
			for (i = 0; i < property.include_handles.count; i++) {
				g_free(property.include_handles.handle[i]);
			}
			g_free(property.include_handles.handle);
		}
		if (property.char_handle.count != 0 && property.char_handle.handle) {
			for (i = 0; i < property.char_handle.count; i++) {
				g_free(property.char_handle.handle[i]);
			}
			g_free(property.char_handle.handle);
		}
	}

	return ret;
}

int bt_gatt_foreach_included_services(bt_gatt_attribute_h service,
				bt_gatt_included_service_cb callback,
				void *user_data)
{
	int i;
	int ret;
	bool foreach_call = true;
	bt_gatt_service_property_t property;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(service);
	BT_CHECK_INPUT_PARAMETER(callback);

	memset(&property, 0x00, sizeof(bt_gatt_service_property_t));

	ret = _bt_get_error_code(bluetooth_gatt_get_service_property((const char *)service, &property));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	} else {
		if (property.char_handle.count != 0 && property.char_handle.handle) {
			for (i = 0; i < property.char_handle.count; i++) {
				g_free(property.char_handle.handle[i]);
			}
			g_free(property.char_handle.handle);
		}
		if (property.include_handles.count == 0 ||
		     property.include_handles.handle == NULL) {
			return ret;
		}

		for (i = 0; i < property.include_handles.count; i++) {
			if (property.include_handles.handle[i] == NULL)
				continue;

			if (foreach_call == true &&
			    !callback((bt_gatt_attribute_h)property.include_handles.handle[i],
					user_data)) {
				foreach_call = false;
			}

			g_free(property.include_handles.handle[i]);
		}
		g_free(property.include_handles.handle);
	}
	return ret;
}

int bt_gatt_set_characteristic_changed_cb(bt_gatt_characteristic_changed_cb callback,
				void *user_data)
{
	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_set_cb(BT_EVENT_GATT_CLIENT_VALUE_CHANGED_LEGACY, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_gatt_unset_characteristic_changed_cb()
{
	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	_bt_unset_cb(BT_EVENT_GATT_CLIENT_VALUE_CHANGED_LEGACY);

	return BT_ERROR_NONE;
}

int bt_gatt_watch_characteristic_changes(bt_gatt_attribute_h service)
{
	int ret;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(service);

	ret = _bt_get_error_code(bluetooth_gatt_watch_characteristics((const char *)service));

	if (ret != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);

	return ret;
}

int bt_gatt_unwatch_characteristic_changes(bt_gatt_attribute_h service)
{
	int ret;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(service);

	ret = _bt_get_error_code(bluetooth_gatt_unwatch_characteristics((const char *)service));

	if (ret != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);

	return ret;
}

int bt_gatt_get_characteristic_declaration(bt_gatt_attribute_h characteristic,
				char **uuid, unsigned char **value,
				int *value_length)
{
	int ret;
	bt_gatt_char_property_t property;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(characteristic);

	memset(&property, 0x00, sizeof(bt_gatt_char_property_t));

	ret = _bt_get_error_code(bluetooth_gatt_get_characteristics_property((const char *)characteristic, &property));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	} else {
		if (property.uuid) {
			*uuid = g_strdup(property.uuid);
			g_free(property.uuid);
		}

		if (property.val && property.val_len != 0) {
			*value = g_memdup(property.val, property.val_len);
			*value_length = property.val_len;
			g_free(property.val);
		}

		g_free(property.name);

	}

	return ret;
}

int bt_gatt_set_characteristic_value(bt_gatt_attribute_h characteristic,
				const unsigned char *value,
				int value_length)
{
	int ret;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(characteristic);
	BT_CHECK_INPUT_PARAMETER(value);

	if (value_length <= 0)
		return BT_ERROR_INVALID_PARAMETER;

	ret = _bt_get_error_code(bluetooth_gatt_set_characteristics_value((const char *)characteristic,
							(const guint8 *)value, value_length));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_gatt_set_characteristic_value_request(bt_gatt_attribute_h characteristic,
				const unsigned char *value, int value_length,
				bt_gatt_characteristic_write_cb callback)
{
	int ret;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(characteristic);
	BT_CHECK_INPUT_PARAMETER(value);

	if (value_length <= 0)
		return BT_ERROR_INVALID_PARAMETER;

	ret = _bt_get_error_code(bluetooth_gatt_set_characteristics_value_request(
					(const char *)characteristic,
					(const guint8 *)value, value_length));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	} else {
		_bt_set_cb(BT_EVENT_GATT_CLIENT_WRITE_CHARACTERISTIC_LEGACY, callback, characteristic);
	}

	return ret;
}

int bt_gatt_clone_attribute_handle(bt_gatt_attribute_h *clone,
				bt_gatt_attribute_h origin)
{
	int error = BT_ERROR_NONE;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(origin);

	*clone = g_strdup((char *)origin);

	return error;
}

int bt_gatt_destroy_attribute_handle(bt_gatt_attribute_h handle)
{
	int error = BT_ERROR_NONE;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(handle);

	g_free(handle);

	return error;
}

int bt_gatt_read_characteristic_value(bt_gatt_attribute_h characteristic,
		bt_gatt_characteristic_read_cb callback)
{
	int ret;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(characteristic);
	BT_CHECK_INPUT_PARAMETER(callback);

	ret = _bt_get_error_code(bluetooth_gatt_read_characteristic_value((const char *)characteristic));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	} else {
		_bt_set_cb(BT_EVENT_GATT_CLIENT_READ_CHARACTERISTIC_LEGACY, callback, NULL);
	}

	return ret;
}

int bt_gatt_discover_characteristic_descriptor(bt_gatt_attribute_h characteristic_handle,
				bt_gatt_characteristic_descriptor_discovered_cb callback,
				void *user_data)
{
	int ret;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(characteristic_handle);
	BT_CHECK_INPUT_PARAMETER(callback);

	ret = _bt_get_error_code(bluetooth_gatt_discover_characteristic_descriptor
			((const char *)characteristic_handle));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	} else {
		_bt_set_cb(BT_EVENT_GATT_CLIENT_CHARACTERISTIC_DESCRIPTOR_DISCOVERED_LEGACY, callback, user_data);
	}

	return ret;
}
#endif

int bt_gatt_connect(const char *address, bool auto_connect)
{
	int ret;
	bluetooth_device_address_t bd_addr = { {0,} };

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	_bt_convert_address_to_hex(&bd_addr, address);

	ret = _bt_get_error_code(bluetooth_connect_le(&bd_addr, auto_connect ? TRUE : FALSE));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_gatt_disconnect(const char *address)
{
	int ret;
	bluetooth_device_address_t bd_addr = { {0,} };

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	_bt_convert_address_to_hex(&bd_addr, address);

	ret = _bt_get_error_code(bluetooth_disconnect_le(&bd_addr));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_gatt_set_connection_state_changed_cb(bt_gatt_connection_state_changed_cb callback, void *user_data)
{
	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_GATT_CONNECTION_STATUS, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_gatt_unset_connection_state_changed_cb(void)
{
	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_GATT_CONNECTION_STATUS);
	return BT_ERROR_NONE;
}

int bt_gatt_get_uuid_specification_name(const char *uuid, char **name)
{
	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(name);

	int i;
	int offset = 0;
	static struct {
		const char *uuid;
		const char *specification_name;
	} bt_uuid_name[] = {
		/* GATT Services */
		{"1800", "Generic Access"},
		{"1801", "Generic Attribute"},
		{"1802", "Immediate Alert"},
		{"1803", "Link Loss"},
		{"1804", "Tx Power"},
		{"1805", "Current Time Service"},
		{"1806", "Reference Time Update Service"},
		{"1807", "Next DST Change Service"},
		{"1808", "Glucose"},
		{"1809", "Health Thermometer"},
		{"180A", "Device Information"},
		{"180D", "Heart Rate"},
		{"180F", "Battery Service"},
		{"1810", "Blood Pressure"},
		{"1811", "Alert Notification Service"},
		{"1812", "Human Interface Device"},

		/* GATT Declarations */
		{"2800", "Primary Service Declaration"},
		{"2801", "Secondary Service Declaration"},
		{"2802", "Include Declaration"},
		{"2803", "Characteristic Declaration"},

		/* GATT Descriptors */
		{"2900", "Characteristic Extended Properties"},
		{"2901", "Characteristic User Description"},
		{"2902", "Client Characteristic Configuration"},
		{"2903", "Server Characteristic Configuration"},
		{"2904", "Characteristic Format"},
		{"2905", "Characteristic Aggregate Formate"},
		{"2906", "Valid Range"},
		{"2907", "External Report Reference"},
		{"2908", "Report Reference"},

		/* GATT Characteristics */
		{"2A00", "Device Name"},
		{"2A01", "Appearance"},
		{"2A02", "Peripheral Privacy Flag"},
		{"2A03", "Reconnection Address"},
		{"2A04", "Peripheral Preferred Connection Parameters"},
		{"2A05", "Service Changed"},
		{"2A06", "Alert Level"},
		{"2A07", "Tx Power Level"},
		{"2A08", "Date Time"},
		{"2A09", "Day of Week"},
		{"2A0A", "Day Date Time"},
		{"2A19", "Battery Level"},
		{"2A1E", "Intermediate Temperature"},
		{"2A23", "System ID"},
		{"2A24", "Model Number String"},
		{"2A25", "Serial Number String"},
		{"2A26", "Firmware Revision String"},
		{"2A27", "Hardware Revision String"},
		{"2A28", "Software Revision String"},
		{"2A29", "Manufacturer Name String"},
		{"2A2A", "IEEE 11073-20601 Regulatory Certification Data List"},
		{"2A2B", "Current Time"},
		{"2A37", "Heart Rate Measurement"},
		{"2A38", "Body Sensor Location"},
		{"2A3F", "Alert Status"},
		{"2A46", "New Alert"},
		{"2A4A", "HID Information"},
		{"2A4C", "HID Control Point"},
		{"2A50", "PnP ID"},
		{NULL, NULL}
	};

	if (strlen(uuid) >= 8)
		offset = 4;

	for (i = 0; bt_uuid_name[i].uuid; i++) {
		if (!g_ascii_strncasecmp(uuid + offset, bt_uuid_name[i].uuid, 4)) {
			*name = g_strdup(bt_uuid_name[i].specification_name);
			return BT_ERROR_NONE;
		}
	}

	*name = g_strdup("Unknown");
	return BT_ERROR_NONE;
}

static void __bt_gatt_free_descriptor(bt_gatt_h gatt_handle)
{
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s*)gatt_handle;

	g_free(desc->path);
	g_free(desc->uuid);
	g_free(desc->value);
	g_free(desc);
}

static void __bt_gatt_free_characteristic(bt_gatt_h gatt_handle)
{
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)gatt_handle;

	g_slist_free_full(chr->descriptors, __bt_gatt_free_descriptor);

	g_free(chr->path);
	g_free(chr->uuid);
	g_free(chr->value);
	g_free(chr);
}

static void __bt_gatt_free_service(bt_gatt_h gatt_handle)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_service_s *svc = (bt_gatt_service_s*)gatt_handle;

	g_slist_free_full(svc->included_services, __bt_gatt_free_service);
	g_slist_free_full(svc->characteristics, __bt_gatt_free_characteristic);

	if (svc->role == BT_GATT_ROLE_SERVER) {
		ret = _bt_get_error_code(bluetooth_gatt_unregister_service(svc->path));
		if (ret != BT_ERROR_NONE)
			BT_ERR("%s(0x%08x) : Failed to unregister service",
					_bt_convert_error_to_string(ret), ret);
	}

	g_free(svc->path);
	g_free(svc->uuid);
	g_free(svc);
}

static void __bt_gatt_destroy_descriptor(bt_gatt_h gatt_handle)
{
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s*)gatt_handle;
	bt_gatt_characteristic_s *parent_chr = (bt_gatt_characteristic_s*)desc->parent;

	if (parent_chr)
		parent_chr->descriptors = g_slist_remove(parent_chr->descriptors, desc);

	__bt_gatt_free_descriptor(gatt_handle);
}

static void __bt_gatt_destroy_characteristic(bt_gatt_h gatt_handle)
{
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)gatt_handle;
	bt_gatt_service_s *parent_svc = (bt_gatt_service_s*)chr->parent;

	if (parent_svc)
		parent_svc->characteristics = g_slist_remove(parent_svc->characteristics, chr);

	__bt_gatt_free_characteristic(gatt_handle);
}

static void __bt_gatt_destroy_service(bt_gatt_h gatt_handle)
{
	bt_gatt_service_s *svc = (bt_gatt_service_s*)gatt_handle;
	bt_gatt_server_s *server = NULL;
	bt_gatt_client_s *client = NULL;
	bt_gatt_service_s *parent_svc = NULL;

	if (svc->is_included_service == FALSE) {
		if (svc->role == BT_GATT_ROLE_SERVER) {
			server = (bt_gatt_server_s*)svc->parent;
			if (server)
				server->services = g_slist_remove(server->services, svc);
		} else {
			client = (bt_gatt_client_s*)svc->parent;
			if (client)
				client->services = g_slist_remove(client->services, svc);
		}
	} else {
		parent_svc = (bt_gatt_service_s*)svc->parent;
		if (parent_svc)
			parent_svc->included_services = g_slist_remove(parent_svc->included_services, svc);
	}

	__bt_gatt_free_service(gatt_handle);
}

static int __convert_int_to_signed_bits(int i, int size)
{
	if (i < 0)
		i = (1 << (size - 1)) + (i & ((1 << (size - 1)) - 1));

	return i;
 }

static int __convert_unsigned_to_signed(int value, int size)
{
	if ((value & (1 << (size-1))) != 0) {
		value = -1 * ((1 << (size-1)) - (value & ((1 << (size-1)) - 1)));
	}
	return value;
}

static int __convert_unsigned_byte_to_int(char b)
{
	return b & 0xFF;
}

static int __convert_unsigned_bytes_to_int16(char b0, char b1)
{
	return (__convert_unsigned_byte_to_int(b0) + (__convert_unsigned_byte_to_int(b1) << 8));
}

static int __convert_unsigned_bytes_to_int32(char b0, char b1, char b2, char b3)
{
	return (__convert_unsigned_byte_to_int(b0) + (__convert_unsigned_byte_to_int(b1) << 8))
		 + (__convert_unsigned_byte_to_int(b2) << 16) + (__convert_unsigned_byte_to_int(b3) << 24);
}

static float __convert_bytes_to_short_float(char b0, char b1)
{
	int mantissa;
	int exponent;
	float tmp;
	mantissa = __convert_unsigned_to_signed(__convert_unsigned_byte_to_int(b0)
		+ ((__convert_unsigned_byte_to_int(b1) & 0x0F) << 8), 12);
	exponent = __convert_unsigned_to_signed(__convert_unsigned_byte_to_int(b1) >> 4, 4);
	tmp = pow(10, exponent);

	return (float)(mantissa * tmp);
}

float __convert_bytes_to_float(char b0, char b1, char b2, char b3)
{
	int mantissa;
	float exponent;
	mantissa = __convert_unsigned_to_signed(__convert_unsigned_byte_to_int(b0)
	                + (__convert_unsigned_byte_to_int(b1) << 8)
	                + (__convert_unsigned_byte_to_int(b2) << 16), 24);
	exponent = pow(10, b3);

	return (float)(mantissa * exponent);
}

static int __get_data_type_float_size(bt_data_type_float_e type)
{
	switch (type) {
		case BT_DATA_TYPE_SFLOAT:
			return 2;
		case BT_DATA_TYPE_FLOAT:
			return 4;
		default:
			return 0;
	}
 }

static int __get_data_type_int_size(bt_data_type_int_e format)
{
	switch (format) {
		case BT_DATA_TYPE_SINT8:
		case BT_DATA_TYPE_UINT8:
			return 1;
		case BT_DATA_TYPE_SINT16:
		case BT_DATA_TYPE_UINT16:
			return 2;
		case BT_DATA_TYPE_SINT32:
		case BT_DATA_TYPE_UINT32:
			return 4;
		default:
			return 0;
	}
 }

int bt_gatt_destroy(bt_gatt_h gatt_handle)
{
	bt_gatt_common_s *handle = (bt_gatt_common_s*)gatt_handle;

	BT_CHECK_INPUT_PARAMETER(gatt_handle);

	if (handle->type == BT_GATT_TYPE_SERVICE)
		__bt_gatt_destroy_service(gatt_handle);
	else if (handle->type == BT_GATT_TYPE_CHARACTERISTIC)
		__bt_gatt_destroy_characteristic(gatt_handle);
	else if (handle->type == BT_GATT_TYPE_DESCRIPTOR)
		__bt_gatt_destroy_descriptor(gatt_handle);
	else {
		BT_ERR("Type is invalid(type:%d)", handle->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	return BT_ERROR_NONE;
}

int bt_gatt_get_value(bt_gatt_h gatt_handle, char **value, int *value_length)
{
	bt_gatt_common_s *handle = (bt_gatt_common_s*)gatt_handle;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)gatt_handle;
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s*)gatt_handle;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(gatt_handle);
	BT_CHECK_INPUT_PARAMETER(value);
	BT_CHECK_INPUT_PARAMETER(value_length);

	if (handle->type == BT_GATT_TYPE_CHARACTERISTIC) {
		*value_length = chr->value_length;
		if (chr->value_length > 0)
			*value = g_memdup(chr->value, chr->value_length);
	} else if (handle->type == BT_GATT_TYPE_DESCRIPTOR) {
		*value_length = desc->value_length;
		if (desc->value_length > 0)
			*value = g_memdup(desc->value, desc->value_length);
	} else {
		BT_ERR("Type is invalid(type:%d)", handle->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	return BT_ERROR_NONE;
}

int bt_gatt_get_int_value(bt_gatt_h gatt_handle, bt_data_type_int_e type, int offset, int *value)
{
	char *val;
	int val_len;
	bt_gatt_common_s *handle = (bt_gatt_common_s*)gatt_handle;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)gatt_handle;
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s*)gatt_handle;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(gatt_handle);
	BT_CHECK_INPUT_PARAMETER(value);

	if (handle->type == BT_GATT_TYPE_CHARACTERISTIC) {
		val = chr->value;
		val_len = chr->value_length;
	} else if (handle->type == BT_GATT_TYPE_DESCRIPTOR) {
		val = desc->value;
		val_len = desc->value_length;
	} else {
		BT_ERR("Type is invalid(type:%d)", handle->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	if ((offset + __get_data_type_int_size(type)) > val_len) {
		BT_ERR("Offset exceed the memory");
		return BT_ERROR_INVALID_PARAMETER;
	}

	switch (type) {
		case BT_DATA_TYPE_UINT8:
			*value = __convert_unsigned_byte_to_int(val[offset]);
			break;
		case BT_DATA_TYPE_UINT16:
			*value = __convert_unsigned_bytes_to_int16(val[offset], val[offset+1]);
			break;

		case BT_DATA_TYPE_UINT32:
			*value = __convert_unsigned_bytes_to_int32(val[offset],	val[offset+1],
									  val[offset+2], val[offset+3]);
			break;
		case BT_DATA_TYPE_SINT8:
			*value = __convert_unsigned_to_signed(__convert_unsigned_byte_to_int(val[offset]), 8);
			break;

		case BT_DATA_TYPE_SINT16:
			*value = __convert_unsigned_to_signed(__convert_unsigned_bytes_to_int16(val[offset],
													   val[offset+1]), 16);
			break;

		case BT_DATA_TYPE_SINT32:
			*value = __convert_unsigned_to_signed(__convert_unsigned_bytes_to_int32(val[offset],
					val[offset+1], val[offset+2], val[offset+3]), 32);
			break;
		default:
			return BT_ERROR_INVALID_PARAMETER;
	}

	return BT_ERROR_NONE;
}

int bt_gatt_get_float_value(bt_gatt_h gatt_handle, bt_data_type_float_e type, int offset, float *value)
{
	char *val;
	int val_len;
	bt_gatt_common_s *handle = (bt_gatt_common_s*)gatt_handle;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)gatt_handle;
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s*)gatt_handle;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(gatt_handle);
	BT_CHECK_INPUT_PARAMETER(value);

	if (handle->type == BT_GATT_TYPE_CHARACTERISTIC) {
		val = chr->value;
		val_len = chr->value_length;
	} else if (handle->type == BT_GATT_TYPE_DESCRIPTOR) {
		val = desc->value;
		val_len = desc->value_length;
	} else {
		BT_ERR("Type is invalid(type:%d)", handle->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	if ((offset + __get_data_type_float_size(type)) > val_len) {
		BT_ERR("Offset exceed the memory");
		return BT_ERROR_INVALID_PARAMETER;
	}

	switch (type) {
		case BT_DATA_TYPE_SFLOAT:
			*value = __convert_bytes_to_short_float(val[offset], val[offset+1]);
			break;

		case BT_DATA_TYPE_FLOAT:
			*value = __convert_bytes_to_float(val[offset], val[offset+1],
				val[offset+2], val[offset+3]);
			break;
		default:
			return BT_ERROR_INVALID_PARAMETER;
	}

	return BT_ERROR_NONE;
}

int bt_gatt_set_value(bt_gatt_h gatt_handle, const char *value, int value_length)
{
	int ret;
	char **val = NULL;
	int *val_len = NULL;
	bt_gatt_common_s *handle = (bt_gatt_common_s*)gatt_handle;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)gatt_handle;
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s*)gatt_handle;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(gatt_handle);
	BT_CHECK_INPUT_PARAMETER(value);

	if (handle->type == BT_GATT_TYPE_CHARACTERISTIC) {
		val = &chr->value;
		val_len = &chr->value_length;
	} else if (handle->type == BT_GATT_TYPE_DESCRIPTOR) {
		val = &desc->value;
		val_len = &desc->value_length;
	} else {
		BT_ERR("Type is invalid(type:%d)", handle->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	if (handle->role == BT_GATT_ROLE_SERVER && handle->path) {
		ret = _bt_get_error_code(bluetooth_gatt_update_characteristic(handle->path, value, value_length));
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			return ret;
		}
	}

	g_free(*val);
	*val = NULL;
	if (value_length > 0)
		*val = g_memdup(value, value_length);
	*val_len = value_length;

	return BT_ERROR_NONE;
}

int bt_gatt_set_int_value(bt_gatt_h gatt_handle, bt_data_type_int_e type, int value, int offset)
{
	int ret;
	char **val = NULL;
	int *val_len = NULL;
	char *tmp;
	int fmt_size;
	int idx = offset;
	bt_gatt_common_s *handle = (bt_gatt_common_s*)gatt_handle;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)gatt_handle;
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s*)gatt_handle;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(gatt_handle);

	if (handle->type == BT_GATT_TYPE_CHARACTERISTIC) {
		val = &chr->value;
		val_len = &chr->value_length;
	} else if (handle->type == BT_GATT_TYPE_DESCRIPTOR) {
		val = &desc->value;
		val_len = &desc->value_length;
	} else {
		BT_ERR("Type is invalid(type:%d)", handle->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	if (*val_len < offset) {
		BT_ERR("Offset is invalid");
		return BT_ERROR_INVALID_PARAMETER;
	}

	fmt_size = __get_data_type_int_size(type);
	if (*val == NULL) {
		*val = g_malloc0(fmt_size);
		*val_len = fmt_size;
	} else if (*val_len == offset) {// Added
		tmp = g_malloc0(*val_len + fmt_size);
		/* Fix : NULL_RETURNS */
		if (!tmp) {
			g_free(*val);
			return BT_ERROR_OUT_OF_MEMORY;
		}
		memcpy(tmp, *val, *val_len);
		g_free(*val);
		*val = tmp;
		*val_len += fmt_size;
	} else if (*val_len < offset + fmt_size) {// Overlapped
		tmp = g_malloc0(offset + fmt_size);
		/* Fix : NULL_RETURNS */
		if (!tmp) {
			g_free(*val);
			return BT_ERROR_OUT_OF_MEMORY;
		}
		memcpy(tmp, *val, *val_len);
		g_free(*val);
		*val = tmp;
		*val_len = offset + fmt_size;
	}

	switch (type) {
		case BT_DATA_TYPE_SINT8:
			value = __convert_int_to_signed_bits(value, 8);
		case BT_DATA_TYPE_UINT8:
			if (*val)
				(*val)[idx] = (char)(value & 0xFF);
			break;

		case BT_DATA_TYPE_SINT16:
			value = __convert_int_to_signed_bits(value, 16);
		case BT_DATA_TYPE_UINT16:
			if (*val) {
				 (*val)[idx++] = (char)(value & 0xFF);
				 (*val)[idx] = (char)((value >> 8) & 0xFF);
			}
			break;

		case BT_DATA_TYPE_SINT32:
			value = __convert_int_to_signed_bits(value, 32);
		case BT_DATA_TYPE_UINT32:
			if (*val) {
				(*val)[idx++] = (char)(value & 0xFF);
				(*val)[idx++] = (char)((value >> 8) & 0xFF);
				(*val)[idx++] = (char)((value >> 16) & 0xFF);
				(*val)[idx] = (char)((value >> 24) & 0xFF);
			}
			break;

		default:
			return BT_ERROR_INVALID_PARAMETER;
	}

	if (handle->role == BT_GATT_ROLE_SERVER && handle->path) {
		ret = _bt_get_error_code(bluetooth_gatt_update_characteristic(handle->path, *val, *val_len));
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			return ret;
		}
	}

	return BT_ERROR_NONE;
}

int bt_gatt_set_float_value(bt_gatt_h gatt_handle, bt_data_type_float_e type,
			int mantissa,  int exponent, int offset)
{
	int ret;
	char **val = NULL;
	int *val_len = NULL;
	int mant;
	int exp;
	char *tmp;
	int fmt_size;
	int idx = offset;
	bt_gatt_common_s *handle = (bt_gatt_common_s*)gatt_handle;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)gatt_handle;
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s*)gatt_handle;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(gatt_handle);

	if (handle->type == BT_GATT_TYPE_CHARACTERISTIC) {
		val = &chr->value;
		val_len = &chr->value_length;
	} else if (handle->type == BT_GATT_TYPE_DESCRIPTOR) {
		val = &desc->value;
		val_len = &desc->value_length;
	} else {
		BT_ERR("Type is invalid(type:%d)", handle->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	if (*val_len < offset) {
		BT_ERR("Offset is invalid");
		return BT_ERROR_INVALID_PARAMETER;
	}

	fmt_size = __get_data_type_float_size(type);
	if (fmt_size == 0) {
		BT_ERR("Format is invalid");
		return BT_ERROR_INVALID_PARAMETER;
	}

	if (*val == NULL) {
		*val = g_malloc0(fmt_size);
		/* Fix : NULL_RETURNS */
		if (*val == NULL)
			return BT_ERROR_OUT_OF_MEMORY;
		*val_len = fmt_size;
	} else if (*val_len == offset) {// Added
		tmp = g_malloc0(*val_len + fmt_size);
		/* Fix : NULL_RETURNS */
		if (tmp == NULL) {
			g_free(*val);
			return BT_ERROR_OUT_OF_MEMORY;
		}
		memcpy(tmp, *val, *val_len);
		g_free(*val);
		*val = tmp;
		*val_len += fmt_size;
	} else if (*val_len < offset + fmt_size) {// Overlapped
		tmp = g_malloc0(offset + fmt_size);
		/* Fix : NULL_RETURNS */
		if (tmp == NULL) {
			g_free(*val);
			return BT_ERROR_OUT_OF_MEMORY;
		}
		memcpy(tmp, *val, *val_len);
		g_free(*val);
		*val = tmp;
		*val_len = offset + fmt_size;
	}

	switch (type) {
		case BT_DATA_TYPE_FLOAT:
			mant = __convert_int_to_signed_bits(mantissa, 24);
			exp = __convert_int_to_signed_bits(exponent, 8);
			(*val)[idx++] = (char)(mant & 0xFF);
			(*val)[idx++] = (char)((mant >> 8) & 0xFF);
			(*val)[idx++] = (char)((mant >> 16) & 0xFF);
			(*val)[idx] = (char)(exp & 0xFF);
			break;

		case BT_DATA_TYPE_SFLOAT:
			mant = __convert_int_to_signed_bits(mantissa, 12);
			exp = __convert_int_to_signed_bits(exponent, 4);
			(*val)[idx++] = (char)(mant & 0xFF);
			(*val)[idx] = (char)((mant >> 8) & 0x0F);
			(*val)[idx] += (char)((exp & 0x0F) << 4);
			break;

		default:
			return BT_ERROR_INVALID_PARAMETER;
	}

	if (handle->role == BT_GATT_ROLE_SERVER && handle->path) {
		ret = _bt_get_error_code(bluetooth_gatt_update_characteristic(handle->path, *val, *val_len));
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			return ret;
		}
	}

	return BT_ERROR_NONE;
}

int bt_gatt_get_permissions(bt_gatt_h gatt_handle, int *permissions)
{
	bt_gatt_common_s *handle = (bt_gatt_common_s*)gatt_handle;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)gatt_handle;
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s*)gatt_handle;

	BT_CHECK_INPUT_PARAMETER(gatt_handle);
	BT_CHECK_INPUT_PARAMETER(permissions);

	if (handle->type == BT_GATT_TYPE_CHARACTERISTIC)
		*permissions = chr->permissions;
	else if (handle->type == BT_GATT_TYPE_DESCRIPTOR)
		*permissions = desc->permissions;
	else {
		BT_ERR("Type is invalid(type:%d)", handle->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	return BT_ERROR_NONE;
}

int bt_gatt_set_permissions(bt_gatt_h gatt_handle, int permissions)
{
	bt_gatt_common_s *handle = (bt_gatt_common_s*)gatt_handle;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)gatt_handle;
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s*)gatt_handle;

	BT_CHECK_INPUT_PARAMETER(gatt_handle);

	if (handle->type == BT_GATT_TYPE_CHARACTERISTIC)
		chr->permissions = permissions;
	else if (handle->type == BT_GATT_TYPE_DESCRIPTOR)
		desc->permissions = permissions;
	else {
		BT_ERR("Type is invalid(type:%d)", handle->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	return BT_ERROR_NONE;
}

int bt_gatt_get_uuid(bt_gatt_h gatt_handle, char **uuid)
{
	bt_gatt_common_s *handle = (bt_gatt_common_s*)gatt_handle;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(gatt_handle);
	BT_CHECK_INPUT_PARAMETER(uuid);

	*uuid = g_strdup(handle->uuid);

	return BT_ERROR_NONE;
}

int bt_gatt_get_type(bt_gatt_h gatt_handle, bt_gatt_type_e *gatt_type)
{
	bt_gatt_common_s *handle = (bt_gatt_common_s*)gatt_handle;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(gatt_handle);
	BT_CHECK_INPUT_PARAMETER(gatt_type);

	*gatt_type = handle->type;

	return BT_ERROR_NONE;
}

int bt_gatt_service_create(const char *uuid, bt_gatt_service_type_e type,
			   bt_gatt_h *service)
{
	bt_gatt_service_s *svc;

	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(service);

	svc = (bt_gatt_service_s*)g_malloc0(sizeof(bt_gatt_service_s));
	if (svc == NULL)
		return BT_ERROR_OUT_OF_MEMORY;
	svc->type = BT_GATT_TYPE_SERVICE;
	svc->role = BT_GATT_ROLE_SERVER;
	if (strlen(uuid) == 8)
		svc->uuid = __convert_uuid_to_uuid128(uuid);
	else
		svc->uuid = strdup(uuid);
	if (svc->uuid == NULL) {
		g_free(svc);
		return BT_ERROR_OUT_OF_MEMORY;
	}
	svc->is_included_service = false;
	svc->service_type = type;

	*service = (bt_gatt_h)svc;

	return BT_ERROR_NONE;
}

int bt_gatt_service_add_characteristic(bt_gatt_h service,
					bt_gatt_h characteristic)
{
	bt_gatt_service_s *svc = (bt_gatt_service_s*)service;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)characteristic;

	BT_CHECK_INPUT_PARAMETER(service);
	BT_CHECK_INPUT_PARAMETER(characteristic);
	if (chr->parent) {
		BT_ERR("This characteristic is already added.");
		return BT_ERROR_INVALID_PARAMETER;
	}

	svc->characteristics = g_slist_append(svc->characteristics, chr);
	chr->parent = (void *)service;

	return BT_ERROR_NONE;
}

int bt_gatt_service_add_included_service(bt_gatt_h service,
					bt_gatt_h included_service)
{
	bt_gatt_service_s *svc = (bt_gatt_service_s*)service;
	bt_gatt_service_s *included_svc = (bt_gatt_service_s*)included_service;

	BT_CHECK_INPUT_PARAMETER(service);
	BT_CHECK_INPUT_PARAMETER(included_service);
	if (included_svc->parent) {
		BT_ERR("This service is already added.");
		return BT_ERROR_INVALID_PARAMETER;
	}

	svc->included_services = g_slist_append(svc->included_services, included_svc);
	included_svc->parent = (void *)service;

	return BT_ERROR_NONE;
}

int bt_gatt_service_get_server(bt_gatt_h service, bt_gatt_server_h *server)
{
	bt_gatt_service_s *svc = (bt_gatt_service_s *)service;

	BT_CHECK_INPUT_PARAMETER(service);
	BT_CHECK_INPUT_PARAMETER(server);

	if (svc->is_included_service) {
		BT_ERR("This is included service of %p", svc->parent);
		return BT_ERROR_INVALID_PARAMETER;
	}

	if (svc->role != BT_GATT_ROLE_SERVER) {
		BT_ERR("This is not server's service");
		return BT_ERROR_INVALID_PARAMETER;
	}

	*server = (bt_gatt_server_h)svc->parent;

	return BT_ERROR_NONE;
}

int bt_gatt_service_get_client(bt_gatt_h service, bt_gatt_client_h *client)
{
	bt_gatt_service_s *svc = (bt_gatt_service_s *)service;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(service);
	BT_CHECK_INPUT_PARAMETER(client);

	if (svc->is_included_service) {
		BT_ERR("This is included service of %p", svc->parent);
		return BT_ERROR_INVALID_PARAMETER;
	}

	if (svc->role != BT_GATT_ROLE_CLIENT) {
		BT_ERR("This is not client's service");
		return BT_ERROR_INVALID_PARAMETER;
	}

	*client = (bt_gatt_client_h)svc->parent;

	return BT_ERROR_NONE;
}

int bt_gatt_service_get_characteristic(bt_gatt_h service, const char *uuid,
					bt_gatt_h *characteristic)
{
	bt_gatt_service_s *svc = (bt_gatt_service_s*)service;
	bt_gatt_h gatt_handle = NULL;
	int ret;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(service);
	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(characteristic);

	ret = __get_gatt_handle_by_uuid(svc->characteristics, uuid, &gatt_handle);
	if (ret == BT_ERROR_NONE && gatt_handle != NULL) {
		*characteristic = gatt_handle;
		return BT_ERROR_NONE;
	}

	BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	return ret;
}

int bt_gatt_service_foreach_characteristics(bt_gatt_h service,
					bt_gatt_foreach_cb callback, void *user_data)
{
	bt_gatt_service_s *svc = (bt_gatt_service_s*)service;
	bt_gatt_characteristic_s *chr = NULL;
	GSList *l = NULL;
	int total = 0;
	int index = 1;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(service);
	BT_CHECK_INPUT_PARAMETER(callback);

	total = g_slist_length(svc->characteristics);

	for (l = svc->characteristics; l; l = g_slist_next(l)) {
		chr = l->data;
		if (!callback(total, index++, (bt_gatt_h)chr, user_data))
			break;
	}

	return BT_ERROR_NONE;
}

int bt_gatt_service_get_included_service(bt_gatt_h service, const char *uuid,
						bt_gatt_h *included_service)
{
	bt_gatt_service_s *svc = (bt_gatt_service_s*)service;
	bt_gatt_h gatt_handle = NULL;
	int ret;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(service);
	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(included_service);

	ret = __get_gatt_handle_by_uuid(svc->included_services, uuid, &gatt_handle);
	if (ret == BT_ERROR_NONE && gatt_handle != NULL) {
		*included_service = gatt_handle;
		return BT_ERROR_NONE;
	}

	BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	return ret;
}

int bt_gatt_service_foreach_included_services(bt_gatt_h service,
					bt_gatt_foreach_cb callback, void *user_data)
{
	bt_gatt_service_s *svc = (bt_gatt_service_s*)service;
	bt_gatt_service_s *included_svc = NULL;
	GSList *l = NULL;
	int total = 0;
	int index = 1;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(service);
	BT_CHECK_INPUT_PARAMETER(callback);

	total = g_slist_length(svc->included_services);

	for (l = svc->included_services; l; l = g_slist_next(l)) {
		included_svc = l->data;
		if (!callback(total, index++, (bt_gatt_h)included_svc, user_data))
			break;
	}

	return BT_ERROR_NONE;
}

int bt_gatt_characteristic_create(const char *uuid, int permissions,
				int properties, const char *value, int value_length,
				bt_gatt_h *characteristic)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_characteristic_s *chr = NULL;

	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(characteristic);
	if (value_length > 0)
		BT_CHECK_INPUT_PARAMETER(value);

	chr = malloc(sizeof(*chr));
	if (chr == NULL)
		return BT_ERROR_OUT_OF_MEMORY;

	memset(chr, 0x00, sizeof(*chr));

	chr->type = BT_GATT_TYPE_CHARACTERISTIC;
	chr->role = BT_GATT_ROLE_SERVER;
	if (strlen(uuid) == 8)
		chr->uuid = __convert_uuid_to_uuid128(uuid);
	else
		chr->uuid = g_strdup(uuid);
	if (chr->uuid == NULL) {
		ret = BT_ERROR_OUT_OF_MEMORY;
		goto fail;
	}
	chr->permissions = permissions;
	chr->properties = properties;
	chr->write_type = BT_GATT_WRITE_TYPE_WRITE;
	chr->value_length = value_length;
	if (value_length > 0) {
		chr->value = g_memdup(value, value_length);
		if (chr->value == NULL) {
			ret = BT_ERROR_OUT_OF_MEMORY;
			goto fail;
		}
	}

	*characteristic = (bt_gatt_h)chr;

	return BT_ERROR_NONE;

fail:
	g_free(chr->value);
	g_free(chr->uuid);
	g_free(chr);
	BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	return ret;
}

int bt_gatt_characteristic_add_descriptor(bt_gatt_h characteristic,
					  bt_gatt_h descriptor)
{
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)characteristic;
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s *)descriptor;

	BT_CHECK_INPUT_PARAMETER(characteristic);
	BT_CHECK_INPUT_PARAMETER(descriptor);

	if (chr->type != BT_GATT_TYPE_CHARACTERISTIC ||
	    desc->type != BT_GATT_TYPE_DESCRIPTOR) {
		BT_ERR("Wrong type. chr : %d, desc : %d", chr->type, desc->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	if (desc->parent) {
		BT_ERR("Already added descriptor : %p", desc->parent);
		return BT_ERROR_INVALID_PARAMETER;
	}

	chr->descriptors = g_slist_append(chr->descriptors, desc);
	desc->parent = (void *)characteristic;

	return BT_ERROR_NONE;
}

int bt_gatt_characteristic_get_service(bt_gatt_h characteristic, bt_gatt_h *service)
{
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)characteristic;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(characteristic);
	BT_CHECK_INPUT_PARAMETER(service);

	*service = (bt_gatt_h)chr->parent;

	return BT_ERROR_NONE;
}

int bt_gatt_characteristic_get_properties(bt_gatt_h characteristic, int *properties)
{
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)characteristic;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(characteristic);
	BT_CHECK_INPUT_PARAMETER(properties);

	if (chr->type != BT_GATT_TYPE_CHARACTERISTIC) {
		BT_ERR("Wrong type of GATT handle : %d", chr->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	*properties = chr->properties;

	return BT_ERROR_NONE;
}

int bt_gatt_characteristic_set_properties(bt_gatt_h characteristic, int properties)
{
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)characteristic;

	BT_CHECK_INPUT_PARAMETER(characteristic);

	if (chr->type != BT_GATT_TYPE_CHARACTERISTIC) {
		BT_ERR("Wrong type of GATT handle : %d", chr->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	chr->properties = properties;

	return BT_ERROR_NONE;
}

int bt_gatt_characteristic_get_write_type(bt_gatt_h characteristic,
					bt_gatt_write_type_e *write_type)
{
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)characteristic;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(characteristic);
	BT_CHECK_INPUT_PARAMETER(write_type);

	if (chr->type != BT_GATT_TYPE_CHARACTERISTIC) {
		BT_ERR("Wrong type of GATT handle : %d", chr->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	*write_type = chr->write_type;

	return BT_ERROR_NONE;
}

int bt_gatt_characteristic_set_write_type(bt_gatt_h characteristic,
					bt_gatt_write_type_e write_type)
{
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)characteristic;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(characteristic);

	if (chr->type != BT_GATT_TYPE_CHARACTERISTIC) {
		BT_ERR("Wrong type of GATT handle : %d", chr->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	chr->write_type = write_type;

	return BT_ERROR_NONE;
}

int bt_gatt_characteristic_get_descriptor(bt_gatt_h characteristic, const char *uuid,
						bt_gatt_h *descriptor)
{
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)characteristic;
	bt_gatt_h gatt_handle = NULL;
	int ret;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(characteristic);
	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(descriptor);

	ret = __get_gatt_handle_by_uuid(chr->descriptors, uuid, &gatt_handle);
	if (ret == BT_ERROR_NONE && gatt_handle != NULL) {
		*descriptor = gatt_handle;
		return BT_ERROR_NONE;
	}

	BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	return ret;
}

int bt_gatt_characteristic_foreach_descriptors(bt_gatt_h characteristic,
					bt_gatt_foreach_cb callback, void *user_data)
{
	GSList *l;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)characteristic;
	int total;
	int i;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(characteristic);
	BT_CHECK_INPUT_PARAMETER(callback);

	if (chr->type != BT_GATT_TYPE_CHARACTERISTIC) {
		BT_ERR("Wrong type of GATT handle : %d", chr->type);
		return BT_ERROR_INVALID_PARAMETER;
	}

	total = g_slist_length(chr->descriptors);

	i = 1;
	for (l = chr->descriptors; l; l = g_slist_next(l)) {
		if (!callback(total, i++, (bt_gatt_h)l->data, user_data))
			break;
	}

	return BT_ERROR_NONE;
}

int bt_gatt_descriptor_create(const char *uuid, int permissions,
				const char *value, int value_length,
				bt_gatt_h *descriptor)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_descriptor_s *desc = NULL;

	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(descriptor);
	if (value_length > 0)
		BT_CHECK_INPUT_PARAMETER(value);

	desc = malloc(sizeof(*desc));
	if (desc == NULL)
		return BT_ERROR_OUT_OF_MEMORY;

	memset(desc, 0x00, sizeof(*desc));

	desc->type = BT_GATT_TYPE_DESCRIPTOR;
	desc->role = BT_GATT_ROLE_SERVER;
	if (strlen(uuid) == 8)
		desc->uuid = __convert_uuid_to_uuid128(uuid);
	else
		desc->uuid = g_strdup(uuid);
	if (desc->uuid == NULL) {
		ret = BT_ERROR_OUT_OF_MEMORY;
		goto fail;
	}
	desc->permissions = permissions;
	desc->value_length = value_length;
	if (value_length > 0) {
		desc->value = g_memdup(value, value_length);
		if (desc->value == NULL) {
			ret = BT_ERROR_OUT_OF_MEMORY;
			goto fail;
		}
	}

	*descriptor = (bt_gatt_h)desc;

	return ret;

fail:
	g_free(desc->uuid);
	g_free(desc->value);
	g_free(desc);

	return ret;
}

int bt_gatt_descriptor_get_characteristic(bt_gatt_h descriptor, bt_gatt_h *characteristic)
{
	bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s *)descriptor;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(descriptor);
	BT_CHECK_INPUT_PARAMETER(characteristic);

	*characteristic = (bt_gatt_h)desc->parent;

	return BT_ERROR_NONE;
}

int bt_gatt_server_initialize(void)
{
	BT_CHECK_INIT_STATUS();

	int ret = BT_ERROR_NONE;

	if (!is_gatt_server_initialized) {
		ret = _bt_get_error_code(bluetooth_gatt_init());

		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)",
				_bt_convert_error_to_string(ret), ret);
			return BT_ERROR_OPERATION_FAILED;
		}

		is_gatt_server_initialized = true;
		return BT_ERROR_NONE;
	}

	BT_DBG("Gatt-service already initialized");

	return ret;
}

int bt_gatt_server_deinitialize(void)
{
	BT_CHECK_INIT_STATUS();

	int ret = BT_ERROR_NONE;

	if (is_gatt_server_initialized) {
		GSList *l;
		for (l = gatt_server_list; l; l = g_slist_next(l)) {
			bt_gatt_server_s *serv = l->data;
			g_slist_free_full(serv->services, __bt_gatt_free_service);
			g_free(serv);
		}
		g_slist_free(gatt_server_list);
		gatt_server_list = NULL;

		ret = _bt_get_error_code(bluetooth_gatt_deinit());

		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)",
				_bt_convert_error_to_string(ret), ret);
			return BT_ERROR_OPERATION_FAILED;
		}

		is_gatt_server_initialized = false;
		return BT_ERROR_NONE;
	}

	BT_DBG("Gatt-service is not initialized");

	return ret;
}

int bt_gatt_server_create(bt_gatt_server_h *server)
{
	bt_gatt_server_s *serv = NULL;

	BT_CHECK_INPUT_PARAMETER(server);

	serv = g_malloc0(sizeof(bt_gatt_server_s));
	if (serv == NULL)
		return BT_ERROR_OUT_OF_MEMORY;

	*server = (bt_gatt_server_h)serv;

	gatt_server_list = g_slist_append(gatt_server_list, serv);

	return BT_ERROR_NONE;
}

int bt_gatt_server_destroy(bt_gatt_server_h server)
{
	bt_gatt_server_s *serv = (bt_gatt_server_s*)server;

	BT_CHECK_INPUT_PARAMETER(server);

	g_slist_free_full(serv->services, __bt_gatt_free_service);
	gatt_server_list = g_slist_remove(gatt_server_list, serv);
	g_free(serv);

	return BT_ERROR_NONE;
}

int bt_gatt_server_set_read_value_requested_cb(bt_gatt_h gatt_handle,
				bt_gatt_server_read_value_requested_cb callback,
				void *user_data)
{
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s*)gatt_handle;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_GATT_SERVER_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(gatt_handle);
	BT_CHECK_INPUT_PARAMETER(callback);

	chr->read_requested_cb = callback;
	chr->read_requested_user_data = user_data;

	_bt_set_cb(BT_EVENT_GATT_SERVER_READ_REQUESTED, callback, user_data);

	return BT_ERROR_NONE;
}

int bt_gatt_server_register_service(bt_gatt_server_h server, bt_gatt_h service)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_server_s *serv = (bt_gatt_server_s*)server;
	bt_gatt_service_s *svc = (bt_gatt_service_s*)service;
	GSList *char_l;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_GATT_SERVER_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(server);
	BT_CHECK_INPUT_PARAMETER(service);

	if (g_slist_find(serv->services, svc)) {
		BT_ERR("Already added service.");
		return BT_ERROR_ALREADY_DONE;
	}

	ret = _bt_get_error_code(bluetooth_gatt_add_service(svc->uuid, &svc->path));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	for (char_l = svc->characteristics; char_l; char_l = g_slist_next(char_l)) {
		GSList *desc_l;
		bt_gatt_characteristic_s *chr = char_l->data;

		ret = _bt_get_error_code(bluetooth_gatt_add_new_characteristic(svc->path, chr->uuid,
									(bt_gatt_characteristic_property_t)chr->properties, &chr->path));
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			goto fail;
		}

		ret = _bt_get_error_code(bluetooth_gatt_set_characteristic_value(chr->path,
									chr->value, chr->value_length));

		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
			goto fail;
		}

		for (desc_l = chr->descriptors; desc_l; desc_l = g_slist_next(desc_l)) {
			bt_gatt_descriptor_s *desc = desc_l->data;

			ret = _bt_get_error_code(bluetooth_gatt_add_descriptor(chr->path, desc->uuid, &desc->path));

			if (ret != BT_ERROR_NONE) {
				BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
				goto fail;
			}

			ret = _bt_get_error_code(bluetooth_gatt_set_descriptor_value(desc->path, desc->value, desc->value_length));

			if (ret != BT_ERROR_NONE) {
				BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
				goto fail;
			}
		}
	}

	ret = _bt_get_error_code(bluetooth_gatt_register_service(svc->path));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		goto fail;
	}

	serv->services = g_slist_append(serv->services, svc);
	svc->parent = (void *)server;

	return ret;

fail:
	bluetooth_gatt_unregister_service(svc->path);

	return ret;
}

int bt_gatt_server_unregister_service(bt_gatt_server_h server, bt_gatt_h service)
{
	bt_gatt_server_s *serv = (bt_gatt_server_s*)server;
	bt_gatt_service_s *svc = (bt_gatt_service_s*)service;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_GATT_SERVER_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(server);
	BT_CHECK_INPUT_PARAMETER(service);

	if (!g_slist_find(serv->services, svc)) {
		BT_ERR("Service is NOT found.");
		return BT_ERROR_SERVICE_NOT_FOUND;
	}

	__bt_gatt_destroy_service(svc);

	return BT_ERROR_NONE;
}

int bt_gatt_server_unregister_all_services(bt_gatt_server_h server)
{
	bt_gatt_server_s *serv = (bt_gatt_server_s*)server;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_GATT_SERVER_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(server);

	g_slist_free_full(serv->services, __bt_gatt_free_service);
	serv->services = NULL;

	return BT_ERROR_NONE;
}

int bt_gatt_server_send_response(int request_id,
		int offset, char *value, int value_length)
{
	int ret = BT_ERROR_NONE;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(value);

	if (value_length <= 0)
		return BT_ERROR_INVALID_PARAMETER;

	ret = _bt_get_error_code(bluetooth_gatt_send_response(request_id, offset, value, value_length));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_gatt_server_notify(bt_gatt_h characteristic, bool need_confirm,
		bt_gatt_server_notification_sent_cb callback, void *user_data)
{
	return BT_ERROR_NONE;
}

int bt_gatt_server_set_value_changed_cb(bt_gatt_h characteristic,
		bt_gatt_server_value_changed_cb callback,
		void *user_data)
{
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)characteristic;

	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(characteristic);
	BT_CHECK_INPUT_PARAMETER(callback);

	chr->server_value_changed_cb = callback;
	chr->server_value_changed_user_data = user_data;

	return BT_ERROR_NONE;
}

int bt_gatt_server_get_service(bt_gatt_server_h server, const char *uuid,
				bt_gatt_h *service)
{
	bt_gatt_server_s *server_s = (bt_gatt_server_s *)server;
	bt_gatt_h gatt_handle = NULL;
	int ret;

	BT_CHECK_INPUT_PARAMETER(server);
	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(service);

	ret = __get_gatt_handle_by_uuid(server_s->services, uuid, &gatt_handle);
	if (ret == BT_ERROR_NONE && gatt_handle != NULL) {
		*service = gatt_handle;
		return BT_ERROR_NONE;
	}

	BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	return ret;
}

int bt_gatt_server_foreach_services(bt_gatt_server_h server,
		bt_gatt_foreach_cb callback, void *user_data)
{
	bt_gatt_server_s *server_s = (bt_gatt_server_s *)server;
	GSList *l = NULL;
	int total = 0;
	int index = 1;

	BT_CHECK_INPUT_PARAMETER(server);
	BT_CHECK_INPUT_PARAMETER(callback);

	total = g_slist_length(server_s->services);

	for (l = server_s->services; l; l = g_slist_next(l)) {
		if (!callback(total, index++, (bt_gatt_h)l->data, user_data))
			break;
	}

	return BT_ERROR_NONE;
}

int bt_gatt_client_create(const char *remote_address, bt_gatt_client_h *client)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_client_s *client_s;
	GSList *l;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	for (l = gatt_client_list; l; l = g_slist_next(gatt_client_list)) {
		bt_gatt_client_s *c = (bt_gatt_client_s *)l->data;

		if (!g_ascii_strcasecmp(c->remote_address, remote_address)) {
			BT_ERR("Gatt client for %s is already created",
					remote_address);
			return BT_ERROR_ALREADY_DONE;
		}
	}

	client_s = g_malloc0(sizeof(*client_s));
	if (client_s == NULL) {
		ret = BT_ERROR_OUT_OF_MEMORY;
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}
	memset(client_s, 0x00, sizeof(*client_s));

	client_s->remote_address = g_strdup(remote_address);
	if (client_s->remote_address == NULL) {
		free(client_s);
		ret = BT_ERROR_OUT_OF_MEMORY;
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}
	client_s->services_discovered = false;

	*client = (bt_gatt_client_h)client_s;
	gatt_client_list = g_slist_append(gatt_client_list, client_s);


	if (_bt_gatt_client_update_all(*client) == BT_ERROR_NONE)
			client_s->services_discovered = true;

	return ret;
}

int bt_gatt_client_destroy(bt_gatt_client_h client)
{
	bt_gatt_client_s *client_s = (bt_gatt_client_s *)client;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(client);

	g_free(client_s->remote_address);
	g_slist_free_full(client_s->services, __bt_gatt_free_service);
	gatt_client_list = g_slist_remove(gatt_client_list, client_s);
	g_free(client_s);

	return BT_ERROR_NONE;
}

int bt_gatt_client_get_remote_address(bt_gatt_client_h client,
					char **remote_address)
{
	bt_gatt_client_s *client_s = (bt_gatt_client_s *)client;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(client);
	BT_CHECK_INPUT_PARAMETER(remote_address);

	*remote_address = g_strdup(client_s->remote_address);

	return BT_ERROR_NONE;
}

static bool __bt_gatt_client_is_in_progress(void)
{
	if (_bt_check_cb(BT_EVENT_GATT_CLIENT_WRITE_CHARACTERISTIC) ||
	    _bt_check_cb(BT_EVENT_GATT_CLIENT_WRITE_DESCRIPTOR) ||
	    _bt_check_cb(BT_EVENT_GATT_CLIENT_READ_CHARACTERISTIC) ||
	    _bt_check_cb(BT_EVENT_GATT_CLIENT_READ_DESCRIPTOR)) {
		BT_ERR("Operation is in progress");
		return true;
	}

	return false;
}

int bt_gatt_client_read_value(bt_gatt_h gatt_handle,
		bt_gatt_client_request_completed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_common_s *c = (bt_gatt_common_s *)gatt_handle;
	bt_gatt_client_cb_data_s *cb_data;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(gatt_handle);
	BT_CHECK_INPUT_PARAMETER(callback);

	if (__bt_gatt_client_is_in_progress()) {
		BT_ERR("Operation is in progress");
		return BT_ERROR_NOW_IN_PROGRESS;
	}

	cb_data = malloc(sizeof(bt_gatt_client_cb_data_s));
	if (cb_data == NULL) {
		BT_ERR("Cannot alloc cb_data");
		return BT_ERROR_OPERATION_FAILED;
	}

	cb_data->gatt_handle = gatt_handle;
	cb_data->user_data = user_data;

	if (c->type == BT_GATT_TYPE_CHARACTERISTIC) {
		bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)gatt_handle;

		ret = _bt_get_error_code(bluetooth_gatt_read_characteristic_value(chr->path));
		if (ret != BT_ERROR_NONE) {
			g_free(cb_data);
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		} else {
			_bt_set_cb(BT_EVENT_GATT_CLIENT_READ_CHARACTERISTIC, callback, cb_data);
		}
	} else if (c->type == BT_GATT_TYPE_DESCRIPTOR) {
		bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s *)gatt_handle;

		ret = _bt_get_error_code(bluetooth_gatt_read_descriptor_value(desc->path));
		if (ret != BT_ERROR_NONE) {
			g_free(cb_data);
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		} else {
			_bt_set_cb(BT_EVENT_GATT_CLIENT_READ_DESCRIPTOR, callback, cb_data);
		}
	} else {
		BT_ERR("Invalid handle type for read ");
		g_free(cb_data);
	}

	return ret;
}

int bt_gatt_client_write_value(bt_gatt_h gatt_handle,
		bt_gatt_client_request_completed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;
	bt_gatt_common_s *c = (bt_gatt_common_s *)gatt_handle;
	bt_gatt_client_cb_data_s *cb_data;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(gatt_handle);
	BT_CHECK_INPUT_PARAMETER(callback);

	if (__bt_gatt_client_is_in_progress()) {
		BT_ERR("Operation is in progress");
		return BT_ERROR_NOW_IN_PROGRESS;
	}

	cb_data = malloc(sizeof(bt_gatt_client_cb_data_s));
	if (cb_data == NULL) {
		BT_ERR("Cannot alloc cb_data");
		return BT_ERROR_OPERATION_FAILED;
	}

	cb_data->gatt_handle = gatt_handle;
	cb_data->user_data = user_data;

	if (c->type == BT_GATT_TYPE_CHARACTERISTIC) {
		bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)gatt_handle;

		BT_DBG("path : %s", chr->path);
		if (chr->write_type == BT_GATT_WRITE_TYPE_WRITE)
			ret = _bt_get_error_code(bluetooth_gatt_set_characteristics_value_by_type(
					chr->path, (guint8 *)chr->value, chr->value_length,
					BT_GATT_PROPERTY_WRITE));
		else if (chr->write_type == BT_GATT_WRITE_TYPE_WRITE_NO_RESPONSE)
			ret = _bt_get_error_code(bluetooth_gatt_set_characteristics_value_by_type(
					chr->path, (guint8 *)chr->value, chr->value_length,
					BT_GATT_PROPERTY_WRITE_WITHOUT_RESPONSE));
		else {
			BT_ERR("Unknow write type : %d", chr->write_type);
			ret = BT_ERROR_OPERATION_FAILED;
		}
		if (ret != BT_ERROR_NONE) {
			g_free(cb_data);
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		} else {
			_bt_set_cb(BT_EVENT_GATT_CLIENT_WRITE_CHARACTERISTIC, callback, cb_data);
		}
	} else if (c->type == BT_GATT_TYPE_DESCRIPTOR) {
		bt_gatt_descriptor_s *desc = (bt_gatt_descriptor_s *)gatt_handle;

		ret = _bt_get_error_code(bluetooth_gatt_write_descriptor_value(desc->path,
						(guint8 *)desc->value, desc->value_length));
		if (ret != BT_ERROR_NONE) {
			g_free(cb_data);
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		} else {
			_bt_set_cb(BT_EVENT_GATT_CLIENT_WRITE_DESCRIPTOR, callback, cb_data);
		}
	} else {
		BT_ERR("Invalid handle type for write ");
		g_free(cb_data);
	}

	return ret;
}

static const bt_gatt_client_h __find_gatt_client(const char *remote_address)
{
	GSList *l = NULL;

	for (l = gatt_client_list; l; l = g_slist_next(l)) {
		bt_gatt_client_s *client_s = l->data;

		if (!g_strcmp0(client_s->remote_address, remote_address)) {
			return (const bt_gatt_client_h)l->data;
		}
	}

	return NULL;
}

static gboolean __get_bdaddr_from_path(const char *path, char *addr)
{
	int i;
	char *tmp;

	/*
	 * e.g.
	 * /org/bluez/hci0/dev_E7_56_2A_AA_0C_FD/service001b
	 */
	tmp = g_strstr_len(path, strlen(path), "dev_");
	if (tmp == NULL)
		return FALSE;
	tmp += 4;

	for (i = 0; *tmp != '\0' && i < BT_ADDR_STR_LEN; i++, tmp++) {
		if (*tmp == '_')
			addr[i] = ':';
		else
			addr[i] = *tmp;
	}
	addr[i] = '\0';

	BT_DBG("path : %s, addr : %s", path, addr);

	return TRUE;
}

static void __value_changed_cb(char *char_path,
			       char *value, int value_length, void *user_data)
{
	GSList *l;
	GSList *ll;
	bt_gatt_client_h client;
	bt_gatt_client_s *client_s;
	bt_gatt_service_s *svc;
	bt_gatt_characteristic_s *chr;
	char remote_address[BT_ADDR_STR_LEN + 1] = { 0, };

	BT_DBG("%s", char_path);

	if (__get_bdaddr_from_path(char_path, remote_address) == FALSE) {
		BT_ERR("Cannot get addr from path : %s", char_path);
		return;
	}

	client = __find_gatt_client(remote_address);
	if (client == NULL) {
		BT_ERR("Cannot find client [%s]", remote_address);
		return;
	}

	client_s = (bt_gatt_client_s *)client;
	for (l = client_s->services; l; l = g_slist_next(l)) {
		svc = (bt_gatt_service_s *)l->data;
		for (ll = svc->characteristics; ll; ll = g_slist_next(ll)) {
			chr = (bt_gatt_characteristic_s *)ll->data;
			if (g_ascii_strcasecmp(chr->path, char_path) == 0) {
				if (chr->value_changed_cb)
					chr->value_changed_cb(chr, value,
						value_length,
						chr->value_changed_user_data);

				bt_gatt_set_value(chr, value, value_length);
				return;
			}
		}
	}
}

int bt_gatt_client_set_characteristic_value_changed_cb(bt_gatt_h characteristic,
		bt_gatt_client_characteristic_value_changed_cb callback,
		void *user_data)
{
	int ret;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)characteristic;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(characteristic);
	BT_CHECK_INPUT_PARAMETER(callback);

	chr->value_changed_cb = callback;
	chr->value_changed_user_data = user_data;

	if (chr->properties &
		(BT_GATT_PROPERTY_NOTIFY | BT_GATT_PROPERTY_INDICATE)) {
		ret = _bt_get_error_code(bluetooth_gatt_watch_characteristics(
					chr->path));
		if (ret != BT_ERROR_NONE)
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		else
			_bt_set_cb(BT_EVENT_GATT_CLIENT_VALUE_CHANGED,
					__value_changed_cb, NULL);
	} else {
		BT_DBG("notify or indication is not supported");
		ret = BT_ERROR_NOT_SUPPORTED;
	}

	return BT_ERROR_NONE;
}

int bt_gatt_client_unset_characteristic_value_changed_cb(bt_gatt_h characteristic)
{
	int ret;
	bt_gatt_characteristic_s *chr = (bt_gatt_characteristic_s *)characteristic;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(characteristic);

	if (chr->properties &
		(BT_GATT_PROPERTY_NOTIFY | BT_GATT_PROPERTY_INDICATE)) {
		ret = _bt_get_error_code(bluetooth_gatt_unwatch_characteristics(
					chr->path));
		if (ret != BT_ERROR_NONE)
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		else {
			chr->value_changed_cb = NULL;
			chr->value_changed_user_data = NULL;
		}
	} else {
		BT_DBG("notify or indication is not supported");
		ret = BT_ERROR_NOT_SUPPORTED;
	}

	return ret;
}

int bt_gatt_client_get_service(bt_gatt_client_h client, const char *uuid,
				bt_gatt_h *service)
{
	bt_gatt_client_s *client_s = (bt_gatt_client_s *)client;
	bt_gatt_h gatt_handle = NULL;
	int ret;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(client);
	BT_CHECK_INPUT_PARAMETER(uuid);
	BT_CHECK_INPUT_PARAMETER(service);

	ret = __get_gatt_handle_by_uuid(client_s->services, uuid, &gatt_handle);
	if (ret == BT_ERROR_NONE && gatt_handle != NULL) {
		*service = gatt_handle;
		return BT_ERROR_NONE;
	}

	BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	return ret;
}

int bt_gatt_client_foreach_services(bt_gatt_client_h client,
				    bt_gatt_foreach_cb callback, void *user_data)
{
	bt_gatt_client_s *client_s = (bt_gatt_client_s *)client;
	GSList *l = NULL;
	int total = 0;
	int index = 1;

	BT_CHECK_GATT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(client);
	BT_CHECK_INPUT_PARAMETER(callback);

	total = g_slist_length(client_s->services);

	for (l = client_s->services; l; l = g_slist_next(l)) {
		if (!callback(total, index++, (bt_gatt_h)l->data, user_data))
			break;
	}

	return BT_ERROR_NONE;
}
