#include "libliveview.h"

#include <stdarg.h>
#include <time.h>

void pexit(const char *str)
{
	perror(str ? str : "");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	struct liveview_event lv_ev;
	struct liveview lv;

	/*liveview_msg_create(3, "bsla", 3, "abc", 0xFFFF, 3, "abcd");*/
	liveview_msg_create(3, "blsa", 3, 0xFFFF, "abc", 3, "abc");
	pexit("");

	if (liveview_init(&lv) == -1)
		pexit("liveview_init");

	printf("waiting...\n");

	if (liveview_connect(&lv) == -1)
		pexit("liveview_connect");

	printf("connected!\n");

	while (liveview_read(&lv, &lv_ev) != -1) {
		printf("loop\n");

		liveview_send_ack(&lv, lv_ev.type);
		if (lv_ev.type == M_DISPLAY_PROPERTIES_RESPONSE) {
			printf(":: propresp\n");
			liveview_send_menu_size(&lv, 1);
			liveview_send_menu_settings(&lv, 5, 0);
		} else if (lv_ev.type == M_SETMENUSIZE_ACK) {
			printf(":: menusizeack\n");
		} else if (lv_ev.type == M_GETMENUITEMS) {
			printf(":: getmenu\n");
			init_menu(&lv);
			/* liveview_send_menu_item(&lv, 0, "Play", "");*/
		} else if (lv_ev.type == M_GETTIME) {
			printf(":: gettime\n");
			liveview_send_time(&lv, time(NULL), 0);
		}
	}

	pexit("read");

	return 0;
}
