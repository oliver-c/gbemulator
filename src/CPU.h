#ifndef _CPU_H_
#define _CPU_H_

#include "GB_type.h"
#include "CPU_type.h"

#define NUM_REGISTERS 6
#define CLOCK_SPEED 4194304

typedef union {
   word value;

   struct {
      /* Assumes little endian */
      byte low;   
      byte high;
   } bytes;
} reg;

typedef enum Register16 {
   PC,
   SP,
   AF,
   BC,
   DE,
   HL 
} Register16;

typedef enum Register8 {
   A,
   F,
   B,
   C,
   D,
   E,
   H,
   L
} Register8;

/* Constructor and Destructor */
CPU CPU_init (GB gb);
void CPU_free (CPU cpu);

/* Run the next instruction, returns the number of cycles used */
int CPU_step (CPU cpu);

/* Gets and sets CPU register values */ 
word CPU_get16bitRegisterValue (CPU cpu, Register16 r);
void CPU_set16bitRegisterValue (CPU cpu, Register16 r, word value);
byte CPU_get8bitRegisterValue (CPU cpu, Register8 r);
void CPU_set8bitRegisterValue (CPU cpu, Register8 r, byte value);

#endif
