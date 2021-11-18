#version 300 es                                     
precision mediump float;

// 在每次爆炸开始时由 C++ 代码里面的 update() 函数设置
uniform vec4 u_color;

// 由顶点着色器设置的v_lifetime输入变量，表示粒子的剩余寿命。
in float v_lifetime;

layout(location = 0) out vec4 fragColor;

// 声明一个采样器来绑定烟雾的2D纹理图像
uniform sampler2D s_texture;

void main()
{
    vec4 texColor;
	// 纹理读取使用gl_PointCoord变量作为纹理坐标。这个用于点精灵的特殊变量设置为固定值-----点精灵的各个角
	// 如果精灵需要旋转，也可以扩展这个片段着色器以旋转点精灵坐标
    texColor = texture(s_texture, gl_PointCoord);

	// 纹理颜色由u_color变量进行衰减
    fragColor = vec4(u_color) * texColor;
	// Alpha值根据粒子寿命进行衰减
    fragColor.a *= v_lifetime;
}