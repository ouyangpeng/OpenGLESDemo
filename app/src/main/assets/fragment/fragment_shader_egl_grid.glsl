#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;
uniform vec2 u_texSize;

void main() {
    float size = u_texSize.x / 75.0;
    float radius = size * 0.5;
    vec2 fragCoord = v_texCoord * u_texSize.xy;
    vec2 quadPos = floor(fragCoord.xy / size) * size;
    vec2 quad = quadPos/u_texSize.xy;
    vec2 quadCenter = (quadPos + size/2.0);
    float dist = length(quadCenter - fragCoord.xy);

    if (dist > radius) {
        outColor = vec4(0.25);
    } else {
        outColor = texture(s_TextureMap, v_texCoord);
    }
}