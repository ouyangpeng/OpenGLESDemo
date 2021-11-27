//
// Created by OuyangPeng on 2021/10/25.
//

#ifndef OPENGLESDEMO_PARTICLESYSTEM_H
#define OPENGLESDEMO_PARTICLESYSTEM_H

#include "../../utils/GLUtils.h"

#define PARTICLE_SYSTEM_NUM_PARTICLES   1000
#define PARTICLE_SIZE   7

#define PARTICLE_SYSTEM_ATTRIBUTE_LIFETIME_LOCATION       0
#define PARTICLE_SYSTEM_ATTRIBUTE_START_POSITION_LOCATION  1
#define PARTICLE_SYSTEM_ATTRIBUTE_END_POSITION_LOCATION    2

class ParticleSystem : public BaseGLSample{
public:
    ParticleSystem();

    virtual ~ParticleSystem();

    virtual void create();

    virtual void change(int width, int height);

    virtual void draw();

    virtual void shutdown();

private:
    // Uniform location
    GLint mTimeLoc;
    GLint mCenterPositionLoc;
    GLint mColorLoc;
    GLint mSamplerLoc;

    // Texture handle
    GLuint mTextureId;

    // Particle vertex data
    float mParticleData[PARTICLE_SYSTEM_NUM_PARTICLES * PARTICLE_SIZE ];

    long mLastTime = 0;

    // Current time
    float mTime;

    float getDeltaTime();

    void update(float deltaTime);
};


#endif //OPENGLESDEMO_PARTICLESYSTEM_H
