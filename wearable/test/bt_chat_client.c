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


#include <bluetooth.h>
#include <dlog.h>
#include <glib.h>
#include <string.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "BT_CHAT_CLIENT"

/**
 *   Variables
 **/
static GMainLoop* g_mainloop = NULL;
static bt_adapter_state_e bt_state = BT_ADAPTER_DISABLED;
static int socket_fd = -1;
static char* bt_address = NULL;
static char* server_name = "chat_server";
static char quit_command[5] = "Quit";
static int bonding_state = BT_ERROR_OPERATION_FAILED;


/**
 *   Callback functions
 **/
gboolean timeout_func(gpointer data)
{
	LOGE("[%s] Callback: Timeout.", __FUNCTION__);
	g_main_loop_quit((GMainLoop*)data);
	return FALSE;
}

void bt_state_changed_impl(int result, bt_adapter_state_e adapter_state, void* user_data)
{
	if(adapter_state == BT_ADAPTER_ENABLED)
	{
		if(result == BT_ERROR_NONE)
		{
			LOGI("[%s] Callback: BT was enabled successfully.", __FUNCTION__);
			bt_state = BT_ADAPTER_ENABLED;
		}
		else
		{
			LOGE("[%s] Callback: Failed to enable BT.", __FUNCTION__);
		}
	}

	if(g_mainloop)
	{
		g_main_loop_quit(g_mainloop);
	}
}

void bt_socket_connection_state_changed_impl(int result, bt_socket_connection_state_e connection_state,
	bt_socket_connection_s *connection, void *user_data)
{
	if(result == BT_ERROR_NONE)
	{
		LOGI("[%s] Callback: Result is BT_ERROR_NONE.", __FUNCTION__);
	}
	else
	{
		LOGI("[%s] Callback: Result is not BT_ERROR_NONE.", __FUNCTION__);
	}

	if(connection_state == BT_SOCKET_CONNECTED)
	{
		LOGI("[%s] Callback: Connected.", __FUNCTION__);
		if(result == BT_ERROR_NONE && connection != NULL)
		{
			socket_fd = connection->socket_fd;
			LOGI("[%s] Callback: Socket of connection - %d.", __FUNCTION__, socket_fd);
			LOGI("[%s] Callback: Role of connection - %d.", __FUNCTION__, connection->local_role);
			LOGI("[%s] Callback: Address of connection - %s.", __FUNCTION__, connection->remote_address);

			if(bt_socket_send_data(socket_fd, quit_command, strlen(quit_command)) == BT_ERROR_NONE)
			{
				LOGI("[%s] Callback: Send quit command.", __FUNCTION__);
			}
			else
			{
				LOGE("[%s] Callback: bt_socket_send_data() failed.", __FUNCTION__);
				if(g_mainloop)
				{
					g_main_loop_quit(g_mainloop);
				}
			}
		}
		else
		{
			LOGI("[%s] Callback: Failed to connect", __FUNCTION__);
			if(g_mainloop)
			{
				g_main_loop_quit(g_mainloop);
			}
		}
	}
	else
	{
		LOGI("[%s] Callback: Disconnected.", __FUNCTION__);
	}
}

void bt_socket_data_received_impl(bt_socket_received_data_s *data, void *user_data)
{
	if(socket_fd == data->socket_fd)
	{
		if(data->data_size > 0)
		{
			if(!strncmp(data->data, quit_command, data->data_size))
			{
				LOGI("[%s] Callback: Quit command.", __FUNCTION__);
				if(g_mainloop)
				{
					g_main_loop_quit(g_mainloop);
				}
			}
		}
		else
		{
			LOGE("[%s] Callback: No data.", __FUNCTION__);
		}
	}
	else
	{
		LOGI("[%s] Callback: Another socket - %d.", __FUNCTION__, data->socket_fd);
	}
}

bool bt_adapter_bonded_device_impl(bt_device_info_s *device_info, void *user_data)
{
	int i = 0;
	if(device_info != NULL)
	{
		if(device_info->remote_name != NULL && !strcmp(device_info->remote_name, (char*)user_data))
		{
			LOGI("[%s] Callback: chat_server is found in bonded list.", __FUNCTION__);
			if( device_info->remote_address != NULL )
			{
				LOGI("[%s] Callback: Address of chat_server - %s.", __FUNCTION__, device_info->remote_address);
				bt_address = strdup(device_info->remote_address);
				LOGI("[%s] Callback: The number of service_count - %d.", __FUNCTION__, device_info->service_count);
				if(device_info->service_count <= 0)
				{
					bonding_state = BT_ERROR_SERVICE_SEARCH_FAILED;
				}
				else
				{
					bonding_state = BT_ERROR_NONE;
					for(i=0; i<device_info->service_count; i++)
					{
						LOGI("[%s] Callback: service[%d] - %s", __FUNCTION__, i+1, device_info->service_uuid[i]);
					}
					LOGI("[%s] Callback: is_bonded - %d.", __FUNCTION__, device_info->is_bonded);
					LOGI("[%s] Callback: is_connected - %d.", __FUNCTION__, device_info->is_connected);
					LOGI("[%s] Callback: is_authorized - %d.", __FUNCTION__, device_info->is_authorized);
				}
			}
			else
			{
				LOGE("[%s] Callback: Address of chat_server is NULL.", __FUNCTION__);
			}

			return false;
		}
	}

	return true;
}

