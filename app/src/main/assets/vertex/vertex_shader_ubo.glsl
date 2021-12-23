#version 300 es

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec2 a_texCoord;

// 将 MVP 变换矩阵设置为一个 uniform 块，即我们后面创建的 UBO 中将保存 3 个矩阵
layout (std140) uniform MVPMatrix
{
    mat4 projection;
    mat4 view;
    mat4 model;
};

out vec2 v_texCoord;

void main()
{
    gl_Position = projection * view * model * a_position;
    v_texCoord = a_texCoord;
}