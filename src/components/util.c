#include <time.h>

#include "./../internal.h"

GUI_API GuiId gui_hash(const char* name) {
    GuiId value = 0;
    for(size_t i = 0; i < strlen(name); i++) {
        value += (name[i] * 0x946443) << 6 ;
    }

    return value;
}

GUI_API bool gui_widget_hovererd(struct GuiRect bb) {
    struct GuiIO *io = ctx->io;
    f32 mouse_y = io->window_size.y - io->mouse.position.y;
    return io->mouse.position.x > bb.min.x && io->mouse.position.x < bb.max.x &&
        mouse_y > bb.min.y && mouse_y < bb.max.y;
}

GUI_API const char *gui_get_char_from_key() {
    struct GuiIO *io = ctx->io;
    const char *c = NULL;
    for (size_t i = 0; i < GLFW_KEY_LAST; i++) {
        if(io->keyboard.keys[i].pressed) {
            c = glfwGetKeyName(i, 0);
        }
    }
    return c;
}

GUI_API u32 rand_range(u32 lower, u32 upper) {
    // srand(time(0));
    return (rand() % (upper - lower + 1) + lower);
}

GUI_API inline const char *gui_error_to_string(GuiErrorType error) {
    static char *error_messages[] = {
        "Widget out of bounds",
        "Expected non zero value",
    };
    
    return error_messages[error];
}

GUI_API void gui_panic_expl(GuiErrorType error, u32 line, char *file, const char *format, ...) {
    struct GuiContext *g = ctx;

    va_list args;
    va_start(args, format);
    String message = gaia_string_init_va(format, args);
    va_end(args);

    if(g->panic_handler) {
        g->panic_handler(gui_error_to_string(error), message.c_str, line, file);
        return;
    }

    fprintf(stderr, "[%s] --> \"%s\"|%d: %s|\n", gui_error_to_string(error), message.c_str, line, file);
    exit(error);
}

GUI_API void gui_panic_handler_set(error_handler handler) {
    struct GuiContext *g = ctx;

    if(!g)
        return;

    g->panic_handler = handler;
}

GUI_API bool gui_window_just_created(struct GuiWindow *window) {
    return window->flags.just_created;
}

GUI_API inline vec4s rgb2vec4(float r, float g, float b) {
    return (vec4s){{(float)r / (float)255, (float)g / (float)255, (float)b / (float)255, 1.0f}};
}

GUI_API inline bool gui_vec2s_cmp(vec2s a, vec2s b) {
    return a.x == b.x && a.y == b.y;
}