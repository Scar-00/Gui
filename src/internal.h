#ifndef _GUI_INTERNAL_H_
#define _GUI_INTERNAL_H_

#include <gui.h>

//general
#include <Gaia/gaia.h>

//window
#include <glad/glad.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

//fonts
#include <freetype/freetype.h>

//math/vector
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wmissing-braces"
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstatic-in-inline"

#include <cglm/cglm.h>
#include <cglm/struct.h>

#pragma GCC diagnostic pop
#pragma GCC diagnostic pop

//std
#include <stdarg.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <time.h>

typedef uint32_t GuiId;
typedef f64 GuiTime;

#define WIDGET_DEFAULT_HIEGHT 16
#define WIDGET_DEFAULT_WIDTH 60
#define WIDGET_DEFAULT_PADDING_Y 4
#define WIDGET_DEFAULT_PADDING_X 4
#define WIDGET_DEFAULT_SIZE (vec2s){{ WIDGET_DEFAULT_WIDTH, WIDGET_DEFAULT_HIEGHT }}
#define GUI_TEXT_PADDING_LEFT 2

#define COLOR_WHITE (vec4s){{1, 1, 1, 1}}
#define WIDGET_SIZE_FROM_STRING(str) ((str.length  * 9) + 4)
#define FLAG_SET(f, v) ((f) |= (v))
#define FLAG_CLEAR(f, v) ((f) &= ~(v))
#define FLAG_CHECK(f, v) ((f & v) == (v))
#define FLAG_FLIP(f, v) ((f) ^= (v))
#define FLAS_SET_BOOL(f, b, v) ((f) |= b ? (v) : 0)

#define clamp(x, mn, mx) ({\
    __typeof__ (x) _x = (x); \
    __typeof__ (mn) _mn = (mn); \
    __typeof__ (mx) _mx = (mx); \
    max(_mn, min(_mx, _x)); })


typedef int ivec2[2];
typedef union ivec2s {
  ivec2 raw;
  struct {
    int x;
    int y;
  };
}ivec2s;

typedef struct GuiVAO {
    GLuint handle;
}GuiVAO;

typedef struct GuiVBO {
    GLuint handle;
    GLint type;
    bool dynamic;
}GuiVBO;

typedef struct GuiShader {
    GLuint handle;
    GLuint vs_handle;
    GLuint fs_handle;
}GuiShader;

typedef struct GuiViewProj {
    mat4s view, proj;
}GuiViewProj;

typedef struct GuiCamera {
    GuiViewProj view_proj;
    vec2s pos, min, max;
}GuiCamera;

typedef struct GuiTexture {
    GLuint handle;
    vec2s size;
}GuiTexture;

typedef struct GuiViewPort {
    vec2s pos, size;
}GuiViewPort;

typedef struct GuiRenderingData {
    GuiVAO vao;
    GuiVBO vbo;
    GuiVBO ibo;

    GuiShader shader;
    GuiCamera camera;
    GuiViewPort view_port;

    struct GuiWindow **redering_order;
    struct GuiTexture textures[32];
    float texture_count;
}GuiRenderingData;

typedef struct GuiChar {
    vec2s size;
    vec2s bearing;
    vec2s uv_min;
    vec2s uv_max;
    u32 advance;
}GuiChar;

typedef struct GuiFont {
    struct GuiChar *chars;
    struct GuiTexture atlas;
    vec2s size;
}GuiFont;

typedef struct GuiDrawList {
    f32 *vertex_buffer;
    u32 *index_buffer;
}GuiDrawList;

typedef struct GuiWindowSaveData {
    vec2s size;
    vec2s pos;
    GuiWindowFlags flags;
    bool populated;
}GuiWindowSaveData;

typedef struct GuiRect {
    vec2s min;
    vec2s max;
}GuiRect;

//item flags
typedef u32 GuiItemFlags;
typedef enum GuiItemFlags_ {
    GUI_ITEM_NONE = 0,
    GUI_ITEM_HIDDEN = BIT(1),
}GuiItemFlags_;

//contains data for every component added to a window
typedef struct GuiItem {
    GuiId id;
    struct GuiRect bb;
    vec2s pos;
    vec2s size;
    GuiItemFlags flags;
}GuiItem;

