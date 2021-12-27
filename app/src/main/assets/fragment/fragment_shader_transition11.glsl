#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/WaterDrop

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;



const float amplitude  = 30.0;
const float speed  = 30.0;

vec4 transition(vec2 uv) {
    vec2 dir = uv - vec2(.5);
    float dist = length(dir);

    if (dist > u_offset) {
        return mix(texture(u_texture0, uv), texture(u_texture1, uv), u_offset);
    } else {
        vec2 offset = dir * sin(dist * amplitude - u_offset * speed);
        return mix(texture(u_texture0, uv + offset), texture(u_texture1, uv), u_offset);
    }
}


void main()
{
    outColor = transition(v_texCoord);
}