void bt_adapter_device_discovery_state_changed_impl(int result, bt_adapter_device_discovery_state_e discovery_state,
	bt_adapter_device_discovery_info_s *discovery_info, void *user_data)
{
	if(discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FOUND)
	{
		if(discovery_info->remote_address != NULL && !strcmp(discovery_info->remote_name, server_name))
		{
			LOGI("[%s] Callback: chat_server is found.", __FUNCTION__);
			LOGI("[%s] Callback: Address of chat_server - %s.", __FUNCTION__, discovery_info->remote_address);
			LOGI("[%s] Callback: Device major class - %d.", __FUNCTION__, discovery_info->bt_class.major_device_class);
			LOGI("[%s] Callback: Device minor class - %d.", __FUNCTION__, discovery_info->bt_class.minor_device_class);
			LOGI("[%s] Callback: Service major class - %d.", __FUNCTION__, discovery_info->bt_class.major_service_class_mask);
			bt_address = strdup(discovery_info->remote_address);
			LOGI("[%s] Callback: is_bonded - %d.", __FUNCTION__, discovery_info->is_bonded);
			bt_adapter_stop_device_discovery();
		}
		else
		{
			LOGE("[%s] Callback: Another device is found.", __FUNCTION__);
		}
	}
	else if(discovery_state == BT_ADAPTER_DEVICE_DISCOVERY_FINISHED)
	{
		LOGI("[%s] Callback: device discovery finished.", __FUNCTION__);
		if(g_mainloop)
		{
			g_main_loop_quit(g_mainloop);
		}
	}
}

void bt_device_bond_created_impl(int result, bt_device_info_s *device_info, void *user_data)
{
	if(device_info != NULL && !strcmp(device_info->remote_address, bt_address))
	{
		bonding_state = result;
		if(result == BT_ERROR_NONE)
		{
			LOGI("[%s] Callback: A bond with chat_server is created.", __FUNCTION__);
			LOGI("[%s] Callback: The number of service - %d.", __FUNCTION__, device_info->service_count);

			int i = 0;
			for(i=0; i<device_info->service_count; i++)
			{
				LOGI("[%s] Callback: service[%d] - %s", __FUNCTION__, i+1, device_info->service_uuid[i]);
			}
			LOGI("[%s] Callback: is_bonded - %d.", __FUNCTION__, device_info->is_bonded);
			LOGI("[%s] Callback: is_connected - %d.", __FUNCTION__, device_info->is_connected);
		}
		else
		{
			LOGE("[%s] Callback: Creating a bond is failed.", __FUNCTION__);
		}
	}
	else
	{
		LOGE("[%s] Callback: A bond with another device is created.", __FUNCTION__);
	}

	if(g_mainloop)
	{
		g_main_loop_quit(g_mainloop);
	}
}

void bt_device_service_searched_impl(int result, bt_device_sdp_info_s* sdp_info, void* user_data)
{
	if(sdp_info != NULL && !strcmp(sdp_info->remote_address, bt_address))
	{
		bonding_state = result;
		if(result == BT_ERROR_NONE)
		{
			LOGI("[%s] Callback: Services of chat_service are found.", __FUNCTION__);
			LOGI("[%s] Callback: The number of service - %d.", __FUNCTION__, sdp_info->service_count);

			int i = 0;
			for(i = 0; i < sdp_info->service_count; i++)
			{
				LOGI("[%s] Callback: service[%d] - %s", __FUNCTION__, i+1, sdp_info->service_uuid[i]);
			}
		}
	}
	else
	{
		LOGE("[%s] Callback: Services of another device are found.", __FUNCTION__);
	}

	if(g_mainloop)
	{
		g_main_loop_quit(g_mainloop);
	}
}




