//
// Created by OuyangPeng on 2021/10/25.
//

#include <cstdlib>
#include "ParticleSystem.h"

ParticleSystem::ParticleSystem() {

}

ParticleSystem::~ParticleSystem() {

}

void ParticleSystem::create() {
    GLUtils::printGLInfo();

    // Main Program
    const char *vertex = GLUtils::openTextFile("vertex/vertex_shader_particlesystem.glsl");
    const char *fragment = GLUtils::openTextFile(
            "fragment/fragment_shader_particlesystem.glsl");

    // Set program handles
    programObject = GLUtils::createProgram(&vertex, &fragment);

    if (!programObject) {
        LOGD("Could not create program");
        return;
    }

    // Get the uniform locations
    mTimeLoc = glGetUniformLocation ( programObject, "u_time" );
    mCenterPositionLoc = glGetUniformLocation ( programObject, "u_centerPosition" );

    mColorLoc = glGetUniformLocation ( programObject, "u_color" );
    mSamplerLoc = glGetUniformLocation ( programObject, "s_texture" );

    glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

    // Fill in particle data array
    srand(0);

    int i;
    for(i = 0; i < NUM_PARTICLES; i++)
    {
        float *particleData = &mParticleData[i * PARTICLE_SIZE];

        // Lifetime of particle
        ( *particleData++ ) = ( ( float ) ( random() % 10000 ) / 10000.0f );

        // End position of particle
        ( *particleData++ ) = ( ( float ) ( random() % 10000 ) / 5000.0f ) - 1.0f;
        ( *particleData++ ) = ( ( float ) ( random() % 10000 ) / 5000.0f ) - 1.0f;
        ( *particleData++ ) = ( ( float ) ( random() % 10000 ) / 5000.0f ) - 1.0f;

        // Start position of particle
        ( *particleData++ ) = ( ( float ) ( random() % 10000 ) / 40000.0f ) - 0.125f;
        ( *particleData++ ) = ( ( float ) ( random() % 10000 ) / 40000.0f ) - 0.125f;
        ( *particleData++ ) = ( ( float ) ( random() % 10000 ) / 40000.0f ) - 0.125f;
    }

    // Initialize time to cause reset on first update
    mTime = 1.0f;

    // Load the textures
    // 从assets目录下 取出对应的Texture
    mTextureId = GLUtils::loadTexture(  "texture/smoke.png" );
}

void ParticleSystem::change(int width, int height) {
    mWidth = width;
    mHeight = height;
    LOGD("change() width = %d , height = %d\n", width, height);

    // Set the viewport
    glViewport ( 0, 0, mWidth, mHeight );
}

void ParticleSystem::draw() {
    // 每次更新一下
    update(getDeltaTime());

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT );

    // Use the program object
    glUseProgram ( programObject );

    
    // Load the vertex attributes

    // a_lifetime
    glVertexAttribPointer ( ATTRIBUTE_LIFETIME_LOCATION, 1, GL_FLOAT,
                            GL_FALSE, PARTICLE_SIZE * sizeof ( GLfloat ),
                            mParticleData );
    // a_endPosition
    glVertexAttribPointer (ATTRIBUTE_END_POSITION_LOCATION, 3, GL_FLOAT,
                           GL_FALSE, PARTICLE_SIZE * sizeof ( GLfloat ),
                           &mParticleData[1] );

    // a_startPosition
    glVertexAttribPointer (ATTRIBUTE_START_POSITION_LOCATION, 3, GL_FLOAT,
                           GL_FALSE, PARTICLE_SIZE * sizeof ( GLfloat ),
                           &mParticleData[4] );

    glEnableVertexAttribArray ( ATTRIBUTE_LIFETIME_LOCATION );
    glEnableVertexAttribArray (ATTRIBUTE_END_POSITION_LOCATION );
    glEnableVertexAttribArray (ATTRIBUTE_START_POSITION_LOCATION );

    // Blend particles   启用Alpha混合
    // 片段着色器中产生的Alpha值与片段的颜色进行调制。
    // 然后，该值被加到片段目标中保存的值，结果得到粒子系统的相加混合效果
    glEnable ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE );

    // Bind the texture 绑定烟雾纹理
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, mTextureId );

    // Set the sampler texture unit to 0
    glUniform1i ( mSamplerLoc, 0 );

    // 绘制粒子
    glDrawArrays ( GL_POINTS, 0, NUM_PARTICLES );
    
}

void ParticleSystem::shutdown() {
    // Delete texture object
    glDeleteTextures ( 1, &mTextureId );

    // Delete program object
    glDeleteProgram ( programObject );
}

void ParticleSystem::update(float deltaTime) {

    mTime += deltaTime;

    glUseProgram(programObject);

    // 一秒之后 重置时间，然后设置另一次爆炸的新的中心位置和时间
    if (mTime >= 1.0f )
    {
        float centerPos[3];
        float color[4];

        mTime = 0.0f;

        // Pick a new start location and color
        centerPos[0] = ( ( float ) ( random() % 10000 ) / 10000.0f ) - 0.5f;
        centerPos[1] = ( ( float ) ( random() % 10000 ) / 10000.0f ) - 0.5f;
        centerPos[2] = ( ( float ) ( random() % 10000 ) / 10000.0f ) - 0.5f;

        glUniform3fv ( mCenterPositionLoc, 1, &centerPos[0] );

        // Random color
        color[0] = ( ( float ) ( random() % 10000 ) / 20000.0f ) + 0.5f;
        color[1] = ( ( float ) ( random() % 10000 ) / 20000.0f ) + 0.5f;
        color[2] = ( ( float ) ( random() % 10000 ) / 20000.0f ) + 0.5f;
        color[3] = 0.5;

        glUniform4fv ( mColorLoc, 1, &color[0] );
    }

    // Load uniform time variable
    glUniform1f ( mTimeLoc, mTime );
}

float ParticleSystem::getDeltaTime() {
    if (mLastTime == 0) {
        mLastTime = TimeUtils::currentTimeMillis();
    }
    long currentTime = TimeUtils::currentTimeMillis();
    long elapsedTime = currentTime - mLastTime;
    float deltaTime = (float)elapsedTime / 1000.0f;
    mLastTime = currentTime;
    return deltaTime;
}


////////
ParticleSystem* particleSystem;

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_ParticleSystemRenderer_nativeSurfaceCreate(
        JNIEnv *env, jobject thiz, jobject assetManager) {
    // 初始化设置assetManager  一定要记得初始化，否则会报空指针异常
    GLUtils::setEnvAndAssetManager(env, assetManager);

    if(particleSystem){
        delete particleSystem;
        particleSystem = nullptr;
    }
    particleSystem = new ParticleSystem();
    particleSystem->create();
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_ParticleSystemRenderer_nativeSurfaceChange(
        JNIEnv *env, jobject thiz, jint width, jint height) {
    if(particleSystem != nullptr){
        particleSystem->change(width,height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_ParticleSystemRenderer_nativeDrawFrame(
        JNIEnv *env, jobject thiz) {
    if(particleSystem != nullptr){
        particleSystem->draw();
    }
}