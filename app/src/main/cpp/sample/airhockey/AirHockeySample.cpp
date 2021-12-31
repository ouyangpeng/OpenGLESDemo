//
// Created by OuyangPeng on 2021/12/30.
//

#include "AirHockeySample.h"

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

#define VERTEX_POS_SIZE       2 // x, y
#define VERTEX_COLOR_SIZE     3 // r, g, b

// 10 vertices, with (x,y,z) ,(r, g, b, a)  per-vertex
static GLfloat tableVerticesWithTriangles[ 10 * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE)] = {
        // Order of coordinates: X, Y, R, G, B

        // Triangle Fan 三角形扇  其实绘制了4个三角形
        // 123,134,145,152
        // 第1个顶点：以中心顶点作为起始点
        0.0f, 0.0f,     1.0f, 1.0f, 1.0f,
        // 第2个顶点
        -0.5f, -0.5f,   0.7f, 0.7f, 0.7f,
        // 第3个顶点
        0.5f, -0.5f,    0.7f, 0.7f, 0.7f,
        // 第4个顶点
        0.5f, 0.5f,     0.7f, 0.7f, 0.7f,
        // 第5个顶点
        -0.5f, 0.5f,    0.7f, 0.7f, 0.7f,
        // 重复第2个点
        -0.5f, -0.5f,   0.7f, 0.7f, 0.7f,

        // Line 1
        -0.5f, 0.0f,    1.0f, 0.0f, 0.0f,
        0.5f, 0.0f,     1.0f, 0.0f, 0.0f,

        // Mallets
        0.0f, -0.25f,   0.0f, 0.0f, 1.0f,
        0.0f, 0.25f,    1.0f, 0.0f, 0.0f
};

void AirHockeySample::Create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_air_hockey.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_air_hockey.glsl");

    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!m_ProgramObj) {
        LOGD("Could not Create program")
        return;
    }

    glUseProgram(m_ProgramObj);

    GLint vtxStride = sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE);

    glVertexAttribPointer(VERTEX_POS_INDX, VERTEX_POS_SIZE,
                          GL_FLOAT, GL_FALSE,
                          vtxStride,tableVerticesWithTriangles);

    glVertexAttribPointer(VERTEX_COLOR_INDX, VERTEX_COLOR_SIZE,
                          GL_FLOAT, GL_FALSE,
                          vtxStride,tableVerticesWithTriangles+ VERTEX_POS_SIZE);

    glEnableVertexAttribArray(VERTEX_POS_INDX);
    glEnableVertexAttribArray(VERTEX_COLOR_INDX);

    // 设置清除颜色为黑色
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void AirHockeySample::Draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    // Draw the table.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    // Draw the center dividing line.
    glDrawArrays(GL_LINES, 6, 2);

    // Draw the first mallet.
    glDrawArrays(GL_POINTS, 8, 1);

    // Draw the second mallet.
    glDrawArrays(GL_POINTS, 9, 1);

    glDisableVertexAttribArray(VERTEX_POS_INDX);
    glDisableVertexAttribArray(VERTEX_COLOR_INDX);
}

void AirHockeySample::Shutdown() {
    // Delete program object
    GLUtils::DeleteProgram(m_ProgramObj);
}
