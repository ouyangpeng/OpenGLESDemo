#version 300 es

// 粒子渲染顶点着色器
// 在发出变化反馈缓冲区之后，该缓冲区被绑定为一个顶点缓冲区对象，在其中渲染粒子。

// 这个顶点着色器 使用 变化反馈输出 作为 输入变量。
// 每个粒子的当前年龄根据每个粒子创建时在 a_curtime 属性中保存的时间戳计算。
// 粒子的速度和位置根据这个时间进行更新。
// 粒子的大小在粒子的整个声明周期中逐渐减少。

#define ATTRIBUTE_POSITION      0
#define ATTRIBUTE_VELOCITY      1
#define ATTRIBUTE_SIZE          2
#define ATTRIBUTE_CURTIME       3
#define ATTRIBUTE_LIFETIME      4

layout(location = ATTRIBUTE_POSITION) in vec2 a_position;
layout(location = ATTRIBUTE_VELOCITY) in vec2 a_velocity;
layout(location = ATTRIBUTE_SIZE) in float a_size;
layout(location = ATTRIBUTE_CURTIME) in float a_curtime;
layout(location = ATTRIBUTE_LIFETIME) in float a_lifetime;

uniform float u_time;
uniform vec2 u_acceleration;

void main()
{
    float deltaTime = u_time - a_curtime;
    // 检查粒子的寿命是否已经到达
    if (deltaTime <= a_lifetime)
    {
        // 每个粒子的当前年龄根据每个粒子创建时在 a_curtime 属性中保存的时间戳计算。
        // 粒子的速度和位置根据这个时间进行更新。
        // 粒子的大小在粒子的整个声明周期中逐渐减少。
        vec2 velocity = a_velocity + deltaTime * u_acceleration;
        vec2 position = a_position + deltaTime * velocity;
        gl_Position = vec4(position, 0.0, 1.0);
        gl_PointSize = a_size * (1.0 - deltaTime / a_lifetime);
    }
    else
    {
        // 如果粒子的寿命已经到达，则将gl_Position变量设置为(-1000，-1000),
        // 这是迫使该点落在屏幕之外的快速方法。因为该点会被裁剪，所以对过期的点精灵的后续处理可以跳过
        gl_Position = vec4(-1000, -1000, 0, 0);
        // 粒子大小设置为 0
        gl_PointSize = 0.0;
    }
}