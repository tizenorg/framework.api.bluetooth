#!/bin/bash
set -x

LOG_FILE_NAME=/opt/usr/media/.bt_dump/log.txt

function get_error_message_from_device {
	all_log_dump.sh
	sleep 10
	sync
}

function check_bt_up_and_logging {
	if (hciconfig | grep hci0) ; then
		DOWNLOAD_TIME=`systemctl status bluetooth-hci-device -l|grep Break`
		echo $COUNTER "ON" `date -R` ${DOWNLOAD_TIME##*.} >> $LOG_FILE_NAME

		if (hciconfig hci0 sspmode | grep Disabled) ; then
			echo "BT SSP is not setuped" `date -R` >> $LOG_FILE_NAME
			hciconfig -a >> $LOG_FILE_NAME
			get_error_message_from_device;
		fi

		return 0
	else
		echo $COUNTER "FAIL" `date -R` >> $LOG_FILE_NAME
		hciconfig -a >> $LOG_FILE_NAME
		get_error_message_from_device;
		return 1
	fi
}

echo "Infinite Loop"
touch $LOG_FILE_NAME

sleep 10
check_bt_up_and_logging
sleep 10
reboot

