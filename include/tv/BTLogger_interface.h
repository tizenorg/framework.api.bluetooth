/*
 * Bluetooth Configuration Module
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd. All rights reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
 
 /**
 * This file declares Logger functions for Logger Interface.
 *
 * @file	BTLogger_interface.h
 * @author	Amit Kumar Verma
 */


#ifndef __BT_LOGGER_INTERFACE__
#define __BT_LOGGER_INTERFACE__

#if defined __cplusplus
 extern "C" {
#endif


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @bt_Send_App_ID_to_BTLogger
 *
 * @param[in]   TimeDuration
 * @param[in]   long

 * @return true on success, otherwise a false
 */
bool bt_Send_App_ID_to_BTLogger(long TimeDuration, const char *appid);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @bt_Send_SC_Battery_Status_to_BTLogger
 *
 * @param[in]   batteryStat
 * @param[in]   unsigned char

 * @return true on success, otherwise a false
 */
bool bt_Send_SC_Battery_Status_to_BTLogger(unsigned char batteryStat);


/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @bt_Send_Speaker_Details_to_BTLogger
 *
 * @param[in1]   DeviceName of Remote Device
 * @param[in1]   const char*

 * @param[in2]   Model Number
 * @param[in2]   const char*

 * @return true on success, otherwise a false
 */
bool bt_Send_Speaker_Details_to_BTLogger(const char*DeviceName, const char *Model);

/**
 * @ingroup CAPI_NETWORK_BLUETOOTH_ADAPTER_MODULE
 * @bt_Send_RSSI_Level_to_BTLogger
 *
 * @param[in1]   Pid of device
 * @param[in1]   unsigned short 

 * @param[in2]   App id
 * @param[in2]   const char*

 * @param[in3]   RSSI Level
 * @param[in3]   int

 * @return true on success, otherwise a false
 */
bool bt_Send_RSSI_Level_to_BTLogger(unsigned short pid,  int rssi);


#if defined __cplusplus
        }
#endif
#endif

