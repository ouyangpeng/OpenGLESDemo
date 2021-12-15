#version 300 es
layout(location = 0) in vec4 a_position;

uniform mat4 u_MVPMatrix;

void main()
{
    gl_Position = u_MVPMatrix * a_position;
}