#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "./../internal.h"

void gui_texture_pixels_load(const char *path, u8 **pixels_out, u32 *width_out, u32 *height_out) {
    s32 width, height, channels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char* image = stbi_load(path, &width, &height, &channels, STBI_rgb_alpha);
    assert(image != NULL);

    *pixels_out = malloc(width * height * 4);
    memcpy(*pixels_out, image, width * height * 4);
    *width_out = width;
    *height_out = height;

    stbi_image_free(image);
}

struct GuiTexture gui_texture_from_pixels(u8 *pixels, u32 width, u32 height) {
    struct GuiTexture self = {
        .size = (vec2s) {{ width, height }}
    };

    glGenTextures(1, &self.handle);
    glBindTexture(GL_TEXTURE_2D, self.handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
    return self;
}

struct GuiTexture gui_texture_from_font(u8 *pixels, u32 width, u32 height) {
    struct GuiTexture self = {
        .size = (vec2s) {{ width, height }}
    };

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glGenTextures(1, &self.handle);
    glBindTexture(GL_TEXTURE_2D, self.handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //GLint swizzle[] = { GL_ONE, GL_ONE, GL_ONE, GL_RED };
    //glTexParameteriv(GL_TEXTURE_2D, GL_TEXTURE_SWIZZLE_RGBA, swizzle);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RED, width, height, 0, GL_RED, GL_UNSIGNED_BYTE, pixels);
    return self;
}

struct GuiTexture gui_texture_from_path(const char *path) {
    u8* pixels;
    u32 width, height;
    gui_texture_pixels_load(path, &pixels, &width, &height);
    struct GuiTexture self = gui_texture_from_pixels(pixels, width, height);
    free(pixels);
    return self;
}

void gui_texture_destroy(struct GuiTexture self) {
    glDeleteTextures(1, &self.handle);
}

void gui_texture_bind(struct GuiTexture self) {
    glBindTexture(GL_TEXTURE_2D, self.handle);
}
