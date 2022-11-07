#include "./../internal.h"
#include "GLFW/glfw3.h"
#include "Gaia/io/io.h"
#include "Gaia/io/parse.h"
#include "Gaia/util/array.h"
#include "cglm/types-struct.h"
#include "gui.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

GUI_API bool gui_begin(const char *name, f32 x, f32 y, f32 width, f32 height, bool *open){
    struct GuiContext *g = ctx;
    struct GuiWindow *window = gui_window_get(name);

    if(open != NULL && (*open) == false) {
        if(window) {
            gui_window_remove(window);
        }
        return false;
    }

    vec2s pos = {{x, y}};
    vec2s size = {{width, height}};
    bool window_just_created = (window == NULL);
    if(window_just_created)
        window = gui_window_create(name, pos, size, window_just_created);
    else
        gui_window_update(window, window_just_created);

    g->current_window = window;

    window->has_button = (open != NULL);
    window->flags |= window_just_created ? GUI_WINDOW_JUST_CREATED : GUI_WINDOW_NONE;
    if(open) {
        window->visible = *open;
    }

    if(window->has_button && *open == false)
        return false;

    if((window->flags & GUI_WINDOW_NO_PADDING) == GUI_WINDOW_NO_PADDING)
        window->padding = (vec2s){{0, 0}};

    struct GuiRect tile_bar_bb = {0};
    if((window->flags & GUI_WINDOW_IS_POPUP) || (window->flags & GUI_WINDOW_NO_TILEBAR)) {
        window->tmp_data.bb = (struct GuiRect){.min = window->pos, .max = glms_vec2_add(window->pos, window->size)};
    }else {
        tile_bar_bb = (struct GuiRect){(vec2s){{window->pos.x, window->pos.y + window->size.y}}, glms_vec2_add(window->pos, (vec2s){{window->size.x, window->size.y + 16}})};
        window->tmp_data.bb = (struct GuiRect){.min = window->pos, .max = tile_bar_bb.max};
    }

    window->tmp_data.cursor_pos = window->tmp_data.cursor_start_pos = (vec2s){{window->pos.x, window->pos.y + window->size.y - WIDGET_DEFAULT_HIEGHT}};

    vec4s color = rgb2vec4(21, 22, 23);

    gui_box_add(window->tmp_data.draw_list, window->pos, window->size, color, blank);

    if(!((window->flags & GUI_WINDOW_NO_TILEBAR) == GUI_WINDOW_NO_TILEBAR))
        gui_window_render_tile_bar(window, tile_bar_bb, name, open);

    if(!((window->flags & GUI_WINDOW_NO_BORDER) == GUI_WINDOW_NO_BORDER))
        gui_rect_add(window->tmp_data.draw_list, window->tmp_data.bb, rgb2vec4(115, 140, 153));

    /*(if(FLAG_CHECK(window->flags, GUI_WINDOW_RESIZABLE)) {
        GuiTriangle t = {
            .p0 = {{ window->pos.x + window->size.x - 12, window->pos.y + 1 }},
            .p1 = {{ window->pos.x + window->size.x, window->pos.y +1 }},
            .p2 = {{ window->pos.x + window->size.x, window->pos.y + 13 }},
        };
        gui_triangle_add(window->tmp_data.draw_list, (vec2s){{window->pos.x + window->size.x - 12, window->pos.y + 1}}, (vec2s){{12, 12}}, gui_point_in_triangle(t, (vec2s){{ctx->io->mouse.position.x, ctx->io->mouse.position.y - ctx->io->window_size.y}}) ? rgb2vec4(0, 0, 0) : rgb2vec4(255, 99, 226) , GUI_TRIANGLE_CORNER_BOTTOM_R);
    }*/

    if(window->flags & GUI_WINDOW_AUTO_RESIZE) {
        window->tmp_data.calced_size.y = 0;
        //window->tmp_data.calced_size.x = 0;
    }

    return true;
}

GUI_API void gui_end() {
    struct GuiWindow *window = gui_window_current();
    if(!window)
        return;

    if(window->flags & GUI_WINDOW_AUTO_RESIZE) {
        window->tmp_data.calced_size.y += 8;
        gui_window_set_size(window->tmp_data.calced_size.x, window->tmp_data.calced_size.y);
        window->flags |= gui_vec2s_cmp(window->size, window->tmp_data.calced_size) ? GUI_WINDOW_SIZE_CALCED : GUI_WINDOW_NONE;
    }
}

