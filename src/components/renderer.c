#include "./../internal.h"
#include "../shader/shader.h"

#include "glad/glad.h"

struct GuiDrawList *gui_draw_list_create() {
    struct GuiDrawList *self = calloc(1, sizeof(struct GuiDrawList));
    *self = (struct GuiDrawList) {
        .vertex_buffer = gaia_array_create(f32, 4000),
        .index_buffer = gaia_array_create(u32, 600),
    };
    return self;
}

void gui_window_bring_to_front(struct GuiWindow *window) {
    struct GuiRenderingData *data = &ctx->rendering_data;
    s32 index = window->tmp_data.draw_index;

    size_t array_length = gaia_array_length(data->redering_order);

    for(u32 i = index; i < array_length - 1; i++) {
        data->redering_order[i] = data->redering_order[i + 1];
        data->redering_order[i]->tmp_data.draw_index--;
    }
    data->redering_order[array_length - 1] = window;
    window->tmp_data.draw_index = array_length - 1;
}

GUI_API void gui_renderer_data_init() {
    struct GuiRenderingData *data = &ctx->rendering_data;

    //TODO: this may change in the future (more shaders / gl objects)

    *data = (struct GuiRenderingData){
        .vao = gui_vao_create(),
        .vbo = gui_vbo_create(GL_ARRAY_BUFFER, true),
        .ibo = gui_vbo_create(GL_ELEMENT_ARRAY_BUFFER, true),
        .shader = gui_shader_create("C:\\Users\\fiffi\\OneDrive\\Desktop\\tmp\\windows\\libs\\gui\\src\\shader\\gui.vs", "C:\\Users\\fiffi\\OneDrive\\Desktop\\tmp\\windows\\libs\\gui\\src\\shader\\gui.fs"),
        .camera = gui_camera_init((vec2s){{0, 0}}, ctx->io->window_size),
    };

    data->redering_order = gaia_array_create(struct GuiWindow *, 10);
    data->texture_count = 0;

    gui_shader_bind(data->shader);

    blank = gui_texture_from_path("C:\\Users\\fiffi\\OneDrive\\Desktop\\tmp\\windows\\libs\\gui\\src\\shader\\button.png");
}

GUI_API void gui_render_begin() {
    struct GuiContext *g = ctx;
    for(u32 i = 0; i < gaia_array_length(g->windows); i++) {
        struct GuiDrawList *draw_list = g->windows[i]->tmp_data.draw_list;
        gaia_array_length(draw_list->vertex_buffer) = 0;
        gaia_array_length(draw_list->index_buffer) = 0;
        memset(draw_list->vertex_buffer, 0, gaia_array_capacity(draw_list->vertex_buffer) * sizeof(f32));
        memset(draw_list->index_buffer, 0, gaia_array_capacity(draw_list->index_buffer) * sizeof(u32));
    }
    gui_io_key_update();
}

static void gui_render_clear() {
    glClear(/*GL_DEPTH_BUFFER_BIT |*/ GL_COLOR_BUFFER_BIT);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_SCISSOR_TEST);

    gui_camera_update(&ctx->rendering_data.camera);
}

static void gui_render_prep() {
    gui_render_clear();
    struct GuiContext *g = ctx;
    for(u32 i = 0; i < gaia_array_length(g->rendering_data.redering_order); i++) {
        struct GuiDrawList *draw_data = g->rendering_data.redering_order[i]->tmp_data.draw_list;
        u32 index_count = gaia_array_length(draw_data->index_buffer);
        gaia_array_length(draw_data->index_buffer) = 0;
        size_t offset = 0;
        for(u32 j = 0; j < index_count; j += 6) {
            gaia_array_pushback(draw_data->index_buffer, offset + 0);
            gaia_array_pushback(draw_data->index_buffer, offset + 1);
            gaia_array_pushback(draw_data->index_buffer, offset + 2);
            gaia_array_pushback(draw_data->index_buffer, offset + 2);
            gaia_array_pushback(draw_data->index_buffer, offset + 3);
            gaia_array_pushback(draw_data->index_buffer, offset + 0);
            offset += 4;
        }

        if(!gaia_array_length(draw_data->index_buffer))
            continue;

        struct GuiRenderingData *data = &ctx->rendering_data;
        gui_shader_uniform_view_proj(data->shader, data->camera.view_proj);
        gui_shader_uniform_mat4(data->shader, "m", glms_mat4_identity());
        //binding textures
        gui_shader_uniform_textures2D(data->shader, "textures");
        for(size_t b = 0; b < 32; b++)
            glBindTextureUnit(b, data->textures[b].handle);

        gui_vbo_buffer(data->vbo, draw_data->vertex_buffer, 0, gaia_array_length(draw_data->vertex_buffer) * sizeof(f32));
        gui_vbo_buffer(data->ibo, draw_data->index_buffer, 0, gaia_array_length(draw_data->index_buffer) * sizeof(u32));

        gui_vao_attr(data->vao, data->vbo, 0, 3, GL_FLOAT, 11 * sizeof(f32), 0 * sizeof(f32));
        gui_vao_attr(data->vao, data->vbo, 1, 2, GL_FLOAT, 11 * sizeof(f32), 3 * sizeof(f32));
        gui_vao_attr(data->vao, data->vbo, 2, 1, GL_FLOAT, 11 * sizeof(f32), 5 * sizeof(f32));
        gui_vao_attr(data->vao, data->vbo, 3, 4, GL_FLOAT, 11 * sizeof(f32), 6 * sizeof(f32));
        gui_vao_attr(data->vao, data->vbo, 4, 1, GL_FLOAT, 11 * sizeof(f32), 10 * sizeof(f32));

        gui_vao_bind(data->vao);
        gui_vbo_bind(data->ibo);
        GuiWindow *window = g->rendering_data.redering_order[i];
        glScissor(window->pos.x, window->pos.y, window->size.x + 17, window->size.y + 17);
        glDrawElements(GL_TRIANGLES, gaia_array_length(draw_data->index_buffer), GL_UNSIGNED_INT, (void *) 0);
    }
    glScissor(0, 0, g->io->window_size.x, g->io->window_size.y);
}

