#pragma once

#include "../../utils/GLUtils.h"

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

class NativeTriangleMapBuffers : public BaseGLSample {
public:
    NativeTriangleMapBuffers();

    virtual ~NativeTriangleMapBuffers();

    virtual void create();

    virtual void change(int width, int height);

    virtual void draw();

    virtual void shutdown();

private:
    GLuint vboIds[2];

    void DrawPrimitiveWithVBOsMapBuffers(GLint numVertices, GLfloat *vtxBuf,
                                         GLint vtxStride, GLint numIndices,
                                         GLushort *indices);
};


