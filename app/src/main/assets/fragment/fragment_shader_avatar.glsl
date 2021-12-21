#version 300 es
precision highp float;

in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

uniform sampler2D s_TextureMap;

//图像分辨率
uniform vec2 u_texSize;
//判断是否需要做形变
uniform float u_needRotate;
//通过旋转角度控制形变的程度，旋转角度需要与重力传感器数据绑定
uniform float u_rotateAngle;

// 简单的正余弦形变，和 fragment_shader_egl_rotate片段着色器类似
vec2 rotate(float radius, float angle, vec2 texSize, vec2 texCoord)
{
    vec2 newTexCoord = texCoord;
    vec2 center = vec2(texSize.x / 2.0, texSize.y / 2.0);
    vec2 tc = texCoord * texSize;
    tc -= center;
    float dist = length(tc);
    if (dist < radius) {
        float percent = (radius - dist) / radius;
        float theta = percent * percent * angle * 8.0;
        float s = sin(theta);
        float c = cos(theta);
        tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
        tc += center;

        newTexCoord = tc / texSize;
    }
    return newTexCoord;
}

void main()
{
    vec2 texCoord = v_texCoord;

    if (u_needRotate > 0.0)
    {
        texCoord = rotate(0.5 * u_texSize.x, u_rotateAngle, u_texSize, v_texCoord);
    }

    outColor = texture(s_TextureMap, texCoord);

    // 素材图里的人像层和外层是部分区域透明的 PNG 图，而背景层是每个像素透明度均为最大值的 JPG 图。
    // 所以，在绘制 3 张图时，要先绘制背景层，然后依次是人像层、外层，为了防止遮挡，
    // 在绘制人像层、外层时需要利用片段着色器来丢弃透明度比较低的片元，这种操作俗称 alpha 测试。

    // 丢弃透明度比较低的片元
    if (outColor.a < 0.6) discard;
}
