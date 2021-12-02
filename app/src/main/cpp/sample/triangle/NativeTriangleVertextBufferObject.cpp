#include <cstring>
#include "NativeTriangleVertextBufferObject.h"

// 可以参考这篇讲解： https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/

// 3 vertices, with (x,y,z) ,(r, g, b, a)  per-vertex
static GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] =
        {
                // 逆时针 三个顶点
                -0.5f, 0.5f, 0.0f,       // v0
                1.0f, 0.0f, 0.0f, 1.0f,  // c0

                -1.0f, -0.5f, 0.0f,        // v1
                0.0f, 1.0f, 0.0f, 1.0f,  // c1

                0.0f, -0.5f, 0.0f,        // v2
                0.0f, 0.0f, 1.0f, 1.0f,  // c2
        };

// Index buffer data
static GLushort indices[3] = {0, 1, 2};

void NativeTriangleVBO::create() {
    GLUtils::printGLInfo();
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_hello_triangle_vbo.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_hello_triangle2.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    offsetLoc = glGetUniformLocation(mProgram, "u_offset");

    if (!mProgram) {
        LOGD("Could not create program")
        return;
    }

    vboIds[0] = 0;
    vboIds[1] = 0;

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void NativeTriangleVBO::draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(mProgram);

    // without VBOs
    glUniform1f(offsetLoc, 0.0f);
    DrawPrimitiveWithoutVBOs(vertices,
                             sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                             3, indices);

    // with VBOs
    // Offset tge vertex positions so both can be seen
    glUniform1f(offsetLoc, 1.0f);
    DrawPrimitiveWithVBOs(3, vertices,
                          sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                          3, indices);
}

// verticesParam   - pointer to a buffer that contains vertex attribute data
// vtxStride  - stride of attribute data / vertex in bytes
// numIndices - number of indicesParam that make up primitive drawn as triangles
// indicesParam    - pointer to element index buffer.
void NativeTriangleVBO::DrawPrimitiveWithoutVBOs(GLfloat *verticesParam,
                                                 GLint vtxStride, GLint numIndices,
                                                 GLushort *indicesParam) {
    FUN_BEGIN_TIME("NativeTriangleVBO::DrawPrimitiveWithoutVBOs")
        GLfloat *vtxBuf = verticesParam;

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

        glEnableVertexAttribArray(VERTEX_POS_INDX);
        glEnableVertexAttribArray(VERTEX_COLOR_INDX);

        glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
                              GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);

        vtxBuf += VERTEX_POS_SIZE;

        glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
                              GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);

        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indicesParam);

        glDisableVertexAttribArray(VERTEX_POS_INDX);
        glDisableVertexAttribArray(VERTEX_COLOR_INDX);
    FUN_END_TIME("NativeTriangleVBO::DrawPrimitiveWithoutVBOs")
}

void NativeTriangleVBO::DrawPrimitiveWithVBOs(GLint numVertices, GLfloat *vtxBuf,
                                              GLint vtxStride, GLint numIndices,
                                              GLushort *indicesParam) {
    FUN_BEGIN_TIME("NativeTriangleVBO::DrawPrimitiveWithVBOs")
        GLuint offset = 0;

        // vboIds[0] - used to store vertex attribute data
        // vboIds[l] - used to store element indicesParam
        if (vboIds[0] == 0 && vboIds[1] == 0) {
            //Only allocate on the first draw
            glGenBuffers(2, vboIds);

            glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
            glBufferData(GL_ARRAY_BUFFER, vtxStride * numVertices,
                         vtxBuf, GL_STATIC_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         sizeof(GLushort) * numIndices,
                         indicesParam, GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);

        glEnableVertexAttribArray(VERTEX_POS_INDX);
        glEnableVertexAttribArray(VERTEX_COLOR_INDX);

        glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
                              GL_FLOAT, GL_FALSE, vtxStride, (const void *) offset);

        offset += VERTEX_POS_SIZE * sizeof(GLfloat);
        glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
                              GL_FLOAT, GL_FALSE, vtxStride, (const void *) offset);

        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);

        glDisableVertexAttribArray(VERTEX_POS_INDX);
        glDisableVertexAttribArray(VERTEX_COLOR_INDX);

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    FUN_END_TIME("NativeTriangleVBO::DrawPrimitiveWithVBOs")
}

void NativeTriangleVBO::shutdown() {
    // Delete program object
    GLUtils::DeleteProgram(mProgram);

    glDeleteBuffers(2, &vboIds[0]);
}