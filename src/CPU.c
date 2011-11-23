#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "CPU.h"

struct CPU {
   GB gb;
};

CPU CPU_init (GB gb) {
   CPU newCPU = (CPU)malloc(sizeof(struct CPU));
   assert (newCPU != NULL);

   newCPU->gb = gb;

   return newCPU;
}

void CPU_free (CPU cpu) {
   free (cpu);
}
