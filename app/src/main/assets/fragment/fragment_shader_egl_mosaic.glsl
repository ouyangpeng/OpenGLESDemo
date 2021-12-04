#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;
uniform vec2 u_texSize;

// 马赛克 效果
vec4 CrossStitching(vec2 uv) {
    float stitchSize = u_texSize.x / 35.0;
    int invert = 0;
    vec4 color = vec4(0.0);
    float size = stitchSize;
    vec2 cPos = uv * u_texSize.xy;
    vec2 tlPos = floor(cPos / vec2(size, size));
    tlPos *= size;
    int remX = int(mod(cPos.x, size));
    int remY = int(mod(cPos.y, size));
    if (remX == 0 && remY == 0)
    tlPos = cPos;
    vec2 blPos = tlPos;
    blPos.y += (size - 1.0);
    if ((remX == remY) || (((int(cPos.x) - int(blPos.x)) == (int(blPos.y) - int(cPos.y))))) {
        if (invert == 1)
        color = vec4(0.2, 0.15, 0.05, 1.0);
        else
        color = texture(s_TextureMap, tlPos * vec2(1.0 / u_texSize.x, 1.0 / u_texSize.y)) * 1.4;
    } else {
        if (invert == 1)
        color = texture(s_TextureMap, tlPos * vec2(1.0 / u_texSize.x, 1.0 / u_texSize.y)) * 1.4;
        else
        color = vec4(0.0, 0.0, 0.0, 1.0);
    }
    return color;
}

void main() {
    outColor = CrossStitching(v_texCoord);
}