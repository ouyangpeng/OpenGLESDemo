#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/CircleCrop

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;



const vec4 bgcolor = vec4(0.0, 0.0, 0.0, 1.0);
// A that corresponds to height/width  一般为16:9
const float ratio = 16.0/9.0;
vec2 ratio2 = vec2(1.0, 1.0 / ratio);

vec4 transition(vec2 uv) {
    float dist = length((vec2(uv) - 0.5) * ratio2);

    float s = pow(2.0 * abs(u_offset - 0.5), 3.0);

    return mix(
        // branching is ok here as we statically depend on u_offset uniform (branching won't change over pixels)
        u_offset < 0.5 ? texture(u_texture0, uv) : texture(u_texture1, uv),
        bgcolor,
        step(s, dist)
    );
}


void main()
{
    outColor = transition(v_texCoord);
}