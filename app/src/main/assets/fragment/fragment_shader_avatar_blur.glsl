#version 300 es
precision highp float;

// 绘制外层图像的片段着色器

layout(location = 0) out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D s_TextureMap;

void main() {
    vec4 sample0, sample1, sample2, sample3;
    float blurStep = 0.16;
    float step = blurStep / 100.0;
    sample0 = texture(s_TextureMap, vec2(v_texCoord.x - step, v_texCoord.y - step));
    sample1 = texture(s_TextureMap, vec2(v_texCoord.x + step, v_texCoord.y + step));
    sample2 = texture(s_TextureMap, vec2(v_texCoord.x + step, v_texCoord.y - step));
    sample3 = texture(s_TextureMap, vec2(v_texCoord.x - step, v_texCoord.y + step));
    outColor = (sample0 + sample1 + sample2 + sample3) / 4.0;
    //放宽 alpha 值过滤范围，使白斑变的更大
    if (outColor.a > 0.05)
    {
        //叠加一些强度，使白斑变的更亮
        outColor += vec4(0.2, 0.2, 0.2, 0.0);
    }
    else
    {
        discard;
    }
}