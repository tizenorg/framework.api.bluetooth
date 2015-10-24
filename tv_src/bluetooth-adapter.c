/*
 * Copyright (c) 2012-2013 Samsung Electronics Co., Ltd.
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

#include <glib.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <dlog.h>
#include <stdio.h>
#include <stdbool.h>
#include <bluetooth-api.h>
#include <fcntl.h>
#include <sqlite3.h>
#include <db-util.h>

#include "bluetooth.h"
#include "bluetooth_product.h"
#include "bluetooth_private.h"

#ifdef TIZEN_LE_DISABLE
#define BT_CHECK_LE_SUPPORT() \
		{ \
			BT_CHECK_BT_SUPPORT(); \
			LOGE("[%s] NOT_SUPPORTED(0x%08x)", __FUNCTION__, BT_ERROR_NOT_SUPPORTED); \
			return BT_ERROR_NOT_SUPPORTED; \
		}
#else
#define BT_CHECK_LE_SUPPORT()
#endif

#define BT_MOBILE_HEADLESS_DB		"/opt/usr/dbspace/.bluetooth_headless_mobilelist.db"
#define BT_DB_HEADLESS_TABLE	"inbound"
#define BT_MOBILE_HEADLESS_DB_QUERY_LEN		512
#define TEXT(s, n) (const char *)sqlite3_column_text(s, n)


/*Mobile Headless Start*/
static void __delete_glistdata(gpointer data, gpointer user_data)
{
	bt_headlessdb_device_info *noti_data = data;

	if (noti_data == NULL)
		return;
	else
	{
	     free(noti_data->bdaddress);
	     free(noti_data->dev_type);
	}	
	free(noti_data);
}

void  bt_mobileheadless_close_db(void **sdb1)
{
	sqlite3 **sdb =(sqlite3 **) sdb1;
	if(*sdb)
	{
		db_util_close(*sdb);
	}
	*sdb = NULL;
}

static bool _bt_mobileheadless_check_ifaddress_present_in_db(sqlite3 *sdb,const char *device_address)
{

	BT_CHECK_INPUT_PARAMETER(sdb);
	BT_CHECK_INPUT_PARAMETER(device_address);

	int ret = 0;	
	sqlite3_stmt *stmt = NULL;

	char query[BT_MOBILE_HEADLESS_DB_QUERY_LEN] = {0, };
	snprintf(query, sizeof(query),
			"SELECT * FROM %s WHERE addr='%s';",
			BT_DB_HEADLESS_TABLE,device_address);

	ret = sqlite3_prepare(sdb, query, -1, &stmt, NULL);
	if (ret != SQLITE_OK  || stmt == NULL) {
		sqlite3_finalize(stmt);
		return false;
	}

	ret = sqlite3_step(stmt);
	if (ret == SQLITE_ROW) {
		sqlite3_finalize(stmt);
		return true;		
	}

	sqlite3_finalize(stmt);
	return false;
}

static int _bt_mobileheadless_add_db(sqlite3 *sdb, const char *remote_address,const char *dev_type)
{
	BT_DBG("+\n");

	BT_CHECK_INPUT_PARAMETER(sdb);		
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(dev_type);
	if (_bt_mobileheadless_check_ifaddress_present_in_db(sdb,remote_address))
	{
		BT_DBG("Remote Address : [%s] is already added to DB\n ", remote_address);
		return BLUETOOTH_ERROR_NONE;

	}
	
	if(bt_adapter_register_headless_device(remote_address) != BT_ERROR_NONE)
	    BT_ERR("Failed to register device to headless list");

	char *query = NULL;
	sqlite3_stmt *stmt = NULL;

	query = sqlite3_mprintf("INSERT INTO %s (id,addr,dev_type) VALUES(?, '%q', '%q');",BT_DB_HEADLESS_TABLE, remote_address,dev_type);
	if (!query)
	{
		return BLUETOOTH_ERROR_INTERNAL;
	}

	BT_DBG("query : %s", query);
	int ret = sqlite3_prepare_v2(sdb, query, strlen(query), &stmt, NULL);
	if (ret != SQLITE_OK)
		goto error;

	ret = sqlite3_step(stmt);
	if (ret != SQLITE_DONE)
		goto error;

	sqlite3_finalize(stmt);
	sqlite3_free(query);

	BT_DBG("-\n");
	return BLUETOOTH_ERROR_NONE;

error:
	BT_ERR("[ERROR] szQuery : %s", query);
	BT_ERR("[ERROR] SQL error msg : %s", sqlite3_errmsg(sdb));
	if (stmt) {
		sqlite3_finalize(stmt);
	}
	sqlite3_free(query);

	return BLUETOOTH_ERROR_INTERNAL;				
}

static GSList* _bt_mobileheadless_getall_db(sqlite3 *sdb)
{
	if (NULL == sdb)
		return NULL;
	
	GSList *slist = NULL;
	sqlite3_stmt *stmt = NULL;
	int idx = 0;

	char query[BT_MOBILE_HEADLESS_DB_QUERY_LEN] = {0, };
	snprintf(query, sizeof(query),
			"SELECT * FROM %s;",
			BT_DB_HEADLESS_TABLE);

	int ret = sqlite3_prepare(sdb, query, -1, &stmt, NULL);
	if (ret != SQLITE_OK  || stmt == NULL) {
		BT_ERR("SQL error\n");
		sqlite3_finalize(stmt);
		return NULL;
	}

	ret = sqlite3_step(stmt);

	while (ret == SQLITE_ROW) {
		bt_headlessdb_device_info* btheadless = malloc(sizeof(bt_headlessdb_device_info));
		idx = 0;
		if (btheadless)
		{
			btheadless->bdaddress = (char*)g_strdup(TEXT(stmt, idx+1));	
			btheadless->dev_type = (char*)g_strdup(TEXT(stmt, idx+2));
			slist = g_slist_append(slist, btheadless);
			ret = sqlite3_step(stmt);
		}
		else
		{
			BT_ERR("Failed to Get memory for bt_headlessdb_device_info\n");
		}
	}

	sqlite3_finalize(stmt);
	return slist;
}

