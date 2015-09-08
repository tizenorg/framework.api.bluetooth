/*
 * Copyright (c) 2014 Samsung Electronics Co., Ltd All Rights Reserved
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

#include <stdio.h>
#include <dlog.h>
#include <glib.h>
#include <sys/time.h>
#include <bluetooth.h>

#undef LOG_TAG
#define LOG_TAG "BT_SPP_TEST"

#define BT_DBG(fmt, args...) \
        SLOGD(fmt, ##args)
#define BT_INFO(fmt, args...) \
        SLOGI(fmt, ##args)
#define BT_ERR(fmt, args...) \
        SLOGE(fmt, ##args)

typedef enum
{
	BT_SPP_TEST_ROLE_SERVER = 0x00,
	BT_SPP_TEST_ROLE_CLIENT,
} bt_spp_test_role_e;

#define BT_INFINITE_SPP_TEST_REMOTE_ADDRESS_FILE_PATH "/opt/usr/media/Others/.bt_dump/spp_test_addr"
#define BT_INFINITE_SPP_TEST_RESULT_FILE_PATH "/opt/usr/media/Others/.bt_dump/spp_result.log"

bool __bt_connect_spp(void);
static gboolean __bt_send_data(gpointer user_data);
bool __bt_verify_data(char *data, int data_size);
bool __bt_bond_remote_device(void);
bool __bt_is_paired(void);

static GMainLoop* g_mainloop = NULL;

static bt_spp_test_role_e spp_role = BT_SPP_TEST_ROLE_SERVER;
static char remote_addr[18] = {0,};
static bt_adapter_state_e bt_state = BT_ADAPTER_DISABLED;

const char *spp_secure_uuid = "fa87c0d0-afac-11de-8a39-0800200c9a66";
static int server_fd = -1;
static int socket_fd = -1;
static bool is_receive_complete = false;

static int packet_size = 65536;
static char *buf = NULL;
static int total_count = 32;
static int sent_count = 0;
static int receive_count = 0;
static int receive_index = 0;

static struct timeval start_time = {0,};
static struct timeval end_time = {0,};
static struct timeval diff_time = {0,};

static int sum_time_ms = 0;
static int avg_time_ms = 0;
static int total_test_count = 0;

void __bt_create_log_dump(void)
{
	char *argv[] = {NULL};

	execv("/usr/bin/all_log_dump.sh", argv);

	if (g_mainloop)
		g_main_loop_quit(g_mainloop);
	else
		exit(0);
}

static void __bt_socket_data_received_cb(bt_socket_received_data_s *data, void *user_data)
{
	if (data == NULL) {
		BT_ERR("data is NULL");
		return;
	}
	BT_INFO("FD = %d, Buffer len = %d", data->socket_fd, data->data_size);

	if (__bt_verify_data(data->data, data->data_size) == false)
		__bt_create_log_dump();

	if (is_receive_complete == true)
		g_idle_add(__bt_send_data, NULL);
}

static void __bt_device_bond_created_cb(int result, bt_device_info_s *device_info, void *user_data)
{
	if (result != BT_ERROR_NONE) {
		BT_ERR("result[0x%X]", result);
		return;
	}

	if (strcmp(device_info->remote_address, remote_addr) == 0) {
		if (__bt_connect_spp() == false)
			BT_ERR("__bt_connect_spp() failed.");
	}
}

static bool __bt_adapter_bonded_device_cb(bt_device_info_s *device_info, void *user_data)
{
	bool *is_paired = (bool*)user_data;

	if (strcmp(device_info->remote_address, remote_addr) == 0) {
		BT_INFO("remote_addr is exist.");
		*is_paired = true;
		return false;
	}

	return true;
}

void __bt_adapter_state_changed_cb(int result, bt_adapter_state_e adapter_state, void *user_data)
{
	BT_INFO("result [0x%04x] state = %d", result, adapter_state);

	if (result == BT_ERROR_NONE) {
		bt_state = adapter_state;
		if (adapter_state == BT_ADAPTER_ENABLED) {
			if (__bt_is_paired() == true)
				__bt_connect_spp();
			else
				__bt_bond_remote_device();
		}
	}
}

void __bt_socket_connection_state_changed_cb(int result, bt_socket_connection_state_e connection_state, bt_socket_connection_s *connection, void *user_data)
{
	if (connection_state == BT_SOCKET_CONNECTED) {
		BT_INFO("RFCOMM connected, result [0x%04x] Fd = %d, device add = %s, Role = %s",
				result, connection->socket_fd, connection->remote_address,
				(connection->local_role == BT_SOCKET_SERVER) ? "SERVER" : "CLIENT");

		if (connection->socket_fd > 0) {
			socket_fd = connection->socket_fd;
			if (spp_role == BT_SPP_TEST_ROLE_CLIENT)
				g_idle_add(__bt_send_data, NULL);
		} else {
			BT_ERR("socket_fd is INVALID");
		}
	} else {
		BT_INFO("RFCOMM disconnected, result [0x%04x] Fd = %d, device add = %s",
				result, connection->socket_fd, connection->remote_address);
		__bt_create_log_dump();
	}
}

void __bt_spp_calc_speed(void)
{
	int total_time_ms;
	FILE *fp = NULL;
	static char str_speed[30] = "\0";
	int speed;

	diff_time.tv_sec = end_time.tv_sec - start_time.tv_sec;
	diff_time.tv_usec = end_time.tv_usec - start_time.tv_usec;

	if(diff_time.tv_usec < 0)
	{
		diff_time.tv_sec = diff_time.tv_sec - 1;
		diff_time.tv_usec = diff_time.tv_usec + 1000000;
	}

	fp = fopen(BT_INFINITE_SPP_TEST_RESULT_FILE_PATH, "a+");
	/* Fix : NULL_RETURNS */
	if (fp == NULL) {
		BT_ERR("Cannot open file: %s\n", BT_INFINITE_SPP_TEST_RESULT_FILE_PATH);
		return;
	}
	total_time_ms = (diff_time.tv_sec * 1000);
	total_time_ms += diff_time.tv_usec / 1000;

	speed = (packet_size * total_count * 8) / total_time_ms;

	BT_INFO("%s @ %d kbps", "2MB", speed);
	sprintf(str_speed, "%s @ %d kbps", "2MB", speed);
	fprintf(fp, "%s\n", str_speed);

	total_test_count++;
	sum_time_ms += total_time_ms;
	if (total_test_count % 10 == 0) {
		avg_time_ms = sum_time_ms / total_test_count;
		speed = (packet_size * total_count * 8) / avg_time_ms;
		BT_INFO("[AVG(%d times)] %d kbps", total_test_count, speed);
		sprintf(str_speed, "[AVG(%d times)] %d kbps", total_test_count, speed);
		fprintf(fp, "%s\n", str_speed);
	}

	if (total_test_count == 1000) {
		sum_time_ms = 0;
		avg_time_ms = 0;
		total_test_count = 0;
	}

	fclose(fp);
}

