#ifndef _LIB_LIVEVIEW_H_
#define _LIB_LIVEVIEW_H_

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#include "msg.h"
#include "img.h"

#define SW_VERSION "0.0.3"

struct liveview {
	int fd;
	int listen_fd;
	sdp_session_t *session;
};

struct liveview_event {
	int type;
	
	uint8_t menu_id;
	uint8_t menu_item_id;
	uint8_t in_alert;
	uint8_t alert_action;
	uint16_t max_body_size;
	uint8_t nav_action;
	uint8_t nav_type;
};

enum {
	M_DISPLAY_PROPERTIES_REQUEST = 1,
	M_DISPLAY_PROPERTIES_RESPONSE = 2,
	M_DISPLAYTEXT = 3,
	M_DISPLAYTEXT_ACK = 4,
	M_DISPLAYPANEL = 5,
	M_DISPLAYPANEL_ACK = 6,
	M_DEVICESTATE = 7,
	M_DEVICESTATE_ACK = 8,
	M_DISPLAYBITMAP = 19,
	M_DISPLAYBITMAP_ACK = 20,
	M_CLEARDISPLAY = 21,
	M_CLEARDISPLAY_ACK = 22,
	M_SETMENUSIZE = 23,
	M_SETMENUSIZE_ACK = 24,
	M_GETMENUITEM = 25,
	M_GETMENUITEM_RESP = 26,
	M_GETALERT = 27,
	M_GETALERT_RESP = 28,
	M_NAVIGATION = 29,
	M_NAVIGATION_RESP = 30,
	M_SETSTATUSBAR = 33,
	M_SETSTATUSBAR_RESP = 34,
	M_GETMENUITEMS = 35,
	M_SETMENUSETTINGS = 36,
	M_SETMENUSETTINGS_ACK = 37,
	M_GETTIME = 38,
	M_GETTIME_RESP = 39,
	M_SETLED = 40,
	M_SETLED_ACK = 41,
	M_SETVIBRATE = 42,
	M_SETVIBRATE_ACK = 43,
	M_ACK = 44
};

enum {
	RESULT_OK     = 0,
	RESULT_ERROR  = 1,
	RESULT_OOM    = 2,
	RESULT_EXIT   = 3,
	RESULT_CANCEL = 4
};

enum {
	ALERTACTION_CURRENT = 0,
	ALERTACTION_FIST    = 1,
	ALERTACTION_LAST    = 2,
	ALERTACTION_NEXT    = 3,
	ALERTACTION_PREV    = 4
};

enum {
	NAVACTION_PRESS       = 0,
	NAVACTION_LONGPRESS   = 1,
	NAVACTION_DOUBLEPRESS = 2
};

enum {
	NAVTYPE_UP         = 0,
	NAVTYPE_DOWN       = 1,
	NAVTYPE_LEFT       = 2,
	NAVTYPE_RIGHT      = 3,
	NAVTYPE_SELECT     = 4,
	NAVTYPE_MENUSELECT = 5
};

int liveview_connect(struct liveview *);

int liveview_init(struct liveview *);

int liveview_read(struct liveview *, struct liveview_event *);

int liveview_send_ack(struct liveview *, char);
int liveview_send_display_properties_request(struct liveview *);
int liveview_send_menu_item(struct liveview *, int, int, int, const char *, struct liveview_img *);
int liveview_send_menu_size(struct liveview *, unsigned char);
int liveview_send_menu_settings(struct liveview *, uint8_t, uint8_t);
int liveview_send_time(struct liveview *, uint32_t, uint8_t);
int liveview_send_navigation(struct liveview *, unsigned char);
int liveview_send_vibrate(struct liveview *, int, int);
int liveview_send_status(struct liveview *);
int liveview_send_text(struct liveview *, const char *);
int liveview_send_image(struct liveview *, int, int, struct liveview_img *);
int liveview_send_clear(struct liveview *);
int liveview_send_alert(struct liveview *, int, int, int, const char *, const char *, const char *, struct liveview_img *);
int liveview_send_panel(struct liveview *, const char *, const char *, struct liveview_img *, uint8_t);

#endif
