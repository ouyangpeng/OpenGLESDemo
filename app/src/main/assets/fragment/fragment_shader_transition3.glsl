#version 300 es
precision mediump float;

// 源着色器地址： https://gl-transitions.com/editor/SimpleZoom

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
uniform sampler2D u_texture0;
uniform sampler2D u_texture1;
uniform float u_offset;
uniform vec2 u_texSize;

const float zoom_quickness = 0.8;

vec2 zoom(vec2 uv, float amount) {
    return 0.5 + ((uv - 0.5) * (1.0-amount));
}

vec4 transition (vec2 uv) {
    float nQuick = clamp(zoom_quickness, 0.2, 1.0);
    return mix(
        texture(u_texture0, zoom(uv, smoothstep(0.0, nQuick, u_offset))),
        texture(u_texture1, uv),
        smoothstep(nQuick-0.2, 1.0, u_offset)
    );
}

void main()
{
    outColor = transition(v_texCoord);
}