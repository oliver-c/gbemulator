#include <stdio.h>
#include <assert.h>

#include <SDL.h>

#include "GB.h"

void showUsage (const char *name);

int main (int argc, char *argv[]) {
   GB gb; 

   if (argc < 2) {
      showUsage (argv[0]);
   } else {
      gb = GB_init ();
      assert (gb != NULL);

      GB_loadRom (gb, argv[1]);
      GB_run (gb);
       
      GB_free (gb);
   }

   return 0;
}

void showUsage (const char *name) {
   printf ("%s path_to_rom\n", name);
}
