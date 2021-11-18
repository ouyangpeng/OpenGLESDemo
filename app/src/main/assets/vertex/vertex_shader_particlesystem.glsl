#version 300 es
// 当前经过时间。当时间超过单次爆炸的时长时，该值被重置为0.0
uniform float u_time;
// 该变量在新的爆炸开始时被设置为爆炸的当前位置。
uniform vec3 u_centerPosition;

// 粒子的寿命
layout(location = 0) in float a_lifetime;
// 粒子的开始位置
layout(location = 1) in vec3 a_startPosition;
// 粒子的结束位置
layout(location = 2) in vec3 a_endPosition;

// 粒子的剩余寿命。该寿命将在片段着色器中使用，在粒子寿命到期时使粒子消失
out float v_lifetime;

void main()
{
	// 检查粒子的寿命是否已经到达
    if (u_time <= a_lifetime)
    {
		// 如果粒子仍然存活，则它的位置被设置为开始和结束之间的线性插值
        gl_Position.xyz = a_startPosition + (u_time * a_endPosition);
        gl_Position.xyz += u_centerPosition;
        gl_Position.w = 1.0;
    }
    else
    {
		// 如果粒子的寿命已经到达，则将gl_Position变量设置为(-1000，-1000),
		// 这是迫使该点落在屏幕之外的快速方法。因为该点会被裁剪，所以对过期的点精灵的后续处理可以跳过
        gl_Position = vec4(-1000, -1000, 0, 0);
    }

	// 在v_lifetime向片段着色器传递粒子的剩余寿命
    v_lifetime = 1.0 - (u_time / a_lifetime);
    v_lifetime = clamp (v_lifetime, 0.0, 1.0);

	// 根据粒子剩余寿命决定点的大小，这能够实现在粒子接近生命尽头的时候缩小的效果
    gl_PointSize = (v_lifetime * v_lifetime) * 40.0;
}