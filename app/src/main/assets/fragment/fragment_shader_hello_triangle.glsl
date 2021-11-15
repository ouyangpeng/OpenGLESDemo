#version 300 es
// 表示OpenGL ES着色器语言V3.00

// 声明着色器中浮点变量的默认精度
precision mediump float;
// 声明一个输出变量fragColor，这是一个4分量的向量，
// 写入这个变量的值将被输出到颜色缓冲器
out vec4 fragColor;
void main()
{
	// 所有片段的着色器输出都是红色( 1.0, 0.0, 0.0, 1.0 )
	fragColor = vec4 ( 1.0, 0.0, 0.0, 1.0 );

	// 会输出橘黄色
	// fragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
}