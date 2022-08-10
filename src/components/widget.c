#include "./../internal.h"

#define MOUSE_Y ctx->io->window_size.y - ctx->io->mouse.position.y

// TODO: add functions to actually set these values
static inline vec2s gui_widget_calc_pos(struct GuiWindow *window) {
    if(window->tmp_data.next_item_data.pos.x >= 0.0f && window->tmp_data.next_item_data.pos.y >= 0.0f) {
        //TODO: make this relative to the window
        window->tmp_data.next_item_data.pos = (vec2s){{-1, -1}};
        return window->tmp_data.next_item_data.pos;
    }

    vec2s pos = glms_vec2_add(window->tmp_data.cursor_pos, window->padding);
    gui_window_cursor_advance();
    return pos;
}

static inline vec2s gui_widget_calc_size(struct GuiWindow *window, String name) {
    //FIXME: limit the size to the parent window
    vec2s size = {0};

    if(window->tmp_data.next_item_data.size.x >= 0.0f) {
        size.x = window->tmp_data.next_item_data.size.x;
    }else {
        size.x = (name.length  * 9) + 4;
    }

    if(window->tmp_data.next_item_data.size.y >= 0.0f) {
        size.y = window->tmp_data.next_item_data.size.y;
    }else {
        size.y = WIDGET_DEFAULT_HIEGHT;
    }
    if(gui_vec2s_cmp(size, (vec2s){{0, 0}}))
        gui_panic(EXPECT_NO_ZERO, "invalid size for widget[%s]", name.c_str);

    window->tmp_data.next_item_data.size = (vec2s){{-1, -1}};
    return size;
}

GUI_API struct GuiItem gui_widget_data_get(String name) {
    struct GuiWindow *window = gui_window_current();
    struct GuiItem self = {0};

    self.id = gui_hash(name.c_str);

    self.pos = gui_widget_calc_pos(window);

    self.size = gui_widget_calc_size(window, name);

    //determine weather widget should be centered and center it 
    if(window->flags.widget_centered) {
        self.pos.x += window->padding.x;
        self.size.x = window->size.x - (window->padding.x * 4);
    }

    self.bb = (struct GuiRect){.min = self.pos, .max = glms_vec2_add(self.pos, self.size)};

    // TODO: account for horizontal expantion/shrinking
    // if pos/size is usedefined let use handle bounds checking etc.
    if(window->flags.auto_rezize) {
        window->tmp_data.calced_size.y += self.size.y;
        if(!window->flags.no_padding) {
            window->tmp_data.calced_size.y -= window->padding.y;
        }
    }

    gui_item_add(self.bb, self.id);
    return self;
}

GUI_API bool gui_button(const char *label) {
    struct GuiWindow *window = gui_window_current();

    if(!window || !window->visible)
        return false;

    assert(label && "you must provoide a label for widget 'gui_button'");

    struct GuiItem data = gui_widget_data_get(gaia_string_init(label));

    vec4s color = rgb2vec4(173, 14, 143);

    bool hovered;
    bool pressed = gui_button_behavior(data.bb, &hovered);

    if(hovered)
        color = rgb2vec4(255, 99, 226);

    gui_box_add(window->tmp_data.draw_list, data.pos, data.size, color, blank);
    gui_text_add(window->tmp_data.draw_list, (vec2s){{data.pos.x + 2, data.pos.y + data.size.y / 4}}, 1, label);

    return pressed;
}

GUI_API bool gui_close_button(vec2s pos) {
    struct GuiWindow *window = gui_window_current();
    struct GuiRect bb = {.min = pos, .max = glms_vec2_add(pos, (vec2s){{ 16.0f, 16.0f }})};

    bool hovered;
    bool pressed = gui_button_behavior(bb, &hovered);

    gui_text_add(window->tmp_data.draw_list, pos, 1, "X");

    return pressed;
}

GUI_API bool gui_button_behavior(struct GuiRect bb, bool *out_hovererd) {
    struct GuiWindow *window = gui_window_current();
    struct GuiIO *io = ctx->io;
    if(!window)
        return false;
    
    bool pressed = false;
    bool hovererd = gui_widget_hovererd(bb);

    if(!gui_is_clickable())
        hovererd = false;

    if(hovererd) {
        if(io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed) {
            pressed = true;
        }
    }

    if(out_hovererd)
        *out_hovererd = hovererd;

    return pressed;
}

GUI_API void gui_same_line() {
    struct GuiWindow *window = gui_window_current();
    window->tmp_data.cursor_pos.y += WIDGET_DEFAULT_HIEGHT + WIDGET_DEFAULT_PADDING_Y;
    window->tmp_data.cursor_pos.x += ctx->last_item_added.size.x + 10;
}

