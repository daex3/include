#include <stdint.h>
#include <stdio.h>

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
	int max, len;
} Pixels;


// Resizal bullshit here v
static void px_assert(_Bool condition, char *message) {
	if (!condition)
#ifdef ERR_HANDLE
		ERR_HANDLE;
#else
		perror(message),
		exit(-1);
#endif
}

#define ERROR "\x1b[31mError:\x1b[m "

static void alloc_px_x(Pixels *px, int total) {
	px_assert(
		(px->x = malloc(
			(px->max = total) * sizeof(Px)
		)),
		ERROR"malloc"
	);
}

// Slow
int get_orig_pixel_index_at_pos(Pixels *px, D2 *pos, int total) {
	for(int i = 0; i < total; ++i) {
		Px *p = &px->orig_x[i];

		if (p->pos.x == pos->x && p->pos.y == pos->y)
			return i;
	}

	return -1;
}

void resize_pixels(Pixels *px, D2 *new) {
	px->size = *new;

	{
		int total = new->x * new->y;

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
	
	int total = px->orig_size.x * px->orig_size.y;
	D2 pos = { };
	for(; pos.y < new->y; ++pos.y, pos.x = 0)
		for(; pos.x < new->x; ++pos.x) {
			Px	*o,
				*p = &px->x[pos.y * new->x + pos.x];

			D2 o_pos = {
				(pos.x * px->orig_size.x + new->x / 2) / new->x,
				(pos.y * px->orig_size.y + new->y / 2) / new->y
			};

			// Only grid is optimized: Needs Hash
			if (px->with_grid)
				o = &px->orig_x[o_pos.y * px->orig_size.x + o_pos.x];
			else {
				int i = get_orig_pixel_index_at_pos(px, &o_pos, total);

				if (i == (int)-1)
					continue;

				o = &px->orig_x[i];
			}

			p->color = o->color,

			p->pos = pos;
		}
}
