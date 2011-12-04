#include <stdlib.h>
#include <assert.h>

#include <SDL.h>

#include "Timer.h"
#include "types.h"

struct timer {
   bool running;
   unsigned int startTime;
   unsigned int stopTime;
};

Timer Timer_init () {
   Timer newTimer = (Timer)malloc(sizeof(struct timer));
   assert (newTimer != NULL);

   newTimer->running = FALSE;
   newTimer->startTime = SDL_GetTicks ();
   newTimer->stopTime = newTimer->startTime;

   return newTimer;
}

void Timer_free (Timer timer) {
   free (timer);
}

void Timer_start (Timer timer) {
   if (!(timer->running)) {
      timer->running = TRUE;
      timer->startTime = SDL_GetTicks ();
   }
}

void Timer_stop (Timer timer) {
   if (timer->running) {
      timer->running = FALSE;
      timer->stopTime = SDL_GetTicks ();
   }
}

void Timer_reset (Timer timer) {
   timer->startTime = SDL_GetTicks ();
}

unsigned int Timer_getTicks (Timer timer) {
   unsigned int ticks;
   if (timer->running) {
      ticks = SDL_GetTicks () - timer->startTime;
   } else {
      ticks = timer->stopTime - timer->startTime;
   }

  return ticks;
}

