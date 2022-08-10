#ifndef _GUI_H_
#define _GUI_H_

// mandatory functions for using:
// create the gui context and init the redering data <- only call once at the beginning of the programm
// gui_create_context();
// gui_renderer_data_init();
// 
// initialize the gui I/O
// if you want to let gui handle the io 
// gui_io_init(GLFWwindow*); 
// if you want to handle I/O you need to pass them to the gui aswell 
// void gui_io_key_callback(GLFWwindow *, int, int, int, int);
// void gui_io_cursor_callback(GLFWwindow *, double, double);
// void gui_io_mouse_callback(GLFWwindow *, int, int, int);
// void gui_io_size_callback(GLFWwindow *, int, int);
// 
// call the begin of rendering
// gui_render_begin
// 
//
//
// must to be called at the end of every frame
// gui_frame_new()
//
// call at the end of rendering(expects orhographic projection)
// gui_render();
//
// clean up
// gui_shutdown();

#ifdef __cplusplus
extern "C" {
#endif

#if defined(DLL_BUILD)
    #define GUI_API  __declspec(dllexport)
#elif defined(DLL_USE)
    #define GUI_API __declspec(dllimport)
#else
    #define GUI_API
#endif

//TODO: let user provide own allocator 

#ifdef __cplusplus
}
#endif

//TODO: fix includes

//general
#include <Gaia/gaia.h>

//window/opengl
#include "../glad/include/glad/glad.h"
#define GLFW_INCLUDE_NONE
#include "../glfw/include/GLFW/glfw3.h"

//math/vector
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstatic-in-inline"

#include "../lib/cglm/include/cglm/cglm.h"
#include "../lib/cglm/include/cglm/struct.h"

#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

//std
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#include "./gfx/texture.h"
#include "./gfx/renderer.h"
#include "./gfx/camera.h"

typedef uint32_t GuiId;

#define WIDGET_DEFAULT_HIEGHT 16
#define WIDGET_DEFAULT_WIDTH 60
#define WIDGET_DEFAULT_PADDING_Y 4
#define WIDGET_DEFAULT_PADDING_X 4
#define WIDGET_DEFAULT_SIZE (vec2s){{ WIDGET_DEFAULT_WIDTH, WIDGET_DEFAULT_HIEGHT }}

#define COLOR_WHITE (vec4s){{1, 1, 1, 1}}
#define WIDGET_SIZE_FROM_STRING(str) ((str.length  * 9) + 4)

struct GuiRenderingData {
    GuiVAO vao;
    GuiVBO vbo;
    GuiVBO ibo;

    GuiShader shader;

    GuiCamera camera;

    struct GuiWindow **redering_order;
    struct GuiTexture textures[32];
    float texture_count;
};

struct GuiChar {
    vec2s size;
    vec2s bearing;
    vec2s uv_min;
    vec2s uv_max;
    u32 advance;
};

struct GuiFont {
    struct GuiChar *chars;
    struct GuiTexture atlas;
    vec2s size;
};

struct GuiDrawList {
    f32 *vertex_buffer;
    u32 *index_buffer;
};

struct GuiWindowFlags {
    bool no_move;
    bool no_tilebar;
    bool widget_centered;
    bool auto_rezize;
    bool is_popup;
    bool no_border;
    bool collapsed;
    bool no_sort;
    bool no_padding;
    bool collapsable;
    bool just_created;
    bool size_calced;
};

struct GuiRect {
    vec2s min;
    vec2s max;
};

struct GuiItem {
    GuiId id;
    struct GuiRect bb;
    vec2s pos;
    vec2s size;
};

//TODO: maybe remove this hard cap ?
#define WINDOW_MAX_INTERACTIONS 6
typedef void (*IntFunc)(void *args);
struct GuiWindowInteraction {
    String name;
    void *args;
    IntFunc fn;
};

#define WINDOW_MAX_CHILDREN 6
struct WTmpData {
    vec2s cursor_start_pos;
    vec2s cursor_pos;
    struct GuiRect bb;
    struct GuiItemData{
        vec2s pos, size;
    }next_item_data;
    struct GuiWindow *child_windows[WINDOW_MAX_CHILDREN];
    size_t child_windows_count;
    vec2s calced_size;
    s32 draw_index;
    struct GuiDrawList *draw_list;
};

struct GuiWindow {
    String name;
    vec2s pos;
    vec2s size;
    GuiId id;
    struct WTmpData tmp_data;
    bool has_button;
    bool visible;
    vec2s padding;
    struct GuiWindowInteraction interactions[WINDOW_MAX_INTERACTIONS];
    u32 interactions_count;
    struct GuiWindowFlags flags;
};

struct GuiMenu {
    u32 rows;
    u32 cols;
};

struct GuiPopup {
    GuiId id;
    struct GuiWindow *parent_window;
    bool just_opened;
    bool should_close;
    vec2s pos;
};

struct GuiIOButton {
    bool down, last, pressed;
};

struct GuiIOMouse {
    struct GuiIOButton buttons[GLFW_MOUSE_BUTTON_LAST];
    vec2s position, delta;
};

struct GuiIOKeyBoard {
    struct GuiIOButton keys[GLFW_KEY_LAST];
};

struct GuiIO {
    struct GuiIOMouse mouse;
    struct GuiIOKeyBoard keyboard;
    vec2s window_size;
};

struct GuiTree {
    GuiId id;
    s32 item_count;
    bool collapsed;
};

typedef enum GuiErrorType {
    WIDGET_OUT_OF_BOUNDS,
    EXPECT_NO_ZERO,
}GuiErrorType;

typedef void (*error_handler)(const char *prefix, const char *message, u32 line, const char *file);

