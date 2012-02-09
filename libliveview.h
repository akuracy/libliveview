#ifndef _LIB_LIVEVIEW_H_
#define _LIB_LIVEVIEW_H_

#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>

#define SW_VERSION "0.0.3"

struct liveview {
	int fd;
	int listen_fd;
	sdp_session_t *session;
};

struct liveview_event {
	int type;
};

struct liveview_msg {
	uint8_t id;
	uint8_t header_len;
	uint32_t payload_len;
	char *payload;
};

enum {
	M_DISPLAY_PROPERTIES_REQUEST = 1,
	M_DISPLAY_PROPERTIES_RESPONSE = 2,
	M_DEVICESTATE = 7,
	M_DEVICESTATE_ACK = 8,
	M_SETMENUSIZE = 23,
	M_SETMENUSIZE_ACK = 24,
	M_GETMENUITEM_RESP = 26,
	M_GETMENUITEMS = 35,
	M_SETMENUSETTINGS = 36,
	M_SETMENUSETTINGS_ACK = 37,
	M_ACK = 44
};

int liveview_connect(struct liveview *);

int liveview_init(struct liveview *);

int liveview_read(struct liveview *, struct liveview_event *);

int liveview_send_ack(struct liveview *, char);

int liveview_send_menu_size(struct liveview *, unsigned char);

int liveview_send_menu_settings(struct liveview *, uint8_t, uint8_t);

init_menu(struct liveview *);

#endif
