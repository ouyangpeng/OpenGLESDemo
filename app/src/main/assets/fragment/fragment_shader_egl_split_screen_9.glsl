#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

//  九分屏
//  纹理坐标x、y变化规则：
//  当 x 在[0, 1/3]范围时，x = x*3
//  当 x 在[1/3, 2/3]范围时，x = (x-1/3)*3
//  当 x 在[2/3, 1]范围时，x = (x-2/3)*3

//  当 y 在[0, 1/3]范围时，y= y*3
//  当 y 在[1/3, 2/3]范围时，y = (y-1/3)*3
//  当 y在[2/3, 1]范围时，y = (y-2/3)*3

//  链接：https://juejin.cn/post/6859934701932118024

void main() {
    lowp vec2 uv = v_texCoord.xy;
    if (uv.x < 1.0 / 3.0) {
        uv.x = uv.x * 3.0;
    } else if (uv.x < 2.0 / 3.0) {
        uv.x = (uv.x - 1.0 / 3.0) * 3.0;
    } else {
        uv.x = (uv.x - 2.0 / 3.0) * 3.0;
    }
    if (uv.y <= 1.0 / 3.0) {
        uv.y = uv.y * 3.0;
    } else if (uv.y < 2.0 / 3.0) {
        uv.y = (uv.y - 1.0 / 3.0) * 3.0;
    } else {
        uv.y = (uv.y - 2.0 / 3.0) * 3.0;
    }
    outColor = texture(s_TextureMap, uv);
}