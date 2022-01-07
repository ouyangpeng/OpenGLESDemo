#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

//  六分屏  六分屏是二分屏的演变
//  纹理坐标x、y变化规则：
//  当 x 在[0, 1/3]范围时，x = x+1/3
//  当 x 在[1/3, 2/3]范围时，x 不变
//  当 x 在[2/3, 1]范围时，x = x-1/3

//  当 y 在[0, 0.5]范围时，y = y+0.25
//  当 y 在[0.5, 1]范围时，y = y-0.25
//
//  链接：https://juejin.cn/post/6859934701932118024
// https://blog.csdn.net/lin1109221208/article/details/107900718

void main() {
    lowp vec2 uv = v_texCoord.xy;
    if (uv.x <= 1.0 / 3.0){
        uv.x = uv.x + 1.0/3.0;
    } else if (uv.x >= 2.0/3.0){
        uv.x = uv.x - 1.0/3.0;
    }

    if (uv.y <= 0.5){
        uv.y = uv.y + 0.25;
    } else {
        uv.y = uv.y - 0.25;
    }
    outColor = texture(s_TextureMap, uv);
}