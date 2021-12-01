#version 300 es
// 位置变量的属性位置值为 0
layout(location = 0) in vec4 a_position;
// 颜色变量的属性位置值为 1
layout(location = 1) in vec4 a_color;
// 向片段着色器输出一个颜色
out vec4 v_color;

void main()
{
    v_color = a_color;
    gl_Position = a_position;
}