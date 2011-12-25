#ifndef _GUI_H_
#define _GUI_H_

#include <SDL.h>
#include "GB_type.h"
#include "GUI_type.h"

#include "types.h"

#define WINDOW_CAPTION "GBEMU"

#define WINDOW_WIDTH 160
#define WINDOW_HEIGHT 144

GUI GUI_init (GB gb);
void GUI_free (GUI gui);
void GUI_update (GUI gui);
Uint32 * GUI_getFramebuffer (GUI gui);
SDL_PixelFormat * GUI_getScreenFormat (GUI gui);

#endif
