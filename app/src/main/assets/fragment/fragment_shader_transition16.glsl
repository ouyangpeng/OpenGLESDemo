#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/Radial

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;


const float smoothness = 1.0;
const float PI = 3.141592653589;

vec4 transition(vec2 p) {
    vec2 rp = p*2.-1.;
    return mix(
    texture(u_texture1, p),
    texture(u_texture0, p),
    smoothstep(0., smoothness, atan(rp.y, rp.x) - (u_offset-.5) * PI * 2.5)
    );
}



void main()
{
    outColor = transition(v_texCoord);
}