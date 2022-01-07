#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

// 二分屏
//  纹理坐标的x值不变的，y值变化：
//  当 y 在[0, 0.5]范围时，屏幕的（0，0）坐标需要对应图片的（0，0.25），所以y = y+0.25
//  当 y 在[0.5, 1]范围时，屏幕的（0，0.5）坐标需要对应图片的（0，0.25），所以y = y-0.25

//  链接：https://juejin.cn/post/6859934701932118024

void main() {
    lowp vec2 uv = v_texCoord.xy;
    if (uv.y >= 0.0 && uv.y <= 0.5) {
        uv.y += 0.25;
    } else {
        uv.y -= 0.25;
    }
    outColor = texture(s_TextureMap, uv);
}