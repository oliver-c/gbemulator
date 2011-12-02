#ifndef _TIMER_H_
#define _TIMER_H_

#include "Timer_type.h"

Timer Timer_init ();
void Timer_free (Timer timer);

void Timer_start ();
void Timer_stop ();
void Timer_reset ();
unsigned int Timer_getTicks (Timer timer);

#endif
