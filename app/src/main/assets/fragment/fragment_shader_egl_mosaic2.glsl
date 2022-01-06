#version 300 es
precision highp float;

// 马赛克效果就是把图片的一个相当大小的区域用同一个点的颜色来表示.可以认为是大规模的降低图像的分辨率,而让图像的一些细节隐藏起来。

// https://www.jianshu.com/p/b1f9572b01a1
// 正方形马赛克
// 正方形马赛克就是在获取像素点颜色的时候不去获取当前像素点的颜色，而是使用周边的颜色对这个像素点进行覆盖。如图所示：
// 比如一个图片有4个像素点，分别为
//      1  2
//      3  4
// 图中有1234 四个像素点。
// 在我们进行马赛克处理的时候。会用2号色块的颜色去覆盖134号色块的颜色。
// 其次在我们进行马赛克处理的时候覆盖的颜色 不是凭空产生的 是通过像素点中原有的色块进行覆盖。

layout(location = 0) out vec4 outColor;
// 纹理坐标
in vec2 v_texCoord;
// 纹理采样器
uniform sampler2D s_TextureMap;
// 纹理图片size
const vec2 u_texSize = vec2(400.0, 400.0);
// 马赛克size 通过设置size的大小来控制马赛克的大小
const vec2 mosaicSize = vec2(8.0, 8.0);

void main() {
    //  计算实际图像位置
    vec2 intXY = vec2(
        v_texCoord.x * u_texSize.x,
        v_texCoord.y * u_texSize.y
    );
    // floor(x) 内建函数，返回小于/等于X的最大整数值
    // floor(intXY.x / mosaicSize.x) * mosaicSize.x 计算一个小马赛克的坐标
    vec2 XYMosaic = vec2(
        floor(intXY.x / mosaicSize.x) * mosaicSize.x,
        floor(intXY.y / mosaicSize.y) * mosaicSize.y
    );
    // 换算回纹理坐标
    vec2 UVMosaic = vec2(
        XYMosaic.x / u_texSize.x,
        XYMosaic.y / u_texSize.y
    );
    // 获取到马赛克后的纹理坐标的颜色值
    outColor = texture(s_TextureMap, UVMosaic);
}