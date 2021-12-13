#version 300 es
precision mediump float;
layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;
layout(location = 2) in vec3 a_normal;

out vec3 normal;
out vec3 fragPos;
out vec2 v_texCoord;

uniform mat4 u_MVPMatrix;
uniform mat4 u_ModelMatrix;

void main()
{
    gl_Position = u_MVPMatrix * a_position;
    fragPos = vec3(u_ModelMatrix * a_position);
    normal = mat3(transpose(inverse(u_ModelMatrix))) * a_normal;
    v_texCoord = a_texCoord;
}