void __bt_make_data(void)
{
	int i;
	int j;
	char w = 0x20;

	memset(buf, 0x00, packet_size);
	for (i = 0; i < packet_size / 256; i++) {
		for (j = 0; j < 256; j++) {
			buf[i * 256 + j] = w;
		}

		w++;
		if (w > 0x7e)
			w = 0x20;
	}
}

bool __bt_verify_data(char *data, int data_size)
{
	int i;

	if (receive_count == 0 && receive_index == 0)
		gettimeofday(&start_time, NULL);

	for (i = 0; i < data_size; i++) {
		if (data[i] != buf[receive_index]) {
			BT_ERR("data error. rcv_cnt=%d, data[%d]=%d, buf[%d]=%d", receive_count, i, data[i], receive_index, buf[receive_index]);
			return false;
		}

		receive_index++;
		if (receive_index == packet_size) {
			receive_index = 0;
			receive_count++;

			if (receive_count == total_count)
				is_receive_complete = true;
		}
	}

	if (is_receive_complete == true) {
		gettimeofday(&end_time, NULL);
		__bt_spp_calc_speed();
		receive_count = 0;
		receive_index = 0;
	}

	return true;
}

static gboolean __bt_send_data(gpointer user_data)
{
	int ret = BT_ERROR_NONE;
	int write = 0;
	int state = 0;
	struct timeval tv_write_timeout = {0, };
	fd_set wfds;
	int fail_count = 0;

	tv_write_timeout.tv_sec = 1;

	is_receive_complete = false;

	if (sent_count == 0)
		gettimeofday(&start_time, NULL);

	while (write < packet_size) {
		FD_ZERO(&wfds);
		FD_SET(socket_fd, &wfds);
		state = select(socket_fd + 1, NULL, &wfds, NULL, &tv_write_timeout);
		if (state == -1) {
			BT_ERR("select() error.[0x%X]", ret);
			__bt_create_log_dump();
			return FALSE;
		} else {
			if (FD_ISSET(socket_fd, &wfds)) {
				ret = bt_socket_send_data(socket_fd, buf + write, packet_size - write);
				if(ret <= 0) {
						BT_ERR("bt_socket_send_data() failed.[0x%X]", ret);
						__bt_create_log_dump();
						return FALSE;
				} else {
					write += ret;
				}
				fail_count = 0;
			} else {
				BT_ERR("socket is NOT ready.(fail_count = %d)", fail_count);
				if (fail_count++ == 10) {
					BT_ERR("__bt_send_data failed.");
					__bt_create_log_dump();
					return FALSE;
				}
			}
		}
	}

	sent_count++;
	if (sent_count == total_count) {
		BT_INFO("Last packet.");
		gettimeofday(&end_time, NULL);
		__bt_spp_calc_speed();
		sent_count = 0;
	} else {
		return TRUE;
	}

	return FALSE;
}

