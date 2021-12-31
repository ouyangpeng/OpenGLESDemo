//
// Created by OuyangPeng on 2021/12/30.
//

#include "AirHockeySample.h"

#define VERTEX_POS_INDX       0

static GLfloat tableVerticesWithTriangles[] = {
        // Triangle Fan 三角形扇  其实绘制了4个三角形
        // 123,134,145,152
        0.0f, 0.0f,     // 第1个顶点：以中心顶点作为起始点
        -0.5f, -0.5f,   // 第2个顶点
        0.5f, -0.5f,    // 第3个顶点
        0.5f, 0.5f,     // 第4个顶点
        -0.5f, 0.5f,    // 第5个顶点
        -0.5f, -0.5f,   // 重复第2个点

        // Line 1
        -0.5f, 0.0f,
        0.5f, 0.0f,

        // Mallets
        0.0f, -0.25f,
        0.0f, 0.25f
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

    // Get the uniform locations
    uColorLocation = glGetUniformLocation(m_ProgramObj, "u_Color");

    glVertexAttribPointer(VERTEX_POS_INDX, 2, GL_FLOAT, GL_FALSE, 0, tableVerticesWithTriangles);

    glEnableVertexAttribArray(VERTEX_POS_INDX);

    // 设置清除颜色为黑色
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void AirHockeySample::Draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_ProgramObj);

    // Draw the table.
    glUniform4f(uColorLocation, 1.0f, 1.0f, 1.0f, 1.0f);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    // Draw the center dividing line.
    glUniform4f(uColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 6, 2);

    // Draw the first mallet blue.
    glUniform4f(uColorLocation, 0.0f, 0.0f, 1.0f, 1.0f);
    glDrawArrays(GL_POINTS, 8, 1);

    // Draw the second mallet red.
    glUniform4f(uColorLocation, 1.0f, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_POINTS, 9, 1);
}

void AirHockeySample::Shutdown() {
    // Delete program object
    GLUtils::DeleteProgram(m_ProgramObj);
}
