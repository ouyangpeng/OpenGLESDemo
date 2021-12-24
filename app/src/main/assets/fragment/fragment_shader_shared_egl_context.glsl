#version 300 es
precision mediump float;
in vec2 v_texCoord;
layout(location = 0) out vec4 outColor;

uniform sampler2D s_TextureMap;
uniform float u_Offset;

void main()
{
    vec4 tempColor = texture(s_TextureMap, v_texCoord);
    if (v_texCoord.x < u_Offset) {
        outColor = vec4(1.0 - tempColor.r, 1.0 - tempColor.g, 1.0 - tempColor.b, tempColor.a);
    }
    else if (v_texCoord.x < u_Offset + 0.005) {
        outColor = vec4(1.0);
    }
    else
    {
        outColor = tempColor;
    }
}