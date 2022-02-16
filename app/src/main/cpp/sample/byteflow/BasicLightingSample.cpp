//
// Created by OuyangPeng on 2021/12/10 0010.
//

// 理论知识可以参考：
// https://blog.csdn.net/Kennethdroid/article/details/101220947
// https://blog.csdn.net/Kennethdroid/article/details/101709694
// https://ouyangpeng.blog.csdn.net/article/details/121867616
// https://learnopengl-cn.github.io/02%20Lighting/02%20Basic%20Lighting/

#include "BasicLightingSample.h"

BasicLightingSample::BasicLightingSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;
    m_ModelMatrixLoc = GL_NONE;
    m_LightPosLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_ModelMatrix = glm::mat4(0.0f);
}

BasicLightingSample::~BasicLightingSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void BasicLightingSample::Create() {
    //Create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);


    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_basic_light.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_basic_light.glsl");
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (m_ProgramObj == GL_NONE) {
        LOGE("BasicLightingSample::Init m_ProgramObj == GL_NONE")
        return;
    }
    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_ModelMatrixLoc = glGetUniformLocation(m_ProgramObj, "u_ModelMatrix");
    m_LightPosLoc = glGetUniformLocation(m_ProgramObj, "lightPos");
    m_LightColorLoc = glGetUniformLocation(m_ProgramObj, "lightColor");
    m_ViewPosLoc = glGetUniformLocation(m_ProgramObj, "viewPos");

    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(1, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(basicLightVertices), basicLightVertices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);
    GO_CHECK_GL_ERROR()
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (const void *) (3 * sizeof(GLfloat)));
    GO_CHECK_GL_ERROR()
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (const void *) (5 * sizeof(GLfloat)));
    GO_CHECK_GL_ERROR()
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindVertexArray(GL_NONE);

    GO_CHECK_GL_ERROR()

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void BasicLightingSample::LoadImage(NativeImage *pImage) {
    LOGD("BasicLightingSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void BasicLightingSample::Draw() {
    LOGD("BasicLightingSample::Draw()")

    if (m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) {
        LOGD("BasicLightingSample::Draw()  m_ProgramObj == GL_NONE || m_TextureId == GL_NONE")
        return;
    }
    // Use the program object
    glUseProgram(m_ProgramObj);

    // 如果这句话不写，直接会黑屏。当使用 GL_DEPTH_TEST的时候，要记得调用下面这句话，搭配使用。
    // 黑屏的Bug，参考博客
    // 【我的OpenGL学习进阶之旅】关于OpenGL ES 开启深度测试，直接黑屏的问题的解决方法
    // https://ouyangpeng.blog.csdn.net/article/details/121867616

    // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) m_Width / (float) m_Height);

    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniformMatrix4fv(m_ModelMatrixLoc, 1, GL_FALSE, &m_ModelMatrix[0][0]);

    glUniform3f(m_LightColorLoc, 1.0f, 1.0f, 1.0f);
    glUniform3f(m_LightPosLoc, -2.0f, 0.0f, 2.0f);
    glUniform3f(m_ViewPosLoc, -3.0f, 0.0f, 3.0f);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR()
}

void BasicLightingSample::Shutdown() {
    GLBaseSample::Shutdown();
    glDeleteBuffers(1, m_VboIds);
    glDeleteVertexArrays(1, &m_VaoId);
    glDeleteTextures(1, &m_TextureId);
}

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void BasicLightingSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGD("BasicLightingSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX,
         angleY, ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(-3, 0, 3), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleX, m_ScaleX));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    m_ModelMatrix = Model;

    mvpMatrix = Projection * View * Model;
}


void BasicLightingSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX,
                                                float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}