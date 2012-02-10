#include "msg.h"

static uint32_t calculate_size(const char *, va_list);
static void fill_buf(const char *, va_list, char *);

uint32_t calculate_size(const char *fmt, va_list val)
{
	uint32_t size = 0;

	for ( ; *fmt; fmt++) {
		switch (*fmt) {
			case 'b':
				va_arg(val, int);
				size += 1;
				break;
			case 'h':
				va_arg(val, int);
				size += 2;
				break;
			case 'l':
				va_arg(val, long);
				size += 4;
				break;
			case 's':
				size += strlen(va_arg(val, char*));
				break;
			case 'a':
				size += va_arg(val, int);
				va_arg(val, char *);
				break;
		}
	}

	return size;
}

void fill_buf(const char *fmt, va_list val, char *buf)
{
	char *p;
	int h, i;
	long l;

	for (p = buf ; *fmt; fmt++) {
		switch (*fmt) {
			case 'b':
				*p++ = va_arg(val, int);
				break;
			case 'h':
				h = va_arg(val, int);
				*p++ = (h >> 8) & 0xFF;
				*p++ = h & 0xFF;
				break;
			case 'l':
				l = va_arg(val, long);
				*p++ = (l >> 24) & 0xFF;
				*p++ = (l >> 16) & 0xFF;
				*p++ = (l >> 8) & 0xFF;
				*p++ = l & 0xFF;
				break;
			case 's':
				p = stpcpy(p, va_arg(val, char *));
				break;
			case 'a':
				i = va_arg(val, int);
				memcpy(p, va_arg(val, char*), i);
				p += i;
				break;
		}
	}
}

struct liveview_msg *liveview_msg_create(uint8_t id, const char *fmt, ...)
{
	struct liveview_msg *msg;
	va_list val;

	if (!(msg = calloc(1, sizeof(struct liveview_msg))))
		pexit("");

	msg->id = id;
	msg->header_len = 4;

	va_start(val, fmt);
	msg->payload_len = calculate_size(fmt, val);
	va_end(val);

	va_start(val, fmt);
	if (!(msg->payload = calloc(msg->payload_len, sizeof(char))))
		pexit("");
	fill_buf(fmt, val, msg->payload);
	va_end(val);
	printf("size: %i\n", msg->payload_len);

	return msg;
}

int liveview_msg_free(struct liveview_msg *msg)
{
	if (msg) {
		if (msg->payload)
			free(msg->payload);
		free(msg);
	}
}

