#ifndef _MMU_H_
#define _MMU_H_

/*
Memory Mapping

Interrupt Enable Register
--------------------------- FFFF
Internal RAM
--------------------------- FF80
Empty but unusable for I/O
--------------------------- FF4C
I/O ports
--------------------------- FF00
Empty but unusable for I/O
--------------------------- FEA0
Sprite Attrib Memory (OAM)
--------------------------- FE00
Echo of 8kB Internal RAM
--------------------------- E000
8kB Internal RAM
--------------------------- C000
8kB switchable RAM bank       
--------------------------- A000
8kB Video RAM                 
--------------------------- 8000 --
16kB switchable ROM bank         |
--------------------------- 4000 |= 32kB Cartridge
16kB ROM bank #0                 |
--------------------------- 0000 --
*/

#include "GB_type.h"
#include "MMU_type.h"
#include "types.h"

#define MAPPED_MEM_SIZE 0x10000

/* Constructor and Destructor */
MMU MMU_init (GB gb);
void MMU_free (MMU mmu);

/* Writes and reads bytes */
byte MMU_readByte (MMU mmu, int location);
void MMU_writeByte (MMU mmu, int location, byte byteToWrite);

/* Writes and reads words */
word MMU_readWord (MMU mmu, int location);
void MMU_writeWord (MMU mmu, int location, word wordToWrite);

#endif
