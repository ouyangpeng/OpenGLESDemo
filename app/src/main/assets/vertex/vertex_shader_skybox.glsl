#version 300 es
precision mediump float;

layout(location = 0) in vec3 a_position;

uniform mat4 u_MVPMatrix;

out vec3 v_texCoord;

void main()
{
    gl_Position = u_MVPMatrix * vec4(a_position, 1.0);
    v_texCoord = a_position;
}