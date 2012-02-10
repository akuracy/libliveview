#ifndef _MSG_H_
#define _MSG_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct liveview_msg {
	uint8_t id;
	uint8_t header_len;
	uint32_t payload_len;
	char *payload;
};

struct liveview_msg *liveview_msg_create(uint8_t, const char *, ...);
int liveview_msg_free(struct liveview_msg *);

#endif