GUI_API void gui_text(const char *format, ...) {
    struct GuiWindow *window = gui_window_current();
    if(!window || !window->visible)
        return;

    assert(format && "you must provoide a name for widget 'gui_text'");
   
    va_list args;
    va_start(args, format);
    String str = gaia_string_init_va(format, args);
    va_end(args);

    struct GuiItem data = gui_widget_data_get(str);

    gui_text_add(window->tmp_data.draw_list, (vec2s){{data.pos.x + 2, data.pos.y + data.size.y / 4}}, 1, str.c_str);
}

GUI_API void gui_text_wraped(const char *format, ...) {
    assert(false && "'gui_text_wraped' is not implemented");
    struct GuiWindow *window = gui_window_current();
    if(!window || !window->visible)
        return;
    
    assert(format && "you must provoide a name for widget 'gui_text'");

    // va_list args;
    // va_start(args, format);
    // StringStream stream = gaia_stringstream_init_va(format, args);
    // va_end(args);

    // struct GuiItem data = gui_widget_data_get(str);


}

GUI_API bool gui_selectable(const char *label) {
    struct GuiWindow *window = gui_window_current();
    if(!window || !window->visible)
        return false;

    assert(label && "you must provoide a name for widget 'gui_selectable'");

    struct GuiItem data = gui_widget_data_get(gaia_string_init(label));

    vec4s color = rgb2vec4(21, 22, 23);

    bool hovered;
    bool pressed = gui_button_behavior(data.bb, &hovered);

    if(hovered)
        color = rgb2vec4(255, 99, 226);

    gui_box_add(window->tmp_data.draw_list, data.pos, data.size, color, blank);
    gui_text_add(window->tmp_data.draw_list, (vec2s){{data.pos.x + 2, data.pos.y + data.size.y / 4}}, 1, label);

    return pressed;
}

GUI_API void gui_seperator(s32 len) {
    struct GuiWindow *window = gui_window_current();
    if(!window || !window->visible)
        return;

    struct GuiItem data = gui_widget_data_get(gaia_string_init(" "));  
    data.pos.y += data.size.y / 2;
    
    vec2s size = (vec2s) {{ WIDGET_DEFAULT_WIDTH, 1 }}; 
    if(len != 0 && !window->flags.widget_centered)
        size.x = len;

    gui_box_add(window->tmp_data.draw_list, data.pos, size, rgb2vec4(255, 255, 255), blank);
}

GUI_API void gui_text_input(const char *label, char *buffer, vec2s size) {
    struct GuiContext *g = ctx;
    struct GuiIO *io = ctx->io;
    struct GuiWindow *window = gui_window_current();

    if(!window || !window->visible)
        return;

    size_t buffer_len = strlen(buffer);

    struct GuiItem data = gui_widget_data_get(gaia_string_init(label));

    vec2s size_i = size;

    size_t label_len = 0;
    if(label)
        label_len = strlen(label);

    if(window->flags.widget_centered) {
        data.pos.x += 8;
        size_i.x = window->size.x - 16;
        size_i.x -= label_len * 8;
    }


    struct GuiRect bb_abs = {.min = data.pos, .max = glms_vec2_add(data.pos, size)};

    vec4s color = rgb2vec4(173, 14, 143);

    gui_box_add(window->tmp_data.draw_list, data.pos, size_i, color, blank);
    gui_text_add(window->tmp_data.draw_list, (vec2s){{data.pos.x + 2, data.pos.y + size_i.y / 4}}, 1, buffer);

    if(label)
        gui_text_add(window->tmp_data.draw_list, (vec2s) {{ (data.pos.x + size_i.x) + 4, data.pos.y + size_i.y / 4 }}, 1, label);

    vec2s cursor_pos = (vec2s) {{ data.pos.x + 2 + (buffer_len * 9), data.pos.y + 4 }};

    if(gui_button_behavior(bb_abs, NULL)) {
        g->widget_active_id = data.id;
    }

    if(g->widget_active_id == data.id) {
        if(!gui_widget_hovererd(data.bb) && (io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed))
            g->widget_active_id = 0;

        const char *c = gui_get_char_from_key();
        if(io->keyboard.keys[GLFW_KEY_BACKSPACE].pressed) {
            if(buffer_len > 0)
                buffer[buffer_len - 1] = 0;
        }else if(io->keyboard.keys[GLFW_KEY_SPACE].pressed && buffer_len < 32) {
            buffer[buffer_len] = ' ';
        }
        if(c && buffer_len < 32) {
            buffer[buffer_len] = c[0];
        }

        // FIXME: cringe
        if(g->state < 60) {
            gui_box_add(window->tmp_data.draw_list, cursor_pos, (vec2s){{ 1, 10 }}, (vec4s){{ 1, 1, 1, 1 }}, blank);
        }else if(g->state > 120) {
            g->state = 0;
        }
        g->state++;
    }
}

