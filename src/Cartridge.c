#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "Cartridge.h"

#include "types.h"

struct Cartridge {
   GB gb;
   MBC mbcType;

   bool loaded;
   byte *data;
};

Cartridge Cartridge_init (GB gb) {
   Cartridge newCartridge = (Cartridge)malloc(sizeof(struct Cartridge));      
   assert (newCartridge != NULL);

   newCartridge->gb = gb;
   newCartridge->loaded = FALSE;
   newCartridge->data = NULL;

   return newCartridge;
}

void Cartridge_free (Cartridge cartridge) {
   assert (cartridge != NULL);
   
   if (cartridge->loaded) {
      free (cartridge->data);
   }

   free (cartridge);
}

void Cartridge_load (Cartridge cartridge, const char *location) {
   FILE *input;
   int cartridgeSize;
   byte cartridgeType;
   int i;

   input = fopen (location, "rb");

   if (input != NULL) {
      /* Get size of ROM file */
      fseek (input, 0, SEEK_END);  
      cartridgeSize = ftell (input);
      fseek (input, 0, SEEK_SET);

      /* Allocate memory for the data */
      cartridge->data = (byte*)malloc(cartridgeSize);   

      /* Load contents of file into buffer */
      for (i = 0; i < cartridgeSize; i++) {
         cartridge->data[i] = (byte) fgetc (input);   
      }

      /* Determine MBC type */
      cartridgeType = cartridge->data[0x0147];

      switch (cartridgeType) {
         case 0x00:
            cartridge->mbcType = MBC_NONE; 
            break;
         case 0x01:
         case 0x02:
         case 0x03:
            cartridge->mbcType = MBC_1;
            break;
         case 0x05:
         case 0x06:
            cartridge->mbcType = MBC_2;
            break;
         case 0x10:
         case 0x11:
         case 0x12:
         case 0x13:
            cartridge->mbcType = MBC_3;
            break;
         default:
            break;
      }
   
      /* Cartridge has been loaded */
      cartridge->loaded = TRUE;
      fclose (input);
   } else {
      fprintf (stderr, "Unable to open ROM: %s\n", location);
   }
}

byte * Cartridge_getData (Cartridge cartridge, int bankNumber) {
   /* Return a pointer to the data at the beginning of the specified
      bank */
   assert (cartridge->loaded);
   return (cartridge->data + bankNumber * ROM_BANK_SIZE); 
}