// TODO: add global timer to measure time based events(menu opening tolerance) 
struct GuiContext {
    bool initialized;
    struct GuiWindow **windows;
    struct GuiWindow *current_window;
    struct GuiWindow *window_active;

    struct GuiRenderingData rendering_data;
    struct GuiFont *font;

    struct GuiPopup *popups;

    GuiId *close_list;
    struct GuiItem last_item_added;
    GuiId widget_active_id;

    error_handler panic_handler; 

    // GuiTime time;
    // f32 delta_time;
    size_t state;
    struct GuiIO *io;
};

enum ArrowTypes {
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
};

//gui context
GUI_API bool gui_context_create();
GUI_API struct GuiContext *gui_context_get();
GUI_API void gui_frame_new();
GUI_API void gui_item_add(struct GuiRect bb, GuiId id);
GUI_API struct GuiWindow *gui_window_current();

//gui I/O
GUI_API struct GuiIO *gui_io_init(GLFWwindow *window);
GUI_API struct GuiIO *gui_io_get();
GUI_API void gui_io_key_callback(GLFWwindow *handle, int key, int scancode, int action, int mods);
GUI_API void gui_io_cursor_callback(GLFWwindow *handle, double mx, double my);
GUI_API void gui_io_mouse_callback(GLFWwindow *handle, int button, int action, int mods);
GUI_API void gui_io_size_callback(GLFWwindow *handle, int width, int height);
GUI_API void gui_io_key_update();

//fonts
GUI_API struct GuiFont *gui_font_init(const char *path, u32 size);

//rendering
GUI_API struct GuiDrawList *gui_draw_list_create();
GUI_API void gui_window_bring_to_front(struct GuiWindow *window);
GUI_API void gui_renderer_data_init();
GUI_API void gui_render_begin();
GUI_API void gui_render();
GUI_API void gui_shutdown();

GUI_API void gui_box_add(struct GuiDrawList *draw_list, vec2s pos, vec2s size, vec4s color, struct GuiTexture tex);
GUI_API void gui_text_add(struct GuiDrawList *draw_list, vec2s pos, f32 scale, const char *format);
GUI_API void gui_rect_add(struct GuiDrawList *draw_list, struct GuiRect bb, vec4s color);

//windows
GUI_API struct GuiWindow *gui_window_get(const char *name);
GUI_API bool gui_begin(const char *name, vec2s pos, vec2s size, bool *open);
GUI_API void gui_end();
GUI_API struct GuiWindow *gui_window_create(const char *name, vec2s pos, vec2s size, bool just_created);
GUI_API void gui_window_update(struct GuiWindow *window, bool just_created);
GUI_API void gui_window_render_tile_bar(struct GuiWindow *window, struct GuiRect bb, const char *name, bool *open);
GUI_API void gui_window_cursor_advance();
GUI_API bool gui_is_clickable();
GUI_API void gui_window_set_size(vec2s size);
GUI_API void gui_window_interaction_handler(struct GuiWindow *window);
GUI_API void gui_window_interaction_register(const char *name, IntFunc func, void *args);

//widgets
GUI_API struct GuiItem gui_widget_data_get(String name);
GUI_API bool gui_button(const char *label);
GUI_API bool gui_close_button(vec2s pos);
GUI_API bool gui_button_behavior(struct GuiRect bb, bool *out_hovererd);
GUI_API void gui_same_line();
GUI_API void gui_text(const char *format, ...);
GUI_API void gui_text_wraped(const char *format, ...);
GUI_API bool gui_selectable(const char *label);
GUI_API void gui_seperator(s32 len);
GUI_API void gui_text_input(const char *label, char *buffer, vec2s size);
GUI_API bool gui_item_hovererd();
GUI_API void gui_tooltip_set(const char *format, ...);
//TODO: add slider [gui_slider_type(const char *name, type *value, type lower, type upper);]
GUI_API bool gui_slider_f32(const char *name, f32 *value, f32 lower, f32 upper);

//helper
GUI_API GuiId gui_hash(const char* name);
GUI_API bool gui_widget_hovererd(struct GuiRect bb);
GUI_API const char *gui_get_char_from_key();
GUI_API void gui_window_child_push(struct GuiWindow *window, struct GuiWindow *window_child);
GUI_API struct GuiWindow *gui_window_child_pop(struct GuiWindow *window);
GUI_API u32 rand_range(u32 lower, u32 upper);
GUI_API inline const char *gui_error_to_string(GuiErrorType error);
#define gui_panic(error, format, ...) gui_panic_expl(error, __LINE__, __FILE__, format, __VA_ARGS__)
GUI_API void gui_panic_expl(GuiErrorType error, u32 line, char *file, const char *format, ...);
GUI_API void gui_panic_handler_set(error_handler handler);
GUI_API bool gui_window_just_created(struct GuiWindow *window);
GUI_API inline vec4s rgb2vec4(float r, float g, float b);
GUI_API inline bool gui_vec2s_cmp(vec2s a, vec2s b);

//popups
GUI_API void gui_popup_open(const char *popup_name);
GUI_API bool gui_popup_is_open(GuiId id);
GUI_API struct GuiPopup *gui_popup_get(GuiId id);
GUI_API bool gui_popup_begin(const char *name);
GUI_API void gui_popup_end();

//trees
GUI_API struct GuiTree *gui_tree_get(const char *name);
GUI_API bool gui_tree_begin(const char *label, u32 col_count);
GUI_API void gui_tree_end();

//menus
GUI_API vec2s gui_menu_pos_get(struct GuiItem data);
GUI_API bool gui_menu_begin(const char *label, ...);
GUI_API bool gui_menu_item(const char *name);
GUI_API void gui_menu_end();

#endif