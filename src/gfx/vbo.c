#include "./../internal.h"

GuiVBO gui_vbo_create(GLint type, bool dynamic) {
    GuiVBO self = {
        .type = type,
        .dynamic = dynamic,
    };
    glGenBuffers(1, &self.handle);
    return self;
}

void gui_vbo_bind(GuiVBO self) {
    glBindBuffer(self.type, self.handle);
}

void gui_vbo_buffer(GuiVBO self, void *data, size_t offset, size_t count) {
    gui_vbo_bind(self);
    glBufferData(self.type, count - offset, data, self.dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
}

void gui_vbo_destroy(GuiVBO self) {
    glDeleteBuffers(1, &self.handle);
}