GUI_API bool gui_item_hovererd() {
    struct GuiContext *g = ctx;
    bool hovered = false;
    if(gui_widget_hovererd(g->last_item_added.bb))
        hovered = true;
    
    return hovered;
}

// FIXME: reorder draw data to avoid clipped display - > next items data has to be changed
GUI_API void gui_tooltip_set(const char *format, ...) {
    struct GuiIO *io = gui_io_get();
    struct GuiWindow *window = gui_window_current();
    vec2s pos = {{io->mouse.position.x + 15, MOUSE_Y - 15}};
    
    va_list args;
    va_start(args, format);
    String str = gaia_string_init_va(format, args);
    va_end(args);
    
    vec2s size = {{ str.length * 8 + 16, 20 }};

    struct GuiRect bb = {.min = pos, .max = glms_vec2_add(pos, size)};

    gui_box_add(window->tmp_data.draw_list, pos, size, rgb2vec4(21, 22, 23), blank);
    gui_text_add(window->tmp_data.draw_list, (vec2s){{pos.x + 8, pos.y + 6}}, 1, str.c_str);
    gui_rect_add(window->tmp_data.draw_list, bb, rgb2vec4(115, 140, 153));
}

#define SLIDER_LEN 600
#define SLIDER_PRECISION 1000
#define SLIDER_SIZE 6
GUI_API bool gui_slider_f32(const char *name, f32 *value, f32 lower, f32 upper) {
    struct GuiIO *io = ctx->io;
    struct GuiItem data = gui_widget_data_get(gaia_string_init(name));
    struct GuiWindow *window = gui_window_current();
    // u32 base_len = data.size; 
    data.size.x = SLIDER_LEN;
    data.bb.max.x = data.bb.min.x + SLIDER_LEN;
    gui_item_add(data.bb, data.id);

    String value_string = gaia_string_init("%.2f", *value);

    f32 slider_value = ((*value - lower) / (upper - lower)) * SLIDER_LEN - 8;
    vec2s slider_pos = {{ 4 + data.pos.x + slider_value, data.pos.y + 2 }};
    slider_pos.x = clamp(slider_pos.x, data.pos.x + 4, data.pos.x + SLIDER_LEN - 4); 

    bool hovered = false;
    bool pressed = gui_button_behavior(data.bb, &hovered);

    bool window_no_move = window->flags.no_move;

    if(hovered && io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].down) {
        if(!window_no_move)
            window->flags.no_move = true;
        
        *value = ((io->mouse.position.x - data.bb.min.x) / (data.bb.max.x - data.bb.min.x)) * (upper - lower) + lower;
    }

    if(!window_no_move && !io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].down)
        window->flags.no_move = false;

    vec4s color = hovered ? rgb2vec4(255, 99, 226) : rgb2vec4(173, 14, 143);
    
    gui_rect_add(window->tmp_data.draw_list, data.bb, rgb2vec4(115, 140, 153));
    gui_box_add(window->tmp_data.draw_list, slider_pos, (vec2s){{SLIDER_SIZE, data.size.y - 4}}, color, blank);
    gui_text_add(window->tmp_data.draw_list, (vec2s){{data.pos.x + data.size.x + 4, data.pos.y + data.size.y / 4}}, 1, name);
    gui_text_add(window->tmp_data.draw_list, (vec2s){{(data.pos.x + (data.size.x / 2)) - WIDGET_SIZE_FROM_STRING(value_string) / 2 + 6, data.pos.y + data.size.y / 4}}, 1, value_string.c_str);
    return true;
}

// popups
// maybe also populate the width with a calculated value
#define POPUP_DEFAULT_WIDTH 150
static vec2s gui_popup_calc_pos (struct GuiWindow *parent_window) {
    struct GuiIO *io = ctx->io;
    vec2s default_pos = (vec2s){{ io->mouse.position.x, MOUSE_Y }};


    return default_pos;
}

GUI_API void gui_popup_open(const char *popup_name) {
    struct GuiContext *g = ctx;
    struct GuiWindow *window = gui_window_current();
    GuiId id = gui_hash(popup_name);
    if(gui_popup_is_open(id))
        return;

    struct GuiPopup popup_ref;
    popup_ref.id = id;
    popup_ref.parent_window = window;
    popup_ref.just_opened = true;
    popup_ref.should_close = false;
    popup_ref.pos = gui_popup_calc_pos(window);

    gaia_array_pushback(g->popups, popup_ref);
}

GUI_API bool gui_popup_is_open(GuiId id) {
    struct GuiContext *g = ctx;
    bool open = false;
    for(size_t i = 0; i < gaia_array_length(g->popups); i++) {
        if(g->popups[i].id == id){
            open = true;
        }
    }
    return open; 
}

