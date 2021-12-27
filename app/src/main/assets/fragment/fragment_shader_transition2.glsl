#version 300 es
precision mediump float;
in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;

const float speed = 1.0;
const float angle = 1.0;
const float power = 1.5;

vec4 transition(vec2 uv) {
    vec2 p = uv.xy / vec2(1.0).xy;
    vec2 q = p;
    float t = pow(u_offset, power)*speed;
    p = p -0.5;
    for (int i = 0; i < 7; i++) {
        p = vec2(sin(t)*p.x + cos(t)*p.y, sin(t)*p.y - cos(t)*p.x);
        t += angle;
        p = abs(mod(p, 2.0) - 1.0);
    }
    abs(mod(p, 1.0));
    return mix(
    mix(texture(u_texture0, q), texture(u_texture1, q), u_offset),
    mix(texture(u_texture0, p), texture(u_texture1, p), u_offset), 1.0 - 2.0*abs(u_offset - 0.5));
}

void main()
{
    outColor = transition(v_texCoord);
}