#define WINDOW_MAX_CHILDREN 6
typedef struct WTmpData {
    vec2s cursor_start_pos;
    vec2s cursor_pos;
    struct GuiRect bb;
    struct GuiItemData{
        vec2s pos, size;
    }next_item_data;
    struct GuiWindow *child_windows[WINDOW_MAX_CHILDREN];
    size_t child_windows_count;
    vec2s calced_size;
    struct GuiMenu *menus;
    struct GuiTree *trees;
    s32 draw_index;
    struct GuiDrawList *draw_list;
}WTmpData;

typedef struct GuiWindow {
    String name;
    vec2s pos;
    vec2s size;
    GuiId id;
    struct WTmpData tmp_data;
    bool has_button;
    bool visible;
    vec2s padding;
    s32 scroll;
    GuiWindowFlags flags;
}GuiWindow;

typedef struct GuiPopup {
    GuiId id;
    struct GuiWindow *parent_window;
    bool just_opened;
    bool should_close;
    vec2s pos;
}GuiPopup;

typedef u32 GuiMenuFlags;
typedef enum GuiMenuFlags_ {
    GUI_MENU_NONE = 0,
    GUI_MENU_ACTIVE = BIT(1),
    GUI_MENU_SHOULD_CLOSE = BIT(2),
}GuiMenuFlags_;

typedef struct GuiMenu {
    String label;
    GuiId id;
    GuiMenuFlags flags;
}GuiMenu;

typedef struct GuiTree {
    u32 depth;
    u32 size;
    vec2s pos;
    String name;
}GuiTree;

typedef struct GuiIOButton {
    bool down, last, pressed;
}GuiIOButton;

typedef struct GuiIOMouse {
    struct GuiIOButton buttons[GLFW_MOUSE_BUTTON_LAST];
    vec2s position, delta;
}GuiIOMouse;

typedef struct GuiIOKeyBoard {
    struct GuiIOButton keys[GLFW_KEY_LAST];
}GuiIOKeyBoard;

typedef struct GuiIO {
    struct GuiIOMouse mouse;
    struct GuiIOKeyBoard keyboard;
    vec2s window_size;
}GuiIO;

typedef enum GuiErrorType {
    ERR_WIDGET_OUT_OF_BOUNDS,
    ERR_EXPECT_NO_ZERO,
    ERR_GUI_GL_ERROR,
}GuiErrorType;

// TODO: add global timer to measure time based events(menu opening tolerance)
typedef struct GuiContext {
    bool initialized;
    struct GuiWindow **windows;
    //GuiId *closed_windows
    struct GuiWindow *current_window;
    struct GuiWindow *window_active;

    struct GuiRenderingData rendering_data;
    struct GuiFont *font;

    struct GuiPopup *popups;

    GuiId *close_list;
    struct GuiItem last_item_added;
    GuiId widget_active_id;

    error_handler panic_handler;

    clock_t time;
    GuiTime time_passed;
    size_t state;
    struct GuiIO *io;
}GuiContext;

enum ArrowTypes {
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
};

typedef struct GuiFrameBuffer {
    ivec2s size;
    u32 samples;
    bool render_target;
    u32 color_buffer;
    u32 renderer_id;
}GuiFrameBuffer;

typedef struct GuiTriangle {
    vec2s p0, p1, p2;
}GuiTriangle;

//globals
extern struct GuiContext *ctx;
extern struct GuiTexture arrows[4];
extern struct GuiTexture blank;

//context
void gui_window_remove(struct GuiWindow *window);
void gui_item_add(struct GuiRect bb, GuiId id);

//io
void gui_io_key_update();

//fonts
struct GuiFont *gui_font_init(const char *path, u32 size);

