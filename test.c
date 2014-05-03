#include "libliveview.h"

#include <stdarg.h>
#include <time.h>
#include <signal.h>

void pexit(const char *);
void sighandler(int);

struct liveview_img *img_heart;

struct liveview_img *img_alert;
struct liveview_img *img_lamp;
struct liveview_img *img_lamp_active;
struct liveview_img *img_heat;
struct liveview_img *img_heat_active;

uint8_t menu_alert_id = 0;
uint8_t menu_lamp_id  = 1;
uint8_t menu_heat_id  = 2;

uint8_t current_alert      = 0;
uint8_t alert_count        = 3;
uint8_t unread_alert_count = 3;

uint8_t lamp_active = 0;

uint8_t heat_active = 0;

uint8_t in_heat_panel = 0;

void free_images(void)
{
	printf("freeing images...\n");
	img_free(img_heart);
	img_free(img_alert);
	img_free(img_lamp);
	img_free(img_lamp_active);
	img_free(img_heat);
	img_free(img_heat_active);
}

void sighandler(int signum)
{
	free_images();
	exit(EXIT_FAILURE);
}

void pexit(const char *str)
{
	free_images();
	perror(str ? str : "");
	exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
	struct liveview_event lv_ev;
	struct liveview lv;
	struct liveview_img *img;

	printf("loading images...\n");
	img_heart       = img_read("weather.png");
	img_alert       = img_read("menu_gmail.png");
	img_lamp        = img_read("menu_lamp.png");		// http://icones.pro/ampoule-3-image-png.html
	img_lamp_active = img_read("menu_lamp_active.png");	// http://icones.pro/ampoule-jaune-image-png.html
	img_heat        = img_read("menu_heat.png");
	img_heat_active = img_read("menu_heat_active.png");

	signal(SIGINT, sighandler);

	printf("waiting...\n");

	if (liveview_init(&lv) == -1) {
		pexit("liveview_init");
	}

	if (liveview_connect(&lv) == -1) {
		pexit("liveview_connect");
	}

	printf("connected!\n");

	liveview_send_display_properties_request(&lv);

	while (liveview_read(&lv, &lv_ev) != -1) {
		printf("<= %d\n", lv_ev.type);

		liveview_send_ack(&lv, lv_ev.type);
		printf("=> %d (ACK)\n", lv_ev.type);
	
		if (lv_ev.type == M_DEVICESTATE) {
			liveview_send_status(&lv);
			liveview_send_menu_settings(&lv, 100, menu_alert_id);
			liveview_send_menu_size(&lv, 3);

		} else if (lv_ev.type == M_DISPLAY_PROPERTIES_RESPONSE) {
			liveview_send_menu_settings(&lv, 100, menu_lamp_id);
			liveview_send_menu_size(&lv, 3);

		} else if (lv_ev.type == M_SETMENUSIZE_ACK) {

		} else if (lv_ev.type == M_GETMENUITEMS) {
			liveview_send_menu_item(&lv, menu_alert_id, 1, alert_count, "Alertes", img_alert);
			liveview_send_menu_item(&lv, menu_lamp_id, 0, 0, lamp_active ? "Lampe ON" : "Lampe OFF", lamp_active ? img_lamp_active : img_lamp);
			liveview_send_menu_item(&lv, menu_heat_id, 0, 0, heat_active ? "Chauffage ON" : "Chauffage OFF", heat_active ? img_heat_active : img_heat);
			//liveview_send_vibrate(&lv, 0, 100);

		} else if (lv_ev.type == M_GETTIME) {
			liveview_send_time(&lv, time(NULL) + 3600 * 2, 1);

		} else if (lv_ev.type == M_NAVIGATION) {
			printf("DEBUG) in_alert     = %d\n", lv_ev.in_alert);
			printf("DEBUG) menu_id      = %d\n", lv_ev.menu_id);
			printf("DEBUG) menu_item_id = %d\n", lv_ev.menu_item_id);
			printf("DEBUG) nav_action   = %d\n", lv_ev.nav_action);
			printf("DEBUG) nav_type     = %d\n", lv_ev.nav_type);

			if (lv_ev.in_alert) {
				if (lv_ev.nav_type == NAVTYPE_MENUSELECT) {
					alert_count--;
					if (current_alert == alert_count) {
						current_alert--;
					}
					//liveview_send_navigation(&lv, RESULT_OK);
					liveview_send_navigation(&lv, RESULT_CANCEL);
					liveview_send_alert(&lv, current_alert, alert_count, unread_alert_count, "date2", "header2", "body2", img_alert);
					if (alert_count == 0) {
						//liveview_send_navigation(&lv, RESULT_EXIT);
						liveview_send_navigation(&lv, RESULT_CANCEL);
						liveview_send_menu_settings(&lv, 100, menu_alert_id);
						liveview_send_menu_size(&lv, 3);
					}
				} else {
					liveview_send_navigation(&lv, RESULT_CANCEL);
				}

			} else {

				if (in_heat_panel) {
					
					if (lv_ev.nav_type == NAVTYPE_UP) {
						liveview_send_panel(&lv, "Chauffage", "OFF", img_heart, 0);
					} else if (lv_ev.nav_type == NAVTYPE_SELECT) {
						in_heat_panel = 0;	
						liveview_send_menu_settings(&lv, 100, menu_lamp_id);
						liveview_send_menu_size(&lv, 3);
					} else if (lv_ev.nav_type == NAVTYPE_RIGHT) {
						liveview_send_image(&lv, 36, 36, img_heart);
					}

				} else {
					if (lv_ev.nav_type == NAVTYPE_SELECT) {
	
					} else if (lv_ev.nav_type == NAVTYPE_MENUSELECT) {
	
						if (lv_ev.menu_item_id == menu_lamp_id) {
							lamp_active = !lamp_active;
							//send_menu(&lv);
							//liveview_send_navigation(&lv, RESULT_EXIT);
							liveview_send_navigation(&lv, RESULT_CANCEL);
							//liveview_send_panel(&lv, "Etat lampe", "ON", lamp_active ? img_lamp_active : img_lamp, 0);
							liveview_send_menu_settings(&lv, 100, menu_lamp_id);
							liveview_send_menu_size(&lv, 3);

						} else if (lv_ev.menu_item_id == menu_heat_id) {
							//alert_count++;
							liveview_send_navigation(&lv, RESULT_OK);
							liveview_send_panel(&lv, "Chauffage", "AUTO", heat_active ? img_heat_active : img_heat, 0);
							liveview_send_image(&lv, 36, 36, img_alert);
							in_heat_panel = 1;
						
						} else {
							liveview_send_navigation(&lv, RESULT_CANCEL);
						}
						//liveview_send_navigation(&lv, RESULT_OK);
						//liveview_send_vibrate(&lv, 0, 1000);
						//liveview_send_clear(&lv);
						//liveview_send_text(&lv, "Coucou");
						//liveview_send_image(&lv, 7, 7, img4);
	
					} else {
						liveview_send_navigation(&lv, RESULT_CANCEL);
					}
				}
			}

		} else if (lv_ev.type == M_GETALERT) {
			printf("DEBUG) menu_item_id = %d\n", lv_ev.menu_item_id);
			printf("DEBUG) alert_action = %d\n", lv_ev.alert_action);

			if (lv_ev.alert_action == ALERTACTION_FIST) {
				current_alert = 0;
			} else if (lv_ev.alert_action == ALERTACTION_NEXT) {
				current_alert = (current_alert + 1) % alert_count;
			} else if (lv_ev.alert_action == ALERTACTION_PREV) {
				current_alert--;
				if (current_alert >= alert_count) {
					current_alert = alert_count - 1;
				}
			} else if (lv_ev.alert_action == ALERTACTION_LAST) {
				current_alert = alert_count - 1;
			}

			liveview_send_alert(&lv, current_alert, alert_count, unread_alert_count, "date", "header", "body", img_alert);
		}
	}

	pexit("read");

	return 0;
}
