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


#include <bluetooth.h>
#include <dlog.h>
#include <glib.h>
#include <string.h>

#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "BT_CHAT_SERVER"

/**
 *   Variables
 **/
static GMainLoop* g_mainloop = NULL;
static bt_adapter_visibility_mode_e visibility_mode = BT_ADAPTER_VISIBILITY_MODE_NON_DISCOVERABLE;
static bt_adapter_state_e bt_state = BT_ADAPTER_DISABLED;
static int service_socket = -1;
static int connected_socket = -1;
static char* quit_command = "Quit";


/**
 *   Callback functions
 **/
gboolean timeout_func(gpointer data)
{
	LOGE("[%s] Timeout.", __FUNCTION__);
	if(g_mainloop)
	{
		g_main_loop_quit((GMainLoop*)data);
	}

	return FALSE;
}

void bt_state_changed_impl(int result, bt_adapter_state_e adapter_state, void* user_data)
{
	if( adapter_state == BT_ADAPTER_ENABLED )
	{
		if( result == BT_ERROR_NONE )
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
	if( result == BT_ERROR_NONE )
	{
		LOGI("[%s] Callback: Result is BT_ERROR_NONE.", __FUNCTION__);
	}
	else
	{
		LOGI("[%s] Callback: Result is not BT_ERROR_NONE.", __FUNCTION__);
	}

	if( connection_state == BT_SOCKET_CONNECTED )
	{
		LOGI("[%s] Callback: Connected.", __FUNCTION__);
		if( connection != NULL )
		{
			connected_socket = connection->socket_fd;
			LOGI("[%s] Callback: Socket of connection - %d.", __FUNCTION__, connected_socket);
			LOGI("[%s] Callback: Role of connection - %d.", __FUNCTION__, connection->local_role);
			LOGI("[%s] Callback: Address of connection - %s.", __FUNCTION__, connection->remote_address);
		}
		else
		{
			LOGI("[%s] Callback: No connection data", __FUNCTION__);
		}
	}
	else
	{
		LOGI("[%s] Callback: Disconnected.", __FUNCTION__);
		LOGI("[%s] Callback: Socket of connection - %d.", __FUNCTION__, connected_socket);
		LOGI("[%s] Callback: Role of connection - %d.", __FUNCTION__, connection->local_role);
		LOGI("[%s] Callback: Address of connection - %s.", __FUNCTION__, connection->remote_address);
	}
}

void bt_socket_data_received_impl(bt_socket_received_data_s *data, void *user_data)
{
	if( data->data_size > 0 )
	{
		if( !strncmp(data->data, quit_command, data->data_size) )
		{
			LOGI("[%s] Callback: Quit command.", __FUNCTION__);
			if(g_mainloop)
			{
				g_main_loop_quit(g_mainloop);
			}
		}

		if( bt_socket_send_data(connected_socket, quit_command, strlen(quit_command)) == BT_ERROR_NONE )
		{
			LOGI("[%s] Callback: Send quit command.", __FUNCTION__);
		}
		else
		{
			LOGE("[%s] Callback: bt_socket_send_data() failed.", __FUNCTION__);
		}
	}
	else
	{
		LOGE("[%s] Callback: No data.", __FUNCTION__);
	}
}


int main()
{
	g_mainloop = g_main_loop_new(NULL, FALSE);
	const char* my_uuid="11011101-0000-1000-8000-00805F9B34FB";
	int timeout_id = -1;

	LOGI("[%s] Server starts.", __FUNCTION__);

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

	//  Set name as "chat_server"
	if(bt_state == BT_ADAPTER_ENABLED)
	{
		char* name = NULL;
		if(bt_adapter_get_name(&name) != BT_ERROR_NONE)
		{
			LOGE("[%s] bt_adapter_get_name() failed.", __FUNCTION__);
			return -1;
		}

		if (NULL == name)
			return -1;

		if(strncmp(name, "chat_server", strlen(name)) != 0)
		{
			if(bt_adapter_set_name("chat_server") != BT_ERROR_NONE)
			{
				if (NULL != name)
					free(name);
				LOGE("[%s] bt_adapter_set_name() failed.", __FUNCTION__);
				return -1;
			}
		}
		free(name);
	}
	else
	{
		LOGE("[%s] BT is not enabled.", __FUNCTION__);
		return -1;
	}
	//  Set visibility as BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE
	if(bt_adapter_get_visibility(&visibility_mode, NULL) != BT_ERROR_NONE)
	{
		LOGE("[%s] bt_adapter_get_visibility() failed.", __FUNCTION__);
		return -1;
	}

	if(visibility_mode != BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE)
	{
		if(bt_adapter_set_visibility(BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE, 0) != BT_ERROR_NONE)
		{
			LOGE("[%s] bt_adapter_set_visibility() failed.", __FUNCTION__);
			return -1;
		}
		visibility_mode = BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE;
	}
	else
	{
		LOGI("[%s] Visibility mode was already set as BT_ADAPTER_VISIBILITY_MODE_GENERAL_DISCOVERABLE.", __FUNCTION__);
	}

	//	Connecting socket as a server
	if(bt_socket_create_rfcomm(my_uuid, &service_socket) != BT_ERROR_NONE)
	{
		LOGE("[%s] bt_socket_create_rfcomm() failed.", __FUNCTION__);
		return -1;
	}
	LOGI("[%s] socket is created - %d.", __FUNCTION__, service_socket);

	if(bt_socket_set_connection_state_changed_cb(bt_socket_connection_state_changed_impl, NULL) != BT_ERROR_NONE)
	{
		LOGE("[%s] bt_socket_set_connection_state_changed_cb() failed.", __FUNCTION__);
		return -1;
	}

	if(bt_socket_set_data_received_cb(bt_socket_data_received_impl, NULL) != BT_ERROR_NONE)
	{
		LOGE("[%s] bt_socket_set_data_received_cb() failed.", __FUNCTION__);
		return -1;
	}

	if(bt_socket_listen_and_accept_rfcomm(service_socket, 5) == BT_ERROR_NONE)
	{
		LOGI("[%s] bt_socket_connection_state_changed_cb will be called.", __FUNCTION__);
		g_main_loop_run(g_mainloop);
	}
	else
	{
		LOGE("[%s] bt_socket_listen_and_accept_rfcomm() failed.", __FUNCTION__);
		return -1;
	}

	sleep(5);	// Wait for completing delivery
	if(bt_socket_destroy_rfcomm(service_socket) != BT_ERROR_NONE)
	{
		LOGE("[%s] bt_socket_destroy_rfcomm() failed.", __FUNCTION__);
		return -1;
	}
	else
	{
		LOGE("[%s] bt_socket_destroy_rfcomm() succeeded.", __FUNCTION__);
	}

	bt_deinitialize();

	LOGI("[%s] Server ends.", __FUNCTION__);
	return 0;
}
