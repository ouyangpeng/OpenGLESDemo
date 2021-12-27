#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/pinwheel

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;


uniform float speed;// = 2.0;

vec4 transition(vec2 uv) {
    vec2 p = uv.xy / vec2(1.0).xy;
    float circPos = atan(p.y - 0.5, p.x - 0.5) + u_offset * speed;
    float modPos = mod(circPos, 3.1415 / 4.);
    float signed = sign(u_offset - modPos);
    return mix(
        texture(u_texture1, p),
        texture(u_texture0, p),
        step(signed, 0.5)
    );
}



void main()
{
    outColor = transition(v_texCoord);
}