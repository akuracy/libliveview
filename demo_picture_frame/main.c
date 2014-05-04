#define UNUSED(x) (void)(x)
#define IMAGE_COUNT 5

#include "libliveview.h"

#include <stdarg.h>
#include <time.h>
#include <signal.h>
#include <pthread.h>

void pexit(const char *);
void free_images(void);
void sighandler(int);

uint8_t current_image = 0;

struct liveview_img* images[IMAGE_COUNT];

void free_images(void)
{
	int i;
	printf("freeing images...\n");

	for (i = 0; i < IMAGE_COUNT; i++) {
		img_free(images[i]);
	}
}

void sighandler(int signum)
{
	UNUSED(signum);

	free_images();
	exit(EXIT_FAILURE);
}

void pexit(const char *str)
{
	free_images();
	perror(str ? str : "");
	exit(EXIT_FAILURE);
}

static void* next_image(void* data)
{
	struct liveview* lv;

	lv = (struct liveview*) data;
	
	while (1) {
		sleep(3);
		
		current_image++;
		current_image %= IMAGE_COUNT;

		liveview_send_image(lv, 0, 0, images[current_image]);
		
		//sleep(1);
		//liveview_send_clear(lv);
	
		//liveview_send_vibrate(lv, 0, 50);

		//liveview_send_text(lv, "Coucou");
	}
}

int main(int argc, char **argv)
{
	struct liveview_event lv_ev;
	struct liveview lv;
	
	UNUSED(argc);
	UNUSED(argv);

	printf("loading images...\n");
	images[0] = img_read("hamburger_128.png");
	images[1] = img_read("console1_128.png");
	images[2] = img_read("console2_128.png");
	images[3] = img_read("console3_128.png");
	images[4] = img_read("console4_128.png");

	signal(SIGINT, sighandler);

	printf("waiting...\n");

	if (liveview_init(&lv) == -1) {
		pexit("liveview_init");
	}

	if (liveview_connect(&lv) == -1) {
		pexit("liveview_connect");
	}

	printf("connected!\n");

	pthread_t next_image_thread;
	pthread_create(&next_image_thread, NULL, next_image, &lv);

	liveview_send_display_properties_request(&lv);

	while (liveview_read(&lv, &lv_ev) != -1) {
		printf("<= %d\n", lv_ev.type);

		liveview_send_ack(&lv, lv_ev.type);
		printf("=> %d (ACK)\n", lv_ev.type);
	
		if (lv_ev.type == M_DEVICESTATE) {
			liveview_send_status(&lv);
			liveview_send_menu_size(&lv, 0);
			liveview_send_clear(&lv);

		} else if (lv_ev.type == M_DISPLAY_PROPERTIES_RESPONSE) {
			liveview_send_menu_size(&lv, 0);
			liveview_send_clear(&lv);

		} else if (lv_ev.type == M_SETMENUSIZE_ACK) {
			liveview_send_clear(&lv);

		} else if (lv_ev.type == M_CLEARDISPLAY_ACK) {
			liveview_send_image(&lv, 0, 0, images[current_image]);
			liveview_send_screenmode(&lv, 75, 0);

		} else if (lv_ev.type == M_GETTIME) {
			liveview_send_time(&lv, time(NULL) + 3600 * 2, 1);

		} else if (lv_ev.type == M_NAVIGATION) {
			printf("DEBUG) in_alert     = %d\n", lv_ev.in_alert);
			printf("DEBUG) menu_id      = %d\n", lv_ev.menu_id);
			printf("DEBUG) menu_item_id = %d\n", lv_ev.menu_item_id);
			printf("DEBUG) nav_action   = %d\n", lv_ev.nav_action);
			printf("DEBUG) nav_type     = %d\n", lv_ev.nav_type);

			if (lv_ev.nav_type == NAVTYPE_LEFT) {
				current_image--;
				if (current_image >= IMAGE_COUNT) {
					current_image = IMAGE_COUNT - 1;
				}
			} else if (lv_ev.nav_type == NAVTYPE_RIGHT) {
				current_image++;
				current_image %= IMAGE_COUNT;
			}
			liveview_send_clear(&lv);
		}

	}

	pexit("read");

	return 0;
}
