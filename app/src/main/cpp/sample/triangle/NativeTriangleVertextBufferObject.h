#pragma once

#include <BaseGLSample.h>

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

class NativeTriangleVBO : public BaseGLSample{

public:
    NativeTriangleVBO();

    virtual ~NativeTriangleVBO();

    virtual void create();

    virtual void change(int width, int height);

    virtual void draw();

    virtual void shutdown();

private:

    // VertexBufferObject Ids
    GLuint vboIds[2];

    // x-offset uniform location
    GLuint offsetLoc;

    void DrawPrimitiveWithoutVBOs(GLfloat *vertices, GLint vtxStride,
                                  GLint numIndices, GLushort *indices);

    void DrawPrimitiveWithVBOs(GLint numVertices, GLfloat *vtxBuf,
                               GLint vtxStride, GLint numIndices, GLushort *indices);
};


