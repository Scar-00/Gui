#include "./../internal.h"
#include "GLFW/glfw3.h"
#include "cglm/struct/vec2.h"
#include <time.h>

struct GuiContext *ctx = NULL;
struct GuiTexture arrows[4];

struct GuiTexture blank;

static void gui_glad_init() {
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        fprintf(stderr, "%s",  "error initializing GLAD\n");
        glfwTerminate();
        exit(1);
    }
}

GUI_API bool gui_context_create() {
    gui_glad_init();
    ctx = calloc(1, sizeof(*ctx));
    *ctx = (GuiContext) {
        .initialized = true,
        .windows = gaia_array_create(GuiWindow *, 10),
        .current_window = NULL,
        .window_active = NULL,
        .rendering_data = {0},
        .font = gui_font_init("C:/Users/fiffi/OneDrive/Desktop/Coding/gui/res/fonts/consola.ttf", 16),
        .popups = gaia_array_create(GuiPopup, 10),
        .last_item_added = {0},
        .widget_active_id = 0,
        .panic_handler = NULL,
        .time = clock(),
        .state = 0,
        .io = calloc(1, sizeof(struct GuiIO)),
    };
    ctx->io->window_size = (vec2s){{1280, 720}};
    // arrows[ARROW_RIGHT] = gui_texture_from_path("res/images/right.png");
    // arrows[ARROW_DOWN] = gui_texture_from_path("res/images/down.png");
    return true;
}

GUI_API struct GuiContext *gui_context_get() {
    return ctx;
}

/*static void gui_handle_predefined_elements() {
    struct GuiContext *g = ctx;
    struct GuiIO *io = ctx->io;
    struct GuiWindow *window = g->window_active;
}*/

GUI_API void gui_frame_new() {
    struct GuiContext *g = ctx;
    struct GuiIO *io = ctx->io;
    struct GuiWindow *window_active = g->window_active;
    static vec2s offset;
    if(window_active) {
        if(gui_widget_hovererd(window_active->tmp_data.bb) && window_active->visible && !((window_active->flags & GUI_WINDOW_NO_MOVE) == GUI_WINDOW_NO_MOVE) && !FLAG_CHECK(window_active->flags, GUI_WINDOW_LOCKED)) {
            vec2s mouse_pos = (vec2s){{io->mouse.position.x, io->window_size.y - io->mouse.position.y}};

            if(io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed) {
                offset = glms_vec2_sub(mouse_pos, window_active->pos);
            }

            if(io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].down)
                window_active->pos = glms_vec2_sub(mouse_pos, offset);
        }
    }
    //FIXME time_passed can overflow
    g->io->mouse.delta = glms_vec2_zero();
    g->time_passed = (((f64)clock() - (f64)g->time) / 1000);
}

GUI_API struct GuiWindow *gui_window_current() {
    return ctx->current_window;
}

void gui_item_add(struct GuiRect bb, GuiId id) {
    struct GuiContext *g = ctx;
    g->last_item_added.bb = bb;
    g->last_item_added.id = id;
    g->last_item_added.pos = bb.min;
    g->last_item_added.size = (vec2s){{bb.max.x - bb.min.x, bb.max.y - bb.min.y}};
}

void gui_window_remove(struct GuiWindow *window) {
    gaia_array_loop(ctx->windows, i) {
        if(ctx->windows[i] != NULL && ctx->windows[i]->id == window->id) {
            ctx->windows[i] = NULL;
            for(u32 j = i; j < gaia_array_length(ctx->windows); j++) {
                ctx->windows[j] = ctx->windows[j + 1];
            }
            gaia_array_length(ctx->windows) -= 1;
        }
    }

    gaia_array_loop(ctx->rendering_data.redering_order, i) {
        if(ctx->rendering_data.redering_order[i] != NULL && ctx->rendering_data.redering_order[i]->id == window->id) {
            ctx->rendering_data.redering_order[i] = NULL;
            for(u32 j = i; j < gaia_array_length(ctx->rendering_data.redering_order) - 1; j++) {
                ctx->rendering_data.redering_order[j] = ctx->rendering_data.redering_order[j + 1];
            }
            gaia_array_length(ctx->rendering_data.redering_order) -= 1;
        }
    }

    gui_window_destroy(window);
}
