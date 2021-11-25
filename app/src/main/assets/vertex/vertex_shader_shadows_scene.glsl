#version 300 es

// 从眼睛位置将场景渲染的顶点着色器

// 从眼睛位置创建的MVP矩阵，结果记录在 gl_Position中
uniform mat4 u_mvpMatrix;
// 从光源位置创建的MVP矩阵，结果记录在 v_shadowCoord中
uniform mat4 u_mvpLightMatrix;

layout(location = 0) in vec4 a_position;
layout(location = 1) in vec4 a_color;

out vec4 v_color;
// v_shadowCoord的结果和渲染到阴影贴图时的顶点位置结果相同
// 使用v_shadowCoord作为纹理坐标
out vec4 v_shadowCoord;

void main()
{
    v_color = a_color;
    gl_Position = u_mvpMatrix * a_position;
    v_shadowCoord = u_mvpLightMatrix * a_position;

    // 将坐标从齐次坐标 [-1,1] 变化到 [0,1]
    // transform from [-1,1] to [0,1];
    v_shadowCoord = v_shadowCoord * 0.5 + 0.5;
}