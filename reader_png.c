#include <png.h>
#include <stdint.h>
#include <stdlib.h>
#include <pixels_px.c>

static void re_assert(_Bool condition, char *message) {
	if (!condition) {
		perror(message);
		exit(-1);
	}
}

#define ERROR "\x1b[31mError:\x1b[m "

// TODO: Fix alpha
Pixels read_png(char *path) {
	FILE *f;
	png_structp png;
	png_infop info;
	png_bytepp rows;

	re_assert((f = fopen(path, "rb")), ERROR"fopen");

	png	= png_create_read_struct(PNG_LIBPNG_VER_STRING, 0, 0, 0),
	info	= png_create_info_struct(png),

	png_init_io(png, f),
	png_read_png(png, info, PNG_TRANSFORM_IDENTITY, 0),

	rows = png_get_rows(png, info);

	Pixels r;

	r.with_grid = 1,
	r.orig_size = r.size = (D2){
		png_get_image_width(png, info),
		png_get_image_height(png, info)
	},
	r.x = malloc(
		(r.max = r.len = r.size.x * r.size.y) * sizeof(Px)
	),

	re_assert(r.x, ERROR"malloc");

	D2 a = { };
	for(size_t i = 0; a.y < r.size.y; ++a.y)
		for(a.x = 0; a.x < r.size.x; ++a.x, ++i) {
			Px	 *p = &r.x[i];
			png_byte *c;

			switch (png_get_color_type(png, info)) {
				case PNG_COLOR_TYPE_RGB:
					c = &rows[a.y][a.x * 3],
					p->color = (RGBA){
						*c,
						c[1],
						c[2],
						255
					};

					break;
				case PNG_COLOR_TYPE_RGBA:
					c = &rows[a.y][a.x * 4],
					p->color = (RGBA){
						*c,
						c[1],
						c[2],
						c[3]
					};

					break;
				case PNG_COLOR_TYPE_PALETTE:
				case PNG_COLOR_TYPE_GRAY:
					c = &rows[a.y][a.x],
					p->color = (RGBA){
						*c,
						*c,
						*c,
						255
					};

					break;
			
				// TODO: Support
				case PNG_COLOR_TYPE_GRAY_ALPHA:

					break;
			}

			p->pos = a;
		}

	png_destroy_read_struct(&png, &info, 0),
	fclose(f);

	return r;
}
