//
// Created by OuyangPeng on 2021/12/15.
//

// 参考自博客：
// 不瞒你说，我被这个特效感动哭了（OpenGL ES 特效） https://blog.csdn.net/Kennethdroid/article/details/104536532
// 【ShaderToy】跳动的心❤️https://blog.csdn.net/candycat1992/article/details/44040273
// 心跳的原Shader地址【shadertoy】：https://www.shadertoy.com/view/XsfGRn
// 心脏线的定义：https://baike.baidu.com/item/%E5%BF%83%E8%84%8F%E7%BA%BF/10323843?fr=aladdin

#include "BeatingHeartSample.h"

BeatingHeartSample::BeatingHeartSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

void BeatingHeartSample::create() {
    // 编译链接用于普通渲染的着色器程序
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_beating_heart.glsl");
    // 用于普通渲染的片段着色器脚本，简单纹理映射
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_toy_beating_heart.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (mProgram == GL_NONE) {
        LOGE("CoordSystemSample::Init mProgram == GL_NONE")
        return;
    }

    m_MVPMatLoc = glGetUniformLocation(mProgram, "u_MVPMatrix");
    m_SizeLoc = glGetUniformLocation(mProgram, "u_screenSize");
    m_TimeLoc = glGetUniformLocation(mProgram, "u_time");

    GLfloat verticesCoords[] = {
            -1.0f,  1.0f, 0.0f,  // Position 0
            -1.0f, -1.0f, 0.0f,  // Position 1
            1.0f,  -1.0f, 0.0f,  // Position 2
            1.0f,   1.0f, 0.0f,  // Position 3
    };

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(2, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);


    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[1]);
    glBindVertexArray(GL_NONE);

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void BeatingHeartSample::draw() {
    LOGD("BeatingHeartSample::Draw()")

    if(mProgram == GL_NONE) return;

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the program object
    glUseProgram (mProgram);

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float)mWidth / (float)mHeight);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    auto time = static_cast<float>(fmod(GetSysCurrentTime(), 2000) / 2000);
    LOGD("BeatingHeartSample::Draw() time=%f",time)
    // 控制输入时间周期为 2000ms
    glUniform1f(m_TimeLoc, time);
    // 输入屏幕的尺寸
    glUniform2f(m_SizeLoc, mWidth, mHeight);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void BeatingHeartSample::shutdown() {
    if (mProgram)
    {
        glDeleteProgram(mProgram);
        glDeleteBuffers(2, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
    }
}


void
BeatingHeartSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const {
    LOGD("BeatingHeartSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    mvpMatrix = Projection * View * Model;
}
