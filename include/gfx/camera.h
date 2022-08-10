#ifndef _GUI_CAMERA_H_
#define _GUI_CAMERA_H_

#include <gui.h>
#include "renderer.h"

typedef struct GuiCamera {
    GuiViewProj view_proj;
    vec2s pos, min, max;
}GuiCamera;

GUI_API GuiCamera gui_camera_init(vec2s min, vec2s max);
GUI_API void gui_camera_update(GuiCamera *self);

#endif