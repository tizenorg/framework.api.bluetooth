#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <glib.h>
#include <ctype.h>
#include <time.h>

#include "bluetooth.h"
#include "bluetooth_internal.h"
#include "bluetooth_private.h"
#include "bluetooth-telephony-api.h"

#define PRT(format, args...) do { \
	if (gatt_debug)\
		printf("%s:%d() "format, __FUNCTION__, __LINE__, ##args);\
}while(0)

#define DBG_PRT(format, args...) PRT(format"\n", ##args)
#define USR_PRT(format, args...) printf(format"\n", ##args)
#define BUFFER_LEN 49

typedef enum
{
	GATT_ROLE_INVALID = 0x00,
	GATT_ROLE_CLIENT,
	GATT_ROLE_SERVER,
}gatt_test_role_e;


int hr_client_initialize(void);
void hr_client_signal_handler(int);

void hr_client_accept_input(char *buf);

gboolean hr_client_key_event_cb(GIOChannel *chan, GIOCondition cond,
				gpointer data);


