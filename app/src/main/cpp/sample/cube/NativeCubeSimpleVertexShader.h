#pragma once

#include <GLBaseSample.h>

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

class NativeCubeSimpleVertexShader : public GLBaseSample{

public:
    NativeCubeSimpleVertexShader() = default;

    virtual ~NativeCubeSimpleVertexShader() = default;

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

private:
    // Uniform locations
    GLint mvpLoc;

    // Vertex daata
    GLfloat *vertices;
    GLuint *indices;

    int numIndices;

    // Rotation angle
    GLfloat angle;

    // MVP matrix
    ESMatrix mvpMatrix;

    long mLastTime = 0;

    void update(float deltaTime);

    float getDeltaTime();
};


