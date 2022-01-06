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
const float mosaicSize = 0.03;

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
    //即 矩阵坐标wx = int（纹理坐标x/ 矩阵长），矩阵长 = dy * len
    //即 矩阵坐标wy = int（纹理坐标y/ 矩阵宽），矩阵宽 = dx * len
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
            v2 = vec2(length * dx * float(wx + 1), length * dy * float(wy + 1));
        } else { // 奇数列
            //偶行奇列：（0，1）（1，0）\，即左下、右上
            v1 = vec2(length * dx * float(wx), length * dy * float(wy + 1));
            v2 = vec2(length * dx * float(wx + 1), length * dy * float(wy));
        }
    } else { // 奇数行
        if (wy/2 * 2 == wy) { //偶数列
            //奇行偶列：（0，1）（1，0）\，即左下、右上
            v1 = vec2(length * dx * float(wx), length * dy * float(wy + 1));
            v2 = vec2(length * dx * float(wx + 1), length * dy * float(wy));
        } else { // 奇数列
            //奇行奇列：（0，0）（1，1）/，即左上、右下
            v1 = vec2(length * dx * float(wx), length * dy * float(wy));
            v2 = vec2(length * dx * float(wx + 1), length * dy * float(wy + 1));
        }
    }

    //利用距离公式，计算中心点与当前像素点的距离
    // 当前坐标到v1、v2的距离
    float d1 = sqrt(pow(v1.x - x, 2.0)  +  pow(v1.y - y, 2.0));
    float d2 = sqrt(pow(v2.x - x, 2.0)  +  pow(v2.y - y, 2.0));

    //选择距离小的则为六边形的中心点，且获取它的颜色
    vn = (d1<d2) ? v1 : v2;

    //====================片元着色器代码：在六边形滤镜算法（即 vn = (s1 < s2) ? v1 : v2;）后增加如下代码 ====================

    // 将π分为3等分
    float PI3 = 3.14159/3.0;
    // 获得弧度值  获取像素点与中心点的角度
    // atan是GLSL中的内建函数，有两种计算方式
    //  1、atan(y,x) 值域是[0，π],
    //  2、atan(y/x),值域是[-π/2, π/2]
    float a = atan((y - vn.y), (x - vn.x));
    // 每个中心点与原点的xy偏移值
    float xoffset = length * 0.5;
    float yoffset = xoffset * dy;

    // 判断夹角，属于哪个三角形，则获取哪个三角形的中心点坐标
    // 对象图中6个三角形区域的中心点
    vec2 area1 = vec2(vn.x + xoffset, vn.y + yoffset);
    vec2 area2 = vec2(vn.x, vn.y + yoffset);
    vec2 area3 = vec2(vn.x - xoffset, vn.y + yoffset);
    vec2 area4 = vec2(vn.x - xoffset, vn.y - yoffset);
    vec2 area5 = vec2(vn.x, vn.y - yoffset);
    vec2 area6 = vec2(vn.x + xoffset, vn.y - yoffset);

    // 判断当前坐标位于哪个区域
    if (a >= 0.0 && a < PI3) {
        vn = area1;
    } else if (a >= PI3 && a < PI3 * 2.0) {
        vn = area2;
    } else if (a >= PI3 * 2.0 && a <= PI3 * 3.0) {
        vn = area3;
    } else if (a <=  - PI3 * 2.0 && a >=  - PI3 * 3.0) {
        vn = area4;
    } else if (a <=  - PI3 && a >  - PI3 * 2.0) {
        vn = area5;
    } else if (a <= 0.0 && a <  - PI3) {
        vn = area6;
    }
    //获取对应三角形重心的颜色值
    vec4 mask = texture(s_TextureMap, vn);
    // 将颜色值填充到outColor
    outColor = mask;
}