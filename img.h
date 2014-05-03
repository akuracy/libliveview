#ifndef _IMG_H_
#define _IMG_H_

#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct liveview_img {
	long int length;
	char *content;
};

struct liveview_img* img_read(const char *);
void img_free(struct liveview_img *);

#endif
