#version 300 es
// 从眼睛位置将场景渲染的片段着色器
precision lowp float;

uniform lowp sampler2DShadow s_shadowMap;

in vec4 v_color;
in vec4 v_shadowCoord;

layout(location = 0) out vec4 outColor;

float lookup (float x, float y)
{
    float pixelSize = 0.002;        // 1/500
    vec4 offset = vec4 (x * pixelSize * v_shadowCoord.w,
                        y * pixelSize * v_shadowCoord.w,
                        -0.005 * v_shadowCoord.w,
                        0.0);
    return textureProj (s_shadowMap, v_shadowCoord + offset);
}

void main()
{
    // 3x3 kernel with 4 taps per sample, effectively 6x6 PCF
    float sum = 0.0;
    float x, y;
    for (x = -2.0; x <= 2.0; x += 2.0){
        for (y = -2.0; y <= 2.0; y += 2.0){
            sum += lookup (x, y);
        }
    }

    // divide sum by 9.0
    sum = sum * 0.11;
    outColor = v_color * sum;
}