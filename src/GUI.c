#include <stdlib.h>
#include <assert.h>

#include <SDL.h>

#include "GB.h"
#include "GUI.h"

#include "types.h"

struct GUI {
   GB gb;
   SDL_Surface *screen;
   colour framebuffer[WINDOW_WIDTH * WINDOW_HEIGHT];
};

void GUI_handleEvents (GUI gui);

GUI GUI_init (GB gb) {
   GUI newGUI = (GUI)malloc(sizeof(struct GUI));
   assert (newGUI != NULL);

   newGUI->gb = gb;

   SDL_Init (SDL_INIT_VIDEO); 
   
   SDL_WM_SetCaption (WINDOW_CAPTION, NULL); 
   newGUI->screen = SDL_SetVideoMode (WINDOW_WIDTH, WINDOW_HEIGHT, 0, 
                                      SDL_DOUBLEBUF | SDL_HWPALETTE);

   return newGUI;
}

void GUI_free (GUI gui) {
   SDL_FreeSurface (gui->screen);

   free (gui);
}

void GUI_update (GUI gui) {
   GUI_handleEvents (gui);
}

void GUI_handleEvents (GUI gui) {
   SDL_Event event;

   while (SDL_PollEvent (&event)) {
      if (event.type == SDL_QUIT) {
         GB_setRunning (gui->gb, FALSE);
      }
   }
}

colour * GUI_getFramebuffer (GUI gui) {
   return gui->framebuffer;
}
