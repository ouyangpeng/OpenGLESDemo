#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/heart

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;


float inHeart (vec2 uv, vec2 center, float size) {
    if (size==0.0) return 0.0;

    // 这里 得把坐标改一下，否则心形是反的  uv 改成 -uv
    //    vec2 o = (uv + center)/(1.6 * size);
    vec2 o = (-uv + center)/(1.6 * size);
    float a = o.x * o.x + o.y * o.y - 0.3;
    // step(edge,x)  如果x < edge，返回0.0，否则返回1.0
    return step(a * a * a, o.x * o.x * o.y * o.y * o.y);
}

vec4 transition (vec2 uv) {
    return mix(
        texture(u_texture0,uv),
        texture(u_texture1,uv),
        inHeart(uv, vec2(0.5, 0.5), u_offset)
    );
}


void main()
{
    outColor = transition(v_texCoord);
}