bool __bt_connect_spp(void)
{
	int ret = BT_ERROR_NONE;

	if (spp_role == BT_SPP_TEST_ROLE_SERVER) {
		if (server_fd < 0) {
			ret = bt_socket_create_rfcomm(spp_secure_uuid, &server_fd);
			if (ret != BT_ERROR_NONE) {
				BT_ERR("bt_socket_create_rfcomm() failed.[0x%X]", ret);
				return false;
			}
		}

		ret = bt_socket_listen_and_accept_rfcomm(server_fd, 1);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("bt_socket_listen_and_accept_rfcomm() failed.[0x%X]", ret);
			return false;
		}
	} else {
		ret = bt_socket_connect_rfcomm(remote_addr, spp_secure_uuid);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("bt_socket_connect_rfcomm() failed.[0x%X]", ret);
			bt_socket_destroy_rfcomm(server_fd);
			return false;
		}
	}

	return true;
}

bool __bt_bond_remote_device(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_device_set_bond_created_cb(__bt_device_bond_created_cb, NULL);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("bt_device_set_bond_created_cb() failed.[0x%X]", ret);
		return false;
	}

	if (spp_role == BT_SPP_TEST_ROLE_CLIENT) {
		ret = bt_device_create_bond(remote_addr);
		if (ret != BT_ERROR_NONE) {
			BT_ERR("bt_device_create_bond() failed.[0x%X]", ret);
			return false;
		}
	}

	return true;
}

bool __bt_is_paired(void)
{
	int ret = BT_ERROR_NONE;
	bool is_paired = false;

	ret = bt_adapter_foreach_bonded_device(__bt_adapter_bonded_device_cb, &is_paired);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("bt_adapter_foreach_bonded_device() failed.[0x%X]", ret);
		return false;
	}

	return is_paired;
}

