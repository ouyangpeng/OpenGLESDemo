#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

//  十六分屏
//  链接：https://juejin.cn/post/7003867568851124238
// https://blog.csdn.net/lin1109221208/article/details/107900718

void main() {
    lowp vec2 uv = v_texCoord.xy;
    uv.x = mod(uv.x, 0.25) * 4.0;
    uv.y = mod(uv.y, 0.25) * 4.0;
    outColor = texture(s_TextureMap, uv);
}