#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/windowslice

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;

const float count = 10.0;
const float smoothness = 0.5;

vec4 transition (vec2 uv) {
    float progress =  u_offset;
    float pr = smoothstep(-smoothness, 0.0, uv.x - progress * (1.0 + smoothness));
    float s = step(pr, fract(count * uv.x));
    return mix(texture(u_texture0, uv), texture(u_texture1, uv), s);
}


void main()
{
    outColor = transition(v_texCoord);
}