static int _bt_mobileheadless_delete_db(sqlite3 *sdb,const char *remote_address)
{
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(sdb);
	
	int ret = BLUETOOTH_ERROR_NONE;
	sqlite3_stmt *stmt = NULL;
	
	if(bt_adapter_unregister_headless_device(remote_address) != BT_ERROR_NONE)
		BT_ERR("Failed to unregister device to headless list");
	
	char query[BT_MOBILE_HEADLESS_DB_QUERY_LEN] = {0, };
	snprintf(query, sizeof(query),
			"DELETE FROM %s WHERE addr='%s';",
			BT_DB_HEADLESS_TABLE, remote_address);


	BT_DBG("query : %s", query);
	
	ret = sqlite3_prepare_v2(sdb, query, strlen(query), &stmt, NULL);
	if (ret != SQLITE_OK) {
		BT_ERR("sqlite3_prepare_v2(%s) Failed(%s)", query, sqlite3_errmsg(sdb));
		return BLUETOOTH_ERROR_INTERNAL;
	}

	ret = sqlite3_step(stmt);
	if (ret != SQLITE_DONE)	{
		BT_ERR("sqlite3_step() Failed(%d)", ret);
		sqlite3_finalize(stmt);
		return BLUETOOTH_ERROR_INTERNAL;
	}

	sqlite3_finalize(stmt);

	return BLUETOOTH_ERROR_NONE;
}

static int _bt_mobileheadless_deleteall_db(sqlite3 *sdb)
{
	BT_CHECK_INPUT_PARAMETER(sdb);
	int ret = BLUETOOTH_ERROR_NONE;
	GSList *dev_list = NULL;
	dev_list = _bt_mobileheadless_getall_db(sdb);
	if (ret < BT_ERROR_NONE) 
	{
		BT_ERR("failed to get bluetooth_get_all_headless_mobile_in_DB ");
	}
	else
	{
		if (dev_list == NULL) 
		{
			BT_ERR(" BT_MOBILE_HEADLESS_DB database is Empty\n ");
		}
		else
		{
			BT_DBG("Showing all Address present in BT_MOBILE_HEADLESS_DB db\n");
			GSList* tmp = dev_list;
			while (tmp != NULL)
			{
				if(bt_adapter_unregister_headless_device((gchar*) tmp->data) != BT_ERROR_NONE)
					BT_ERR("Failed to unregister device to headless list");
				tmp = g_slist_next(tmp);
			}

			if (dev_list !=NULL)
			{
				g_slist_foreach(dev_list,(GFunc) __delete_glistdata, NULL);
				g_slist_free(dev_list);
				dev_list = NULL;
			}
			tmp = NULL;
		}
	}


	ret = BLUETOOTH_ERROR_NONE;
	
	sqlite3_stmt *stmt = NULL;

	char query[BT_MOBILE_HEADLESS_DB_QUERY_LEN] = {0, };
	snprintf(query, sizeof(query),
			"DELETE FROM %s;",
			BT_DB_HEADLESS_TABLE);


	BT_DBG("query : %s", query);
	
	ret = sqlite3_prepare_v2(sdb, query, strlen(query), &stmt, NULL);
	if (ret != SQLITE_OK) {
		BT_ERR("sqlite3_prepare_v2(%s) Failed(%s)", query, sqlite3_errmsg(sdb));
		return BLUETOOTH_ERROR_INTERNAL;
	}

	ret = sqlite3_step(stmt);
	if (ret != SQLITE_DONE)	{
		BT_ERR("sqlite3_step() Failed(%d)", ret);
		sqlite3_finalize(stmt);
		return BLUETOOTH_ERROR_INTERNAL;
	}

	sqlite3_finalize(stmt);

	return BLUETOOTH_ERROR_NONE;
}

int bt_headless_open_db(void **sdb1)
{	
	sqlite3 **sdb =(sqlite3 **) sdb1;
	if(*sdb)
	{
		return BLUETOOTH_ERROR_NONE;
	}
	
	int ret = db_util_open(BT_MOBILE_HEADLESS_DB, sdb, DB_UTIL_REGISTER_HOOK_METHOD);
	BT_DBG("\n db_util_open result for BT_MOBILE_HEADLESS_DB is [%d]\n",ret);
	
	if (ret) {
		BT_ERR("Can't open database: %s", sqlite3_errmsg(*sdb));
		db_util_close(*sdb);
		return BLUETOOTH_ERROR_INTERNAL;
	}		
	
	if (*sdb == NULL)
	{
		return BLUETOOTH_ERROR_INTERNAL;
	}
	return BLUETOOTH_ERROR_NONE;
}

int bt_bluetooth_add_headless_device_in_DB(void *sdb1,const char *remote_address,const char*dev_type)
{
	sqlite3 *sdb =(sqlite3 *) sdb1;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(dev_type);
	BT_CHECK_INPUT_PARAMETER(sdb);
		
	return (_bt_mobileheadless_add_db(sdb,remote_address,dev_type));
}

