#version 300 es
precision highp float;

// 图像颠倒实现比较简单，在文理采样的时候我们只需要反转UV坐标，便可以实现图像颠倒的效果。

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

void main() {
    // 将纹理坐标做映射 （y -> 1 - y）
    vec4 color = texture(s_TextureMap, vec2(v_texCoord.x, 1.0 - v_texCoord.y));
    outColor = color;
}