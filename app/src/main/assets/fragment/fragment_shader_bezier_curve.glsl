#version 300 es
precision mediump float;

layout(location = 0) out vec4 outColor;

//设置绘制三角形或者点的颜色
uniform vec4 u_Color;

void main()
{
    outColor = u_Color;
}