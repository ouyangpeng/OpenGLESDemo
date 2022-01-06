#version 300 es
precision highp float;

// https://www.jianshu.com/p/b1f9572b01a1
// https://www.jianshu.com/p/0eec2727fc2f
// https://www.jianshu.com/p/87866b7b46d9
// 六边形马赛克，六边形马赛克 顾名思义。就是马赛克的图形是一个六边形组成。
// 我们再获取像素点的颜色的时候 是已这个像素点距离六边形中心点距离更近的那一个像素点的颜色。

layout(location = 0) out vec4 outColor;
// 纹理坐标
in vec2 v_texCoord;
// 纹理采样器
uniform sampler2D s_TextureMap;
// 六边形的边长 马赛克size 通过设置size的大小来控制马赛克的大小
const float mosaicSize = 0.015;

void main() {
    float length = mosaicSize;
    //矩形的长的比例为3，取值 3/2 = 1.5，也可以直接取3
    float dx = 1.5;
    //矩形的高的比例为√3，取值 √3/2 ，也可以直接取√3
    float dy = 0.866025;

    //取出纹理坐标
    float x = v_texCoord.x;
    float y = v_texCoord.y;

    //根据纹理坐标计算出对应的矩阵坐标
    //即 矩阵坐标wx = int（纹理坐标x/ 矩阵长），矩阵长 = dy*len
    //即 矩阵坐标wy = int（纹理坐标y/ 矩阵宽），矩阵宽 = dx*len
    // 当前位于第几行和第几列矩形
    int wx = int(x / dx / length);
    int wy = int(y / dy / length);

    vec2 v1, v2, vn;

    // https://www.jianshu.com/p/87866b7b46d9
    //偶行偶列：（0，0）（1，1）/，即左上、右下
    //偶行奇列：（0，1）（1，0）\，即左下、右上
    //奇行偶列：（0，1）（1，0）\，即左下、右上
    //奇行奇列：（0，0）（1，1）/，即左上、右下

    //判断wx是否为偶数，等价于 wx % 2 == 0
    if (wx/2 * 2 == wx) { // 偶数行
        if (wy/2 * 2 == wy) { // 偶数列
            //偶行偶列：（0，0）（1，1）/，即左上、右下
            v1 = vec2(length * dx * float(wx), length * dy * float(wy));
            v2 = vec2(length * dx * float(wx+1), length * dy * float(wy+1));
        } else { // 奇数列
            //偶行奇列：（0，1）（1，0）\，即左下、右上
            v1 = vec2(length * dx * float(wx), length * dy * float(wy+1));
            v2 = vec2(length * dx * float(wx+1), length * dy * float(wy));
        }
    } else { // 奇数行
        if (wy/2 * 2 == wy) { //偶数列
            //奇行偶列：（0，1）（1，0）\，即左下、右上
            v1 = vec2(length * dx * float(wx), length * dy * float(wy+1));
            v2 = vec2(length * dx * float(wx+1), length * dy * float(wy));
        } else { // 奇数列
            //奇行奇列：（0，0）（1，1）/，即左上、右下
            v1 = vec2(length * dx * float(wx), length * dy * float(wy));
            v2 = vec2(length * dx * float(wx+1), length * dy * float(wy+1));
        }
    }

    //利用距离公式，计算中心点与当前像素点的距离
    // 当前坐标到v1、v2的距离
    float d1 = sqrt(pow(v1.x-x, 2.0) + pow(v1.y-y, 2.0));
    float d2 = sqrt(pow(v2.x-x, 2.0) + pow(v2.y-y, 2.0));

    //选择距离小的则为六边形的中心点，且获取它的颜色
    vn = (d1<d2) ? v1 : v2;
    //获取六边形中心点的颜色值
    vec4 mask = texture(s_TextureMap, vn);
    outColor = mask;
}