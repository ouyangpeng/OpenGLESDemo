#version 300 es

uniform mat4 u_mvpMatrix;
uniform mat4 u_mvMatrix;

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;
layout(location = 2) in vec2 a_texCoord;

out vec4 v_color;
out vec2 v_texCoord;
out vec4 v_eyePos;

void main()
{
    // Transform vertex to view-space
    v_eyePos = u_mvMatrix * a_position;
    v_color = a_color;
    v_texCoord = a_texCoord;
    gl_Position = u_mvpMatrix * a_position;
}