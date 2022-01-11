#version 300 es
precision highp float;

// 素描

layout(location = 0) out vec4 outColor;

in highp vec2 v_texCoord;
precision mediump float;

uniform sampler2D s_TextureMap;

const highp vec3 W = vec3(0.299, 0.587, 0.114);

// 可以从外部传入
//uniform vec2 singleStepOffset;     // 传入 0.5
//uniform float strength;           // 传入  {1.0f / width, 1.0f / height}

// 这里测试写死
const float strength = 0.5;
// 800.0 代表长和宽
const vec2 singleStepOffset = vec2(1.0/ 800.0, 1.0/ 800.0);

void main()
{
    float threshold = 0.0;
    //pic1
    vec4 oralColor = texture(s_TextureMap, v_texCoord);

    //pic2
    vec3 maxValue = vec3(0., 0., 0.);

    // determine the shape of the input image
    for (int i = -2; i <= 2; i ++)
    {
        for (int j = -2; j <= 2; j ++)
        {
            vec4 tempColor = texture(s_TextureMap, v_texCoord + singleStepOffset * vec2(i, j));
            maxValue.r = max(maxValue.r, tempColor.r);
            maxValue.g = max(maxValue.g, tempColor.g);
            maxValue.b = max(maxValue.b, tempColor.b);
            threshold  += dot(tempColor.rgb, W);
        }
    }

    // dot(vec3 a ,vec3 b) 点乘得到的是两个向量a 和 b 之间的夹角的余弦值 即 cosα，范围在[-1, 1]之间，是一个标量

    //pic3
    float gray1 = dot(oralColor.rgb, W);

    //pic4
    float gray2 = dot(maxValue, W);

    //pic5
    float contour = gray1 / gray2;

    threshold = threshold / 25.0;

    float alpha = max(1.0, gray1 > threshold ? 1.0 : (gray1 / threshold));

    float result = contour * alpha + (1.0 - alpha) * gray1;

    outColor = vec4(vec3(result, result, result), oralColor.w);
} 