#version 300 es
// 表示OpenGL ES着色器语言V3.00

// 使用in关键字，在顶点着色器中声明所有的输入顶点属性(Input Vertex Attribute)。
// 声明一个输入属性数组：一个名为vPosition的4分量向量
// 在图形编程中我们经常会使用向量这个数学概念，因为它简明地表达了任意空间中的位置和方向，并且它有非常有用的数学属性。
// 在GLSL中一个向量有最多4个分量，每个分量值都代表空间中的一个坐标，它们可以通过vec.x、vec.y、vec.z和vec.w来获取。
//注意vec.w分量不是用作表达空间中的位置的（我们处理的是3D不是4D），而是用在所谓透视除法(Perspective Division)上。
layout(location = 0) in vec4 vPosition;
void main()
{
	// 为了设置顶点着色器的输出，我们必须把位置数据赋值给预定义的gl_Position变量，它在幕后是vec4类型的。
	// 将vPosition输入属性拷贝到名为gl_Position的特殊输出变量
	// 每个顶点着色器必须在gl_Position变量中输出一个位置，这个位置传递到管线下一个阶段的位置
	gl_Position = vPosition;
}