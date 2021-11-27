#version 300 es
// 表示OpenGL ES着色器语言V3.00

// 声明着色器中浮点变量的默认精度
precision mediump float;

// 声明由上一步顶点着色器传入进来的颜色值
in vec4 v_color;

// 声明一个输出变量fragColor，这是一个4分量的向量，
// 写入这个变量的值将被输出到颜色缓冲器
out vec4 o_fragColor;

void main()
{
	o_fragColor = v_color;
}