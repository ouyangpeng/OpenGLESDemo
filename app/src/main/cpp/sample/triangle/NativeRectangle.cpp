#include "NativeRectangle.h"

// set up vertex data (and buffer(s)) and configure vertex attributes
// ------------------------------------------------------------------
static GLfloat vVertices[] = {
        0.5f,  0.5f, 0.0f,  // top right
        0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f,  // bottom left
        -0.5f,  0.5f, 0.0f   // top left
};

static unsigned int indices[] = {  // note that we start from 0!
        0, 1, 3,  // first Triangle
        1, 2, 3   // second Triangle
};

void NativeRectangle::Create() {
    GLUtils::printGLInfo();

    // Main Program
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_hello_triangle.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_hello_triangle.glsl");

    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!m_ProgramObj) {
        LOGD("Could not Create program")
        return;
    }

    //  Generate VBO Ids and load the VBOs with data
    // 创建 2 个 VBO（EBO 实际上跟 VBO 一样，只是按照用途的另一种称呼）
    glGenBuffers(2, vboIds);

    // 绑定第一个 VBO，拷贝顶点数组到显存
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vVertices), vVertices, GL_STATIC_DRAW);

    // 绑定第二个 VBO（EBO），拷贝图元索引数据到显存
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 创建一个VAO
    glGenVertexArrays(1, &vaoId);

    // Bind the VAO and then setup the vertex attributes
    glBindVertexArray(vaoId);

    // 把顶点数组复制到缓冲中供OpenGL使用
    glBindBuffer(GL_ARRAY_BUFFER, vboIds[0]);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vboIds[1]);

    glEnableVertexAttribArray(VERTEX_POS_INDX);
    // 设置顶点属性指针
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), nullptr);

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void NativeRectangle::Draw() {
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(m_ProgramObj);

    // Bind the VAO
    glBindVertexArray(vaoId);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);

    glDisableVertexAttribArray(0);
}

void NativeRectangle::Shutdown() {
    GLBaseSample::Shutdown();

    glDeleteBuffers(2, &vboIds[0]);
    glDeleteVertexArrays(1, &vaoId);
}
