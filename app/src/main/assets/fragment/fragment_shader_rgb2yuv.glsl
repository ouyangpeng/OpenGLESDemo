#version 300 es
precision mediump float;


in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;
//RGBA 纹理
uniform sampler2D s_TextureMap;
//采样偏移
uniform float u_Offset;

// 可以对比  fragment_shader_texture_yuv_render.glsl 文件

//      YUV转RGB
//      R = Y + 1.402 * (V - 128)
//      G = Y - 0.34414 * (U - 128) - 0.71414 * (V - 128)
//      B = Y + 1.772 * (U - 128)

//      RGB转YUV:
//      Y = 0.299 * R + 0.587 * G + 0.114*B
//      U = - 0.1687 * R - 0.3313 * G + 0.5* B + 128    注： +128 的含义是让UV的范围处于整数区间（0-255）
//      V = 0.5 * R - 0.4187 * G - 0.0813 * B + 128     注： +128 的含义是让UV的范围处于整数区间（0-255）
const vec3 COEF_Y = vec3(0.299, 0.587, 0.114);
const vec3 COEF_U = vec3(-0.1687, -0.3313, 0.5);     // 后面步骤有+0.5
const vec3 COEF_V = vec3(0.5, -0.4187, -0.0813);     // 后面步骤有+0.5

void main()
{
    vec2 texelOffset = vec2(u_Offset, 0.0);
    vec4 color0 = texture(s_TextureMap, v_texCoord);
    // 偏移 offset 采样
    vec4 color1 = texture(s_TextureMap, v_texCoord + texelOffset);
    float y0 = dot(color0.rgb, COEF_Y);
    // 因为YUV转RGB用的是Y、U-128和V-128，texture函数返回向量值的范围是0-1.0，128表示0.5, 所以要加0.5
    float u0 = dot(color0.rgb, COEF_U) + 0.5;
    float v0 = dot(color0.rgb, COEF_V) + 0.5;
    float y1 = dot(color1.rgb, COEF_Y);
    outColor = vec4(y0, u0, y1, v0);
}