GUI_API void gui_render() {
    gui_render_prep();
}

GUI_API void gui_shutdown() {
    struct GuiContext *g = ctx;
    for(u32 i = 0; i < gaia_array_length(g->windows); i++) {
        gaia_array_destroy(g->windows[i]->tmp_data.draw_list->vertex_buffer);
        gaia_array_destroy(g->windows[i]->tmp_data.draw_list->index_buffer);
        free(g->windows[i]->tmp_data.draw_list);
        free(g->windows[i]);
    }

    gaia_array_destroy(g->windows);
    gaia_array_destroy(g->popups);
    gaia_array_destroy(g->font->chars);
    gaia_array_destroy(g->rendering_data.redering_order);
    free(g->font);
    free(g->io);
    free(g);
}

static vec4s QUAD_VERTEX_POS[4] = {
        {{ 0.0,  0.0, 0, 1}},
        {{ 1.0,  0.0, 0, 1}},
        {{ 1.0,  1.0, 0, 1}},
        {{ 0.0,  1.0, 0, 1}}
    };

static vec2s QUAD_UV_CORDS[4] = {
        {{0, 0}},
        {{1, 0}},
        {{1, 1}},
        {{0, 1}}
    };

static f32 gui_check_texture(struct GuiTexture *tex) {
    struct GuiRenderingData *data = &ctx->rendering_data;

    f32 tex_index = -1;
    for(size_t i = 0; i < 32; i++) {
        if(data->textures[i].handle == tex->handle) {
            tex_index = i;
        }
    }

    if(tex_index == -1) {
        tex_index = data->texture_count;
        data->textures[(int)data->texture_count] = *tex;
        data->texture_count++;
    }

    return tex_index;
}

void gui_box_add(struct GuiDrawList *draw_list, vec2s pos, vec2s size, vec4s color, struct GuiTexture tex) {
    f32 tex_index = gui_check_texture(&tex);

    mat4s transform = glms_mat4_mul(
                glms_translate(glms_mat4_identity(), (vec3s){{pos.x, pos.y, 0}}),
                glms_scale(glms_mat4_identity(), (vec3s){{size.x, size.y, 0}})
            );

    for(u32 i = 0; i < 4; i++) {
        vec4s pos = glms_mat4_mulv(transform, QUAD_VERTEX_POS[i]);
        gaia_array_pushback(draw_list->vertex_buffer, pos.x);
        gaia_array_pushback(draw_list->vertex_buffer, pos.y);
        gaia_array_pushback(draw_list->vertex_buffer, pos.z);
        gaia_array_pushback(draw_list->vertex_buffer, QUAD_UV_CORDS[i].x);
        gaia_array_pushback(draw_list->vertex_buffer, QUAD_UV_CORDS[i].y);
        gaia_array_pushback(draw_list->vertex_buffer, tex_index);
        gaia_array_pushback(draw_list->vertex_buffer, color.x);
        gaia_array_pushback(draw_list->vertex_buffer, color.y);
        gaia_array_pushback(draw_list->vertex_buffer, color.z);
        gaia_array_pushback(draw_list->vertex_buffer, color.w);
        gaia_array_pushback(draw_list->vertex_buffer, 0);
    }

    gaia_array_length(draw_list->index_buffer) += 6;
}

