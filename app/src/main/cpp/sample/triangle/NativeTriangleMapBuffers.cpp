#include <cstring>
#include "NativeTriangleMapBuffers.h"

// 可以参考这篇讲解： https://learnopengl-cn.github.io/01%20Getting%20started/04%20Hello%20Triangle/
void NativeTriangleMapBuffers::Create() {
    GLUtils::printGLInfo();

    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_hello_triangle2.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_hello_triangle2.glsl");
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (!m_ProgramObj) {
        LOGD("Could not Create program")
        return;
    }

    vboIds[0] = 0;
    vboIds[1] = 0;

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void NativeTriangleMapBuffers::Draw() {
    // 3 vertices, with (x,y,z) ,(r, g, b, a)  per-vertex
    GLfloat vertices[3 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] =
            {
                    // 逆时针 三个顶点
                    0.0f, 0.5f, 0.0f,            // v0 上角
                    1.0f, 0.0f, 0.0f, 1.0f,      // c0

                    -0.5f, -0.5f, 0.0f,          // v1 左下角
                    0.0f, 1.0f, 0.0f, 1.0f,      // c1

                    0.5f, -0.5f, 0.0f,           // v2 右下角
                    0.0f, 0.0f, 1.0f, 1.0f       // c2
            };

    // Index buffer data
    GLushort indices[3] = {0, 1, 2};


    // Clear the color buffer
    // 清除屏幕
    // 在OpenGL ES中，绘图中涉及多种缓冲区类型：颜色、深度、模板。
    // 这个例子，绘制三角形，只向颜色缓冲区中绘制图形。在每个帧的开始，我们用glClear函数清除颜色缓冲区
    // 缓冲区将用glClearColor指定的颜色清除。
    // 这个例子，我们调用了GLES30.glClearColor(1.0f, 1.0f, 1.0f, 0.0f); 因此屏幕清为白色。
    // 清除颜色应该由应用程序在调用颜色缓冲区的glClear之前设置。
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object
    // 在glUseProgram函数调用之后，每个着色器调用和渲染调用都会使用这个程序对象（也就是之前写的着色器)了。
    // 当我们渲染一个物体时要使用着色器程序 , 将其设置为活动程序。这样就可以开始渲染了
    glUseProgram(m_ProgramObj);

    DrawPrimitiveWithVBOsMapBuffers(3, vertices,
                                    sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
                                    3, indices);
}

void NativeTriangleMapBuffers::DrawPrimitiveWithVBOsMapBuffers(
        GLint numVertices, GLfloat *vtxBuf,
        GLint vtxStride, GLint numIndices, GLushort *indices) {
    GLuint offset = 0;
    // vboIds[0] - used to store vertex attribute data
    // vboIds[l] - used to store element indices
    if (vboIds[0] == 0 && vboIds[1] == 0) {

        // 应用程序也可以将缓冲区对象数据存储映射到应用程序的地址空间（也可以解除映射）
        // 应用程序映射缓冲区而不使用 glBufferData 或者 glBufferSubData加载的理由：
        // 1. 映射缓冲区可以减少应用程序的内存占用，因为可能只需要存储数据的一个副本
        // 2. 在使用共享内存的架构上，映射缓冲区返回GPU存储缓冲区的地址空间的直接地址。
        //    通过映射缓冲区，应用程序可以避免复制步骤，从而实现更好的更新性能

        GLfloat *vtxMappedBuf;
        GLushort *idxMappedBuf;

        // Only allocate on the first Draw
        glGenBuffers(2, vboIds);

        glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
        glBufferData(GL_ARRAY_BUFFER, vtxStride * numVertices, nullptr, GL_STATIC_DRAW);

        // glMapBufferRange 命令返回指向所有或者一部分（范围）缓冲区对象数据存储的指针。
        // 这个指针可以供应用程序使用，以读取或者更新缓冲区对象的内容
        // target 我们设置为 GL_ARRAY_BUFFER
        // offset 表示 缓冲区数据存储中的偏移量，以字节数计算。 我们这里设置为0
        // length 表示 需要映射的缓冲区数据的字节数
        // access 表示 访问标志的位域组合。应用程序必须指定如下标志中的至少一个：
        //      GL_MAP_READ_BIT     应用程序将从返回的指针读取
        //      GL_MAP_WRITE_BIT    应用程序将写入返回的指针
        // 此外，应用程序可以包含如下可选访问标志：
        //      GL_MAP_INVALIDATE_BUFFER_BIT
        //        表示整个缓冲区的内容可以在返回指针之前由驱动程序放弃。这个标志不能与GL_MAP_READ_BIT组合使用
        //      GL_MAP_INVALIDATE_RANGE_BIT
        //        表示指定范围内的缓冲区的内容可以在返回指针之前由驱动程序放弃。这个标志不能与GL_MAP_READ_BIT组合使用
        //      GL_MAP_FLUSH_EXPLICIT_BIT
        //        表示应用程序将明确地用glFlushMappedBufferRange刷新对映射范围子范围的操作。这个标志不能与GL_MAP_WRITE_BIT组合使用
        //      GL_MAP_UNSYNCHRONIZED_BIT
        //        表示应用程序在返回缓冲区范围的指针之前不需要的等待缓冲区对象上的未决操作。
        //        如果有未决操作，则未决操作的结果和缓冲区对象上的任何操作都变为未定义
        vtxMappedBuf = (GLfloat *) glMapBufferRange(GL_ARRAY_BUFFER, 0,
                                                    vtxStride * numVertices,
                                                    GL_MAP_WRITE_BIT |
                                                    GL_MAP_INVALIDATE_BUFFER_BIT);

        if (vtxMappedBuf == nullptr) {
            LOGE("Error mapping vertex buffer object.")
            return;
        }

        // Copy the data into the mapped buffer
        memcpy(vtxMappedBuf, vtxBuf, vtxStride * numVertices);

        // Unmap the buffer
        // glUnmapBuffer命令取消之前的缓冲区映射
        if (glUnmapBuffer(GL_ARRAY_BUFFER) == GL_FALSE) {
            LOGE("Error unmapping array buffer object.")
            return;
        }

        //============================================================================================//

        // Map the index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLushort) * numIndices, nullptr,
                     GL_STATIC_DRAW);

        idxMappedBuf = (GLushort *) glMapBufferRange(GL_ELEMENT_ARRAY_BUFFER, 0,
                                                     sizeof(GLushort) * numIndices,
                                                     GL_MAP_WRITE_BIT |
                                                     GL_MAP_INVALIDATE_BUFFER_BIT);

        if (idxMappedBuf == nullptr) {
            LOGE("Error mapping element array buffer object.")
            return;
        }

        // Copy the data into the mapped buffer
        memcpy(idxMappedBuf, indices, sizeof(GLushort) * numIndices);

        // Unmap the buffer
        if (glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER) == GL_FALSE) {
            LOGE("Error unmapping element array buffer object.")
            return;
        }
    }

    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glVertexAttribPointer(VERTEX_POS_INDX,
                          VERTEX_POS_SIZE,
                          GL_FLOAT, GL_FALSE, vtxStride,
                          (const void *) offset);


    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);
    offset += VERTEX_POS_SIZE * sizeof(GLfloat);
    glVertexAttribPointer(VERTEX_COLOR_INDX,
                          VERTEX_COLOR_SIZE,
                          GL_FLOAT, GL_FALSE, vtxStride,
                          (const void *) offset);

    glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, nullptr);

    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void NativeTriangleMapBuffers::Shutdown() {
    // Delete program object
    GLUtils::DeleteProgram(m_ProgramObj);

    glDeleteBuffers(2, &vboIds[0]);
}

