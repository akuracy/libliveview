#include "libliveview.h"

void left_button_pressed(struct liveview *lv)
{

}

void right_button_pressed(struct liveview *lv)
{

}

void pexit(const char *str)
{
	perror(str ? str : "");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	struct liveview lv;

	if (liveview_init(&lv) < 0)
		pexit("liveview_init");

	printf("waiting...\n");
	if (liveview_connect(&lv) < 0)
		pexit("liveview_connect");

	/*
	liveview_set_left_button_cb(&lv, left_button_pressed);
	liveview_set_right_button_cb(&lv, right_button_pressed)

	for (;;) {
		if (liveview_connect(&lw, "LiveView") < 0)
			perror("liveview_connect");
		else
			liveview_loop(&lw);
		sleep(10);
	}
	*/

	return 0;
}
