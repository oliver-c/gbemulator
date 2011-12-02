#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "CPU.h"
#include "MMU.h"
#include "GPU.h"
#include "Cartridge.h"
#include "GUI.h"

#include "types.h"

struct GB {
   bool isRunning;

   CPU cpu;
   MMU mmu;
   GPU gpu;

   Cartridge cartridge;

   GUI gui;
};

/* Runs the start up sequence for the gameboy */
void GB_runBootSequence (GB gb);

/* Checks for any interrupts and handles them, returns the number of cycles used */
int GB_handleInterrupts (GB gb);

/* Updates the timers */
void GB_handleTimers (GB gb);

GB GB_init () {
   GB newGB = (GB)malloc(sizeof(struct GB));
   assert (newGB != NULL);

   newGB->cpu = CPU_init (newGB);
   newGB->mmu = MMU_init (newGB);
   newGB->gpu = GPU_init (newGB);
   newGB->cartridge = Cartridge_init (newGB);
   newGB->gui = GUI_init (newGB);

   newGB->isRunning = FALSE;
   GB_runBootSequence (newGB);

   return newGB;
}

void GB_free (GB gb) {
   assert (gb != NULL);

   CPU_free (gb->cpu);
   MMU_free (gb->mmu);
   GPU_free (gb->gpu);
   Cartridge_free (gb->cartridge);
   GUI_free (gb->gui);

   free (gb);
}

void GB_loadRom (GB gb, const char *location) {
   Cartridge_load (gb->cartridge, location);   
}

void GB_run (GB gb) {
   int cyclesSoFar = 0;

   gb->isRunning = TRUE;

   while (gb->isRunning) {
      cyclesSoFar += CPU_step (gb->cpu);

      if (cyclesSoFar >= CLOCK_SPEED) {
         /* delay */
      }

      GUI_handleEvents (gb->gui);
      GB_handleTimers (gb);
      cyclesSoFar += GB_handleInterrupts (gb);
   }
}

void GB_setRunning (GB gb, bool running) {
   gb->isRunning = running;
}

CPU GB_getCPU (GB gb) {
   return (gb->cpu);
}

MMU GB_getMMU (GB gb) {
   return (gb->mmu);
}

Cartridge GB_getCartridge (GB gb) {
   return (gb->cartridge);
}

GUI GB_getGUI (GB gb) {
   return (gb->gui);
}

void GB_runBootSequence (GB gb) {
   /* Initialise CPU Registers */
   CPU_set16bitRegisterValue (gb->cpu, AF, 0x01B0);
   CPU_set16bitRegisterValue (gb->cpu, BC, 0x0013);
   CPU_set16bitRegisterValue (gb->cpu, DE, 0x00D8);
   CPU_set16bitRegisterValue (gb->cpu, HL, 0x014D);
   CPU_set16bitRegisterValue (gb->cpu, SP, 0xFFFE);
   CPU_set16bitRegisterValue (gb->cpu, PC, 0x0100);

   /* Initialise values in memory */
   MMU_writeByte (gb->mmu, 0xFF05, 0x00);
   MMU_writeByte (gb->mmu, 0xFF06, 0x00);
   MMU_writeByte (gb->mmu, 0xFF07, 0x00);
   MMU_writeByte (gb->mmu, 0xFF10, 0x80);
   MMU_writeByte (gb->mmu, 0xFF11, 0xBF);
   MMU_writeByte (gb->mmu, 0xFF12, 0xF3);
   MMU_writeByte (gb->mmu, 0xFF14, 0xBF);
   MMU_writeByte (gb->mmu, 0xFF16, 0x3F);
   MMU_writeByte (gb->mmu, 0xFF17, 0x00);
   MMU_writeByte (gb->mmu, 0xFF19, 0xBF);
   MMU_writeByte (gb->mmu, 0xFF1A, 0x7F);
   MMU_writeByte (gb->mmu, 0xFF1B, 0xFF);
   MMU_writeByte (gb->mmu, 0xFF1C, 0x9F);
   MMU_writeByte (gb->mmu, 0xFF1E, 0xBF);
   MMU_writeByte (gb->mmu, 0xFF20, 0xFF);
   MMU_writeByte (gb->mmu, 0xFF21, 0x00);
   MMU_writeByte (gb->mmu, 0xFF22, 0x00);
   MMU_writeByte (gb->mmu, 0xFF23, 0xBF);
   MMU_writeByte (gb->mmu, 0xFF24, 0x77);
   MMU_writeByte (gb->mmu, 0xFF25, 0xF3);
   MMU_writeByte (gb->mmu, 0xFF26, 0xF1);
   MMU_writeByte (gb->mmu, 0xFF40, 0x91);
   MMU_writeByte (gb->mmu, 0xFF42, 0x00);
   MMU_writeByte (gb->mmu, 0xFF43, 0x00);
   MMU_writeByte (gb->mmu, 0xFF45, 0x00);
   MMU_writeByte (gb->mmu, 0xFF47, 0xFC);
   MMU_writeByte (gb->mmu, 0xFF48, 0xFF);
   MMU_writeByte (gb->mmu, 0xFF49, 0xFF);
   MMU_writeByte (gb->mmu, 0xFF4A, 0x00);
   MMU_writeByte (gb->mmu, 0xFF4B, 0x00);
   MMU_writeByte (gb->mmu, 0xFFFF, 0x00);
}

int GB_handleInterrupts (GB gb) {
   int cycles = 0;
   byte IERegister; /* Tells us which interrupts are enabled */
   byte IFRegister; /* Tells us which interrupts have been requested */

   IERegister = MMU_readByte (gb->mmu, 0xFFFF);
   IFRegister = MMU_readByte (gb->mmu, 0xFF0F);

   /* If the Interrupt Master Enable flag is set */
   if (CPU_getIME(gb->cpu)) {
      if ((IERegister & 1) && (IFRegister & 1)) {
         /* V-Blank */
         cycles = CPU_executeInterrupt (gb->cpu, INT_VBLANK);
      } else if ((IERegister & 2) && (IFRegister & 2)) {
         /* LCD STAT */
         cycles = CPU_executeInterrupt (gb->cpu, INT_LCDSTAT);
      } else if ((IERegister & 4) && (IFRegister & 4)) {
         /* Timer */
         cycles = CPU_executeInterrupt (gb->cpu, INT_TIMER);
      } else if ((IERegister & 8) && (IFRegister & 8)) {
         /* Serial */
         cycles = CPU_executeInterrupt (gb->cpu, INT_SERIAL);
      } else if ((IERegister & 16) && (IFRegister & 16)) {
         /* Joypad */
         cycles = CPU_executeInterrupt (gb->cpu, INT_JOYPAD);
      }
   }

   return cycles;
}

void GB_handleTimers (GB gb) {
   MMU mmu;

   mmu = GB_getMMU (gb);

}
