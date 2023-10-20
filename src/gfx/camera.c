#include "./../internal.h"

GuiCamera gui_camera_init(vec2s min, vec2s max) {
    GuiCamera self = {
        .min = min,
        .max = max,
        .pos = glms_vec2_zero(),
        .view_proj = {
            .view = glms_mat4_identity(),
            .proj = glms_mat4_identity(),
        }
    };
    gui_camera_update(&self);
    return self;
}

void gui_camera_update(GuiCamera *self) {
    // self->view_proj.view = glms_lookat(self->pos, glms_vec2_add(self->pos, glms_vec2(0, 1)), glms_vec3(0, 0, 1));
    self->max = ctx->io->window_size;
    
    self->view_proj.view = glms_mat4_identity();
    self->view_proj.proj = glms_ortho(self->min.x, self->max.x, self->min.y, self->max.y, -100, 100);
}