GUI_API struct GuiPopup *gui_popup_get(GuiId id) {
    struct GuiContext *g = ctx;
    for(size_t i = 0; i < gaia_array_length(g->popups); i++) {
        if(g->popups[i].id == id){
            return &g->popups[i];
        }
    }
    return NULL;
}

// FIXME: breaks if parent_window is popup 
GUI_API bool gui_popup_begin(const char *name) {
    struct GuiContext *g = ctx;
    struct GuiIO *io = ctx->io;
    GuiId id = gui_hash(name);
    bool popup_open = gui_popup_is_open(id); 
    struct GuiPopup *popup = gui_popup_get(id);

    if(popup && popup->should_close) {
        gaia_array_pop(g->popups);
    }

    if(!popup_open)
        return false;

    gui_begin(name, popup->pos, (vec2s){{POPUP_DEFAULT_WIDTH, 200}}, NULL);
    struct GuiWindow *window = gui_window_current();
    if(!window) {
        return false;
    }
    window->flags.no_tilebar = true;
    window->flags.widget_centered = true;
    window->flags.no_move = true;
    window->flags.is_popup = true;
    window->flags.auto_rezize = true;

    if(popup->just_opened) {
        gui_window_child_push(popup->parent_window, window);
    }

    if(!popup->just_opened && io->mouse.buttons[GLFW_MOUSE_BUTTON_LEFT].pressed && !gui_widget_hovererd(window->tmp_data.bb)) {
        popup->should_close = true;
        gui_window_child_pop(popup->parent_window);
    }

    if((size_t)window->tmp_data.draw_index != (gaia_array_length(g->rendering_data.redering_order) - 1) && !popup->should_close)
        gui_window_bring_to_front(window);

    return true;
}

GUI_API void gui_popup_end() {
    struct GuiContext *g = ctx;
    struct GuiPopup *popup = &g->popups[gaia_array_length(g->popups) - 1];
    struct GuiWindow *window = gui_window_current();
    if(popup->just_opened) {
        popup->just_opened = false;
    }
    // FIXME: maybe remove that this gets set every frame idk
    window->pos = (vec2s){{ popup->pos.x, popup->pos.y - window->size.y }}; 
    gui_end();
}

// menus
// TODO: make this a proper calculation of the position of the menu
GUI_API vec2s gui_menu_pos_get(struct GuiItem data) {
    // return (vec2s){{data.pos.x + data.size.x + 4, data.pos.y + 16}};
    return (vec2s){{data.pos.x + data.size.x + 16 - 2, data.pos.y - 60}};
}

// TODO: menus
GUI_API bool gui_menu_begin(const char *label, ...) {
    struct GuiWindow *window = gui_window_current();

    if(!window || !window->visible)
        return false;

    va_list args;
    va_start(args, label);
    String str = gaia_string_init_va(label, args);
    va_end(args);

    struct GuiItem data = gui_widget_data_get(str);
    if(!window->flags.widget_centered) {
        data.bb.max.x += 20; 
        data.size.x += 20;
    }
    

    bool hovered = false;
    gui_button_behavior(data.bb, &hovered);

    struct GuiWindow *menu_window = gui_window_get(label);
    bool window_hovered = false;
    //TODO: fix this shit up 
    if(menu_window && hovered && menu_window->visible)
        gui_button_behavior(menu_window->tmp_data.bb, &window_hovered);


    if(hovered || window_hovered)
        gui_box_add(window->tmp_data.draw_list, data.pos, data.size, rgb2vec4(255, 99, 226), blank);

    gui_text_add(window->tmp_data.draw_list, (vec2s){{data.pos.x + 2, data.pos.y + data.size.y / 4}}, 1, str.c_str);
    gui_box_add(window->tmp_data.draw_list, (vec2s){{data.bb.max.x - 16, data.pos.y}}, (vec2s){{16, 16}}, COLOR_WHITE, arrows[ARROW_RIGHT]);

    bool open = (hovered || window_hovered);

    if(gui_begin(label, gui_menu_pos_get(data), (vec2s){{300, 50}}, &open)){
        struct GuiWindow *window = gui_window_current();
        window->flags.no_tilebar = true;
        window->flags.widget_centered = true;
        window->flags.no_move = true;
        window->flags.is_popup = true;
        window->flags.auto_rezize = true;
        // window->flags.no_padding = true;

        // if((size_t)window->tmp_data.draw_index != gaia_array_length(g->rendering_data.redering_order))
            gui_window_bring_to_front(window);

        gui_text("{%.1f}, {%.1f}", data.size.x, data.size.y);

        gui_end();
    }     


    return true;
}

GUI_API bool gui_menu_item(const char *name) {
    return false;
}

GUI_API void gui_menu_end() {

}