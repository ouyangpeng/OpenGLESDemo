#version 300 es
precision mediump float;
//顶点坐标
layout(location = 0) in vec3 a_vertex;
//纹理坐标
layout(location = 1) in vec2 a_texCoord;
//位置偏移，最终确定粒子的位置
layout(location = 2) in vec3 a_offset;
//粒子颜色（照在粒子表面光的颜色）
layout(location = 3) in vec4 a_particlesColor;

//变换矩阵
uniform mat4 u_MVPMatrix;

out vec2 v_texCoord;
out vec4 v_color;

void main()
{
    // vec3(0.0, 0.95, 0.0) 是为了使粒子整体向 y 轴负方向有一个偏移
    gl_Position = u_MVPMatrix * vec4(a_vertex - vec3(0.0, 0.95, 0.0) + a_offset, 1.0);
    v_texCoord = a_texCoord;
    v_color = a_particlesColor;
}