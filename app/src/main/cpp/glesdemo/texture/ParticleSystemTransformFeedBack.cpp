//
// Created by OuyangPeng on 2021/10/25.
//

#include "ParticleSystemTransformFeedBack.h"

ParticleSystemTransformFeedBack::ParticleSystemTransformFeedBack() {

}

ParticleSystemTransformFeedBack::~ParticleSystemTransformFeedBack() {

}

void ParticleSystemTransformFeedBack::create() {
    GLUtils::printGLInfo();

    Particle particleData[ NUM_PARTICLES ];
    int i;

    // 初始化发射粒子
    initEmitParticles();

    const char *vertex2 = GLUtils::openTextFile(
            "vertex/vertex_shader_particlesystem_transform_feedback2.glsl");
    const char *fragment2 = GLUtils::openTextFile(
            "fragment/fragment_shader_particlesystem_transform_feedback2.glsl");
    // Load the shaders and get a linked program object
    drawProgramObject = GLUtils::createProgram(&vertex2, &fragment2);

    // Get the uniform locations
    drawTimeLoc = glGetUniformLocation ( drawProgramObject, "u_time" );
    drawAccelerationLoc = glGetUniformLocation ( drawProgramObject, "u_acceleration" );

    drawColorLoc = glGetUniformLocation ( drawProgramObject, "u_color" );
    samplerLoc = glGetUniformLocation ( drawProgramObject, "s_texture" );

    time = 0.0f;
    curSrcIndex = 0;

    glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

    textureId = GLUtils::loadTexture(  "texture/smoke.png" );

    if ( textureId <= 0 )
    {
        return;
    }

    // Create a 3D noise texture for random values
    noiseTextureId = Create3DNoiseTexture ( 128, 50.0 );

    // Initialize particle data
    for ( i = 0; i < NUM_PARTICLES; i++ )
    {
        Particle *particle = &particleData[i];
        particle->position[0] = 0.0f;
        particle->position[1] = 0.0f;
        particle->velocity[0] = 0.0f;
        particle->velocity[1] = 0.0f;
        particle->size = 0.0f;
        particle->curtime = 0.0f;
        particle->lifetime = 0.0f;
    }

    // 在初始化的时候，分配两个缓冲区对象以保存粒子数据。
    // 算法在两个缓冲区之间来回切换，每次都切换用于粒子发射的输入或者输出缓冲区。

    // Create the particle VBOs
    glGenBuffers ( 2, &particleVBOs[0] );

    for ( i = 0; i < 2; i++ )
    {
        glBindBuffer ( GL_ARRAY_BUFFER, particleVBOs[i] );
        glBufferData ( GL_ARRAY_BUFFER, sizeof ( Particle ) * NUM_PARTICLES, particleData, GL_DYNAMIC_COPY );
    }
}

// 初始化发射粒子
void ParticleSystemTransformFeedBack::initEmitParticles() {
// Main Program
    const char *vertex = GLUtils::openTextFile(
            "vertex/vertex_shader_particlesystem_transform_feedback.glsl");
    const char *fragment = GLUtils::openTextFile(
            "fragment/fragment_shader_particlesystem_transform_feedback.glsl");

    // Set program handles
    emitProgramObject = GLUtils::createProgram(&vertex, &fragment);

    if (!emitProgramObject) {
        LOGD("Could not create program");
        return;
    }

    // 为了在这个顶点着色器中使用变化反馈，输出变量必须在链接程序对象之前标记为用于变化反馈
    const char *feedbackVaryings[5] =
            {
                "v_position",
                "v_velocity",
                "v_size",
                "v_curtime",
                "v_lifetime"
            };

    // Set the vertex shader outputs as transform feedback varyings   输出变量必须在链接程序对象之前标记为用于变化反馈
    // glTransformFeedbackVaryings 调用确保传入的输出变量用于变化反馈
    // GL_INTERLEAVED_ATTRIBS 参数指定输出变量在输出缓冲区对象中交叉存取。
    // 变量的顺序和布局必须与预期的缓冲区对象布局相符
    glTransformFeedbackVaryings ( emitProgramObject, 5, feedbackVaryings, GL_INTERLEAVED_ATTRIBS );

    // Link program must occur after calling glTransformFeedbackVaryings
    glLinkProgram ( emitProgramObject );

    // Get the uniform locations - this also needs to happen after glLinkProgram is called again so
    // that the uniforms that output to varyings are active
    emitTimeLoc = glGetUniformLocation ( emitProgramObject, "u_time" );
    emitEmissionRateLoc = glGetUniformLocation ( emitProgramObject, "u_emissionRate" );
    emitNoiseSamplerLoc = glGetUniformLocation ( emitProgramObject, "s_noiseTex" );
}



