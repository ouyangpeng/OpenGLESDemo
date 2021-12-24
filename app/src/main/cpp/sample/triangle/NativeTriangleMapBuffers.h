#pragma once

#include <GLBaseSample.h>

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

class NativeTriangleMapBuffers : public GLBaseSample {
public:
    NativeTriangleMapBuffers() = default;

    virtual ~NativeTriangleMapBuffers() = default;

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

private:
    GLuint vboIds[2];

    void DrawPrimitiveWithVBOsMapBuffers(GLint numVertices, GLfloat *vtxBuf,
                                         GLint vtxStride, GLint numIndices,
                                         GLushort *indices);
};


