#include <stdint.h>

#pragma once


// 32-bits are often enough
typedef struct {
	int x, y;
} D2;

// Red, Green, Blue, Alpha
typedef struct {
	uint8_t r, g, b, a;
} RGBA;


// Save each single pixel
typedef struct {
	D2 pos;
	RGBA color;
} Px;

// Structure is more complex to remember resizal
typedef struct {
	_Bool with_grid;
	D2	size,
		orig_size;
	Px	*orig_x,
		*x;
	size_t max, len;
} Pixels;


// Resizal bullshit here v
static void px_assert(_Bool condition, char *message) {
	if (!condition)
		perror(message),
		exit(-1);
}

#define ERROR "\x1b[31mError:\x1b[m "

static void alloc_px_x(Pixels *px, size_t total) {
	px_assert(
		px->x = malloc(
			(px->max = total) * sizeof(Px)
		),
		ERROR"malloc"
	);
}

size_t get_orig_pixel_index_at_pos(Pixels *px, D2 *pos, size_t total) {
	for(size_t i = 0; i < total; ++i) {
		Px *p = &px->orig_x[i];

		if (p->pos.x == pos->x && p->pos.y == pos->y)
			return i;
	}

	return -1;
}

void resize_pixels(Pixels *px, D2 *new) {
	px->size.y = new->y,
	px->size.x = new->x;

	{
		size_t total = new->x * new->y;

		if (!total)
			return;

		px->len = total;

		// Reallocate when bigger
		if (!px->orig_x)
			px->orig_x = px->x,
			alloc_px_x(px, total);
		else if (total > px->max)
			alloc_px_x(px, total);
	}
	
	size_t total = px->orig_size.x * px->orig_size.y;
	D2 pos = { };
	for(; pos.y < new->y; ++pos.y, pos.x = 0)
		for(; pos.x < new->x; ++pos.x) {
			Px	*o,
				*p = &px->x[pos.y * new->x + pos.x];

			D2 o_pos = {
				(pos.x * px->orig_size.x + new->x / 2) / new->x,
				(pos.y * px->orig_size.y + new->y / 2) / new->y
			};

			// Only grid is optimized
			if (px->with_grid)
				o = &px->orig_x[o_pos.y * px->orig_size.x + o_pos.x];
			else {
				size_t i = get_orig_pixel_index_at_pos(px, &o_pos, total);

				if (i == (size_t)-1)
					continue;

				o = &px->orig_x[i];
			}

			p->color.r = o->color.r,
			p->color.g = o->color.g,
			p->color.b = o->color.b,

			p->pos.x = pos.x,
			p->pos.y = pos.y;
		}
}
