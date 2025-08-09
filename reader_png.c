#include <png.h>
#include <stdint.h>
#include <stdlib.h>
#include <pixels_px.c>

static void re_assert(_Bool condition, char *message) {
	if (!condition)
		perror(message),
		exit(-1);
}

#define ERROR "\x1b[31mError:\x1b[m "

// TODO: Fix alpha
Pixels read_png(char *path) {
	FILE *f;
	png_structp png;
	png_infop info;
	png_bytepp rows;

	re_assert((f = fopen(path, "rb")), ERROR"fopen"),

	png	= png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0),
	info	= png_create_info_struct(png),

	png_init_io(png, f),
	png_read_png(png, info, PNG_TRANSFORM_IDENTITY, 0),

	rows = png_get_rows(png, info);

	Pixels r;

	r.orig_size.y = r.size.y	= png_get_image_height(png, info),
	r.orig_size.x = r.size.x	= png_get_image_width(png, info),
	r.len = r.max			= r.size.x * r.size.y,
	r.x				= malloc(r.len * sizeof(Px)),
	r.orig_x			= NULL,
	r.with_grid			= 1,

	re_assert(r.x, ERROR"malloc");

	int bytes = 0;

	switch (png_get_color_type(png, info)) {
	    case PNG_COLOR_TYPE_RGB:
		bytes = 3;

		break;
	    case PNG_COLOR_TYPE_RGBA:
		bytes = 4;

		break;
	    case PNG_COLOR_TYPE_PALETTE:
	    case PNG_COLOR_TYPE_GRAY:
		bytes = 1;

		break;
	    case PNG_COLOR_TYPE_GRAY_ALPHA:
		bytes = 2;

		break;
	}

	for(int y = 0, i = 0; y < r.size.y; ++y)
		for(int x = 0; x < r.size.x; ++x, ++i) {
			Px	 *p = &r.x[i];
			png_byte *c = &rows[y][x * bytes];

			p->color.r = *c,
			p->color.g = c[1],
			p->color.b = c[2],
			p->color.a = c[3],

			p->pos.x = x,
			p->pos.y = y;
		}

	png_destroy_read_struct(&png, &info, 0),
	fclose(f);

	return r;
}
