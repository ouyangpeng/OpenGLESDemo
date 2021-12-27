#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/circleopen

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;



const float smoothness  = 0.3;
const bool opening = true;
const vec2 center = vec2(0.5, 0.5);
const float SQRT_2 = 1.414213562373;

vec4 transition (vec2 uv) {
    float x = opening ? u_offset : 1.-u_offset;
    float m = smoothstep(-smoothness, 0.0, SQRT_2*distance(center, uv) - x*(1.+smoothness));
    return mix(
        texture(u_texture0, uv),
        texture(u_texture1, uv),
        opening ? 1.-m : m);
}

void main()
{
    outColor = transition(v_texCoord);
}