int bt_bluetooth_add_headless_mobile_in_DB(void *sdb1,const char *remote_address)
{
	sqlite3 *sdb =(sqlite3 *) sdb1;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	//BT_CHECK_INPUT_PARAMETER(dev_type);
	BT_CHECK_INPUT_PARAMETER(sdb);
		
	return (bt_bluetooth_add_headless_device_in_DB(sdb,remote_address,"Mobile"));
}

int bt_bluetooth_delete_headless_mobile_in_DB(void *sdb1,const char *remote_address)
{
	sqlite3 *sdb =(sqlite3 *) sdb1;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);
	BT_CHECK_INPUT_PARAMETER(sdb);
      return (_bt_mobileheadless_delete_db(sdb,remote_address));
}

int bt_bluetooth_get_all_headless_device_in_DB(void *sdb1,GSList **dev_list)
{
	sqlite3 *sdb =(sqlite3 *) sdb1;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(sdb);
		
	*dev_list = _bt_mobileheadless_getall_db(sdb);

	if(*dev_list == NULL)
		return BLUETOOTH_ERROR_INTERNAL;			
		
	return BLUETOOTH_ERROR_NONE;
}

int bt_bluetooth_get_all_headless_mobile_in_DB(void *sdb1,GSList **dev_list)
{
	sqlite3 *sdb =(sqlite3 *) sdb1;
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(sdb);
		
	*dev_list = NULL;//_bt_mobileheadless_getall_db(sdb);

	//if(*dev_list == NULL)
	//	return BLUETOOTH_ERROR_INTERNAL;			
		
	return BLUETOOTH_ERROR_NONE;
}

int bt_bluetooth_delete_all_headless_mobile_in_DB(void *sdb1)
{
	sqlite3 *sdb =(sqlite3 *) sdb1;
	BT_CHECK_INIT_STATUS();	
	BT_CHECK_INPUT_PARAMETER(sdb);
	
	return (_bt_mobileheadless_deleteall_db(sdb));
}
/*Headless Mobile End*/


int bt_adapter_enable(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_enable_adapter());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_disable(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_disable_adapter());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_reset(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_reset_adapter());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_get_state(bt_adapter_state_e *adapter_state)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(adapter_state);

	*adapter_state = bluetooth_check_adapter();
	return BT_ERROR_NONE;
}

int bt_adapter_get_address(char **address)
{
	bluetooth_device_address_t loc_address = { {0} };
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);

	error_code = _bt_get_error_code(bluetooth_get_local_address(&loc_address));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
		return error_code;
	}

	error_code = _bt_convert_address_to_string(address, &loc_address);
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
		return error_code;
	}

	return BT_ERROR_NONE;
}

int bt_adapter_get_name(char **name)
{
	int ret = BT_ERROR_NONE;
	bluetooth_device_name_t loc_name = { {0} };

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(name);

	ret = _bt_get_error_code(bluetooth_get_local_name(&loc_name));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	*name = strdup(loc_name.name);
	if (*name == NULL) {
		BT_ERR("OUT_OF_MEMORY(0x%08x)", BT_ERROR_OUT_OF_MEMORY);
		return BT_ERROR_OUT_OF_MEMORY;
	}

	return BT_ERROR_NONE;
}

int bt_adapter_set_name(const char *name)
{
	bluetooth_device_name_t loc_name = { {0} };
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(name);

	strncpy(loc_name.name, name, BLUETOOTH_DEVICE_NAME_LENGTH_MAX);
	loc_name.name[BLUETOOTH_DEVICE_NAME_LENGTH_MAX] = '\0';

	ret = _bt_get_error_code(bluetooth_set_local_name(&loc_name));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_get_visibility(bt_adapter_visibility_mode_e *mode, int *duration)
{
	bluetooth_discoverable_mode_t discoverable_mode = BLUETOOTH_DISCOVERABLE_MODE_CONNECTABLE;
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(mode);

	ret = _bt_get_error_code(bluetooth_get_discoverable_mode(&discoverable_mode));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	*mode = _bt_get_bt_visibility_mode_e(discoverable_mode);

	if (duration)
		*duration = 0;

	if (discoverable_mode == BLUETOOTH_DISCOVERABLE_MODE_TIME_LIMITED_DISCOVERABLE) {
		if (duration == NULL)
			return BT_ERROR_NONE;
		ret = bluetooth_get_timeout_value(duration);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		}
	}

	return BT_ERROR_NONE;
}