GUI_API void gui_window_set_size(f32 width, f32 height) {
    gui_window_current()->size = (vec2s){{width, height}};
}

GUI_API void gui_window_flag_set(struct GuiWindow *window, GuiWindowFlags flag) {
    FLAG_SET(window->flags, flag);
}

GUI_API void gui_window_flag_clear(struct GuiWindow *window, GuiWindowFlags flag) {
    FLAG_CLEAR(window->flags, flag);
}

GUI_API bool gui_window_flag_check(struct GuiWindow *window, GuiWindowFlags flag) {
    return FLAG_CHECK(window->flags, flag);
}

struct GuiWindow *gui_window_create(const char *name, vec2s pos, vec2s size, bool just_created) {
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
            .menus = gaia_array_create(GuiMenu, 1),
            .trees = gaia_array_create(GuiTree, 1),
            .draw_index = gaia_array_length(ctx->rendering_data.redering_order),
            .draw_list = gui_draw_list_create(),
        },
        .has_button = false,
        .visible = true,
        .padding = (vec2s) {{4.0f, -4.0f}},
        .scroll = 0,
        .flags = 0,
    };
    self->flags |= just_created ? GUI_WINDOW_JUST_CREATED : GUI_WINDOW_NONE;
    struct GuiWindowSaveData data = gui_window_load(self->name);
    if(data.populated) {
        self->pos = data.pos;
        self->size = data.size;
        printf("[pos] x: %f | y: %f\n", data.pos.x, data.pos.y);
        printf("[size] x: %f | y: %f\n", data.size.x, data.size.y);
    }
    gaia_array_pushback(ctx->rendering_data.redering_order, self);
    gaia_array_pushback(ctx->windows, self);
    return self;
}

void gui_window_destroy(struct GuiWindow *window) {
    gaia_array_destroy(window->tmp_data.draw_list->vertex_buffer);
    gaia_array_destroy(window->tmp_data.draw_list->index_buffer);
    free(window->tmp_data.draw_list);
    free(window);
    window = NULL;
}

struct GuiWindowSaveData gui_window_load(String name) {
    GaiaConfFile data = gaia_conf_file_parse_c("save.gaia");

    //gaia_array(GaiaTable) windows = gaia_conf_file_get_table_all(data);

    /*
    gaia_array_foreach(windows, window) {
        printf("%s\n", window->name.c_str);
        gaia_array_foreach(window->suffixes, s) {
            printf("s: %s -> %s\n", s->key.c_str, s->value.c_str);
        }
        gaia_array_foreach(window->fields, f) {
            printf("f: %s -> %s\n", f->key.c_str, f->value.c_str);
        }
    }

    if(!gaia_conf_file_has_entry(data, "%s|id", name.c_str)) {
        return (struct GuiWindowSaveData){0};
    }
    */

    vec2s pos = {{-1, -1}};
    vec2s size = {{-1, -1}};
    GuiWindowFlags flags = 0;

    struct GuiWindowSaveData self = {
        .pos = pos,
        .size = size,
        .flags = flags,
        .populated = false,
    };


    if(!gaia_conf_file_has_entry(data, "%s/size_x", name.c_str)) {
        return self;
    }

    size.x = atof(gaia_conf_file_get_entry(data, "%s/size_x", name.c_str).c_str);
    size.y = atof(gaia_conf_file_get_entry(data, "%s/size_y", name.c_str).c_str);
    pos.x = atof(gaia_conf_file_get_entry(data, "%s/pos_x", name.c_str).c_str);
    pos.y = atof(gaia_conf_file_get_entry(data, "%s/pos_y", name.c_str).c_str);

    self.size = size;
    self.pos = pos;
    self.populated = !gui_vec2s_cmp(self.pos, (vec2s){{-1, -1}}) && !gui_vec2s_cmp(self.size, (vec2s){{-1, -1}});

    gaia_conf_file_free(data);

    return self;
}

void gui_window_save(struct GuiWindow *window) {
    GaiaConfFile save_file = gaia_conf_file_parse_c("save.gaia");

    if(gaia_conf_file_has_entry(save_file, "%s|id")) {
        return;
    }
}

