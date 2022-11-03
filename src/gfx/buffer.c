#include "../internal.h"
#include "glad/glad.h"

GuiFrameBuffer gui_frame_buffer_init(ivec2s size) {
    return (GuiFrameBuffer){
        .size = size,
        .samples = 1,
        .render_target = false,
        .color_buffer = 0,
        .renderer_id = 0,
    };
}

void gui_frame_buffer_invalidate(GuiFrameBuffer *self) {
    if(self->renderer_id) {
        glDeleteFramebuffers(1, &self->renderer_id);
        glDeleteTextures(1, &self->color_buffer);
    }

    glCreateFramebuffers(1, &self->renderer_id);
    glBindFramebuffer(GL_FRAMEBUFFER, self->renderer_id);

    glCreateTextures(GL_TEXTURE_2D, 1, &self->color_buffer);
    glBindTexture(GL_TEXTURE_2D, self->color_buffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, self->size.x, self->size.y, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->color_buffer, 0);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        gui_panic(ERR_GUI_GL_ERROR, "Could not complete framebuffer [%d]", self->renderer_id);
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void gui_frame_buffer_bind(GuiFrameBuffer self) {
    glBindFramebuffer(GL_FRAMEBUFFER, self.renderer_id);
    glViewport(0, 0, self.size.x, self.size.y);
}

void gui_frame_buffer_unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void gui_frame_buffer_set_size(GuiFrameBuffer *self, ivec2s size) {
    self->size = size;
    gui_frame_buffer_invalidate(self);
}

void gui_frame_buffer_set_target(GuiFrameBuffer *self) {
    self->render_target = true;
}
