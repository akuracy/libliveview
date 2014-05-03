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
	struct liveview_img *img;

	if (liveview_init(&lv) == -1)
		pexit("liveview_init");

	printf("waiting...\n");

	if (liveview_connect(&lv) == -1)
		pexit("liveview_connect");

	printf("connected!\n");

	liveview_send_display_properties_request(&lv);

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
			img = img_read("menu_music.png");
			liveview_send_menu_item(&lv, 0, 1, 20, "Play", img);
			liveview_send_menu_item(&lv, 1, 1, 40, "Next", NULL);
			liveview_send_menu_item(&lv, 2, 1, 40, "Prev", NULL);
			img_free(img);
		} else if (lv_ev.type == M_GETTIME) {
			printf(":: gettime\n");
			liveview_send_time(&lv, time(NULL), 1);
		}
	}

	pexit("read");

	return 0;
}