void gui_text_add(struct GuiDrawList *draw_list, vec2s pos, f32 scale, const char *format) {
    struct GuiRenderingData *data = &ctx->rendering_data;
    struct GuiFont *font = ctx->font;
    struct GuiTexture tex = font->atlas;

    f32 tex_index = -1;
    for(size_t i = 0; i < 32; i++) {
        if(data->textures[i].handle == tex.handle) {
            tex_index = i;
        }
    }

    if(tex_index == -1) {
        tex_index = data->texture_count;
        data->textures[(int)data->texture_count] = tex;
        data->texture_count++;
    }

    size_t string_len = strlen(format);
    for(size_t i = 0; i < string_len; i++) {
        u32 index = format[i] - 32;

        if(index == '\n') {
            continue;
        }

        struct GuiChar c = font->chars[index];

        vec2s e_pox = {{
            pos.x + c.bearing.x * scale,
            pos.y - (c.size.y - c.bearing.y) * scale
        }};

        vec2s uv_cords[4] = {
            {{c.uv_min.x, c.uv_max.y}},
            {{c.uv_max.x, c.uv_max.y}},
            {{c.uv_max.x, c.uv_min.y}},
            {{c.uv_min.x, c.uv_min.y}}
        };

        vec2s size = c.size;
        mat4s transform = glms_mat4_mul(
            glms_translate(glms_mat4_identity(), (vec3s){{e_pox.x, e_pox.y, 0}}),
            glms_scale(glms_mat4_identity(), (vec3s){{size.x, size.y, 1.0f}})
        );
        for(u32 i = 0; i < 4; i++) {
            vec3s pos = glms_mat4_mulv3(transform, (vec3s){{QUAD_VERTEX_POS[i].x, QUAD_VERTEX_POS[i].y, QUAD_VERTEX_POS[i].z}}, QUAD_VERTEX_POS[i].w);
            gaia_array_pushback(draw_list->vertex_buffer, pos.x);
            gaia_array_pushback(draw_list->vertex_buffer, pos.y);
            gaia_array_pushback(draw_list->vertex_buffer, pos.z);
            gaia_array_pushback(draw_list->vertex_buffer, uv_cords[i].x);
            gaia_array_pushback(draw_list->vertex_buffer, uv_cords[i].y);
            gaia_array_pushback(draw_list->vertex_buffer, tex_index);
            gaia_array_pushback(draw_list->vertex_buffer, 1);
            gaia_array_pushback(draw_list->vertex_buffer, 1);
            gaia_array_pushback(draw_list->vertex_buffer, 1);
            gaia_array_pushback(draw_list->vertex_buffer, 1);
            gaia_array_pushback(draw_list->vertex_buffer, 1);
        }
        gaia_array_length(draw_list->index_buffer) += 6;

        pos.x += (c.advance >> 6) * scale;
    }
}

void gui_rect_add(struct GuiDrawList *draw_list, struct GuiRect bb, vec4s color) {
    vec2s pos = bb.min;
    vec2s size = {{bb.max.x - bb.min.x, bb.max.y - bb.min.y}};

    gui_box_add(draw_list, pos, (vec2s){{1, size.y}}, color, blank); //
    gui_box_add(draw_list, pos, (vec2s){{size.x, 1}}, color, blank);
    gui_box_add(draw_list, (vec2s){{pos.x, pos.y + size.y}}, (vec2s){{size.x, 1}}, color, blank);
    gui_box_add(draw_list, (vec2s){{pos.x + size.x, pos.y}}, (vec2s){{1, size.y + 1}}, color, blank);
}

void gui_triangle_add(struct GuiDrawList *draw_list, vec2s pos, vec2s size, vec4s color, GuiTrangleSide side) {
    struct GuiRenderingData *data = &ctx->rendering_data;
    GuiTexture tex = blank;

    f32 tex_index = -1;
    for(size_t i = 0; i < 32; i++) {
        if(data->textures[i].handle == tex.handle) {
            tex_index = i;
        }
    }

    if(tex_index == -1) {
        tex_index = data->texture_count;
        data->textures[(int)data->texture_count] = tex;
        data->texture_count++;
    }

    f32 vertecies[] = {
        //x                 y             z  uv_mi|uv_ma
        pos.x           , pos.y         , 0, 0.0f, 0.0f, tex_index, color.x, color.y, color.z, color.w, 0,
        pos.x           , pos.y + size.y, 0, 0.0f, 1.0f, tex_index, color.x, color.y, color.z, color.w, 0,
        pos.x + size.x  , pos.y + size.y, 0, 1.0f, 1.0f, tex_index, color.x, color.y, color.z, color.w, 0,
        pos.x + size.x  , pos.y         , 0, 1.0f, 0.0f, tex_index, color.x, color.y, color.z, color.w, 0,
    };

    switch (side) {
        case GUI_TRIANGLE_CORNER_TOP_L:
        vertecies[31] = pos.x;
        break;
        case GUI_TRIANGLE_CORNER_TOP_R:
        vertecies[0] = pos.x + size.x;
        break;
        case GUI_TRIANGLE_CORNER_BOTTOM_L:
        vertecies[22] = pos.y;
        break;
        case GUI_TRIANGLE_CORNER_BOTTOM_R:
        vertecies[12] = pos.y;
        break;
    }

    //indicies: {3, 0, 1, 3, 1, 2}
    for(u32 i = 0; i < 44; i++) {
        gaia_array_pushback(draw_list->vertex_buffer, vertecies[i]);
    }
    gaia_array_length(draw_list->index_buffer) += 6;
}
