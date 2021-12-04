#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;
uniform vec2 u_texSize;

vec2 warpPositionToUse(vec2 centerPostion, vec2 currentPosition, float radius, float scaleRatio, float aspectRatio)
{
    vec2 positionToUse = currentPosition;
    vec2 currentPositionToUse = vec2(currentPosition.x, currentPosition.y * aspectRatio + 0.5 - 0.5 * aspectRatio);
    vec2 centerPostionToUse = vec2(centerPostion.x, centerPostion.y * aspectRatio + 0.5 - 0.5 * aspectRatio);
    //float r = distance(currentPositionToUse, centerPostionToUse);
    float r = distance(currentPosition, centerPostion);
    if (r < radius)
    {
        float alpha = 1.0 - scaleRatio * pow(r / radius - 1.0, 2.0);
        positionToUse = centerPostion + alpha * (currentPosition - centerPostion);
    }
    return positionToUse;
}

void main() {
    vec2 centerPostion = vec2(0.5, 0.5);
    float radius = 0.34;
    float scaleRatio = 1.0;
    float aspectRatio = u_texSize.x / u_texSize.y;
    outColor = texture(s_TextureMap, warpPositionToUse(centerPostion, v_texCoord, radius, scaleRatio, aspectRatio));
}