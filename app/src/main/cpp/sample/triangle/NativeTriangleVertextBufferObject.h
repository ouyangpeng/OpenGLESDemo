#pragma once

#include <GLBaseSample.h>

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

class NativeTriangleVBO : public GLBaseSample {

public:
    NativeTriangleVBO() = default;

    virtual ~NativeTriangleVBO() = default;

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

private:

    // VertexBufferObject Ids
    GLuint vboIds[2];

    // x-offset uniform location
    GLuint offsetLoc;

    static void DrawPrimitiveWithoutVBOs(GLfloat *verticesParam, GLint vtxStride,
                                         GLint numIndices, GLushort *indicesParam);

    void DrawPrimitiveWithVBOs(GLint numVertices, GLfloat *vtxBuf,
                               GLint vtxStride, GLint numIndices, GLushort *indicesParam);
};


