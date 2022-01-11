#version 300 es
precision highp float;

// 怀旧

layout(location = 0) out vec4 outColor;

in highp vec2 v_texCoord;

uniform sampler2D s_TextureMap;

void main()
{
    vec3 originalRGB = texture(s_TextureMap, v_texCoord).rgb;
    //    怀旧特效是将图像的RGB三个分量分别按照一定比例进行处理的结果，其怀旧公式如下所示。
    //    公式参考自：https://blog.51cto.com/u_15127682/4622632
    //    R = 0.393 * R + 0.769 * G + 0.189 * B
    //    G = 0.349 * R + 0.686 * G + 0.168 * B
    //    B = 0.272 * R + 0.534 * G + 0.131 * B
    highp vec3 rgb =  mat3(
        0.393, 0.349, 0.272, //第一列
        0.769, 0.686, 0.534, //第二列
        0.189, 0.168, 0.131//第三列
    ) * originalRGB;
    outColor = vec4(rgb, 1);
} 