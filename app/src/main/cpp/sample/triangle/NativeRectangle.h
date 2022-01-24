#pragma once

#include <GLBaseSample.h>

class NativeRectangle : public GLBaseSample {

#define VERTEX_POS_INDX       0

public:
    NativeRectangle() = default;

    virtual ~NativeRectangle() = default;

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

private:
    // VertexBufferObject Ids
    GLuint vboIds[2];

    // VertexArrayObject Id
    GLuint vaoId;
};