int bt_adapter_set_visibility(bt_adapter_visibility_mode_e visibility_mode, int timeout_sec)
{
	bluetooth_discoverable_mode_t discoverable_mode = BLUETOOTH_DISCOVERABLE_MODE_CONNECTABLE;
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	switch (visibility_mode) {
	case BT_ADAPTER_VISIBILITY_MODE_LIMITED_DISCOVERABLE:
		discoverable_mode = BLUETOOTH_DISCOVERABLE_MODE_TIME_LIMITED_DISCOVERABLE;
		break;
	case BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE:
		discoverable_mode = BLUETOOTH_DISCOVERABLE_MODE_CONNECTABLE;
		timeout_sec = 0;
		break;
	case BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE:
		discoverable_mode = BLUETOOTH_DISCOVERABLE_MODE_GENERAL_DISCOVERABLE;
		timeout_sec = 0;
		break;
	case BT_ADAPTER_VISIBILITY_MODE_NON_CONNECTABLE:
		discoverable_mode = BLUETOOTH_DISCOVERABLE_MODE_NON_CONNECTABLE;
		timeout_sec = 0;
		break;
	default:
		BT_ERR("INVALID_PARAMETER(0x%08x)", BT_ERROR_INVALID_PARAMETER);
		return BT_ERROR_INVALID_PARAMETER;
	}

	error_code = _bt_get_error_code(bluetooth_set_discoverable_mode(discoverable_mode, timeout_sec));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_foreach_bonded_device(bt_adapter_bonded_device_cb foreach_cb, void *user_data)
{
	GPtrArray *dev_list = NULL;
	bt_device_info_s *dev_info = NULL;
	bluetooth_device_info_t *ptr = NULL;
	int ret = BT_ERROR_NONE;
	unsigned int i = 0;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(foreach_cb);

	dev_list = g_ptr_array_new();
	if (dev_list == NULL) {
		BT_ERR("OUT_OF_MEMORY(0x%08x)", BT_ERROR_OUT_OF_MEMORY);
		return BT_ERROR_OUT_OF_MEMORY;
	}

	ret = _bt_get_error_code(bluetooth_get_bonded_device_list(&dev_list));
	if (ret != BT_ERROR_NONE || dev_list == NULL) {
		BT_ERR("%s(0x%08x) : Failed to get bonded device list", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	for (i = 0; i < dev_list->len; i++) {
		ptr = g_ptr_array_index(dev_list, i);
		if (ptr != NULL) {
			ret = _bt_get_bt_device_info_s(&dev_info, (bluetooth_device_info_t *)ptr);
			if (ret != BT_ERROR_NONE) {
				BT_ERR("%s(0x%08x) : Failed to get device info", _bt_convert_error_to_string(ret), ret);
				break;
			}

			if (!foreach_cb(dev_info, user_data)) {
				_bt_free_bt_device_info_s(dev_info);
				break;
			}
			_bt_free_bt_device_info_s(dev_info);
		} else {
			BT_ERR("OPERATION_FAILED(0x%08x)", BT_ERROR_OPERATION_FAILED);
			ret = BT_ERROR_OPERATION_FAILED;
			break;
		}
	}

	if (dev_list != NULL) {
		g_ptr_array_free(dev_list, TRUE);
	}

	return ret;
}

int bt_adapter_get_bonded_device_info(const char *remote_address, bt_device_info_s **device_info)
{
	int ret;
	bluetooth_device_address_t addr_hex = { {0,} };
	bluetooth_device_info_t *info;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	info = (bluetooth_device_info_t *)malloc(sizeof(bluetooth_device_info_t));
	if (info == NULL)
		return BT_ERROR_OUT_OF_MEMORY;

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	ret = _bt_get_error_code(bluetooth_get_bonded_device(&addr_hex, info));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to run function",
					_bt_convert_error_to_string(ret), ret);
	} else {
		ret = _bt_get_bt_device_info_s(device_info, info);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x) : Failed to get device info",
						_bt_convert_error_to_string(ret),
						ret);
		}
	}

	free(info);

	return ret;
}

int bt_adapter_free_device_info(bt_device_info_s *device_info)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_info);

	_bt_free_bt_device_info_s(device_info);

	return BT_ERROR_NONE;
}


int bt_adapter_get_specific_bonded_device_info(const char *remote_address, bt_hid_device_info_s **device_info)
{

	int ret;
	bluetooth_device_address_t addr_hex = { {0,} };
	bluetooth_hid_device_info_t *hid_info;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	hid_info = (bluetooth_hid_device_info_t *)malloc(sizeof(bluetooth_hid_device_info_t));
	if (hid_info == NULL)
		return BT_ERROR_OUT_OF_MEMORY;

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	ret = _bt_get_error_code(bluetooth_get_specific_bonded_device(&addr_hex, hid_info));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to run function",
					_bt_convert_error_to_string(ret), ret);
	} else {
		ret = _bt_get_specific_bt_device_info_s(device_info, hid_info);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("%s(0x%08x) : Failed to get device hid_info",
						_bt_convert_error_to_string(ret),
						ret);
		}
	}

	free(hid_info);

	return ret;
}

int bt_adapter_free_hid_device_info(bt_hid_device_info_s *device_info)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_info);
	_bt_free_specific_bt_device_info_s(device_info);

	return BT_ERROR_NONE;
}

int bt_adapter_is_service_used(const char *service_uuid, bool *used)
{
	int ret = BT_ERROR_NONE;
	gboolean is_used = FALSE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(service_uuid);
	BT_CHECK_INPUT_PARAMETER(used);

	if (strcmp(service_uuid, OPP_UUID) == 0) {
		*used = bluetooth_obex_server_is_activated();
	} else {
		ret = _bt_get_error_code(bluetooth_is_service_used(service_uuid,
									&is_used));
		*used = (is_used ? true : false);
	}

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to run function", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_state_changed_cb(bt_adapter_state_changed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_STATE_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_name_changed_cb(bt_adapter_name_changed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_NAME_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_visibility_mode_changed_cb(bt_adapter_visibility_mode_changed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_VISIBILITY_MODE_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_device_discovery_state_changed_cb(bt_adapter_device_discovery_state_changed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_start_smartrc_tput_mode(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(_bt_start_smartrc_tput_mode());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_stop_smartrc_tput_mode(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(_bt_stop_smartrc_tput_mode());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_le_set_device_discovery_state_changed_cb(bt_adapter_le_device_discovery_state_changed_cb callback, void *user_data)
{
	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);

	_bt_le_adapter_init();
	_bt_set_cb(BT_EVENT_LE_DEVICE_DISCOVERY_STATE_CHANGED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_adapter_le_unset_device_discovery_state_changed_cb(void)
{
	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_LE_DEVICE_DISCOVERY_STATE_CHANGED);
	_bt_le_adapter_deinit();
	return BT_ERROR_NONE;
}

int bt_adapter_unset_state_changed_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_STATE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_unset_name_changed_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_NAME_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_unset_visibility_mode_changed_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_VISIBILITY_MODE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_set_visibility_duration_changed_cb(bt_adapter_visibility_duration_changed_cb callback, void *user_data)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	/* Will impelement it */

	return BT_ERROR_NONE;
}

