#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "CPU.h"
#include "MMU.h"
#include "GUI.h"

#include "types.h"

struct GB {
   bool isRunning;

   CPU cpu;
   MMU mmu;

   GUI gui;
};

GB GB_init () {
   GB newGB = (GB)malloc(sizeof(struct GB));
   assert (newGB != NULL);

   newGB->cpu = CPU_init (newGB);
   newGB->mmu = MMU_init (newGB);
   newGB->gui = GUI_init (newGB);

   newGB->isRunning = FALSE;

   return newGB;
}

void GB_loadRom (GB gb, const char *location) {
   FILE *input;
   
   input = fopen (location, "rb"); 

   if (input == NULL) {
      fprintf (stderr, "Unable to open ROM: %s\n", location);    
   } else {
            
      fclose (input);
   }
}

void GB_run (GB gb) {
   gb->isRunning = TRUE;

   while (gb->isRunning) {
      
      GUI_handleEvents (gb->gui);
   }
}

void GB_free (GB gb) {
   assert (gb != NULL);

   CPU_free (gb->cpu);
   MMU_free (gb->mmu);
   GUI_free (gb->gui);

   free (gb);
}

void GB_setRunning (GB gb, bool running) {
   gb->isRunning = running;
}
