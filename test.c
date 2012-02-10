#include "libliveview.h"

#include <stdarg.h>
#include <time.h>

int calculate_size(const char *fmt, va_list val)
{
	int size = 0;

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
	char *p, *s;
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
				s = va_arg(val, char *);
				memcpy(p, s, i);
				p += i;
				break;
		}
	}
}

struct liveview_msg *liveview_msg_create(uint8_t id, const char *fmt, ...)
{
	va_list val;
	char *s;
	int size = 0;

	va_start(val, fmt);
	size = calculate_size(fmt, val);
	printf("size: %i\n", size);
	va_end(val);

	va_start(val, fmt);

	if (!(s = calloc(size, sizeof(char))))
		pexit("");
	fill_buf(fmt, val, s);
	va_end(val);
#if 0
	uint8_t b;
	uint32_t l;
	uint16_t h;
	char *s;

	for ( ; *fmt; fmt++) {
		switch(*fmt) {
		case 'b':
			b = va_arg(val, int);
			size += 1;
			printf("b: %i\n", b);
			break;
		case 'l':
			l = va_arg(val, long);
			size += 4;
			printf("l: %u\n", l);
			break;
		case 'h':
			h = va_arg(val, short);
			size += 2;
			printf("h: %i\n", h);
			break;
		case 's':
			s = va_arg(val, char *);
			size += strlen(s);
			printf("s: %s\n", s);
			break;
		case 'a':
			size += va_arg(val, int);
			s = va_arg(val, char *);
		}
		printf("fmt: %i\n", *fmt);
	}

	printf("size: %i\n", size);
	va_end(val);
#endif
}

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
			/*			liveview_send_menu_item(&lv, 0, "Play", "");*/
		} else if (lv_ev.type == M_GETTIME) {
			printf(":: gettime\n");
			liveview_send_time(&lv, time(NULL), 0);
		}
	}

	pexit("read");

	return 0;
}
