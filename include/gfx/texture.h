#ifndef _GUI_TEXTURE_H_
#define _GUI_TEXTURE_H_

#include <gui.h>

typedef struct GuiTexture {
    GLuint handle;
    vec2s size;
}GuiTexture;

GUI_API void gui_texture_pixels_load(const char *path, u8 **pixels_out, u32 *width_out, u32 *height_out);
GUI_API struct GuiTexture gui_texture_from_pixels(u8 *pixels, u32 width, u32 height);
GUI_API struct GuiTexture gui_texture_from_font(u8 *pixels, u32 width, u32 height);
GUI_API struct GuiTexture gui_texture_from_path(const char *path);

GUI_API void gui_texture_destroy(struct GuiTexture self);
GUI_API void gui_texture_bind(struct GuiTexture self);

#endif