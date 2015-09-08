#include "bt_unit_test_gatt.h"
#include <getopt.h>

GMainLoop *main_loop = NULL;
gatt_test_role_e role = GATT_ROLE_INVALID;
bool gatt_debug = false;

typedef void (*gatt_test_signal_handler) (int signo);

gatt_test_signal_handler profile_signal_handler_cb = NULL;

void sig_handler(int signo)
{
/*	TODO: decide on what more to be done here. Should each profile register callback for this */
	if(profile_signal_handler_cb)
		profile_signal_handler_cb(signo);
	exit(0);
}

static struct option main_options[] = {
	{ "heart-rate-profile", optional_argument, NULL, 'H' },
	{ "client",	 no_argument, NULL, 'c' },
	{ "server", no_argument, NULL, 's' },
	{ "help", no_argument, NULL, 'h' },
	{ }
};


void usage()
{
	USR_PRT("Usage:\tbt_unit_test_gatt <Role> [option[parameter]]\n\n"\
			"\t Role:\n"\
			"\t -c	client role\n"\
			"\t -s	server role\n\n"\
			"\t [option..]:\n"\
			"\t -H <bd-addr>   Heart rate Profile\n"\
			"\t -d	Debug enable\n"\
			"\t -h 	Help Menu\n");
}

int main(int argc, char *argv[])
{
	GIOChannel *key_io;
	int opt;
	bool profile_selected = false;
	char buf[BUFFER_LEN] = { 0 };
//	g_type_init();

	key_io = g_io_channel_unix_new(fileno(stdin));

	g_io_channel_set_encoding(key_io, NULL, NULL);
	g_io_channel_set_flags(key_io, G_IO_FLAG_NONBLOCK, NULL);

	while ((opt = getopt_long(argc, argv,
				"H:hcsd", main_options, NULL)) != -1) {
		switch(opt) {
		case 'c':
			if (role ==  GATT_ROLE_SERVER) {
				USR_PRT(" both <role> can't be selected");
				goto fail;
			} else {
				role = GATT_ROLE_CLIENT;
			}
			break;
		case 's':
			if (role ==  GATT_ROLE_CLIENT) {
				USR_PRT("both <role> can't be selected");
				goto fail;
			} else {
				role = GATT_ROLE_SERVER;
			}
			break;
		case 'H':
			if (role ==  GATT_ROLE_INVALID) {
				USR_PRT("<role> parameter is missing");
				usage();
				exit(0);
			}else if(role == GATT_ROLE_CLIENT) {
					hr_client_initialize();
					USR_PRT("Launching Heart rate profile test-suite");
					memcpy(buf, optarg, sizeof(buf));
					hr_client_accept_input(buf);
					g_io_add_watch(key_io, G_IO_IN | G_IO_HUP | G_IO_ERR | G_IO_NVAL,
							hr_client_key_event_cb, NULL);
					g_io_channel_unref(key_io);
					profile_selected = true;
					profile_signal_handler_cb = hr_client_signal_handler;
			} else {
				//Heart-rate profile server
				profile_selected = true;
			}
			break;
		case 'd':
			gatt_debug = true;
			break;
		case 'h':
		default:
			usage();
			exit(0);
		}
	}

	if(profile_selected == false) {
		USR_PRT("No profile selected: So, nothing to do");
		goto fail;
	}

	main_loop = g_main_loop_new(NULL, FALSE);

	if (signal(SIGINT, sig_handler) == SIG_ERR)
		USR_PRT("\n can't catch SIGINT\n");

	g_main_loop_run(main_loop);

	bt_deinitialize();

	return 0;
fail:
	usage();
	exit(0);
}
