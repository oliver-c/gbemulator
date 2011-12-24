#ifndef _GPU_H_
#define _GPU_H_

#include "GB_type.h"
#include "GPU_type.h"

#define NUM_VISIBLE_SCANLINES 144
#define NUM_SCANLINES 154
#define SCANLINE_CYCLES 456

#define BG_TILE_WIDTH 8
#define BG_TILE_HEIGHT 8

#define TILE_SIZE_BYTES 16

#define BG_NUM_HORIZONTAL_TILES 32
#define BG_NUM_VERTICAL_TILES 32

#define NUM_COLOURS 4

static const colour COLOUR_WHITE = {255, 255, 255};
static const colour COLOUR_LIGHTGRAY = {190, 190, 190};
static const colour COLOUR_DARKGRAY = {60, 60, 60};
static const colour COLOUR_BLACK = {0, 0, 0};

GPU GPU_init (GB gb);
void GPU_free (GPU gpu);
void GPU_update (GPU gpu, int cycles);

#endif
