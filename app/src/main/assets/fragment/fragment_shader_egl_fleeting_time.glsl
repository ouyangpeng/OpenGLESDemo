#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in highp vec2 v_texCoord;

uniform sampler2D s_TextureMap;

void main()
{
    vec3 originalRGB = texture(s_TextureMap, v_texCoord).rgb;
    //  流年是用来形容如水般流逝的光阴或年华，图像处理中特指将原图像转换为具有时代感或岁月沉淀的特效
    //  它将原始图像的蓝色（B）通道的像素值开根号，再乘以一个权重参数，产生最终的流年效果。
    //  公式参考自：https://blog.51cto.com/u_15127682/4622632

    //  B通道的数值开平方乘以 权重参数3，这个权重参数可以自行调整
    float b = sqrt(originalRGB.z) * 3.0;
    //  判断一下是否越界
    b = b > 1.0 ? 1.0 : b;
    outColor = vec4(vec3(originalRGB.x, originalRGB.y, b), 1.0);
} 