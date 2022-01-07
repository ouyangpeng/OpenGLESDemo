#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

//  三分屏
//  纹理坐标的x值不变，y值变化：
//  当 y 在[0, 1/3]范围时，屏幕的（0，0）坐标需要对应图片的（0，1/3），所以y = y+1/3
//  当 y 在[1/3, 2/3]范围时，屏幕的（0，1/3）坐标需要对应图片的（0，1/3），所以y 不变
//  当 y 在[2/3, 1]范围时，屏幕的（0，2/3）坐标需要对应图片的（0，1/3），所以y = y-1/3

//链接：https://juejin.cn/post/6859934701932118024
// https://blog.csdn.net/lin1109221208/article/details/107900718

void main() {
    lowp vec2 uv = v_texCoord.xy;
    if (uv.y < 1.0/3.0) {
        uv.y = uv.y + 1.0/3.0;
    } else if (uv.y > 2.0/3.0){
        uv.y = uv.y - 1.0/3.0;
    }
    outColor = texture(s_TextureMap, uv);
}