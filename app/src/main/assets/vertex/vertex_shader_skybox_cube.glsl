#version 300 es
// 天空盒内绘制物体（反射周围环境颜色）使用的顶点着色器

precision mediump float;

layout(location = 0) in vec3 a_position;
layout(location = 1) in vec3 a_normal;

uniform mat4 u_MVPMatrix;
uniform mat4 u_ModelMatrix;

// 纹理坐标
out vec3 v_texCoord;

// 法线向量
out vec3 v_normal;

void main()
{
    gl_Position = u_MVPMatrix * vec4(a_position, 1.0);
    v_normal = mat3(transpose(inverse(u_ModelMatrix))) * a_normal;
    v_texCoord = vec3(u_ModelMatrix * vec4(a_position, 1.0));
}