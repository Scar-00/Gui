#version 330 core

uniform sampler2D textures[32];

in vec2 v_uv;
in float v_tex_index;
in vec4 v_color;
in float v_kind;

out vec4 frag_color;

void main() {
    vec4 tex = vec4(1, 0, 1, 1);
    switch(int(v_tex_index)) {
        case  0: tex = v_color * texture(textures[ 0], v_uv); break;
        case  1: tex = v_color * texture(textures[ 1], v_uv); break;
        case  2: tex = v_color * texture(textures[ 2], v_uv); break;
        case  3: tex = v_color * texture(textures[ 3], v_uv); break;
        case  4: tex = v_color * texture(textures[ 4], v_uv); break;
        case  5: tex = v_color * texture(textures[ 5], v_uv); break;
        case  6: tex = v_color * texture(textures[ 6], v_uv); break;
        case  7: tex = v_color * texture(textures[ 7], v_uv); break;
        case  8: tex = v_color * texture(textures[ 8], v_uv); break;
        case  9: tex = v_color * texture(textures[ 9], v_uv); break;
        case 10: tex = v_color * texture(textures[10], v_uv); break;
        case 11: tex = v_color * texture(textures[11], v_uv); break;
        case 12: tex = v_color * texture(textures[12], v_uv); break;
        case 13: tex = v_color * texture(textures[13], v_uv); break;
        case 14: tex = v_color * texture(textures[14], v_uv); break;
        case 15: tex = v_color * texture(textures[15], v_uv); break;
        case 16: tex = v_color * texture(textures[16], v_uv); break;
        case 17: tex = v_color * texture(textures[17], v_uv); break;
        case 18: tex = v_color * texture(textures[18], v_uv); break;
        case 19: tex = v_color * texture(textures[19], v_uv); break;
        case 20: tex = v_color * texture(textures[20], v_uv); break;
        case 21: tex = v_color * texture(textures[21], v_uv); break;
        case 22: tex = v_color * texture(textures[22], v_uv); break;
        case 23: tex = v_color * texture(textures[23], v_uv); break;
        case 24: tex = v_color * texture(textures[24], v_uv); break;
        case 25: tex = v_color * texture(textures[25], v_uv); break;
        case 26: tex = v_color * texture(textures[26], v_uv); break;
        case 27: tex = v_color * texture(textures[27], v_uv); break;
        case 28: tex = v_color * texture(textures[28], v_uv); break;
        case 29: tex = v_color * texture(textures[29], v_uv); break;
        case 30: tex = v_color * texture(textures[30], v_uv); break;
        case 31: tex = v_color * texture(textures[31], v_uv); break;
    }
    if(int(v_kind) == 1) {
        //red part of the texture [everything in a font]
        float d = tex.r;
        //anti aliasing float calculted based on the derivite of the 2 neigouring pixels
        float aaf = fwidth(d);
        //0.5 is the actual shape of the char and we do anti aliasing out/in-ward
        float alpha = smoothstep(0.5 - aaf, 0.5 + aaf, d);
        //combine color and the shape of the char
        frag_color = vec4(v_color.rgb, alpha);
    }else {
        frag_color = tex;
    }
}
