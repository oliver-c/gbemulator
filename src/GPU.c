#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "GPU.h"

struct GPU {
   GB gb;
   GPU_mode mode;
   int currentLine;
};

GPU GPU_init (GB gb) {
   GPU newGPU = (GPU)malloc(sizeof(struct GPU));
   assert (newGPU != NULL);

   newGPU->gb = gb;
   newGPU->currentLine = 0;

   return newGPU;
}

void GPU_free (GPU gpu) {
   free (gpu);
}

void GPU_update (GPU gpu) {

}
