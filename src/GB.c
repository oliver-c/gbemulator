#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "GB.h"
#include "CPU.h"
#include "MMU.h"
#include "GPU.h"
#include "Cartridge.h"
#include "GUI.h"

#include "bitOperations.h"
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
void GB_handleTimers (GB gb, int cycles);

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
   int cyclesThisIteration = 0;

   gb->isRunning = TRUE;

   while (gb->isRunning) {
      cyclesThisIteration = CPU_step (gb->cpu);
      cyclesSoFar += cyclesThisIteration;

      if (cyclesSoFar >= CLOCK_SPEED) {
         /* delay */
      }

      GUI_handleEvents (gb->gui);
      GB_handleTimers (gb, cyclesThisIteration);
      GPU_update (gb->gpu);
      cyclesSoFar += GB_handleInterrupts (gb);
   }
}

void GB_setRunning (GB gb, bool running) {
   gb->isRunning = running;
}

void GB_requestInterrupt (GB gb, int interrupt) {
   byte IFRegister;
   assert (interrupt >= 0 && interrupt <= INT_JOYPAD);

   IFRegister = MMU_readByte (gb->mmu, 0xFF0F);
   setBit (&IFRegister, interrupt);
   MMU_writeByte (gb->mmu, 0xFF0F, IFRegister);
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

void GB_handleTimers (GB gb, int cycles) {
   byte *memory; /* We need direct memory access to change the 
                    divider register */

   byte timerControl; /* This tells us the status of the timer
                         and the frequency */

   static int dividerCounter = 0;
   static int timerCounter = 0;
   int frequencyIndex;
   int dividerTimer;
   int timer;

   memory = MMU_getMemory (gb->mmu);

   /* Get timer information */
   timerControl = MMU_readByte (gb->mmu, 0xFF07);
   dividerTimer = MMU_readByte (gb->mmu, 0xFF04);
   timer = MMU_readByte (gb->mmu, 0xFF05);

   /* Update the divider register */
   dividerCounter += cycles;
   if (dividerCounter >= (CLOCK_SPEED/TIMER_DIVIDER_FREQ)) {
      /* Update the timer based on the cycles passed */
      dividerTimer++;
      dividerCounter -= (CLOCK_SPEED/TIMER_DIVIDER_FREQ);

      if (dividerTimer > 0xFF) {
         /* Overflow */
         dividerTimer = 0;
      }
   }

   if (testBit (timerControl, 2)) {
      /* If the timer is enabled */
      timerCounter += cycles;
      frequencyIndex = ((timerControl & 2) << 1) | (timerControl & 1);

      if (timerCounter >= (CLOCK_SPEED/timerFrequencies[frequencyIndex])) {
         timer++;
         timerCounter -= (CLOCK_SPEED/timerFrequencies[frequencyIndex]);

         if (timer > 0xFF) {
            /* Overflow */
            timer = MMU_readByte (gb->mmu, 0xFF06);
            
            GB_requestInterrupt (gb, INT_TIMER);   
         }
      }
   }

   /* Write new timer values */
   memory[0xFF04] = dividerTimer;
   memory[0xFF05] = timer;
}
