#version 300 es

// 这个是粒子发射顶点着色器
// 这个着色器中的  所有输出变量 被写入 一个 变化反馈缓冲区对象。
// 每当粒子的寿命到期，着色器便将其作为新活跃粒子发射的潜在候选。
// 如果生成了一个新的例子，着色器则使用randomValue函数生成一个随机值来初始化新粒子的速度和大小
// 这种随机数生成基于3D噪音纹理的使用，并使用 gl_vertexID 内建变量 为每个例子选择独特的纹理坐标

#define NUM_PARTICLES           200
#define ATTRIBUTE_POSITION      0
#define ATTRIBUTE_VELOCITY      1
#define ATTRIBUTE_SIZE          2
#define ATTRIBUTE_CURTIME       3
#define ATTRIBUTE_LIFETIME      4

uniform float u_time;
uniform float u_emissionRate;
uniform mediump sampler3D s_noiseTex;

// 位置
layout(location = ATTRIBUTE_POSITION) in vec2 a_position;
// 速度
layout(location = ATTRIBUTE_VELOCITY) in vec2 a_velocity;
// 大小
layout(location = ATTRIBUTE_SIZE) in float a_size;
// 当前时间
layout(location = ATTRIBUTE_CURTIME) in float a_curtime;
// 寿命
layout(location = ATTRIBUTE_LIFETIME) in float a_lifetime;

// 这个着色器中的  所有输出变量 被写入 一个 变化反馈缓冲区对象。
// 为了在这个顶点着色器中使用变化反馈，输出变量必须在链接程序对象之前标记为用于变化反馈,
// 这个在ParticleSystemTransformFeedBack::initEmitParticles函数完成

// 位置
out vec2 v_position;
// 速度
out vec2 v_velocity;
// 大小
out float v_size;
// 当前时间
out float v_curtime;
// 寿命
out float v_lifetime;

float randomValue(inout float seed)
{
    // 使用 gl_vertexID 内建变量 为每个例子选择独特的纹理坐标
    float vertexId = float(gl_VertexID) / float(NUM_PARTICLES);
    vec3 texCoord = vec3(u_time, vertexId, seed);
    seed += 0.1;
    return texture(s_noiseTex, texCoord).r;
}

void main()
{
    float seed = u_time;
    float lifetime = a_curtime - u_time;
    // 检查粒子的寿命是否已经到达
    if (lifetime <= 0.0 && randomValue(seed) < u_emissionRate)
    {
        // 每当粒子的寿命到期，着色器便将其作为新活跃粒子发射的潜在候选。
        //如果生成了一个新的例子，着色器则使用randomValue函数生成一个随机值来初始化新粒子的速度和大小
        v_position = vec2(0.0, -1.0);
        v_velocity = vec2(randomValue(seed) * 2.0 - 1.00, randomValue(seed) * 1.4 + 1.0);
        v_size = randomValue(seed) * 20.0 + 60.0;
        v_curtime = u_time;
        v_lifetime = 2.0;
    }
    else
    {
        v_position = a_position;
        v_velocity = a_velocity;
        v_size = a_size;
        v_curtime = a_curtime;
        v_lifetime = a_lifetime;
    }

    gl_Position = vec4(v_position, 0.0, 1.0);
}