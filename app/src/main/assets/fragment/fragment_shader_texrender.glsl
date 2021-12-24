#version 300 es
precision mediump float;

in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
//单颜色通道的字形位图纹理
uniform sampler2D s_textTexture;
//文字的颜色，
uniform vec3 u_textColor;

void main()
{
    vec4 color = vec4(1.0, 1.0, 1.0, texture(s_textTexture, v_texCoord).r);
    outColor = vec4(u_textColor, 1.0) * color;
}