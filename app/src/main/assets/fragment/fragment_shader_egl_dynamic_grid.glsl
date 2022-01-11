#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in highp vec2 v_texCoord;

uniform sampler2D s_TextureMap;

uniform vec2 u_texSize;

// 这个值可以通过外面传进来，这里做测试写死
//uniform float u_offset;//偏移量
float u_offset = 0.15;

// 动态网格滤镜
// 参考自：https://blog.csdn.net/zhying719/article/details/108332549
// 动态网格滤镜主要是将纹理划分为多个网格，然后根据一个偏移量动态改变网格线的宽度。
// mod 和 floor 为 GLSL 的内建函数，分别表示取模和取整。
// 需要注意的是，计算之前需要将纹理坐标系转换为图片坐标系，保证网格没有被拉伸。
void main()
{
    vec2 imgTexCoord = v_texCoord * u_texSize;//将纹理坐标系转换为图片坐标系
    float sideLength = u_texSize.y / 6.0;//网格的边长
    float maxOffset = 0.15 * sideLength;//设置网格线宽度的最大值
    float x = mod(imgTexCoord.x, floor(sideLength));
    float y = mod(imgTexCoord.y, floor(sideLength));

    float offset = u_offset * maxOffset;

    if (offset <= x
        && x <= sideLength - offset
        && offset <= y
        && y <= sideLength - offset){
        outColor = texture(s_TextureMap, v_texCoord);
    } else {
        outColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}