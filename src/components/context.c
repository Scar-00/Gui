#include "./../internal.h"

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
    ctx->initialized = true;
    ctx->windows = gaia_array_create(struct GuiWindow*, 10);
    ctx->current_window = NULL;
    ctx->window_active = NULL;
    ctx->rendering_data = (struct GuiRenderingData) {0};
    ctx->font = gui_font_init("C:/Users/fiffi/OneDrive/Desktop/Coding/gui/res/fonts/consola.ttf", 16);

    ctx->popups = gaia_array_create(struct GuiPopup, 10);

    ctx->last_item_added = (struct GuiItem){0};
    ctx->widget_active_id = 0;

    ctx->panic_handler = NULL;

    ctx->state = 0;
    ctx->io = calloc(1, sizeof(struct GuiIO));
    ctx->io->window_size = (vec2s){{1280, 720}};

    // arrows[ARROW_RIGHT] = gui_texture_from_path("res/images/right.png"); 
    // arrows[ARROW_DOWN] = gui_texture_from_path("res/images/down.png");
    return true;
}

GUI_API struct GuiContext *gui_context_get() {
    return ctx;
}

GUI_API void gui_frame_new() {
    struct GuiContext *g = ctx;
    struct GuiIO *io = ctx->io;
    struct GuiWindow *window_active = g->window_active;
    static vec2s offset;
    if(window_active) {
        if(gui_widget_hovererd(window_active->tmp_data.bb) && window_active->visible && !window_active->flags.no_move) {
            vec2s mouse_pos = (vec2s){{io->mouse.position.x, io->window_size.y - io->mouse.position.y}};

            if(io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed) {
                offset = glms_vec2_sub(mouse_pos, window_active->pos);
            }

            if(io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].down)
                window_active->pos = glms_vec2_sub(mouse_pos, offset);
        }
    }
}

GUI_API void gui_item_add(struct GuiRect bb, GuiId id) {
    struct GuiContext *g = ctx;
    g->last_item_added.bb = bb;
    g->last_item_added.id = id;
    g->last_item_added.pos = bb.min;
    g->last_item_added.size = (vec2s){{bb.max.x - bb.min.x, bb.max.y - bb.min.y}};
}

GUI_API struct GuiWindow *gui_window_current() {
    return ctx->current_window;
}