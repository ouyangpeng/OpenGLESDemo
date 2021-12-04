#version 300 es
precision highp float;
layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;
uniform vec2 u_texSize;

float distanceTex(vec2 pos0, vec2 pos1, float aspectRatio)
{
    vec2 newPos0 = vec2(pos0.x, pos0.y * aspectRatio + 0.5 - 0.5 * aspectRatio);
    vec2 newPos1 = vec2(pos1.x, pos1.y * aspectRatio + 0.5 - 0.5 * aspectRatio);
    return distance(newPos0, newPos1);
}

vec2 reshape(vec2 src, vec2 dst, vec2 curPos, float radius, float aspectRatio)
{
    vec2 pos = curPos;

    vec2 newSrc = vec2(src.x, src.y * aspectRatio + 0.5 - 0.5 * aspectRatio);
    vec2 newDst = vec2(dst.x, dst.y * aspectRatio + 0.5 - 0.5 * aspectRatio);
    vec2 newCur = vec2(curPos.x, curPos.y * aspectRatio + 0.5 - 0.5 * aspectRatio);


    float r = distance(newSrc, newCur);

    if (r < radius)
    {
        float alpha = 1.0 -  r / radius;
        vec2 displacementVec = (dst - src) * pow(alpha, 1.7);
        pos = curPos - displacementVec;

    }
    return pos;
}

void main() {
    vec2 srcPos = vec2(0.5, 0.5);
    vec2 dstPos = vec2(0.55, 0.55);
    float radius = 0.30;
    float scaleRatio = 1.0;
    float aspectRatio = u_texSize.y/u_texSize.x;

    if (radius <= distanceTex(v_texCoord, srcPos, aspectRatio) && distanceTex(v_texCoord, srcPos, aspectRatio) <= radius + 0.008)
    {
        outColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
    else
    {
        outColor = texture(s_TextureMap, reshape(srcPos, dstPos, v_texCoord, radius, aspectRatio));
    }
}