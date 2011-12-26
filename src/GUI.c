#include <stdlib.h>
#include <assert.h>

#include <SDL.h>

#include "GB.h"
#include "GUI.h"
#include "GPU.h"
#include "MMU.h"
#include "Timer.h"

#include "types.h"
#include "bitOperations.h"

#define NUM_KEYS 8

enum keys {
   KEY_UP,
   KEY_DOWN,
   KEY_LEFT,
   KEY_RIGHT,
   KEY_A,
   KEY_B,
   KEY_START,
   KEY_SELECT
};

struct GUI {
   GB gb;
   SDL_Surface *screen;
   SDL_Color colours[NUM_COLOURS];
   colour framebuffer[WINDOW_WIDTH * WINDOW_HEIGHT];
   Timer frameTimer;
   bool flippedThisFrame;
   bool keyDown[NUM_KEYS];
   int frameCount;
};

void GUI_initPalette (GUI gui);
void GUI_handleEvents (GUI gui);
void GUI_updateJoypad (GUI gui);

GUI GUI_init (GB gb) {
   int i;
   GUI newGUI = (GUI)malloc(sizeof(struct GUI));
   assert (newGUI != NULL);

   newGUI->gb = gb;
   newGUI->flippedThisFrame = FALSE;
   newGUI->frameCount = 0;
   newGUI->frameTimer = Timer_init ();

   for (i = 0; i < NUM_KEYS; i++) {
      newGUI->keyDown[i] = FALSE;
   }

   Timer_reset (newGUI->frameTimer);
   Timer_start (newGUI->frameTimer);

   SDL_Init (SDL_INIT_VIDEO); 
   
   SDL_WM_SetCaption (WINDOW_CAPTION, NULL); 
   newGUI->screen = SDL_SetVideoMode (WINDOW_WIDTH, WINDOW_HEIGHT, 8, 
                                      SDL_DOUBLEBUF | SDL_SWSURFACE |
                                      SDL_HWPALETTE);

   GUI_initPalette (newGUI);

   return newGUI;
}

void GUI_free (GUI gui) {
   SDL_FreeSurface (gui->screen);
   Timer_free (gui->frameTimer);

   free (gui);
}

void GUI_update (GUI gui) {
   MMU mmu;
   int currentLine;

   mmu = GB_getMMU (gui->gb);
   currentLine = MMU_readByte (mmu, 0xFF44);

   /* Update screen */

   if (currentLine < NUM_VISIBLE_SCANLINES) {
      gui->flippedThisFrame = FALSE;
   }
   
   if (currentLine >= NUM_VISIBLE_SCANLINES && !gui->flippedThisFrame) {
      /* Entered V-Blank, flip the SDL screen once */
      SDL_Flip (gui->screen); 
      gui->flippedThisFrame = TRUE;
      gui->frameCount++;

      GUI_handleEvents (gui);

      if (Timer_getTicks (gui->frameTimer) >= 5000) {
         printf ("fps = %.2lf\n", (double)(gui->frameCount)/5);
         gui->frameCount = 0;
         Timer_reset (gui->frameTimer);
      }
   }

   GUI_updateJoypad (gui);
}

void GUI_initPalette (GUI gui) {
   gui->colours[COLOUR_WHITE].r = 255;
   gui->colours[COLOUR_WHITE].g = 255;
   gui->colours[COLOUR_WHITE].b = 255;

   gui->colours[COLOUR_LIGHTGRAY].r = 190;
   gui->colours[COLOUR_LIGHTGRAY].g = 190;
   gui->colours[COLOUR_LIGHTGRAY].b = 190;

   gui->colours[COLOUR_DARKGRAY].r = 60;
   gui->colours[COLOUR_DARKGRAY].g = 60;
   gui->colours[COLOUR_DARKGRAY].b = 60;

   gui->colours[COLOUR_BLACK].r = 0;
   gui->colours[COLOUR_BLACK].g = 0;
   gui->colours[COLOUR_BLACK].b = 0;

   SDL_SetColors (gui->screen, gui->colours, 0, NUM_COLOURS);
}

void GUI_handleEvents (GUI gui) {
   SDL_Event event;
   bool down;

   while (SDL_PollEvent (&event)) {
      if (event.type == SDL_QUIT) {
         GB_setRunning (gui->gb, FALSE);
      } else if (event.type == SDL_KEYDOWN ||
                 event.type == SDL_KEYUP) {

         if (event.type == SDL_KEYDOWN) {
            down = TRUE;
         } else {
            down = FALSE;
         }

         switch (event.key.keysym.sym) {
            case SDLK_UP:
               gui->keyDown[KEY_UP] = down;
               break;
            case SDLK_DOWN:
               gui->keyDown[KEY_DOWN] = down;
               break;
            case SDLK_LEFT:
               gui->keyDown[KEY_LEFT] = down;
               break;
            case SDLK_RIGHT:
               gui->keyDown[KEY_RIGHT] = down;
               break;
            case SDLK_SLASH:
               gui->keyDown[KEY_A] = down;
               break;
            case SDLK_PERIOD:
               gui->keyDown[KEY_B] = down;
               break;
            case SDLK_RSHIFT:
               gui->keyDown[KEY_SELECT] = down;
               break;
            case SDLK_RETURN:
               gui->keyDown[KEY_START] = down;
               break;
            default:
               break;
         }
      }
   }
}

void GUI_updateJoypad (GUI gui) {
   /* TODO: Interrupts */
   MMU mmu;
   byte joypad;

   mmu = GB_getMMU (gui->gb);
   joypad = MMU_readByte (mmu, 0xFF00);

   setBit (&joypad, 7);
   setBit (&joypad, 6);
   setBit (&joypad, 3);
   setBit (&joypad, 2);
   setBit (&joypad, 1);
   setBit (&joypad, 0);

   if (!testBit (joypad, 5)) {
      if (gui->keyDown[KEY_A]) clearBit (&joypad, 0);
      if (gui->keyDown[KEY_B]) clearBit (&joypad, 1);
      if (gui->keyDown[KEY_SELECT]) clearBit (&joypad, 2);
      if (gui->keyDown[KEY_START]) clearBit (&joypad, 3);
   } else if (!testBit (joypad, 4)) {
      if (gui->keyDown[KEY_RIGHT]) clearBit (&joypad, 0);
      if (gui->keyDown[KEY_LEFT]) clearBit (&joypad, 1);
      if (gui->keyDown[KEY_UP]) clearBit (&joypad, 2);
      if (gui->keyDown[KEY_DOWN]) clearBit (&joypad, 3);
   }


   MMU_writeByte (mmu, 0xFF00, joypad);
}

Uint8 * GUI_getFramebuffer (GUI gui) {
   return gui->screen->pixels;
}
