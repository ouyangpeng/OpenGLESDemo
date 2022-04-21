#include <cstring>
#include "NativeTriangleVertextBufferObject.h"

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

void NativeTriangleVBO::Create() {
    GLUtils::printGLInfo();
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_hello_triangle_vbo.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_hello_triangle2.glsl");
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    offsetLoc = glGetUniformLocation(m_ProgramObj, "u_offset");

    if (!m_ProgramObj) {
        LOGD("Could not Create program")
        return;
    }

    vboIds[0] = 0;
    vboIds[1] = 0;

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void NativeTriangleVBO::Draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_ProgramObj);

    // with VBOs
    // Offset tge vertex positions so both can be seen
    glUniform1f(offsetLoc, 1.0f);
    DrawPrimitiveWithVBOs(3, vertices,
                          sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                          3, indices);

    // without VBOs
    glUniform1f(offsetLoc, 0.0f);
    DrawPrimitiveWithoutVBOs(vertices,
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

        // 既然不是要VBO或者VAO等，就记得把这些解绑掉，防止因为自己或者其他人导致的错误
        // 需要关闭它，否则后面的
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices); 就会报错1282
        // 否则改成
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
        glBindVertexArray(GL_NONE);

        glEnableVertexAttribArray(VERTEX_POS_INDX);
        glEnableVertexAttribArray(VERTEX_COLOR_INDX);

        glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
                              GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);

        vtxBuf += VERTEX_POS_SIZE;

        glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
                              GL_FLOAT, GL_FALSE, vtxStride, vtxBuf);

        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, indicesParam);

        GO_CHECK_GL_ERROR()

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
            //Only allocate on the first Draw
            // 使用glGenBuffers函数生成2个VBO对象
            glGenBuffers(2, vboIds);
            // OpenGL有很多缓冲对象类型，顶点缓冲对象的缓冲类型是GL_ARRAY_BUFFER。
            // OpenGL允许我们同时绑定多个缓冲，只要它们是不同的缓冲类型。
            // 我们可以使用glBindBuffer函数把新创建的缓冲绑定到GL_ARRAY_BUFFER目标上
            // 从这一刻起，我们使用的任何（在GL_ARRAY_BUFFER目标上的）缓冲调用都会用来配置当前绑定的缓冲(VBO)。

            // VBO 0 是 缓冲 顶点
            // 复制顶点数组到缓冲中供OpenGL使用
            glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);

            glBufferData(GL_ARRAY_BUFFER, vtxStride * numVertices,
                         vtxBuf, GL_STATIC_DRAW);

            // VBO 1 实际上是 EBO， 缓冲 indices
            // 和顶点缓冲对象一样，EBO也是一个缓冲，它专门储存索引，OpenGL调用这些顶点的索引来决定该绘制哪个顶点。
            // 所谓的索引绘制(Indexed Drawing)正是我们问题的解决方案。

            // 与VBO类似，我们先绑定EBO然后用glBufferData把索引复制到缓冲里。
            // 同样，和VBO类似，我们会把这些函数调用放在绑定和解绑函数调用之间，只不过这次我们把缓冲的类型定义为GL_ELEMENT_ARRAY_BUFFER。
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                         sizeof(GLushort) * numIndices,
                         indicesParam, GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
        // 我们传递了GL_ELEMENT_ARRAY_BUFFER当作缓冲目标。
        // 最后一件要做的事是用glDrawElements来替换glDrawArrays函数，来指明我们从索引缓冲渲染。
        // 使用glDrawElements时，我们会使用当前绑定的索引缓冲对象中的索引进行绘制
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);

        glEnableVertexAttribArray(VERTEX_POS_INDX);
        glEnableVertexAttribArray(VERTEX_COLOR_INDX);

        //设置顶点属性指针
        glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
                              GL_FLOAT, GL_FALSE, vtxStride, (const void *) offset);

        offset += VERTEX_POS_SIZE * sizeof(GLfloat);
        glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
                              GL_FLOAT, GL_FALSE, vtxStride, (const void *) offset);

        glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);

        glDisableVertexAttribArray(VERTEX_POS_INDX);
        glDisableVertexAttribArray(VERTEX_COLOR_INDX);

        // 记得解绑
        glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_NONE);
    FUN_END_TIME("NativeTriangleVBO::DrawPrimitiveWithVBOs")
}

void NativeTriangleVBO::Shutdown() {
    GLBaseSample::Shutdown();

    glDeleteBuffers(2, &vboIds[0]);
}