#version 330

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 uv;
layout (location = 2) in float tex_index;
layout (location = 3) in vec4 color;

uniform mat4 m, v, p;

out vec2 v_uv;
out float v_tex_index;
out vec4 v_color;

void main() {
    gl_Position = p * v * m * vec4(position, 1.0);
    v_uv = uv;
    v_tex_index = tex_index;
    v_color = color;
}