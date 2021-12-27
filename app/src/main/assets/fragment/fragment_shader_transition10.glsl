#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/PolkaDotsCurtain

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;


const float SQRT_2 = 1.414213562373;
const float dots  = 20.0;
const vec2 center = vec2(0, 0);

vec4 transition(vec2 uv) {
    bool nextImage = distance(fract(uv * dots), vec2(0.5, 0.5)) < (u_offset / distance(uv, center));
    return nextImage ?texture(u_texture1, uv) : texture(u_texture0, uv);
}

void main()
{
    outColor = transition(v_texCoord);
}