#version 300 es

layout(location = 0) in vec4 a_Position;
layout(location = 1) in vec4 a_Color;

out vec4 v_Color;

void main()
{
    v_Color = a_Color;

    gl_Position = a_Position;
    gl_PointSize = 10.0;
}