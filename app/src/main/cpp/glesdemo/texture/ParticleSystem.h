//
// Created by OuyangPeng on 2021/10/25.
//

#ifndef OPENGLESDEMO_PARTICLESYSTEM_H
#define OPENGLESDEMO_PARTICLESYSTEM_H

#include "../../utils/GLUtils.h"

#define NUM_PARTICLES   1000
#define PARTICLE_SIZE   7

#define ATTRIBUTE_LIFETIME_LOCATION       0
#define ATTRIBUTE_STARTPOSITION_LOCATION  1
#define ATTRIBUTE_ENDPOSITION_LOCATION    2

class ParticleSystem {
public:
    ParticleSystem();

    ~ParticleSystem();

    void create();

    void change(int width, int height);

    void draw();

    void shutdown();

private:
    // Handle to a program object
    GLuint programObject;

    // Uniform location
    GLint mTimeLoc;
    GLint mCenterPositionLoc;
    GLint mColorLoc;
    GLint mSamplerLoc;

    // Texture handle
    GLuint mTextureId;

    // Particle vertex data
    float mParticleData[ NUM_PARTICLES * PARTICLE_SIZE ];

    long mLastTime = 0;

    // Current time
    float mTime;

    int mWidth;
    int mHeight;

    float getDeltaTime();

    void update(float deltaTime);
};


#endif //OPENGLESDEMO_PARTICLESYSTEM_H
