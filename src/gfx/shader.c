#include "./../internal.h"

static void _log_and_fail(
    GLint handle, const char *adverb, const char *path,
    void (*getlog)(GLuint, GLsizei, GLsizei *, GLchar*),
    void (*getiv)(GLuint, GLenum, GLint *)) {
    GLint loglen;
    getiv(handle, GL_INFO_LOG_LENGTH, &loglen);

    char *logtext = calloc(1, loglen);
    getlog(handle, loglen, NULL, logtext);
    fprintf(stderr, "Error %s shader at %s:\n%s", adverb, path, logtext);

    free(logtext);
    exit(1);
}

static GLint _compile(const char* path, GLenum type) {
    FILE *f;
    char *text;
    long len;

    f = fopen(path, "rb");
    if (f == NULL) {
        fprintf(stderr, "error loading shader at %s\n", path);
        exit(1);
    }

    fseek(f, 0, SEEK_END);
    len = ftell(f);
    assert(len > 0);
    fseek(f, 0, SEEK_SET);
    text = calloc(1, len);
    assert(text != NULL);
    fread(text, 1, len, f);
    assert(strlen(text) > 0);
    fclose(f);

    //create and compile shader
    GLuint handle = glCreateShader(type);
    glShaderSource(handle, 1, (const GLchar *const *) &text, (const GLint *) &len);
    glCompileShader(handle);

    GLint compiled;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &compiled);

    // Check OpenGL logs if compilation failed
    if (compiled == 0) {
        _log_and_fail(handle, "compiling", path, glGetShaderInfoLog, glGetShaderiv);
    }

    free(text);
    //return shader
    return handle;
}

GUI_API GuiShader gui_shader_create(const char *vs_path, const char *fs_path) {
    GuiShader self = {
        .vs_handle = _compile(vs_path, GL_VERTEX_SHADER),
        .fs_handle = _compile(fs_path, GL_FRAGMENT_SHADER),
        .handle = glCreateProgram(),
    };

    glAttachShader(self.handle, self.vs_handle);
    glAttachShader(self.handle, self.fs_handle);

    glLinkProgram(self.handle);

    GLint linked;
        glGetProgramiv(self.handle, GL_LINK_STATUS, &linked);

    if (linked == 0) {
        char buf[512];
        snprintf(buf, 512, "[%s, %s]", vs_path, fs_path);
        _log_and_fail(self.handle, "linking", buf, glGetProgramInfoLog, glGetProgramiv);
    }

    return self;
}

GUI_API void gui_shader_bind(GuiShader self) {
    glUseProgram(self.handle);
}

GUI_API void gui_shader_uniform_float(GuiShader self, char *name, float f) {
    glUniform1f(glGetUniformLocation(self.handle, name), f);
} 

GUI_API void gui_shader_uniform_mat4(GuiShader self, char *name, mat4s m) {
    glUniformMatrix4fv(glGetUniformLocation(self.handle, name), 1, GL_FALSE, (const GLfloat *) &m.raw);
}

GUI_API void gui_shader_uniform_view_proj(GuiShader self, GuiViewProj view_proj) {
    gui_shader_uniform_mat4(self, "p", view_proj.proj);
    gui_shader_uniform_mat4(self, "v", view_proj.view);
}

GUI_API void gui_shader_uniform_vec4(GuiShader self, char *name, vec4s v) {
    glUniform4f(glGetUniformLocation(self.handle, name), v.x, v.y, v.z, v.w);
}

GUI_API void gui_shader_uniform_texture2D(GuiShader self, char *name, struct GuiTexture texture, GLuint n) {
    glActiveTexture(GL_TEXTURE0 + n);
    gui_texture_bind(texture);
    glUniform1i(glGetUniformLocation(self.handle, (const GLchar *) name), n);
}

GUI_API void gui_shader_uniform_textures2D(GuiShader self, char *name) {
    int samplers[32];
    for(size_t i = 0; i < 32; i++) {
        samplers[i] = i;
    }
    glUniform1iv(glGetUniformLocation(self.handle, name), 32, samplers);
}