#version 300 es
precision mediump float;

in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

uniform sampler2D s_TextureMap;

void main()
{
    vec4 tempColor = texture(s_TextureMap, v_texCoord);

    //   RGB转YUV:
    //   Y = 0.299 * R + 0.587 * G + 0.114*B
    //   U = - 0.1687 * R - 0.3313 * G + 0.5* B + 128    注： +128 的含义是让UV的范围处于整数区间（0-255）
    //   V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128     注： +128 的含义是让UV的范围处于整数区间（0-255）
    //  下面的luminance 只取了YUV的Y，所以是黑白的照片
    float luminance = tempColor.r * 0.299 + tempColor.g * 0.587 + tempColor.b * 0.114;
    outColor = vec4(vec3(luminance), tempColor.a);
}