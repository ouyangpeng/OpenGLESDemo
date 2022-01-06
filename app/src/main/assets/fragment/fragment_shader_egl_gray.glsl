#version 300 es
precision highp float;

// 灰度图

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;

const highp vec3 W = vec3(0.2125, 0.7154, 0.0721);

void main() {
    lowp vec4 textureColor = texture(s_TextureMap, v_texCoord);
    float luminance = dot(textureColor.rgb, W);
    outColor = vec4(vec3(luminance), textureColor.a);

}