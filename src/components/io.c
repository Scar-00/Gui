#include "gui.h"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wgnu-statement-expression"

#include "./../internal.h"

#pragma GCC diagnostic pop

GUI_API struct GuiIO *gui_io_init(void *window) {
    struct GuiIO *io = ctx->io;
    glfwSetCursorPosCallback(window, (GLFWcursorposfun)gui_io_cursor_callback);
    glfwSetKeyCallback(window, (GLFWkeyfun)gui_io_key_callback);
    glfwSetMouseButtonCallback(window, (GLFWmousebuttonfun)gui_io_mouse_callback);
    glfwSetFramebufferSizeCallback(window, (GLFWframebuffersizefun)gui_io_size_callback);
    return io;
}

GUI_API struct GuiIO *gui_io_get() {
    struct GuiIO *io = ctx->io;
    assert(io && "io has not been initialized");
    return io;
}

GUI_API f32 gui_io_get_size_x() {
    return ctx->io->window_size.x;
}

GUI_API f32 gui_io_get_size_y() {
    return ctx->io->window_size.y;
}

GUI_API f64 gui_io_get_time() {
    return ctx->time_passed;
}

GUI_API void gui_io_key_callback(void *handle, int key, int scancode, int action, int mods) {
    struct GuiIO *io = ctx->io;
    if(key < 0) {
        return;
    }

    switch (action) {
        case GLFW_PRESS:
            io->keyboard.keys[key].down = true;
            break;
        case GLFW_RELEASE:
            io->keyboard.keys[key].down = false;
            break;
        default:
            break;
    }
}

GUI_API void gui_io_cursor_callback(void *handle, double mx, double my) {
    struct GuiIO *io = ctx->io;
    vec2s pos = {{mx, my}};

    io->mouse.delta = glms_vec2_sub(pos, io->mouse.position);
    io->mouse.delta.x = clamp(io->mouse.delta.x, -100.0f, 100.0f);
    io->mouse.delta.y = clamp(io->mouse.delta.y, -100.0f, 100.0f);

    io->mouse.position = pos;
}

GUI_API void gui_io_mouse_callback(void *handle, int button, int action, int mods) {
    struct GuiIO *io = ctx->io;
    if(button < 0) {
        return;
    }

    switch (action)
    {
    case GLFW_PRESS:
        io->mouse.buttons[button].down = true;
        break;
    case GLFW_RELEASE:
        io->mouse.buttons[button].down = false;
        break;
    default:
        break;
    }
}

GUI_API void gui_io_size_callback(void *handle, int width, int height) {
    struct GuiIO *io = ctx->io;
    glViewport(0, 0, width, height);
    io->window_size = (vec2s) {{ width, height }};
}

void gui_io_key_update() {
    struct GuiIO *io = ctx->io;

    for(size_t i = 0; i < GLFW_MOUSE_BUTTON_LAST; i++) {
        io->mouse.buttons[i].pressed = io->mouse.buttons[i].down && !io->mouse.buttons[i].last;
        io->mouse.buttons[i].last = io->mouse.buttons[i].down;
    }

    for(size_t i = 0; i < GLFW_KEY_LAST; i++) {
        io->keyboard.keys[i].pressed = io->keyboard.keys[i].down && !io->keyboard.keys[i].last;
        io->keyboard.keys[i].last = io->keyboard.keys[i].down;
    }
}
