#version 300 es
precision highp float;
layout(location = 0) out vec4 outColor;
in vec2 v_texCoord;
uniform sampler2D s_TextureMap;
uniform vec2 u_texSize;

vec2 reshape(vec2 src, vec2 dst, vec2 curPos, float radius)
{
    vec2 pos = curPos;

    float r = distance(curPos, src);

    if (r < radius)
    {
        float alpha = 1.0 -  r / radius;
        vec2 displacementVec = (dst - src) * pow(alpha, 2.0);
        pos = curPos - displacementVec;

    }
    return pos;
}

void main() {
    vec2 srcPos = vec2(0.5, 0.5);
    vec2 dstPos = vec2(0.6, 0.5);
    float radius = 0.18;
    float scaleRatio = 1.0;
    float aspectRatio = u_texSize.x / u_texSize.y;

    if (radius <= distance(v_texCoord, srcPos) && distance(v_texCoord, srcPos) <= radius + 0.008)
    {
        outColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
        outColor = texture(s_TextureMap, reshape(srcPos, dstPos, v_texCoord, radius));
    }
}