struct GuiWindow *gui_window_get(const char *name) {
    struct GuiContext *g = ctx;
    for(u32 i = 0; i < gaia_array_length(g->windows); i++) {
        if(g->windows[i]->id == gui_hash(name))
            return g->windows[i];
    }
    return NULL;
}

bool gui_is_clickable() {
    struct GuiWindow *window = gui_window_current();
    struct GuiContext *g = ctx;
    bool clickable = true;

    for(size_t i = 0; i < window->tmp_data.child_windows_count; i++) {
        if(gui_widget_hovererd(window->tmp_data.bb) && gui_widget_hovererd(window->tmp_data.child_windows[i]->tmp_data.bb))
            clickable = false;
    }

    for(u32 i = window->tmp_data.draw_index; i < gaia_array_length(g->rendering_data.redering_order); i++) {
        if(g->rendering_data.redering_order[i]->visible
            && !((g->rendering_data.redering_order[i]->flags & GUI_WINDOW_IS_POPUP) == GUI_WINDOW_IS_POPUP)
            && gui_widget_hovererd(g->rendering_data.redering_order[i]->tmp_data.bb)
            && g->rendering_data.redering_order[i]->tmp_data.draw_index > window->tmp_data.draw_index
        ) {
            clickable = false;
        }
    }
    return clickable;
}

void gui_window_update(struct GuiWindow *window, bool just_created) {
    struct GuiContext *g = ctx;
    struct GuiIO *io = g->io;
    bool clickable = true;

    // FIXME:[Code duplication -> gui_is_clickable()]
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
        if(!((window->flags & GUI_WINDOW_NO_SORT) == GUI_WINDOW_NO_SORT))
            gui_window_bring_to_front(window);
    }

    //GuiTriangle resizer = {
    //    .p0 = {{window->pos.x + window->size.x - 12, window->pos.y + 1}},
    //    .p1 = {{window->pos.x + window->size.x, window->pos.y + 1}},
    //    .p2 = {{window->pos.x + window->size.x, window->pos.y + 13}},
    //};
    //if(clickable && gui_point_in_triangle(resizer, (vec2s){{io->mouse.position.x, io->mouse.position.y - io->window_size.y}}) && window->visible) {
    //    if(io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].down) {
    //        printf("test\n");
    //    }
    //}
}

// TODO: cut window name if name collides|is to long with exit button/window width
void gui_window_render_tile_bar(struct GuiWindow *window, struct GuiRect bb, const char *name, bool *open) {
    struct GuiContext *g = ctx;
    const bool has_close_button = (open != NULL);
    vec2s close_button_pos = {0};
    vec4s color = rgb2vec4(10, 10, 10);

    if(g->window_active == window)
        color = rgb2vec4(25, 70, 135);

    gui_box_add(window->tmp_data.draw_list, bb.min, (vec2s){{window->size.x, 16.0f}}, color, blank);
    if(name)
        gui_text_add(window->tmp_data.draw_list, (vec2s){{ bb.min.x + 2, bb.min.y + 4}}, 1, name);

    if(has_close_button) {
        close_button_pos = (vec2s) {{ bb.max.x - 13, bb.min.y + 2 }};
        if(gui_close_button(close_button_pos))
            *open = false;
    }
}

void gui_window_cursor_advance() {
    struct GuiWindow *window = gui_window_current();
    window->tmp_data.cursor_pos.y -= WIDGET_DEFAULT_HIEGHT + WIDGET_DEFAULT_PADDING_Y;
    window->tmp_data.cursor_pos.x = window->pos.x;
    if(gui_tree_current()) {
        window->tmp_data.cursor_pos.x += gui_tree_current()->depth * 10;
    }
}

void gui_window_child_push(struct GuiWindow *window, struct GuiWindow *window_child) {
    assert(window->tmp_data.child_windows_count < WINDOW_MAX_CHILDREN && "window cannot handle more then 6 child processes");
    window->tmp_data.child_windows[window->tmp_data.child_windows_count] = window_child;
    window->tmp_data.child_windows_count++;
}

struct GuiWindow *gui_window_child_pop(struct GuiWindow *window) {
    assert(window->tmp_data.child_windows_count > 0 && "cannot pop child window from empty stack");
    return window->tmp_data.child_windows[window->tmp_data.child_windows_count--];
}
