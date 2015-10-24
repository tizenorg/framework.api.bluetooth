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
 * This file declares Logger  functions for Logger Interface.
 *
 * @file	BTLogger.h
 * @author	Amit Kumar Verma
 */

 
#ifndef __BT_LOGGER_H__
#define __BT_LOGGER_H__

#include <logger/ILogger.h>
#include <capi-system-info/system_info.h>
#include <iostream>

using namespace std;

typedef enum e_KPIEVENT{
	SC_BATTERY_STATUS = 0,
	APPID,
	SPEAKER_MODEL,
	RSSI_LEVEL,
}KPIEVENT;

class CBTLogger
{
	private:

	ILogger* logger = NULL;
	int mQueueMax;
	int mExpiration;
	int mLoglevel;
	int mLoglevelForEvent;
	int mThreshHold;
	unsigned long lTime;

	//Start and end event Epoch time.
	unsigned long lCurTime;
	unsigned long lEndTime;

	//KPI Log data members.
	std::string mServiceName;
	std::string mEventName;
	std::string mStartTime;
	std::string mEndTime;
	std::string mCategory;
	std::string mValue;
	std::string mDesc;
	std::string mLog;
	std::string mCountryCode;
	std::string mServerURL;
	std::string strModelName;
	std::string strUserID;
	std::string strDUID;

	public:
	CBTLogger();
	CBTLogger(const CBTLogger &Cbtlogger);
	 CBTLogger & operator = (const CBTLogger &Cbtlogger);

	bool m_GetData(string& cInput,system_info_key_e key);
	void m_GetConnectInfo(string &mModelName, string &mUserID, string &mDUID);
	string m_GetProductSWVersion();
	bool m_Send_RC_Battery_Status_to_BTLogger(unsigned char batteryStatus);
	bool m_AddFullLog(string eventName, string msgLog);
	string m_ConvertToFormatTime();
	bool m_ParseConfigXml(string strFileUrl, KPIEVENT eventName);
	bool m_Send_AppID_to_BTLogger(long Timeduration, const char * appid);
	void m_DestroyILogger(void);
	bool  m_Send_Event_to_Logger(KPIEVENT eventName);
	bool m_Send_Speaker_Details_to_BTLogger(const char * DeviceName,const char * Model);
	bool m_Send_RSSI_Level_to_BTLogger(unsigned short pid,  int rssi);
	~CBTLogger();

};
#endif
