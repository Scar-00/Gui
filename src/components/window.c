#include "./../internal.h"

GUI_API struct GuiWindow *gui_window_get(const char *name) {
    struct GuiContext *g = ctx;
    for(u32 i = 0; i < gaia_array_length(g->windows); i++) {
        if(g->windows[i]->id == gui_hash(name))
            return g->windows[i];
    }
    return NULL;
}

//TODO: determine weather window should close and get deallocated (-> using should_close flag) 
GUI_API bool gui_begin(const char *name, vec2s pos, vec2s size, bool *open) {
     struct GuiContext *g = ctx;
    struct GuiWindow *window = gui_window_get(name);

    bool window_just_created = (window == NULL);
    if(window_just_created)
        window = gui_window_create(name, pos, size, window_just_created);
    else
        gui_window_update(window, window_just_created);

    g->current_window = window;

    window->has_button = (open != NULL);
    window->flags.just_created = window_just_created;

    if(open)
        window->visible = *open;

    if(window->has_button && *open == false)
        return false;

    // if(window->flags.is_popup)
    //     window->pos = pos;

    if(window->flags.no_padding)
        window->padding = (vec2s){{0, 0}};

    struct GuiRect tile_bar_bb = {0};
    if(window->flags.is_popup || window->flags.no_tilebar) {
        window->tmp_data.bb = (struct GuiRect){.min = window->pos, .max = glms_vec2_add(window->pos, window->size)};
    }else {
        tile_bar_bb = (struct GuiRect){(vec2s){{window->pos.x, window->pos.y + window->size.y}}, glms_vec2_add(window->pos, (vec2s){{window->size.x, window->size.y + 16}})};
        window->tmp_data.bb = (struct GuiRect){.min = window->pos, .max = tile_bar_bb.max};
    }

    window->tmp_data.cursor_pos = window->tmp_data.cursor_start_pos = (vec2s){{window->pos.x, window->pos.y + window->size.y - WIDGET_DEFAULT_HIEGHT}};

    vec4s color = rgb2vec4(21, 22, 23);

    gui_box_add(window->tmp_data.draw_list, window->pos, window->size, color, blank);

    if(!window->flags.no_tilebar)
        gui_window_render_tile_bar(window, tile_bar_bb, name, open);

    if(!window->flags.no_border)
        gui_rect_add(window->tmp_data.draw_list, window->tmp_data.bb, rgb2vec4(115, 140, 153));

    if(window->flags.auto_rezize) {
        window->tmp_data.calced_size.y = 0;
    }

    return true;
}

GUI_API void gui_end() {
    struct GuiWindow *window = gui_window_current();
    if(window->flags.auto_rezize) {
        window->tmp_data.calced_size.y += 8;
        gui_window_set_size(window->tmp_data.calced_size);
        window->flags.size_calced = gui_vec2s_cmp(window->size, window->tmp_data.calced_size);
    }
}

GUI_API struct GuiWindow *gui_window_create(const char *name, vec2s pos, vec2s size, bool just_created) {
    struct GuiWindow *self = calloc(1, sizeof(struct GuiWindow));
    *self = (struct GuiWindow){ 
        .name = gaia_string_init(name),
        .pos = pos,
        .size = size,
        .id = gui_hash(name),
        .tmp_data = {
            .next_item_data = {
                .pos = {{-1, -1}},
                .size = {{-1, -1}},
            },
            .calced_size = (vec2s){{ size.x, 0 }},
            .draw_index = gaia_array_length(ctx->rendering_data.redering_order),
            .draw_list = gui_draw_list_create(),
        },
        .has_button = false,
        .visible = true,
        .padding = (vec2s) {{4.0f, -4.0f}},
        .flags = {
            .just_created = just_created,
        },
    };
    gaia_array_pushback(ctx->rendering_data.redering_order, self);
    gaia_array_pushback(ctx->windows, self);
    return self;
}

GUI_API bool gui_is_clickable() {
    struct GuiWindow *window = gui_window_current();
    struct GuiContext *g = ctx;
    bool clickable = true;

    for(size_t i = 0; i < window->tmp_data.child_windows_count; i++) {
        if(gui_widget_hovererd(window->tmp_data.bb) && gui_widget_hovererd(window->tmp_data.child_windows[i]->tmp_data.bb))
            clickable = false;
    }

    for(u32 i = window->tmp_data.draw_index; i < gaia_array_length(g->rendering_data.redering_order); i++) {
        if(g->rendering_data.redering_order[i]->visible 
            && !g->rendering_data.redering_order[i]->flags.is_popup 
            && gui_widget_hovererd(g->rendering_data.redering_order[i]->tmp_data.bb) 
            && g->rendering_data.redering_order[i]->tmp_data.draw_index > window->tmp_data.draw_index
        ) {
            clickable = false;
        }
    }
    return clickable;
}

