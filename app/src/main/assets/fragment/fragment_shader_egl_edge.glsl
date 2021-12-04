#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform lowp sampler2D s_TextureMap;
uniform vec2 u_texSize;

void main() {
    vec2 pos = v_texCoord.xy;
    vec2 onePixel = vec2(1, 1) / u_texSize;
    vec4 color = vec4(0);

    mat3 edgeDetectionKernel = mat3(
            -1, -1, -1,
            -1, 8, -1,
            -1, -1, -1
    );

    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            vec2 samplePos = pos + vec2(i - 1, j - 1) * onePixel;
            vec4 sampleColor = texture(s_TextureMap, samplePos);
            sampleColor *= edgeDetectionKernel[i][j];
            color += sampleColor;
        }
    }
    outColor = vec4(color.rgb, 1.0);
}