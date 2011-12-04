#ifndef _GPU_H_
#define _GPU_H_

#include "GB_type.h"
#include "GPU_type.h"

typedef enum GPU_mode {
   MODE_HBLANK,
   MODE_VBLANK,
   MODE_OAM,
   MODE_VRAM
} GPU_mode;

GPU GPU_init (GB gb);
void GPU_free (GPU gpu);

void GPU_update (GPU gpu);

#endif
