#version 300 es

precision mediump float;

uniform vec4 u_Color;

out vec4 fragColor;

void main()
{
    fragColor = u_Color;
}