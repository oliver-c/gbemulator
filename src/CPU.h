#ifndef _CPU_H_
#define _CPU_H_

#include "GB_type.h"
#include "CPU_type.h"

#define CLOCK_SPEED 4194304

CPU CPU_init (GB gb);
void CPU_free (CPU cpu);

#endif