bool __bt_init(void)
{
	int ret = BT_ERROR_NONE;

	ret = bt_initialize();
	if(ret != BT_ERROR_NONE) {
		BT_ERR("bt_initialize() failed.[0x%X]", ret);
		return false;
	}

	ret = bt_adapter_get_state(&bt_state);
	if(ret != BT_ERROR_NONE) {
		BT_ERR("bt_adapter_get_state() failed.[0x%X]", ret);
		return false;
	}

	ret = bt_adapter_set_state_changed_cb(__bt_adapter_state_changed_cb, NULL);
	if(ret != BT_ERROR_NONE) {
		BT_ERR("bt_adapter_get_state() failed.[0x%X]", ret);
		return false;
	}

	ret = bt_socket_set_connection_state_changed_cb(__bt_socket_connection_state_changed_cb, NULL);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("bt_socket_set_connection_state_changed_cb() failed.[0x%X]", ret);
		return false;
	}

	ret = bt_socket_set_data_received_cb(__bt_socket_data_received_cb, NULL);
	if (ret != BT_ERROR_NONE) {
		BT_ERR("bt_socket_set_data_received_cb() failed.[0x%X]", ret);
		return false;
	}

	if (buf == NULL) {
		buf = malloc(packet_size);
		if (buf == NULL) {
			BT_ERR("Out of memory");
			return false;
		}
		__bt_make_data();
	}

	return true;
}

void __bt_deinit(void)
{
	int ret = BT_ERROR_NONE;

	if (buf)
		free(buf);

	if (server_fd >= 0)
		bt_socket_destroy_rfcomm(server_fd);

	ret = bt_socket_unset_data_received_cb();
	if (ret != BT_ERROR_NONE)
		BT_ERR("bt_socket_unset_data_received_cb() failed.[0x%X]", ret);

	ret = bt_socket_unset_connection_state_changed_cb();
	if (ret != BT_ERROR_NONE)
		BT_ERR("bt_socket_unset_connection_state_changed_cb() failed.[0x%X]", ret);

	ret = bt_device_unset_bond_created_cb();
	if (ret != BT_ERROR_NONE)
		BT_ERR("bt_device_unset_bond_created_cb() failed.[0x%X]", ret);

	ret = bt_deinitialize();
	if(ret != BT_ERROR_NONE)
		BT_ERR("bt_deinitialize() failed.[0x%X]", ret);
}

bool __bt_set_remote_address(void)
{
	FILE *fp = NULL;

	if ((fp = fopen(BT_INFINITE_SPP_TEST_REMOTE_ADDRESS_FILE_PATH, "r")) == NULL) {
		BT_ERR("remote_addr file is NOT exist.");
		return false;
	}

	if (fgets(remote_addr, 18, fp) == NULL) {
		BT_ERR("remote_addr is INVALID.");
		fclose(fp);
		return false;
	}

	fclose(fp);
	return true;
}

int main(int argc, char *argv[])
{
	if (argc == 2 && strcmp(argv[1], "client") == 0)
		spp_role = BT_SPP_TEST_ROLE_CLIENT;
	BT_INFO("SPP test role is %s", spp_role==BT_SPP_TEST_ROLE_SERVER?"SERVER":"CLIENT");

	if (__bt_set_remote_address() == false)
		return -1;

	if (__bt_init() == false)
		goto fail;

	if (bt_state == BT_ADAPTER_ENABLED) {
		if (__bt_is_paired() == true) {
			if (__bt_connect_spp() == false)
				goto fail;
		} else {
			if (__bt_bond_remote_device() == false)
				goto fail;
		}
	}

	g_mainloop = g_main_loop_new(NULL, FALSE);
	g_main_loop_run(g_mainloop);

fail:
	if (g_mainloop)
		g_main_loop_unref(g_mainloop);

	__bt_deinit();

	return 0;
}
