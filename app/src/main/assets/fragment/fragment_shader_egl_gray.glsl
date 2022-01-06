#version 300 es
precision highp float;

// 灰度图

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

// 权重值取自GPUImage，RGB三值相加=1
const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);

void main() {
    // 获取纹理图片当前纹理坐标下的 颜色值
    lowp vec4 textureColor = texture(s_TextureMap, v_texCoord);
    // 点乘 : 颜色 点乘 变换因子
    float luminance = dot(textureColor.rgb, W);
    // 将灰度值转化并填充到像素中(luminance，luminance，luminance，textureColor.a)
    outColor = vec4(vec3(luminance), textureColor.a);

}