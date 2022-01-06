#version 300 es
precision highp float;

// https://www.jianshu.com/p/b1f9572b01a1
// https://www.jianshu.com/p/0eec2727fc2f
// 六边形马赛克，六边形马赛克 顾名思义。就是马赛克的图形是一个六边形组成。
// 我们再获取像素点的颜色的时候 是已这个像素点距离六边形中心点距离更近的那一个像素点的颜色。

layout(location = 0) out vec4 outColor;
// 纹理坐标
in vec2 v_texCoord;
// 纹理采样器
uniform sampler2D s_TextureMap;
// 马赛克size 通过设置size的大小来控制马赛克的大小
const float mosaicSize = 0.015;

void main() {
    float length = mosaicSize;

    float dx = 1.5;
    float dy = 0.866025;

    float x = v_texCoord.x;
    float y = v_texCoord.y;

    // 当前位于第几行和第几列矩形
    int wx = int(x / dx / length);
    int wy = int(y / dy / length);

    vec2 v1, v2, vn;

    if (wx/2 * 2 == wx) { // 偶数行
        if (wy/2 * 2 == wy) { // 偶数列
            v1 = vec2(length*dx*float(wx), length*dy*float(wy));
            v2 = vec2(length*dx*float(wx+1), length*dy*float(wy+1));
        } else { // 奇数列
            v1 = vec2(length*dx*float(wx), length*dy*float(wy+1));
            v2 = vec2(length*dx*float(wx+1), length*dy*float(wy));
        }
    } else { // 奇数行
        if (wy/2 * 2 == wy) { //偶数列
            v1 = vec2(length*dx*float(wx), length*dy*float(wy+1));
            v2 = vec2(length*dx*float(wx+1), length*dy*float(wy));
        } else { // 奇数列
            v1 = vec2(length*dx*float(wx), length*dy*float(wy));
            v2 = vec2(length*dx*float(wx+1), length*dy*float(wy+1));
        }
    }

    // 当前坐标到v1、v2的距离
    float d1 = sqrt(pow(v1.x-x, 2.0) + pow(v1.y-y, 2.0));
    float d2 = sqrt(pow(v2.x-x, 2.0) + pow(v2.y-y, 2.0));

    if (d1 < d2) {
        vn = v1;
    } else {
        vn = v2;
    }

    vec4 mask = texture(s_TextureMap, vn);
    outColor = mask;
}