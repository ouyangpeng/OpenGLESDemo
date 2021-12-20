#version 300 es                                     
precision mediump float;

in vec2 v_texCoord;

layout(location = 0) out vec4 outColor;

uniform float drawType;

void main()
{
    if (drawType == 1.0) {
        outColor = vec4(1.5 - v_texCoord.y, 0.3, 0.3, 1.0);
    } else {
        outColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
}