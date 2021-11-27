#version 300 es

// 多重纹理片段着色器

precision mediump float;

in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

// 两个采样器，每个纹理使用一个
uniform sampler2D s_baseMap;
uniform sampler2D s_lightMap;

void main()
{
    vec4 baseColor;
    vec4 lightColor;

    baseColor = texture(s_baseMap, v_texCoord);
    lightColor = texture(s_lightMap, v_texCoord);
    outColor = baseColor * (lightColor + 0.25);
}