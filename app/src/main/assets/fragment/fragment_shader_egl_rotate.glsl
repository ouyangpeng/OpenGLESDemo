#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform lowp sampler2D s_TextureMap;
uniform vec2 u_texSize;

void main() {
    float radius = 200.0;
    float angle = 0.8;
    vec2 center = vec2(u_texSize.x / 2.0, u_texSize.y / 2.0);
    vec2 tc = v_texCoord * u_texSize;
    tc -= center;
    float dist = length(tc);
    if (dist < radius) {
        float percent = (radius - dist) / radius;
        float theta = percent * percent * angle * 8.0;
        float s = sin(theta);
        float c = cos(theta);
        tc = vec2(dot(tc, vec2(c, -s)), dot(tc, vec2(s, c)));
    }
    tc += center;
    outColor = texture(s_TextureMap, tc / u_texSize);
}