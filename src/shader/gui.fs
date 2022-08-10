#version 330

uniform sampler2D textures[32];

in vec2 v_uv;
in float v_tex_index;
in vec4 v_color;

out vec4 frag_color;

void main() {
    // frag_color = color * texture(tex, v_uv);
    switch(int(v_tex_index)) {
        case  0: frag_color = v_color * texture(textures[ 0], v_uv); break;
        case  1: frag_color = v_color * texture(textures[ 1], v_uv); break;
        case  2: frag_color = v_color * texture(textures[ 2], v_uv); break;
        case  3: frag_color = v_color * texture(textures[ 3], v_uv); break;
        case  4: frag_color = v_color * texture(textures[ 4], v_uv); break;
        case  5: frag_color = v_color * texture(textures[ 5], v_uv); break;
        case  6: frag_color = v_color * texture(textures[ 6], v_uv); break;
        case  7: frag_color = v_color * texture(textures[ 7], v_uv); break;
        case  8: frag_color = v_color * texture(textures[ 8], v_uv); break;
        case  9: frag_color = v_color * texture(textures[ 9], v_uv); break;
        case 10: frag_color = v_color * texture(textures[10], v_uv); break;
        case 11: frag_color = v_color * texture(textures[11], v_uv); break;
        case 12: frag_color = v_color * texture(textures[12], v_uv); break;
        case 13: frag_color = v_color * texture(textures[13], v_uv); break;
        case 14: frag_color = v_color * texture(textures[14], v_uv); break;
        case 15: frag_color = v_color * texture(textures[15], v_uv); break;
        case 16: frag_color = v_color * texture(textures[16], v_uv); break;
        case 17: frag_color = v_color * texture(textures[17], v_uv); break;
        case 18: frag_color = v_color * texture(textures[18], v_uv); break;
        case 19: frag_color = v_color * texture(textures[19], v_uv); break;
        case 20: frag_color = v_color * texture(textures[20], v_uv); break;
        case 21: frag_color = v_color * texture(textures[21], v_uv); break;
        case 22: frag_color = v_color * texture(textures[22], v_uv); break;
        case 23: frag_color = v_color * texture(textures[23], v_uv); break;
        case 24: frag_color = v_color * texture(textures[24], v_uv); break;
        case 25: frag_color = v_color * texture(textures[25], v_uv); break;
        case 26: frag_color = v_color * texture(textures[26], v_uv); break;
        case 27: frag_color = v_color * texture(textures[27], v_uv); break;
        case 28: frag_color = v_color * texture(textures[28], v_uv); break;
        case 29: frag_color = v_color * texture(textures[29], v_uv); break;
        case 30: frag_color = v_color * texture(textures[30], v_uv); break;
        case 31: frag_color = v_color * texture(textures[31], v_uv); break;
        // case 32: frag_color = texture(textures[32], v_uv); break;
    }
    // frag_color = texture(tex, v_uv);
}