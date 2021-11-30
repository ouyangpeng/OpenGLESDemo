#version 300 es                                     
precision mediump float;

in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;
//声明采用器
uniform sampler2D s_TextureMap;

void main()
{
    // texture() 为内置的采样函数，v_texCoord 为顶点着色器传进来的纹理坐标
    // 根据纹理坐标对纹理进行采样，输出采样的 rgba 值（4维向量）

    // 内建函数texture的函数形式如下：
    // vec4 texture(sampler2D samper, vec2 coord[,float bias])
    // samper 绑定到纹理单元的采样器，指定纹理为读取来源
    // coord  用于纹理贴图中读取的2D纹理坐标
    // bias 可选参数，提供用于纹理读取的mip贴图偏置。这允许着色器明确地偏置用于mip贴图选择的LOD计算值。
    // texture函数返回一个代表从纹理贴图中读取颜色的vect4

    outColor = texture(s_TextureMap, v_texCoord);
    //outColor = texelFetch(s_TextureMap,  ivec2(int(v_texCoord.x * 404.0), int(v_texCoord.y * 336.0)), 0);
}