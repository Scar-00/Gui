#include "./../internal.h"

struct GuiFont *gui_font_init(const char *path, u32 size) {
    struct GuiFont *self = malloc(sizeof(struct GuiFont));
    self->chars = gaia_array_create(struct GuiChar, 96);

    FT_Library ft;
    if(FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Could not init freetye\n");
        exit(1);
    }

    FT_Face face;
    if(FT_New_Face(ft, path, 0, &face)) {
        fprintf(stderr, "Could not load font\n");
        exit(1);
    }

    FT_Set_Pixel_Sizes(face, 0, size);

    s32 max_dim = (1 + (face->size->metrics.height >> 6)) * ceilf(sqrtf(96));

    ivec2s tex_size = {{1, 0}};
    while(tex_size.x < max_dim) tex_size.x <<= 1;
    tex_size.y = tex_size.x;

    ivec2s pen = {{0, 0}};

    u8 *bitmap = calloc(tex_size.x * tex_size.y, sizeof(u8));

    for(u32 c = 32; c < 128; c++) {
        if(FT_Load_Char(face, c, FT_LOAD_RENDER)) {
            fprintf(stderr, "Could not load char[%c]\n", c);
            exit(1);
        }

        FT_Bitmap *bmp = &face->glyph->bitmap;

        if(pen.x + bmp->width >= (u32)tex_size.x){
            pen.x = 0;
            pen.y += ((face->size->metrics.height >> 6) + 1);
        }

        for(u32 row = 0; row < bmp->rows; ++row){
            for(u32 col = 0; col < bmp->width; ++col){
                int x = pen.x + col;
                int y = pen.y + row;
                bitmap[y * tex_size.x + x] = bmp->buffer[row * bmp->pitch + col];
            }
        }

        struct GuiChar character = {
            .size = {{face->glyph->bitmap.width, face->glyph->bitmap.rows}},
            .bearing = {{face->glyph->bitmap_left, face->glyph->bitmap_top}},
            .uv_min = {{pen.x / (float)tex_size.x, pen.y / (float)tex_size.x}},
            .uv_max = {{(pen.x + bmp->width) / (float)tex_size.y, (pen.y + bmp->rows) / (float)tex_size.y}},
            .advance = face->glyph->advance.x
        };

        gaia_array_pushback(self->chars, character);
        pen.x += bmp->width + 1;
    }

    self->size = (vec2s){{tex_size.x, tex_size.y}};
    self->atlas = gui_texture_from_font(bitmap, tex_size.x, tex_size.y);

    free(bitmap);

    return self;
}
