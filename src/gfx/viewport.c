#include "../internal.h"

GuiViewPort gui_viewport_init(vec2s pos, vec2s size) {
    return (GuiViewPort){.pos = pos, .size = size};
}

GUI_API void gui_viewport_begin(u32 x, u32 y, u32 width, u32 height) {
    ctx->rendering_data.view_port = gui_viewport_init((vec2s){{x, y}}, (vec2s){{width, height}});
}

GUI_API void gui_viewport_end() {
    ctx->rendering_data.view_port = gui_viewport_init((vec2s){{0, 0}}, ctx->io->window_size);
}
