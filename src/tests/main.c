#include <stdio.h>
#include <assert.h>

#include <SDL.h>

#include "GB.h"
#include "CPU.h"
#include "MMU.h"

void testCPU ();

int main (int argc, char *argv[]) {
   testCPU ();
   return 0;
}

void testCPU () {
   GB gb;
   CPU cpu;
   MMU mmu;

   printf ("Testing CPU...\n");
   
   gb = GB_init ();
   mmu = GB_getMMU (gb);
   cpu = GB_getCPU (gb);

   /* Set initial PC */
   CPU_set16bitRegisterValue (cpu, PC, 0);
   CPU_set8bitRegisterValue (cpu, A, 0);
   CPU_set8bitRegisterValue (cpu, B, 42);

   GB_loadRom (gb, "ROMS/test1.ROM");

   /* Test instructions */
   MMU_writeByte (mmu, 0x100, 0x78);

   CPU_step (cpu);
   assert (CPU_get16bitRegisterValue (cpu, PC) == 0x1);
   assert (CPU_get8bitRegisterValue (cpu, A) == 42);

   printf ("CPU tests passed.\n");

   GB_free (gb);
}
