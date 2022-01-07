#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

//  四分屏
//  屏幕四等分，分别显示缩小的纹理图片：
//  当 x 在[0, 0.5]范围时，x = x*2
//  当 x在[0.5, 1]范围时，x = (x-0.5)*2

//  当 y 在[0, 0.5]范围时，y = y*2
//  当 y 在[0.5, 1]范围时，y = (y-0.5)*2

//  链接：https://juejin.cn/post/6859934701932118024
// https://blog.csdn.net/lin1109221208/article/details/107900718

void main() {
    lowp vec2 uv = v_texCoord.xy;
    if (uv.x <= 0.5){
        uv.x = uv.x * 2.0;
    } else {
        uv.x = (uv.x - 0.5) * 2.0;
    }

    if (uv.y<= 0.5) {
        uv.y = uv.y * 2.0;
    } else {
        uv.y = (uv.y - 0.5) * 2.0;
    }
    outColor = texture(s_TextureMap, uv);
}