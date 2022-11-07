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
// -----begin gui code-----
//
//
// ------end gui code------
//
// call at the end of rendering(expects orhographic projection)
// gui_render();
//
// must to be called at the end of every frame
// gui_frame_new()
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

typedef double              f64;
typedef float               f32;
typedef long long int       s64;
typedef int                 s32;
typedef short int           s16;
typedef signed char         s8;

//unsigned types
typedef unsigned long long  size_t;
typedef unsigned long long  u64;
typedef unsigned int        u32;
typedef unsigned short      u16;
typedef unsigned char       u8;

//math/vector
/*
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstatic-in-inline"

#include <cglm/cglm.h>
#include <cglm/struct.h>

#pragma GCC diagnostic pop
#pragma GCC diagnostic pop
*/

//std
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>

#define BIT(x) (1 << x)

typedef u32 GuiWindowFlags;
typedef enum GuiWindowFlags_ {
    GUI_WINDOW_NONE = 0,
    GUI_WINDOW_NO_MOVE = BIT(0),
    GUI_WINDOW_NO_TILEBAR = BIT(1),
    GUI_WINDOW_WIDGETS_CENTERED = BIT(2),
    GUI_WINDOW_AUTO_RESIZE = BIT(3),
    GUI_WINDOW_IS_POPUP = BIT(4),
    GUI_WINDOW_NO_BORDER = BIT(5),
    GUI_WINDOW_COLLAPSED = BIT(6),
    GUI_WINDOW_NO_SORT = BIT(7),
    GUI_WINDOW_NO_PADDING = BIT(8),
    GUI_WINDOW_COLLAPSABLE = BIT(9),
    GUI_WINDOW_JUST_CREATED = BIT(10),
    GUI_WINDOW_SIZE_CALCED = BIT(11),
    GUI_WINDOW_RESIZABLE = BIT(12),
    GUI_WINDOW_LOCKED = BIT(13),
    GUI_WINDOW_SCROLLABLE = BIT(14),
}GuiWindowFlags_;

//gui context
GUI_API bool gui_context_create();
GUI_API struct GuiContext *gui_context_get();
GUI_API void gui_frame_new();
GUI_API struct GuiWindow *gui_window_current();

//gui I/O
GUI_API struct GuiIO *gui_io_init(void *window);
GUI_API struct GuiIO *gui_io_get();
GUI_API f32 gui_io_get_size_x();
GUI_API f32 gui_io_get_size_y();
GUI_API f64 gui_io_get_time();
GUI_API f32 gui_io_get_mouse_x();
GUI_API f32 gui_io_get_mouse_y();
GUI_API void gui_io_key_callback(void *handle, int key, int scancode, int action, int mods);
GUI_API void gui_io_cursor_callback(void *handle, double mx, double my);
GUI_API void gui_io_mouse_callback(void *handle, int button, int action, int mods);
GUI_API void gui_io_size_callback(void *handle, int width, int height);

//rendering
GUI_API void gui_renderer_data_init();
GUI_API void gui_render_begin();
GUI_API void gui_render();
GUI_API void gui_shutdown();

//viewport
GUI_API void gui_viewport_begin(u32 x, u32 y, u32 width, u32 height);
GUI_API void gui_viewport_end();

//windows
GUI_API bool gui_begin(const char *name, f32 x, f32 y, f32 width, f32 height, bool *open);
GUI_API void gui_end();
GUI_API void gui_window_set_size(f32 width, f32 height);
GUI_API void gui_window_flag_set(struct GuiWindow *window, GuiWindowFlags flag);
GUI_API void gui_window_flag_clear(struct GuiWindow *window, GuiWindowFlags flag);
GUI_API bool gui_window_flag_check(struct GuiWindow *window, GuiWindowFlags flag);

//widgets
GUI_API void gui_set_next_item_pos(s32 rel_x, s32 rel_y);
GUI_API void gui_set_next_item_size(s32 width, s32 height);
GUI_API bool gui_button(const char *label);
GUI_API void gui_same_line();
GUI_API void gui_text(const char *format, ...);
GUI_API void gui_text_wraped(const char *format, ...);
GUI_API bool gui_selectable(const char *label);
GUI_API void gui_seperator(s32 len);
GUI_API void gui_text_input(const char *label, char *buffer);
GUI_API bool gui_item_hovererd();
GUI_API void gui_tooltip_set(const char *format, ...);
//TODO: add slider [gui_slider_type(const char *name, type *value, type lower, type upper);]
GUI_API bool gui_slider_f32(const char *name, f32 *value, f32 lower, f32 upper);
GUI_API void gui_image(u32 tex_image_id, s32 width, s32 height);

//helper
typedef void (*error_handler)(const char *prefix, const char *message, u32 line, const char *file);
GUI_API void gui_panic_handler_set(error_handler handler);
GUI_API bool gui_window_just_created(struct GuiWindow *window);

//popups
GUI_API void gui_popup_open(const char *popup_name);
GUI_API bool gui_popup_begin(const char *name);
GUI_API void gui_popup_end();

//menus
GUI_API bool gui_menu_begin(const char *label, ...);
GUI_API bool gui_menu_item(const char *name);
GUI_API void gui_menu_end();

//trees
GUI_API bool gui_tree_begin(char const *label);
GUI_API void gui_tree_end();

//debug
GUI_API void gui_test();

#ifdef __cplusplus
}
#endif
#endif
