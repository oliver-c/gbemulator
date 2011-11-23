#ifndef _GB_H_
#define _GB_H_

#include "GB_type.h"
#include "CPU_type.h"
#include "MMU_type.h"
#include "Cartridge_type.h"
#include "GUI_type.h"

#include "types.h"

GB GB_init ();
void GB_loadRom (GB gb, const char *location);
void GB_run (GB gb);
void GB_free (GB gb);

void GB_setRunning (GB gb, bool running);

CPU GB_getCPU (GB gb);
MMU GB_getMMU (GB gb);
Cartridge GB_getCartridge (GB gb);
GUI GB_getGUI (GB gb);

#endif