GUI_API void gui_window_update(struct GuiWindow *window, bool just_created) {
    struct GuiContext *g = ctx;
    struct GuiIO *io = g->io;
    bool clickable = true;

    // FIXME:[Code duplication -> gui_is_clickable()]
    // maybe this does now work properly <- look
    // child windows(popups) are not checked by other windows
    for(size_t i = 0; i < window->tmp_data.child_windows_count; i++) {
        if(gui_widget_hovererd(window->tmp_data.bb) && gui_widget_hovererd(window->tmp_data.child_windows[i]->tmp_data.bb))
            clickable = false;
    }

    if(clickable && gui_widget_hovererd(window->tmp_data.bb) && window->visible && io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed) {
        for(u32 i = window->tmp_data.draw_index; i < gaia_array_length(g->rendering_data.redering_order); i++) {
            if(g->rendering_data.redering_order[i]->visible 
                // && !g->rendering_data.redering_order[i]->flags.is_popup | TODO: find out why this was necissanry again
                && gui_widget_hovererd(g->rendering_data.redering_order[i]->tmp_data.bb) 
                && g->rendering_data.redering_order[i]->tmp_data.draw_index > window->tmp_data.draw_index
            ) {
                clickable = false;
            }
        }
    }

    if(clickable && gui_widget_hovererd(window->tmp_data.bb) && window->visible && io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed) {
        g->window_active = window;

        if(!window->flags.no_sort)
            gui_window_bring_to_front(window);
    }
}

GUI_API void gui_window_render_tile_bar(struct GuiWindow *window, struct GuiRect bb, const char *name, bool *open) {
    struct GuiContext *g = ctx;
    const bool has_close_button = (open != NULL);
    vec2s close_button_pos;
    vec4s color = rgb2vec4(10, 10, 10);

    if(g->window_active == window)
        color = rgb2vec4(25, 70, 135);

    gui_box_add(window->tmp_data.draw_list, bb.min, (vec2s){{window->size.x, 16.0f}}, color, blank);
    if(name) 
        gui_text_add(window->tmp_data.draw_list, (vec2s){{ bb.min.x + 2, bb.min.y + 4}}, 1, name);

    if(has_close_button){
        close_button_pos = (vec2s) {{ bb.max.x - 13, bb.min.y + 2 }};
    }

    if(has_close_button)
        if(gui_close_button(close_button_pos))
            *open = false;
}

GUI_API void gui_window_cursor_advance() {
    struct GuiWindow *window = gui_window_current();
    window->tmp_data.cursor_pos.y -= WIDGET_DEFAULT_HIEGHT + WIDGET_DEFAULT_PADDING_Y; 
    window->tmp_data.cursor_pos.x = window->pos.x;
    // window->tmp_data.cursor_pos.x = window->tree_active ? window->pos.x + 8 : window->pos.x;
}

GUI_API void gui_window_set_size(vec2s size) {
    gui_window_current()->size = size;
}

void gui_window_interaction_register(const char *name, IntFunc func, void *args) {
    struct GuiWindow *window = gui_window_current();
    assert(window->interactions_count < WINDOW_MAX_INTERACTIONS && "maximum number of window interactions reached(see WINDOW_MAX_INTERACTIONS)");
    window->interactions[window->interactions_count++] = (struct GuiWindowInteraction){.name = gaia_string_init(name), .fn = func, .args = args};
}

GUI_API void gui_window_child_push(struct GuiWindow *window, struct GuiWindow *window_child) {
    assert(window->tmp_data.child_windows_count < WINDOW_MAX_CHILDREN && "window cannot handle more then 6 child processes");
    window->tmp_data.child_windows[window->tmp_data.child_windows_count] = window_child;
    window->tmp_data.child_windows_count++;
}

GUI_API struct GuiWindow *gui_window_child_pop(struct GuiWindow *window) {
    assert(window->tmp_data.child_windows_count > 0 && "cannot pop child window from empty stack");
    return window->tmp_data.child_windows[window->tmp_data.child_windows_count--];
}