void ParticleSystemTransformFeedBack::change(int width, int height) {
    mWidth = width;
    mHeight = height;
    LOGD("change() width = %d , height = %d\n", width, height);

    // Set the viewport
    glViewport ( 0, 0, mWidth, mHeight );
}

void ParticleSystemTransformFeedBack::draw() {
    // 每次更新一下
    update(getDeltaTime());

    // 为了确保绘图调用使用变化反馈结果之前该结果的完整性，我们创建一个同步对象，
    // 并在调用glEndTransformFeedback之后立即插入栅栏命令。
    // 在绘图调用执行之前，我们将用 glWaitSync 调用等待同步对象。
    // 执行绘图调用并恢复状态之后，我们在缓冲区之间来回切换，
    // 使得下一次调用EmitShader时使用前一帧的变化反馈输出作为输入

    // Block the GL server until transform feedback results are completed
    glWaitSync ( emitSync, 0, GL_TIMEOUT_IGNORED );
    glDeleteSync ( emitSync );

    // Set the viewport
    glViewport ( 0, 0, mWidth, mHeight );

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT );

    // Use the program object
    glUseProgram ( drawProgramObject );

    // Load the VBO and vertex attributes
    setupVertexAttributes (particleVBOs[ curSrcIndex ] );

    // Set uniforms
    glUniform1f ( drawTimeLoc, time );
    glUniform4f ( drawColorLoc, 1.0f, 1.0f, 1.0f, 1.0f );
    glUniform2f ( drawAccelerationLoc, 0.0f, ACCELERATION );

    // Blend particles
    glEnable ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE );

    // Bind the texture
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, textureId );

    // Set the sampler texture unit to 0
    glUniform1i ( samplerLoc, 0 );

    glDrawArrays ( GL_POINTS, 0, NUM_PARTICLES );
}

void ParticleSystemTransformFeedBack::shutdown() {
    // Delete texture object
    glDeleteTextures ( 1, &textureId );

    // Delete program object
    glDeleteProgram (drawProgramObject );
    glDeleteProgram (emitProgramObject );

    glDeleteBuffers ( 2, &particleVBOs[0] );
}


void ParticleSystemTransformFeedBack::update(float deltaTime) {

    time += deltaTime;

    emitParticles ( deltaTime );
}

float ParticleSystemTransformFeedBack::getDeltaTime() {
    if (mLastTime == 0) {
        mLastTime = TimeUtils::currentTimeMillis();
    }
    long currentTime = TimeUtils::currentTimeMillis();
    long elapsedTime = currentTime - mLastTime;
    float deltaTime = (float)elapsedTime / 1000.0f;
    mLastTime = currentTime;
    return deltaTime;
}

// 用变化反馈发射粒子
void ParticleSystemTransformFeedBack::emitParticles(float deltaTime) {
    /**
     * 粒子系统用变化反馈更新，然后按如下步骤进行渲染：
     * 1. 在每一帧中，选择某一个粒子VBO作为输入，并绑定为 GL_ARRAY_BUFFER。 输出则绑定为 GL_TRANSFORM_FEEDBACK_BUFFER
     * 2. 启用 GL_RASTERIZER_DISCARD ，从而不绘制任何片段
     * 3. 粒子发射着色器 用点图元执行（每个粒子都是一个点）
     *    顶点着色器输出新的粒子到变化反馈缓冲区，并将现有粒子原封不动地复制到变化反馈缓冲区
     * 4. 禁用 GL_RASTERIZER_DISCARD ， 以便应用程序可以绘制这些粒子。
     * 5. 用于变化反馈渲染的缓冲区现在绑定为  GL_ARRAY_BUFFER。 另外绑定一个顶点/片段着色器以绘制粒子。
     * 6. 粒子被渲染到帧缓冲区
     * 7. 在下一帧中，输入/输出缓冲区对象 被交换，继续相同的过程。
     * **/

    GLuint srcVBO = particleVBOs[ curSrcIndex ];
    GLuint dstVBO = particleVBOs[ ( curSrcIndex + 1 ) % 2 ];

    glUseProgram ( emitProgramObject );

    setupVertexAttributes(srcVBO);

    // Set transform feedback buffer
    glBindBuffer ( GL_TRANSFORM_FEEDBACK_BUFFER, dstVBO );

    // 目标缓冲区对象用glBindBufferBase绑定到GL_TRANSFORM_FEEDBACK_BUFFER目标
    glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0, dstVBO );

    // Turn off rasterization - we are not drawing
    // 通过启用GL_RASTERIZER_DISCARD禁止光栅化，因为我们实际上不会绘制任何片段；
    // 相反，我们只想执行顶点着色器并输出到变化反馈缓冲区
    // 启用 GL_RASTERIZER_DISCARD ，从而不绘制任何片段
    glEnable ( GL_RASTERIZER_DISCARD );

    // Set uniforms
    glUniform1f ( emitTimeLoc, time );
    glUniform1f ( emitEmissionRateLoc, EMISSION_RATE );

    // Bind the 3D noise texture
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_3D, noiseTextureId );
    glUniform1i ( emitNoiseSamplerLoc, 0 );

    // Emit particles using transform feedback
    // 启用变化反馈渲染，
    // 后续 用 GL_POINTS 对 glDrawArrays 的调用将被记录在变化反馈缓冲区中，
    // 直到调用glEndTransformFeedback
    glBeginTransformFeedback ( GL_POINTS );
    glDrawArrays ( GL_POINTS, 0, NUM_PARTICLES );
    glEndTransformFeedback();

    // Create a sync object to ensure transform feedback results are completed before the draw that uses them.
    emitSync = glFenceSync ( GL_SYNC_GPU_COMMANDS_COMPLETE, 0 );

    // Restore state
    // 禁用 GL_RASTERIZER_DISCARD ， 以便应用程序可以绘制这些粒子。
    glDisable ( GL_RASTERIZER_DISCARD );
    glUseProgram ( 0 );
    glBindBufferBase ( GL_TRANSFORM_FEEDBACK_BUFFER, 0, 0 );
    glBindBuffer ( GL_ARRAY_BUFFER, 0 );
    glBindTexture ( GL_TEXTURE_3D, 0 );

    // Ping pong the buffers
    curSrcIndex = ( curSrcIndex + 1 ) % 2;
}

