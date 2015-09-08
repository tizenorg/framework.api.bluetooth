#!/bin/bash
set -x

LOG_FILE_NAME=log.txt
COUNTER=0
MODEL_NAME="SM-Z130H"
SDB_OFFLINE="offline"
SDB_FAILURE_COUNTER=0

function get_error_message_from_device {
	LOG_PATH="LOG"$COUNTER
	sdb shell all_log_dump.sh
	sdb pull /opt/usr/media/SLP_debug $LOG_PATH
	sdb shell rm -rf /opt/usr/media/SLP_debug/*
}

function check_bt_up_and_logging {
	if (sdb shell hciconfig |grep hci0) ; then
		echo "BT is ON"
		echo $COUNTER "ON" `date -R` >> $LOG_FILE_NAME

		if (sdb shell hciconfig hci0 sspmode |grep Disabled) ; then
			echo "BT SSP is disabled"
			echo "BT SSP is not setuped" `date -R` >> $LOG_FILE_NAME
			sdb shell hciconfig -a >> $LOG_FILE_NAME
			get_error_message_from_device;
		else
			sdb shell systemctl status bluetooth-hci-device -l |grep Break >> $LOG_FILE_NAME
		fi

		return 0
	else
		echo $COUNTER "FAIL" `date -R` >> $LOG_FILE_NAME
		sdb shell hciconfig -a >> $LOG_FILE_NAME
		get_error_message_from_device;
		return 1
	fi
}

echo "Infinite Loop"
touch $LOG_FILE_NAME
echo "Start" `date -R` >> $LOG_FILE_NAME

while true
do
	if (sdb devices | grep $MODEL_NAME) ; then
		let COUNTER+=1
		sleep 10
		sdb root on
		sdb shell rm -rf /opt/usr/media/SLP_debug/*
		check_bt_up_and_logging
		sdb shell reboot
		sleep 20
	fi

	if (sdb devices | grep $SDB_OFFLINE) ; then
		let SDB_FAILURE_COUNTER+=1
		echo "SDB is offline"
		if [ $SDB_FAILURE_COUNTER -eq "10" ]
		then
			echo "SDB is offline more than 10 sec"
			sdb kill-server
			sleep 8
			sdb start-server
			let SDB_FAILURE_COUNTER=0
		fi
	fi
	sleep 1
done


