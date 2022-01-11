#version 300 es
precision highp float;

// 蜡笔
// 和 素描的部分代码一样
// 参考自： https://github.com/wuhaoyu1990/MagicCamera

layout(location = 0) out vec4 outColor;

in highp vec2 v_texCoord;
precision mediump float;

uniform sampler2D s_TextureMap;

// 可以从外部传入
//uniform vec2 singleStepOffset;     // 传入 0.5
//uniform float strength;           // 传入  {1.0f / width, 1.0f / height}
// 这里测试写死
const float strength = 0.5;
// 800.0 代表长和宽
const vec2 singleStepOffset = vec2(1.0 / 800.0, 1.0 / 800.0);

const highp vec3 W = vec3(0.299, 0.587, 0.114);

const mat3 rgb2yiqMatrix = mat3(
    0.299, 0.587, 0.114,
    0.596, -0.275, -0.321,
    0.212, -0.523, 0.311
);

const mat3 yiq2rgbMatrix = mat3(
    1.0, 0.956, 0.621,
    1.0, -0.272, -1.703,
    1.0, -1.106, 0.0
);


void main()
{
    vec4 oralColor = texture(s_TextureMap, v_texCoord);

    vec3 maxValue = vec3(0., 0., 0.);

    for (int i = -2; i <= 2; i ++)
    {
        for (int j = -2; j <= 2; j ++)
        {
            vec4 tempColor = texture(s_TextureMap, v_texCoord + singleStepOffset * vec2(i, j));
            maxValue.r = max(maxValue.r, tempColor.r);
            maxValue.g = max(maxValue.g, tempColor.g);
            maxValue.b = max(maxValue.b, tempColor.b);
        }
    }

    vec3 textureColor = oralColor.rgb / maxValue;

    float gray = dot(textureColor, W);
    float k = 0.223529;
    float alpha = min(gray, k)/k;

    textureColor = textureColor * alpha + (1.0 - alpha) * oralColor.rgb;

    vec3 yiqColor = textureColor * rgb2yiqMatrix;

    yiqColor.r = max(0.0, min(1.0, pow(gray, strength)));

    textureColor = yiqColor * yiq2rgbMatrix;

    outColor = vec4(textureColor, oralColor.w);
} 