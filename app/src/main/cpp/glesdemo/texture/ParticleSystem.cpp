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
    GLUtils::printGLString("Version", GL_VERSION);
    GLUtils::printGLString("Vendor", GL_VENDOR);
    GLUtils::printGLString("Renderer", GL_RENDERER);
    GLUtils::printGLString("Extensions", GL_EXTENSIONS);

    const char* VERTEX_SHADER_TRIANGLE =
            "#version 300 es                                      \n"
            "uniform float u_time;                                \n"
            "uniform vec3 u_centerPosition;                       \n"
            "layout(location = 0) in float a_lifetime;            \n"
            "layout(location = 1) in vec3 a_startPosition;        \n"
            "layout(location = 2) in vec3 a_endPosition;          \n"
            "out float v_lifetime;                                \n"
            "void main()                                          \n"
            "{                                                    \n"
            "  if ( u_time <= a_lifetime )                        \n"
            "  {                                                  \n"
            "    gl_Position.xyz = a_startPosition +              \n"
            "                      (u_time * a_endPosition);      \n"
            "    gl_Position.xyz += u_centerPosition;             \n"
            "    gl_Position.w = 1.0;                             \n"
            "  }                                                  \n"
            "  else                                               \n"
            "  {                                                  \n"
            "     gl_Position = vec4( -1000, -1000, 0, 0 );       \n"
            "  }                                                  \n"
            "  v_lifetime = 1.0 - ( u_time / a_lifetime );        \n"
            "  v_lifetime = clamp ( v_lifetime, 0.0, 1.0 );       \n"
            "  gl_PointSize = ( v_lifetime * v_lifetime ) * 40.0; \n"
            "}";

    const char* FRAGMENT_SHADER_TRIANGLE =
            "#version 300 es                                      \n"
            "precision mediump float;                             \n"
            "uniform vec4 u_color;                                \n"
            "in float v_lifetime;                                 \n"
            "layout(location = 0) out vec4 fragColor;             \n"
            "uniform sampler2D s_texture;                         \n"
            "void main()                                          \n"
            "{                                                    \n"
            "  vec4 texColor;                                     \n"
            "  texColor = texture( s_texture, gl_PointCoord );    \n"
            "  fragColor = vec4( u_color ) * texColor;            \n"
            "  fragColor.a *= v_lifetime;                         \n"
            "}                                                    \n";

    // Load the shaders and get a linked program object
    programObject = GLUtils::createProgram(&VERTEX_SHADER_TRIANGLE, &FRAGMENT_SHADER_TRIANGLE);
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
}

void ParticleSystem::draw() {
    // 每次更新一下
    update(getDeltaTime());

    // Set the viewport
    glViewport ( 0, 0, mWidth, mHeight );

    // Clear the color buffer
    glClear ( GL_COLOR_BUFFER_BIT );

    // Use the program object
    glUseProgram ( programObject );

    
    // Load the vertex attributes
    glVertexAttribPointer ( ATTRIBUTE_LIFETIME_LOCATION, 1, GL_FLOAT,
                            GL_FALSE, PARTICLE_SIZE * sizeof ( GLfloat ),
                            mParticleData );

    glVertexAttribPointer ( ATTRIBUTE_ENDPOSITION_LOCATION, 3, GL_FLOAT,
                            GL_FALSE, PARTICLE_SIZE * sizeof ( GLfloat ),
                            &mParticleData[1] );

    glVertexAttribPointer ( ATTRIBUTE_STARTPOSITION_LOCATION, 3, GL_FLOAT,
                            GL_FALSE, PARTICLE_SIZE * sizeof ( GLfloat ),
                            &mParticleData[4] );

    glEnableVertexAttribArray ( ATTRIBUTE_LIFETIME_LOCATION );
    glEnableVertexAttribArray ( ATTRIBUTE_ENDPOSITION_LOCATION );
    glEnableVertexAttribArray ( ATTRIBUTE_STARTPOSITION_LOCATION );

    // Blend particles
    glEnable ( GL_BLEND );
    glBlendFunc ( GL_SRC_ALPHA, GL_ONE );

    // Bind the texture
    glActiveTexture ( GL_TEXTURE0 );
    glBindTexture ( GL_TEXTURE_2D, mTextureId );

    // Set the sampler texture unit to 0
    glUniform1i ( mSamplerLoc, 0 );

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
static ParticleSystem* particleSystem;

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_ParticleSystemRenderer_00024Companion_nativeSurfaceCreate(
        JNIEnv *env, jobject thiz, jobject assetManager) {
    // 初始化设置assetManager  一定要记得初始化，否则会报空指针异常
    GLUtils::setEnvAndAssetManager(env, assetManager);

    particleSystem = new ParticleSystem();
    if(particleSystem != nullptr){
        particleSystem->create();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_ParticleSystemRenderer_00024Companion_nativeSurfaceChange(
        JNIEnv *env, jobject thiz, jint width, jint height) {
    if(particleSystem != nullptr){
        particleSystem->change(width,height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_ParticleSystemRenderer_00024Companion_nativeDrawFrame(
        JNIEnv *env, jobject thiz) {
    if(particleSystem != nullptr){
        particleSystem->draw();
    }
}