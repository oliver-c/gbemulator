#ifndef _GUI_H_
#define _GUI_H_

#include "GB_type.h"
#include "GUI_type.h"

#include "types.h"

#define WINDOW_CAPTION "GBEMU"

#define WINDOW_WIDTH 160
#define WINDOW_HEIGHT 144

typedef struct colour {
   byte r;
   byte g;
   byte b;
} colour;

GUI GUI_init (GB gb);
void GUI_free (GUI gui);
void GUI_update (GUI gui);
colour * GUI_getFramebuffer (GUI gui);

#endif
