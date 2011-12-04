#ifndef _GPU_H_
#define _GPU_H_

#include "GB_type.h"
#include "GPU_type.h"

#define NUM_VISIBLE_SCANLINES 144
#define NUM_SCANLINES 154
#define SCANLINE_CYCLES 456

typedef enum GPU_mode {
   MODE_HBLANK,
   MODE_VBLANK,
   MODE_OAM,
   MODE_VRAM
} GPU_mode;

GPU GPU_init (GB gb);
void GPU_free (GPU gpu);

void GPU_update (GPU gpu, int cycles);

#endif