void ParticleSystemTransformFeedBack::setupVertexAttributes(GLuint vboID) {
    glBindBuffer ( GL_ARRAY_BUFFER, vboID );
    glVertexAttribPointer ( ATTRIBUTE_POSITION, 2, GL_FLOAT,
                            GL_FALSE, sizeof ( Particle ),
                            ( const void * ) nullptr );

    glVertexAttribPointer ( ATTRIBUTE_VELOCITY, 2, GL_FLOAT,
                            GL_FALSE, sizeof ( Particle ),
                            ( const void * ) offsetof ( Particle, velocity[0] ) );

    glVertexAttribPointer ( ATTRIBUTE_SIZE, 1, GL_FLOAT,
                            GL_FALSE, sizeof ( Particle ),
                            ( const void * ) offsetof ( Particle, size ) );

    glVertexAttribPointer ( ATTRIBUTE_CURTIME, 1, GL_FLOAT,
                            GL_FALSE, sizeof ( Particle ),
                            ( const void * ) offsetof ( Particle, curtime ) );

    glVertexAttribPointer ( ATTRIBUTE_LIFETIME, 1, GL_FLOAT,
                            GL_FALSE, sizeof ( Particle ),
                            ( const void * ) offsetof ( Particle, lifetime ) );

    glEnableVertexAttribArray ( ATTRIBUTE_POSITION );
    glEnableVertexAttribArray ( ATTRIBUTE_VELOCITY );
    glEnableVertexAttribArray ( ATTRIBUTE_SIZE );
    glEnableVertexAttribArray ( ATTRIBUTE_CURTIME );
    glEnableVertexAttribArray ( ATTRIBUTE_LIFETIME );
}


////////////////////////////////////////////////////////////////////////////////

ParticleSystemTransformFeedBack* particleSystemTransformFeedBack;


extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_ParticleSystemTransformFeedbackRenderer_nativeSurfaceCreate(
        JNIEnv *env, jobject thiz, jobject asset_manager) {
    // 初始化设置assetManager  一定要记得初始化，否则会报空指针异常
    GLUtils::setEnvAndAssetManager(env, asset_manager);

    if(particleSystemTransformFeedBack){
        delete particleSystemTransformFeedBack;
        particleSystemTransformFeedBack = nullptr;
    }
    particleSystemTransformFeedBack = new ParticleSystemTransformFeedBack();
    particleSystemTransformFeedBack->create();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_ParticleSystemTransformFeedbackRenderer_nativeSurfaceChange(
        JNIEnv *env, jobject thiz, jint width, jint height) {
    if(particleSystemTransformFeedBack != nullptr){
        particleSystemTransformFeedBack->change(width,height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_ParticleSystemTransformFeedbackRenderer_nativeDrawFrame(
        JNIEnv *env, jobject thiz) {
    if(particleSystemTransformFeedBack != nullptr){
        particleSystemTransformFeedBack->draw();
    }
}