int main()
{
	g_mainloop = g_main_loop_new(NULL, FALSE);
	const char* my_uuid="11011101-0000-1000-8000-00805F9B34FB";
	int timeout_id = -1;

	LOGI("[%s] Client starts.", __FUNCTION__);

	if(bt_initialize() != BT_ERROR_NONE)
	{
		LOGE("[%s] bt_initialize() failed.", __FUNCTION__);
		return -1;
	}

	if(bt_adapter_get_state(&bt_state) != BT_ERROR_NONE)
	{
		LOGE("[%s] bt_adapter_get_state() failed.", __FUNCTION__);
		return -1;
	}

	//	Enable BT
	if(bt_state == BT_ADAPTER_DISABLED)
	{
		if(bt_adapter_set_state_changed_cb(bt_state_changed_impl, NULL) != BT_ERROR_NONE)
		{
			LOGE("[%s] bt_adapter_set_state_changed_cb() failed.", __FUNCTION__);
			return -1;
		}

		if(bt_adapter_enable() == BT_ERROR_NONE)
		{
			LOGI("[%s] bt_adapter_state_changed_cb will be called.", __FUNCTION__);
			timeout_id = g_timeout_add (60000, timeout_func, g_mainloop);
			g_main_loop_run(g_mainloop);
			g_source_remove(timeout_id);
		}
		else
		{
			LOGE("[%s] bt_adapter_enable() failed.", __FUNCTION__);
			return -1;
		}
	}
	else
	{
		LOGI("[%s] BT was already enabled.", __FUNCTION__);
	}

	//	Device discovery
	if(bt_state == BT_ADAPTER_ENABLED)
	{
		if(bt_adapter_foreach_bonded_device(bt_adapter_bonded_device_impl, server_name) != BT_ERROR_NONE)
		{
			LOGE("[%s] bt_adapter_foreach_bonded_device() failed.", __FUNCTION__);
			return -1;
		}

		if(bt_address == NULL)
		{
			if(bt_adapter_set_device_discovery_state_changed_cb(bt_adapter_device_discovery_state_changed_impl, NULL)
				!= BT_ERROR_NONE )
			{
				LOGE("[%s] bt_adapter_set_device_discovery_state_changed_cb() failed.", __FUNCTION__);
				return -1;
			}

			if(bt_adapter_start_device_discovery() == BT_ERROR_NONE)
			{
				LOGI("[%s] bt_adapter_device_discovery_state_changed_cb will be called.", __FUNCTION__);
				g_main_loop_run(g_mainloop);
			}
			else
			{
				LOGE("[%s] bt_adapter_start_device_discovery() failed.", __FUNCTION__);
				return -1;
			}
		}
		else
		{
			LOGI("[%s] chat_server is found in bonded device list.", __FUNCTION__);
		}
	}
	else
	{
		LOGE("[%s] BT is not enabled.", __FUNCTION__);
		return -1;
	}

	// Create bond with a server
	if(bonding_state == BT_ERROR_SERVICE_SEARCH_FAILED)
	{
		if(bt_device_set_service_searched_cb(bt_device_service_searched_impl, NULL) != BT_ERROR_NONE)
		{
			LOGE("[%s] bt_device_set_service_searched_cb() failed.", __FUNCTION__);
			return -1;
		}

		if(bt_device_start_service_search(bt_address) == BT_ERROR_NONE)
		{
			LOGI("[%s] bt_device_service_searched_cb will be called.", __FUNCTION__);
			g_main_loop_run(g_mainloop);
		}
		else
		{
			LOGE("[%s] bt_device_start_service_search() failed.", __FUNCTION__);
			return -1;
		}
	}
	else if(bonding_state != BT_ERROR_NONE)
	{
		if(bt_device_set_bond_created_cb(bt_device_bond_created_impl, NULL) != BT_ERROR_NONE)
		{
			LOGE("[%s] bt_device_set_bond_created_cb() failed.", __FUNCTION__);
			return -1;
		}

		if(bt_device_create_bond(bt_address) == BT_ERROR_NONE)
		{
			LOGI("[%s] bt_device_bond_created_cb will be called.", __FUNCTION__);
			g_main_loop_run(g_mainloop);
		}
		else
		{
			LOGE("[%s] bt_device_create_bond() failed.", __FUNCTION__);
			return -1;
		}
	}

	//	Connecting socket as a client
	if( bonding_state == BT_ERROR_NONE )
	{
		if( bt_socket_set_connection_state_changed_cb(bt_socket_connection_state_changed_impl, NULL) != BT_ERROR_NONE )
		{
			LOGE("[%s] bt_socket_set_connection_state_changed_cb() failed.", __FUNCTION__);
			return -1;
		}

		if( bt_socket_set_data_received_cb(bt_socket_data_received_impl, NULL) != BT_ERROR_NONE )
		{
			LOGE("[%s] bt_socket_set_data_received_cb() failed.", __FUNCTION__);
			return -1;
		}

		if( bt_socket_connect_rfcomm(bt_address, my_uuid) == BT_ERROR_NONE )
		{
			LOGI("[%s] bt_socket_connection_state_changed_cb will be called.", __FUNCTION__);
			g_main_loop_run(g_mainloop);
		}
		else
		{
			LOGE("[%s] bt_socket_connect_rfcomm() failed.", __FUNCTION__);
			return -1;
		}

		if( bt_socket_disconnect_rfcomm(socket_fd) != BT_ERROR_NONE )
		{
			LOGE("[%s] bt_socket_disconnect_rfcomm() failed.", __FUNCTION__);
			return -1;
		}
	}
	else
	{
		LOGE("[%s] Bond is not created.", __FUNCTION__);
		return -1;
	}

	bt_deinitialize();

	LOGI("[%s] Client ends.", __FUNCTION__);
	return 0;
}
