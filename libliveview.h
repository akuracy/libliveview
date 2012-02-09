#ifndef _LIB_LIVEVIEW_H_
#define _LIB_LIVEVIEW_H_

#include <stdlib.h>
#include <stdio.h>
#include <bluetooth/bluetooth.h>
#include <bluetooth/rfcomm.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>

struct liveview {
	int fd;
	int listen_fd;
	sdp_session_t *session;
};


enum liveview_msg {
	M_DEVICESTATE = 7,
	M_DEVICESTATE_ACK = 8,
	M_ACK = 44,

	/*
	MSG_GETCAPS             = 1
		MSG_GETCAPS_RESP        = 2

		MSG_DISPLAYTEXT         = 3
		MSG_DISPLAYTEXT_ACK     = 4

		MSG_DISPLAYPANEL        = 5
		MSG_DISPLAYPANEL_ACK    = 6

		MSG_DEVICESTATUS        = 7
		MSG_DEVICESTATUS_ACK    = 8

		MSG_DISPLAYBITMAP       = 19
		MSG_DISPLAYBITMAP_ACK   = 20

		MSG_CLEARDISPLAY        = 21
		MSG_CLEARDISPLAY_ACK    = 22

		MSG_SETMENUSIZE         = 23
		MSG_SETMENUSIZE_ACK     = 24

		MSG_GETMENUITEM         = 25
		MSG_GETMENUITEM_RESP    = 26

		MSG_GETALERT            = 27
		MSG_GETALERT_RESP       = 28

		MSG_NAVIGATION          = 29
		MSG_NAVIGATION_RESP     = 30

		MSG_SETSTATUSBAR        = 33
		MSG_SETSTATUSBAR_ACK    = 34

		MSG_GETMENUITEMS        = 35

		MSG_SETMENUSETTINGS     = 36
		MSG_SETMENUSETTINGS_ACK = 37

		MSG_GETTIME             = 38
		MSG_GETTIME_RESP        = 39

		MSG_SETLED              = 40
		MSG_SETLED_ACK          = 41

		MSG_SETVIBRATE          = 42
		MSG_SETVIBRATE_ACK      = 43

		MSG_ACK                 = 44

		MSG_SETSCREENMODE       = 64
		MSG_SETSCREENMODE_ACK   = 65

		MSG_GETSCREENMODE       = 66
		MSG_GETSCREENMODE_RESP  = 67
		*/
};

int liveview_init(struct liveview *);

int liveview_connect(struct liveview *);

void liveview_set_time_func(struct liveview *, void (*) (struct liveview *));




#endif