int bt_adapter_unset_visibility_duration_changed_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	/* Will impelement it */

	return BT_ERROR_NONE;
}

int bt_adapter_unset_device_discovery_state_changed_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_DEVICE_DISCOVERY_STATE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_set_max_device_connected_cb(bt_adapter_max_conection_reached_cb callback, void *user_data)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_set_cb(BT_EVENT_MAX_CONNECTION_REACHED, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_adapter_set_authentication_req_cb(bt_adapter_authentication_req_cb callback, void *user_data)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_set_cb(BT_EVENT_AUTHENTICATION_REQUEST, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_adapter_unset_authentication_req_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_AUTHENTICATION_REQUEST);
	return BT_ERROR_NONE;
}

int bt_adapter_unset_max_device_connected_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_MAX_CONNECTION_REACHED);
	return BT_ERROR_NONE;
}

int bt_adapter_set_duration(int duration)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_set_discovery_duration((unsigned short)duration));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_start_device_discovery(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_start_discovery(0, 0, BLUETOOTH_DEVICE_MAJOR_MASK_MISC));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_start_device_discovery_with_duration(int duration)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_start_discovery(0, (unsigned short)duration, BLUETOOTH_DEVICE_MAJOR_MASK_MISC));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}


int bt_adapter_stop_device_discovery(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_cancel_discovery());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_start_device_discovery_with_class_of_device(int major, int minor)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_start_discovery_with_class_of_device(major, minor));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_start_device_discovery_with_address(const char *remote_address)
{
	int error_code = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	error_code = _bt_get_error_code(bluetooth_start_discovery_with_address(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_start_device_discovery_for_3dtv(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_start_discovery_for_3dtv());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_is_discovering(bool *is_discovering)
{
	int ret = 0;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(is_discovering);

	ret = bluetooth_is_discovering();
	if (ret >= BLUETOOTH_ERROR_BASE) {
		*is_discovering = (ret == 1) ? true : false;
		return BT_ERROR_NONE;
	} else {
		ret = _bt_get_error_code(ret);
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}
}

/*LE Only API's */
int bt_adapter_le_start_device_discovery(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_start_le_discovery());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	BT_ERR("bt_adapter_le_start_device_discovery<< err= %d", error_code);
	return error_code;
}

int bt_adapter_le_stop_device_discovery(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_stop_le_discovery());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	BT_ERR("bt_adapter_le_stop_device_discovery<< err= %d", error_code);
	return error_code;
}

int bt_adapter_le_is_discovering(bool *is_discovering)
{
	int ret = 0;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(is_discovering);

	ret = bluetooth_is_le_discovering();
	if (ret >= BLUETOOTH_ERROR_BASE) {
		*is_discovering = (ret == 1) ? true : false;
		BT_ERR("bt_adapter_le_is_discovering is_discovering = %d", *is_discovering);
		return BT_ERROR_NONE;
	} else {
		ret = _bt_get_error_code(ret);
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}
}

int bt_adapter_set_device_id(unsigned short vid, unsigned short pid)
{
	unsigned short vendor_id = { 0 };
	unsigned short product_id = { 0 };
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	vendor_id = vid;
	product_id = pid;

	ret = _bt_get_error_code(bluetooth_set_device_id(vendor_id, product_id));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_adapter_block_bt_channels_for_using_wifi(unsigned char first_disabled_channel,
							unsigned char last_disabled_channel)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();

	ret = _bt_get_error_code(bluetooth_block_bt_channels_for_using_wifi(first_disabled_channel,
										last_disabled_channel));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_adapter_set_tx_power(bt_adapter_tx_pwr_class tx_power_class)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_set_tx_power(tx_power_class));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int  bt_adapter_set_headless_mode(bool enable)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_set_headless_mode(enable));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int  bt_adapter_register_headless_device(const char *remote_address)
{
	int error_code = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	error_code = _bt_get_error_code(bluetooth_register_headless_device(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int _bt_get_bt_headless_device_info_s(bt_adapter_headless_dev_info_s **dest_dev,
			bluetooth_headless_device_info_t *source_dev)
{
	char hexclass_string[9] = {0,};

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(source_dev);

	*dest_dev = (bt_adapter_headless_dev_info_s *)malloc(sizeof(bt_adapter_headless_dev_info_s));
	if (*dest_dev == NULL) {
		return BT_ERROR_OUT_OF_MEMORY;
	}

	_bt_convert_address_to_string(&((*dest_dev)->remote_address), &(source_dev->device_address));

	snprintf(hexclass_string, 9, "%2.2X:%2.2X:%2.2X",
			source_dev->hexclass[0], source_dev->hexclass[1], source_dev->hexclass[2]);

	(*dest_dev)->hexclass = strdup(hexclass_string);
	(*dest_dev)->null_pin_code = source_dev->null_pin_code;

	return BT_ERROR_NONE;
}

void _bt_free_headless_device_info_s(bt_adapter_headless_dev_info_s *device_info)
{
	BT_CHECK_BT_SUPPORT();

	if (device_info == NULL)
		return;

	if (device_info->remote_address != NULL) {
		free(device_info->remote_address);
		device_info->remote_address = NULL;
	}

	if (device_info->hexclass != NULL) {
		free(device_info->hexclass);
		device_info->hexclass = NULL;
	}

	free(device_info);
	device_info = NULL;
}

int bt_adapter_free_headless_device_info(bt_adapter_headless_dev_info_s *device_info)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(device_info);
	_bt_free_headless_device_info_s(device_info);

	return BT_ERROR_NONE;
}

int bt_adapter_foreach_headless_device_info(bt_adapter_headless_device_info_cb foreach_cb, void *user_data)
{
	GPtrArray *dev_list = NULL;
	bt_adapter_headless_dev_info_s *dev_info = NULL;
	bluetooth_headless_device_info_t *ptr = NULL;
	int ret = BT_ERROR_NONE;
	unsigned int i = 0;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(foreach_cb);

	dev_list = g_ptr_array_new();
	if (dev_list == NULL) {
		BT_ERR("OUT_OF_MEMORY(0x%08x)", BT_ERROR_OUT_OF_MEMORY);
		return BT_ERROR_OUT_OF_MEMORY;
	}
	ret = _bt_get_error_code(bluetooth_get_headless_device_list(&dev_list));
	if (ret != BT_ERROR_NONE || dev_list == NULL) {
		BT_ERR("%s(0x%08x) : Failed to get bonded device list",
				 _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	for (i = 0; i < dev_list->len; i++) {
		ptr = (bluetooth_headless_device_info_t*)g_ptr_array_index(dev_list, i);
		if (ptr != NULL) {
			ret = _bt_get_bt_headless_device_info_s(&dev_info,
						(bluetooth_headless_device_info_t*)ptr);
			if (ret != BT_ERROR_NONE) {
				BT_ERR("%s(0x%08x) : Failed to get device info",
					_bt_convert_error_to_string(ret), ret);
				break;
			}

			if (!foreach_cb(dev_info, user_data)) {
				_bt_free_headless_device_info_s(dev_info);
				break;
			}
			_bt_free_headless_device_info_s(dev_info);
		} else {
			BT_ERR("OPERATION_FAILED(0x%08x)", BT_ERROR_OPERATION_FAILED);
			ret = BT_ERROR_OPERATION_FAILED;
			break;
		}
	}

	if (dev_list != NULL) {
		g_ptr_array_free(dev_list, TRUE);
	}

	return ret;
}

int  bt_adapter_unregister_headless_device(const char *remote_address)
{
	int error_code = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(remote_address);

	_bt_convert_address_to_hex(&addr_hex, remote_address);

	error_code = _bt_get_error_code(bluetooth_unregister_headless_device(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int  bt_adapter_set_headless_scan_control(bool llr_scan)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_set_headless_scan_control(llr_scan));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int bt_adapter_sleep_bluetooth(bool standby)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	/* If standby is false, then set Normal headless Mode */
	if (!standby) {
	/* TV INstant On */
	} else {
	}
	/* Implement it in later */

	error_code = _bt_get_error_code(bluetooth_set_power_off_mode(BLUETOOTH_STANBY_OFF));

	return error_code;
}

int bt_adapter_power_off(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_set_power_off_mode(BLUETOOTH_POWER_OFF));

	return error_code;

}

int bt_adapter_set_advertising_state_changed_cb(bt_adapter_advertising_state_changed_cb callback, void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_ADVERTISING_STATE_CHANGED, callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_unset_advertising_state_changed_cb(void)
{
	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_ADVERTISING_STATE_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_start_advertising(bt_adapter_advertising_params_s *adv_params)
{
	int org_error_code = BLUETOOTH_ERROR_NONE;
	int error_code = BT_ERROR_NONE;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();

	if (adv_params == NULL) {
		org_error_code = bluetooth_set_advertising(TRUE);
	} else {
		org_error_code = bluetooth_set_custom_advertising(TRUE,
				(bluetooth_advertising_params_t *)adv_params);
	}

	error_code = _bt_get_error_code(org_error_code);
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_stop_advertising(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_set_advertising(FALSE));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_is_advertising(bool *is_advertising)
{
	int ret;
	gboolean advertising = FALSE;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(is_advertising);

	ret = bluetooth_is_advertising(&advertising);
	if (ret >= BLUETOOTH_ERROR_BASE) {
		*is_advertising = advertising ? true : false;
		return BT_ERROR_NONE;
	} else {
		ret = _bt_get_error_code(ret);
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}
}

int bt_adapter_add_white_list(const char *address)
{
	int error_code = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	_bt_convert_address_to_hex(&addr_hex, address);

	error_code = _bt_get_error_code(bluetooth_add_white_list(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_remove_white_list(const char *address)
{
	int error_code = BT_ERROR_NONE;
	bluetooth_device_address_t addr_hex = { {0,} };

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(address);
	_bt_convert_address_to_hex(&addr_hex, address);

	error_code = _bt_get_error_code(bluetooth_remove_white_list(&addr_hex));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

int bt_adapter_clear_white_list(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	error_code = _bt_get_error_code(bluetooth_clear_white_list());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}

	return error_code;
}

static int __bt_get_ad_data_by_type(char *in_data, int in_len,
		char in_type, char **data, int *data_len)
{
	if (in_data == NULL || data == NULL || data_len == NULL)
		return BT_ERROR_OPERATION_FAILED;

	if (in_len < 0)
		return BT_ERROR_OPERATION_FAILED;

	int i;
	int len = 0;
	int type = 0;

	for (i = 0; i < in_len; i++) {
		len = in_data[i];
		if (len <= 0 || i + 1 >= in_len) {
			BT_ERR("Invalid advertising data");
			return BT_ERROR_OPERATION_FAILED;
		}

		type = in_data[i + 1];
		if (type == in_type) {
			i = i + 2;
			len--;
			break;
		}

		i += len;
		len = 0;
	}

	if (i + len > in_len) {
		BT_ERR("Invalid advertising data");
		return BT_ERROR_OPERATION_FAILED;
	} else if (len == 0) {
		BT_INFO("AD Type 0x%02x data is not set", in_type);
		*data = NULL;
		*data_len = 0;
		return BT_ERROR_NONE;
	}

	*data = g_memdup(&in_data[i], len);
	if (*data == NULL)
		return BT_ERROR_OUT_OF_MEMORY;
	*data_len = len;

	return BT_ERROR_NONE;
}


int bt_adapter_get_advertising_manufacturer_data(char **data, int *len)
{
	bluetooth_advertising_data_t adv = { {0} };
	int adv_len = 0;
	int ret;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(data);
	BT_CHECK_INPUT_PARAMETER(len);

	ret = _bt_get_error_code(bluetooth_get_advertising_data(&adv, &adv_len));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	ret = __bt_get_ad_data_by_type((char *)adv.data, adv_len, 0xff,
			data, len);
	if (ret != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);

	return ret;
}

int bt_adapter_set_advertising_manufacturer_data(char *data, int len)
{
	int ret;
	int adv_data_size;
	bluetooth_advertising_data_t adv = { {0} };

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(data);

	adv_data_size = sizeof(adv.data);

	if (len + 2 > adv_data_size || len < 0)
		return BT_ERROR_INVALID_PARAMETER;

	adv.data[0] = len + 1;
	adv.data[1] = 0xFF;

	memcpy(&adv.data[2], data, len);

	ret = _bt_get_error_code(bluetooth_set_advertising_data(&adv, len + 2));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_advertising_manufacturer_data_changed_cb(
		bt_adapter_advertising_manufacturer_data_changed_cb callback,
		void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_ADVERTISING_MANUFACTURER_DATA_CHANGED,
			callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_unset_advertising_manufacturer_data_changed_cb(void)
{
	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_ADVERTISING_MANUFACTURER_DATA_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_get_scan_resp_manufacturer_data(char **data, int *len)
{
	bluetooth_scan_resp_data_t resp = { {0} };
	int resp_len = 0;
	int ret;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(data);
	BT_CHECK_INPUT_PARAMETER(len);

	ret = _bt_get_error_code(bluetooth_get_scan_response_data(&resp, &resp_len));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
		return ret;
	}

	ret = __bt_get_ad_data_by_type((char *)resp.data, resp_len, 0xff,
			data, len);
	if (ret != BT_ERROR_NONE)
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);

	return ret;
}

int bt_adapter_set_scan_resp_manufacturer_data(char *data, int len)
{
	int ret;
	bluetooth_scan_resp_data_t resp = { {0} };
	int resp_data_size;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(data);

	resp_data_size = sizeof(resp.data);

	if (len + 2 > resp_data_size || len < 0)
		return BT_ERROR_INVALID_PARAMETER;

	resp.data[0] = len + 1;
	resp.data[1] = 0xFF;

	memcpy(&resp.data[2], data, len);

	ret = _bt_get_error_code(bluetooth_set_scan_response_data(&resp, len + 2));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_set_scan_resp_manufacturer_data_changed_cb(
		bt_adapter_scan_resp_manufacturer_data_changed_cb callback,
		void *user_data)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_SCAN_RESPONSE_MANUFACTURER_DATA_CHANGED,
			callback, user_data);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_unset_scan_resp_manufacturer_data_changed_cb(void)
{
	BT_CHECK_LE_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_ADVERTISING_MANUFACTURER_DATA_CHANGED);
	return BT_ERROR_NONE;
}

int bt_adapter_set_scan_parameter(float interval, float window)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();

	ret = bluetooth_set_scan_parameters(interval, window);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_adapter_set_headless_advertising_data(char *data, int len)
{
	int ret;
	bluetooth_advertising_data_t adv = { {0} };

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(data);

	memcpy(&adv.data[0], data, len);

	ret = _bt_get_error_code(bluetooth_set_headless_advertising_data(&adv, len));
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}

	return ret;
}

int bt_adapter_is_samsung_audio_dock_connected(bool *connected)
{
	int ret = BT_ERROR_NONE;
	gboolean is_connected = FALSE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(connected);

	ret = _bt_get_error_code(bluetooth_is_samsung_audio_dock_connected(&is_connected));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to run function", _bt_convert_error_to_string(ret), ret);
	} else {
		*connected = (is_connected ? true : false);
		BT_DBG("bt_adapter_is_samsung_audio_dock_connected: connected = %d ret = %d", *connected, ret);
	}

	return ret;
}

int bt_adapter_get_chip_version(int *version)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(version);

	ret = _bt_get_error_code(bluetooth_get_chip_version(version));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to run function", _bt_convert_error_to_string(ret), ret);
	} else {
		BT_DBG("version : %d", *version);
	}

	return ret;
}

int bt_adapter_update_firmware(int chip_type)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();

	ret = bluetooth_update_firmware(chip_type);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_adapter_set_app_ready(bool ready)
{
	int ret = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();

	ret = bluetooth_set_app_ready(ready);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(ret), ret);
	}
	return ret;
}

int bt_adapter_is_required_update_firmware(bool *required)
{
	int ret = BT_ERROR_NONE;
	gboolean is_required = FALSE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INPUT_PARAMETER(required);

	ret = _bt_get_error_code(bluetooth_is_required_update_firmware(&is_required));

	if (ret != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x) : Failed to run function", _bt_convert_error_to_string(ret), ret);
	} else {
		*required = (is_required ? true : false);
		BT_DBG("bt_adapter_is_required_update_firmware: required = %d ret = %d", *required, ret);
	}

	return ret;
}

int bt_adapter_cancel_firmware_update(void)
{
	int error_code = BT_ERROR_NONE;

	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	error_code = _bt_get_error_code(bluetooth_cancel_firmware_update());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}

int bt_adapter_set_bt_chip_fw_update_status_cb(bt_adapter_notify_bt_chip_fw_update_status_cb callback,
							void *user_data)
{                                       
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	BT_CHECK_INPUT_PARAMETER(callback);
	_bt_set_cb(BT_EVENT_BT_CHIP_FW_UPDATE_STATUS, callback, user_data);
	return BT_ERROR_NONE;
}

int bt_adapter_unset_bt_chip_fw_update_status_cb(void)
{
	BT_CHECK_BT_SUPPORT();
	BT_CHECK_INIT_STATUS();
	_bt_unset_cb(BT_EVENT_BT_CHIP_FW_UPDATE_STATUS);
	return BT_ERROR_NONE;
}

int bt_adapter_get_local_oob_data(unsigned char **hash, unsigned char **randomizer,
					int *hash_len, int *randomizer_len)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_set_remote_oob_data(const char *remote_address,
				unsigned char *hash, unsigned char *randomizer,
				int hash_len, int randomizer_len)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_remove_remote_oob_data(const char *remote_address)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_start_scan(bt_adapter_le_scan_result_cb cb, void *user_data)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_stop_scan(void)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_get_scan_result_service_uuids(const bt_adapter_le_device_scan_result_info_s *info,
			bt_adapter_le_packet_type_e pkt_type, char ***uuids, int *count)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_get_scan_result_device_name(const bt_adapter_le_device_scan_result_info_s *info,
			bt_adapter_le_packet_type_e pkt_type, char **name)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_get_scan_result_tx_power_level(const bt_adapter_le_device_scan_result_info_s *info,
			bt_adapter_le_packet_type_e pkt_type, int *power_level)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_get_scan_result_service_solicitation_uuids(const bt_adapter_le_device_scan_result_info_s *info,
			bt_adapter_le_packet_type_e pkt_type, char ***uuids, int *count)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_get_scan_result_service_data_list(const bt_adapter_le_device_scan_result_info_s *info,
			bt_adapter_le_packet_type_e pkt_type, bt_adapter_le_service_data_s **data_list, int *count)
{
	BT_NOT_SUPPORTED_FUNCTION();
}


int bt_adapter_le_free_service_data_list(bt_adapter_le_service_data_s *data_list, int count)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_get_scan_result_appearance(const bt_adapter_le_device_scan_result_info_s *info,
			bt_adapter_le_packet_type_e pkt_type, int *appearance)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_get_scan_result_manufacturer_data(const bt_adapter_le_device_scan_result_info_s *info,
			bt_adapter_le_packet_type_e pkt_type, int *manufacturer_id, char **manufacturer_data, int *manufacturer_data_len)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_create_advertiser(bt_advertiser_h *advertiser)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_destroy_advertiser(bt_advertiser_h advertiser)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_add_advertising_data(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type, bt_adapter_le_packet_data_type_e data_type,
		void *data, unsigned int data_size)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_add_advertising_service_uuid(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type, const char *uuid)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_add_advertising_service_solicitation_uuid(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type, const char *uuid)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_add_advertising_service_data(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type, const char *uuid,
		const char *service_data, int service_data_len)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_set_advertising_appearance(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type, int appearance)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_add_advertising_manufacturer_data(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type, int manufacturer_id,
		const char *manufacturer_data, int manufacturer_data_len)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_set_advertising_device_name(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type, bool include_name)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_set_advertising_tx_power_level(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type, bool include_tx_power)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_remove_advertising_data(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type, bt_adapter_le_packet_data_type_e data_type)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_clear_advertising_data(bt_advertiser_h advertiser,
		bt_adapter_le_packet_type_e pkt_type)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_start_advertising(bt_advertiser_h advertiser,
		bt_adapter_le_advertising_params_s *adv_params,
		bt_adapter_le_advertising_state_changed_cb cb, void *user_data)
{
	BT_NOT_SUPPORTED_FUNCTION();
}


int bt_adapter_le_stop_advertising(bt_advertiser_h advertiser)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_start_advertising_new(bt_advertiser_h advertiser,
		bt_adapter_le_advertising_state_changed_cb cb, void *user_data)
{
	BT_NOT_SUPPORTED_FUNCTION();
}


int bt_adapter_le_set_advertising_mode(bt_advertiser_h advertiser,
		bt_adapter_le_advertising_mode_e mode)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

int bt_adapter_le_set_advertising_connectable(bt_advertiser_h advertiser, bool connectable)
{
	BT_NOT_SUPPORTED_FUNCTION();
}