//rendering
//vbo
GuiVBO gui_vbo_create(GLint type, bool dynamic);
void gui_vbo_bind(GuiVBO self);
void gui_vbo_buffer(GuiVBO self, void *data, size_t offset, size_t count);
void gui_vbo_destroy(GuiVBO self);
//vao
GuiVAO gui_vao_create();
void gui_vao_bind(GuiVAO self);
void gui_vao_attr(GuiVAO vao, GuiVBO vbo, GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset);
void gui_vao_destroy(GuiVAO self);
//shader
GuiShader gui_shader_create(const char *vs_path, const char *fs_path);
void gui_shader_bind(GuiShader self);
void gui_shader_uniform_float(GuiShader self, char *name, float f);
void gui_shader_uniform_mat4(GuiShader self, char *name, mat4s m);
void gui_shader_uniform_view_proj(GuiShader self, GuiViewProj view_proj);
void gui_shader_uniform_vec4(GuiShader self, char *name, vec4s v);
void gui_shader_uniform_texture2D(GuiShader self, char *name, struct GuiTexture texture, GLuint n);
void gui_shader_uniform_textures2D(GuiShader self, char *name);
//camera
GuiCamera gui_camera_init(vec2s min, vec2s max);
void gui_camera_update(GuiCamera *self);
//texture
void gui_texture_pixels_load(const char *path, u8 **pixels_out, u32 *width_out, u32 *height_out);
struct GuiTexture gui_texture_from_pixels(u8 *pixels, u32 width, u32 height);
struct GuiTexture gui_texture_from_font(u8 *pixels, u32 width, u32 height);
struct GuiTexture gui_texture_from_path(const char *path);
void gui_texture_destroy(struct GuiTexture self);
void gui_texture_bind(struct GuiTexture self);

//viewport
GuiViewPort gui_viewport_init(vec2s pos, vec2s size);

typedef enum GuiTrangleSide {
    GUI_TRIANGLE_CORNER_TOP_L,
    GUI_TRIANGLE_CORNER_TOP_R,
    GUI_TRIANGLE_CORNER_BOTTOM_L,
    GUI_TRIANGLE_CORNER_BOTTOM_R,
}GuiTrangleSide;

struct GuiDrawList *gui_draw_list_create();
void gui_window_bring_to_front(struct GuiWindow *window);
void gui_box_add(struct GuiDrawList *draw_list, vec2s pos, vec2s size, vec4s color, struct GuiTexture tex);
void gui_text_add(struct GuiDrawList *draw_list, vec2s pos, f32 scale, const char *format);
void gui_rect_add(struct GuiDrawList *draw_list, struct GuiRect bb, vec4s color);
void gui_triangle_add(struct GuiDrawList *draw_list, vec2s pos, vec2s size, vec4s color, GuiTrangleSide side);
//buffer
GuiFrameBuffer gui_frame_buffer_init();
void gui_frame_buffer_invalidate(GuiFrameBuffer *self);
void gui_frame_buffer_bind(GuiFrameBuffer self);
void gui_frame_buffer_unbind();
void gui_frame_buffer_set_size(GuiFrameBuffer *self, ivec2s size);
void gui_frame_buffer_set_target(GuiFrameBuffer *self);

//windows
struct GuiWindowSaveData gui_window_load(String name);
void gui_window_save(struct GuiWindow *window);
struct GuiWindow *gui_window_get(const char *name);
struct GuiWindow *gui_window_create(const char *name, vec2s pos, vec2s size, bool just_created);
void gui_window_destroy(struct GuiWindow *window);
void gui_window_update(struct GuiWindow *window, bool just_created);
void gui_window_render_tile_bar(struct GuiWindow *window, struct GuiRect bb, const char *name, bool *open);
void gui_window_cursor_advance();
bool gui_is_clickable();
void gui_window_child_push(struct GuiWindow *window, struct GuiWindow *window_child);
struct GuiWindow *gui_window_child_pop(struct GuiWindow *window);

//widgets
struct GuiItem gui_widget_data_get(String name);
bool gui_close_button(vec2s pos);
bool gui_button_behavior(struct GuiRect bb, bool *out_hovererd);

//helper
GuiId gui_hash(const char* name);
bool gui_widget_hovererd(struct GuiRect bb);
const char *gui_get_char_from_key();
u32 rand_range(u32 lower, u32 upper);
inline const char *gui_error_to_string(GuiErrorType error);
#define gui_panic(error, format, ...) gui_panic_expl(error, __LINE__, __FILE__, format, __VA_ARGS__)
void gui_panic_expl(GuiErrorType error, u32 line, char *file, const char *format, ...);
vec4s rgb2vec4(float r, float g, float b);
bool gui_vec2s_cmp(vec2s a, vec2s b);
f32 gui_triangle_area(GuiTriangle self);
bool gui_point_in_triangle(GuiTriangle triangle, vec2s point);

//popups
bool gui_popup_is_open(GuiId id);
struct GuiPopup *gui_popup_get(GuiId id);

//menus
vec2s gui_menu_pos_get(struct GuiItem data);

//trees
GuiTree *gui_tree_current();

#endif
