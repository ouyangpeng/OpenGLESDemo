//
// Created by OuyangPeng on 2021/10/25.
//

#include <cstdlib>
#include "ParticleSystem.h"


void ParticleSystem::create() {
    GLUtils::printGLInfo();

    // Main Program
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_particlesystem.glsl");
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_particlesystem.glsl");

    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (!mProgram) {
        LOGD("Could not create program")
        return;
    }

    // Get the uniform locations
    mTimeLoc = glGetUniformLocation ( mProgram, "u_time" );
    mCenterPositionLoc = glGetUniformLocation ( mProgram, "u_centerPosition" );

    mColorLoc = glGetUniformLocation ( mProgram, "u_color" );
    mSamplerLoc = glGetUniformLocation ( mProgram, "s_texture" );

    glClearColor ( 0.0f, 0.0f, 0.0f, 0.0f );

    // Fill in particle data array
    // 指定seed为当前系统流逝了的时间（单位为秒）：time_t   time(0)
    srand((unsigned)time(nullptr));

    int i;
    for(i = 0; i < PARTICLE_SYSTEM_NUM_PARTICLES; i++)
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

void ParticleSystem::draw() {
    // 每次更新一下
    update(getDeltaTime());

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT );

    // Use the program object
    glUseProgram ( mProgram );

    
    // Load the vertex attributes

    // a_lifetime
    glVertexAttribPointer (PARTICLE_SYSTEM_ATTRIBUTE_LIFETIME_LOCATION, 1, GL_FLOAT,
                           GL_FALSE, PARTICLE_SIZE * sizeof ( GLfloat ),
                           mParticleData );
    // a_endPosition
    glVertexAttribPointer (PARTICLE_SYSTEM_ATTRIBUTE_END_POSITION_LOCATION, 3, GL_FLOAT,
                           GL_FALSE, PARTICLE_SIZE * sizeof ( GLfloat ),
                           &mParticleData[1] );

    // a_startPosition
    glVertexAttribPointer (PARTICLE_SYSTEM_ATTRIBUTE_START_POSITION_LOCATION, 3, GL_FLOAT,
                           GL_FALSE, PARTICLE_SIZE * sizeof ( GLfloat ),
                           &mParticleData[4] );

    glEnableVertexAttribArray (PARTICLE_SYSTEM_ATTRIBUTE_LIFETIME_LOCATION );
    glEnableVertexAttribArray (PARTICLE_SYSTEM_ATTRIBUTE_END_POSITION_LOCATION );
    glEnableVertexAttribArray (PARTICLE_SYSTEM_ATTRIBUTE_START_POSITION_LOCATION );

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
    glDrawArrays (GL_POINTS, 0, PARTICLE_SYSTEM_NUM_PARTICLES );
    
}

void ParticleSystem::shutdown() {
    // Delete texture object
    glDeleteTextures ( 1, &mTextureId );

    // Delete program object
    GLUtils::DeleteProgram(mProgram);
}

void ParticleSystem::update(float deltaTime) {

    mTime += deltaTime;

    glUseProgram(mProgram);

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
