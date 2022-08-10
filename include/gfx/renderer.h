#ifndef _GUI_RENDERER_H_
#define _GUI_RENDERER_H_

#include <gui.h>

typedef struct GuiVBO {
    GLuint handle;
    GLint type;
    bool dynamic;
}GuiVBO;

GUI_API GuiVBO gui_vbo_create(GLint type, bool dynamic);
GUI_API void gui_vbo_bind(GuiVBO self);
GUI_API void gui_vbo_buffer(GuiVBO self, void *data, size_t offset, size_t count);
GUI_API void gui_vbo_destroy(GuiVBO self);

typedef struct GuiVAO {
    GLuint handle;
}GuiVAO;

GUI_API GuiVAO gui_vao_create();
GUI_API void gui_vao_bind(GuiVAO self);
GUI_API void gui_vao_attr(GuiVAO vao, GuiVBO vbo, GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset);
GUI_API void gui_vao_destroy(GuiVAO self);

typedef struct GuiShader {
    GLuint handle;
    GLuint vs_handle;
    GLuint fs_handle;
}GuiShader;

typedef struct GuiViewProj {
    mat4s view, proj;
}GuiViewProj;

GUI_API GuiShader gui_shader_create(const char *vs_path, const char *fs_path);
GUI_API void gui_shader_bind(GuiShader self);
GUI_API void gui_shader_uniform_float(GuiShader self, char *name, float f); 
GUI_API void gui_shader_uniform_mat4(GuiShader self, char *name, mat4s m);
GUI_API void gui_shader_uniform_view_proj(GuiShader self, GuiViewProj view_proj);
GUI_API void gui_shader_uniform_vec4(GuiShader self, char *name, vec4s v);
GUI_API void gui_shader_uniform_texture2D(GuiShader self, char *name, struct GuiTexture texture, GLuint n);
GUI_API void gui_shader_uniform_textures2D(GuiShader self, char *name);

#endif