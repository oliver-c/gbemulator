#ifndef _CARTRIDGE_H_
#define _CARTRIDGE_H_

#include "GB_type.h"
#include "Cartridge_type.h"

#define ROM_BANK_SIZE 16384

typedef enum MBC {
   MBC_NONE,
   MBC_1,
   MBC_2,
   MBC_3
} MBC;

/* Constructor and Destructor */
Cartridge Cartridge_init (GB gb);
void Cartridge_free (Cartridge cartridge);

/* Loads a cartridge */
void Cartridge_load (Cartridge cartridge, const char *location);

/* Get cartridge data at a specified bank number */
byte * Cartridge_getData (Cartridge cartridge, int bankNumber);

#endif
