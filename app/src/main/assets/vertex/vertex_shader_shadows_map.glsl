#version 300 es

// 从光源角度将场景渲染到深度纹理的顶点着色器

uniform mat4 u_mvpLightMatrix;

layout(location = 0) in vec4 a_position;

out vec4 v_color;

void main()
{
    gl_Position = u_mvpLightMatrix * a_position;
}