#version 300 es
//t 取值数组
layout(location = 0) in float a_tData;
//起始点和终止点
uniform vec4 u_StartEndPoints;
//控制点
uniform vec4 u_ControlPoints;
// 矩阵
uniform mat4 u_MVPMatrix;
//y轴方向做一个动态偏移
uniform float u_Offset;

// 绘制三阶贝塞尔曲线，用 GLSL 实现该函数，然后我们从外部输入一组 t 的取值数组，便可以得出一组对应的用于绘制三阶贝塞尔曲线的点。
vec2 bezier3(in vec2 p0, in vec2 p1, in vec2 p2, in vec2 p3, in float t){
    float tt = (1.0 - t) * (1.0 -t);
    return tt * (1.0 -t) *p0 + 3.0 * t * tt * p1 + 3.0 * t *t *(1.0 -t) *p2 + t *t *t *p3;
}

//// 借助于 GLSL 的内置混合函数 mix ，我们可以在用于绘制贝塞尔曲线的点之间进行插值，相当于对上述函数 bezier_3order 进行优化：
//// mix 函数
//// mix(x,y,a) 返回 x和y的线性混合   x*(1-a)+y*a
//vec2 bezier3_mix(in vec2 p0, in vec2 p1, in vec2 p2, in vec2 p3, in float t)
//{
//    vec2 q0 = mix(p0, p1, t);
//    vec2 q1 = mix(p1, p2, t);
//    vec2 q2 = mix(p2, p3, t);
//
//    vec2 r0 = mix(q0, q1, t);
//    vec2 r1 = mix(q1, q2, t);
//
//    return mix(r0, r1, t);
//}

void main() {

    vec4 pos;
    pos.w = 1.0;

    vec2 p0 = u_StartEndPoints.xy;
    vec2 p3 = u_StartEndPoints.zw;

    vec2 p1 = u_ControlPoints.xy;
    vec2 p2 = u_ControlPoints.zw;

    p0.y *= u_Offset;
    p1.y *= u_Offset;
    p2.y *= u_Offset;
    p3.y *= u_Offset;

    float t = a_tData;

//    vec2 point = bezier3_mix(p0, p1, p2, p3, t);
    vec2 point = bezier3(p0, p1, p2, p3, t);

    //用于绘制三角形的时候起作用，类似于绘制扇形
    if (t < 0.0)
    {
        pos.xy = vec2(0.0, 0.0);
    }
    else
    {
        pos.xy = point;
    }
    //设置点的大小
    gl_PointSize = 4.0;
    gl_Position = u_MVPMatrix * pos;
}