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

#include <dlog.h>
#include <stdio.h>
#include <stdbool.h>
#include <bluetooth-api.h>

#include "bluetooth.h"
#include "bluetooth_product.h"
#include "bluetooth_private.h"

int bt_dut_set_configure_mode(bool mode)
{
	int error_code = BT_ERROR_NONE;

	error_code = _bt_get_error_code(bluetooth_dut_set_configure_mode(mode));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

	return error_code;
}

int bt_dut_sent_mode(unsigned short opcode, unsigned char *data, unsigned short length)
{
	int error_code = BT_ERROR_NONE;

	if (NULL == data || 0 == length)
		return BT_ERROR_INVALID_PARAMETER;

	error_code = _bt_get_error_code(bluetooth_dut_send_mode(opcode, data, length));
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code),
								error_code);
	}

    return error_code;
}

int bt_dut_wakeup_test(void)
{
	int error_code = BT_ERROR_NONE;

	error_code = _bt_get_error_code(bluetooth_dut_wakeup_test());
	if (error_code != BT_ERROR_NONE) {
		BT_ERR("%s(0x%08x)", _bt_convert_error_to_string(error_code), error_code);
	}
	return error_code;
}


