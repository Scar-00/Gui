#include "./../internal.h"

GUI_API GuiVAO gui_vao_create() {
    GuiVAO self = {0};
    glGenVertexArrays(1, &self.handle);
    return self;
}

GUI_API void gui_vao_bind(GuiVAO self) {
    glBindVertexArray(self.handle);
}

GUI_API void gui_vao_attr(GuiVAO vao, GuiVBO vbo, GLuint index, GLuint size, GLenum type, GLsizei stride, size_t offset) {
    gui_vao_bind(vao);
    gui_vbo_bind(vbo);
    switch (type) {
        case GL_BYTE:
        case GL_UNSIGNED_BYTE:
        case GL_SHORT:
        case GL_UNSIGNED_SHORT:
        case GL_INT:
        case GL_UNSIGNED_INT:
        case GL_INT_2_10_10_10_REV:
        case GL_UNSIGNED_INT_2_10_10_10_REV:
            glVertexAttribIPointer(index, size, type, stride, (void *) offset);
            break;
        default:
            glVertexAttribPointer(index, size, type, GL_FALSE, stride, (void *) offset);
            break;
    }
    glEnableVertexAttribArray(index);
}

GUI_API void gui_vao_destroy(GuiVAO self) {

}