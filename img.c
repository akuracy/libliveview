#include "img.h"

void pexit(const char *);

struct liveview_img *img_read(const char *filename)
{
	struct liveview_img *img;

	FILE* file = fopen(filename, "r");
	if (file == NULL) {
		return NULL;
	}
	
	if (!(img = calloc(1, sizeof(struct liveview_img))))
		pexit("");

	fseek(file, 0, SEEK_END);
	img->length = ftell(file);
	rewind(file);
	
	if (!(img->content = calloc(img->length, 1)))
		pexit("");
	fread(img->content, 1, img->length, file);

	return img;
}

void img_free(struct liveview_img *img)
{
	if (img) {
		if (img->content)
			free(img->content);
		free(img);
	}
}

