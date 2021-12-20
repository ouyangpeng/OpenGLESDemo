#version 300 es
precision highp float;

layout(location = 0) out vec4 outColor;

in vec2 v_texCoord;

uniform sampler2D s_TextureMap;
uniform float u_type;

void main() {
    if (u_type == 0.0)
    {
        outColor = texture(s_TextureMap, v_texCoord);
    }
    else if (u_type == 1.0)
    {
        outColor = vec4(1.0, 0.0, 0.0, 1.0);
    }
}