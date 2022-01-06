#version 300 es
precision highp float;

// 冷色滤镜
// https://juejin.cn/post/6844903885711245325

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

void main() {
    lowp vec4 textureColor = texture(s_TextureMap, v_texCoord);
    // 将rgb通道的颜色添加相应的蓝色值，再作为rgb通道的值，
    outColor = textureColor + vec4(0.0, 0.0, 0.3, 0.0);
}