//
// Created by OuyangPeng on 2021/12/21 0021.
// 参考博客
// OpenGL ES 多目标渲染（MRT） https://blog.csdn.net/Kennethdroid/article/details/108873665
// 参考  MRT.cpp
//

#include "MRTSample.h"

const GLenum attachments[ATTACHMENT_NUM] = {
        GL_COLOR_ATTACHMENT0,
        GL_COLOR_ATTACHMENT1,
        GL_COLOR_ATTACHMENT2,
        GL_COLOR_ATTACHMENT3
};

static GLfloat verticesCoords[] = {
        -1.0f, 1.0f, 0.0f,  // Position 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        1.0f, -1.0f, 0.0f,  // Position 2
        1.0f, 1.0f, 0.0f,  // Position 3
};

static GLfloat textureCoords[] = {
        0.0f, 0.0f,        // TexCoord 0
        0.0f, 1.0f,        // TexCoord 1
        1.0f, 1.0f,        // TexCoord 2
        1.0f, 0.0f         // TexCoord 3
};

static GLushort indices[] = {
        0, 1, 2,
        0, 2, 3
};

MRTSample::MRTSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_MRTProgramObj = GL_NONE;
}

MRTSample::~MRTSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void MRTSample::Create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_coord_system.glsl");

    // 片段着色器脚本
    const char *FRAGMENT_SHADER2 = GLUtils::openTextFile(
            "fragment/fragment_shader_mrt2.glsl");
    m_MRTProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER2);
    if (!m_MRTProgramObj) {
        LOGE("RotaryHeadSample::Init Create program fail")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(m_MRTProgramObj, "s_Texture");
    m_MVPMatLoc = glGetUniformLocation(m_MRTProgramObj, "u_MVPMatrix");

    // 片段着色器脚本
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_mrt3.glsl");

    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!m_ProgramObj) {
        LOGE("RotaryHeadSample::Init Create program fail")
        return;
    }

    //=========================================== VBO和VAO相关===========================================
    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesCoords), verticesCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(textureCoords), textureCoords, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);


    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    glBindVertexArray(GL_NONE);

    //=========================================== 纹理相关===========================================
    //Create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

    LOGD("MRTSample::Init InitFBO = %d", InitFBO())
}

void MRTSample::Draw() {
    LOGD("MRTSample::Draw()")
    if(m_MRTProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    //首先获取当前默认帧缓冲区的 id
    GLint defaultFrameBuffer = GL_NONE;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFrameBuffer);

    //绑定我们新创建的 FBO 渲染
    // FIRST: use MRTs to output four colors to four buffers
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    glViewport ( 0, 0, m_RenderImage.width, m_RenderImage.height);

    glClear(GL_COLOR_BUFFER_BIT);

    glDrawBuffers(ATTACHMENT_NUM, attachments);



    //使用渲染输出到 4 个纹理的着色器程序
    glUseProgram (m_MRTProgramObj);
    glBindVertexArray(m_VaoId);
    UpdateMVPMatrix(m_MVPMatrix, 180, m_AngleY, (float)m_Width / (float)m_Height);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

    //绑定默认帧缓冲区对象，绘制到屏幕上
    // Restore the default framebuffer
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultFrameBuffer);
    glViewport (0, 0, m_Width, m_Height);
    glClear(GL_COLOR_BUFFER_BIT);
    //渲染（采样） 4 个纹理的着色器程序
    glUseProgram (m_ProgramObj);
    UpdateMVPMatrix(m_MVPMatrix, 0, m_AngleY, (float)m_Width / (float)m_Height);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    //指定 4 个纹理作为输入
    for (int i = 0; i < ATTACHMENT_NUM; ++i)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_AttachTexIds[i]);
        char samplerName[64] = {0};
        sprintf(samplerName, "s_Texture%d", i);
        GLUtils::setInt(m_ProgramObj, samplerName, i);
    }
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void MRTSample::Shutdown() {
    if (m_ProgramObj)
    {
        glDeleteProgram(m_ProgramObj);
        glDeleteProgram(m_MRTProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        // Delete texture objects
        glDeleteTextures(1, &m_TextureId);
        // Delete fbo
        glDeleteFramebuffers(1,&m_FBO);
    }
}

void MRTSample::LoadImage(NativeImage *pImage) {
    LOGD("MRTSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage)
    {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void MRTSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void MRTSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const {
    LOGD("MRTSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

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

bool MRTSample::InitFBO() {
    GLint defaultFrameBuffer = GL_NONE;
    glGetIntegerv(GL_FRAMEBUFFER_BINDING, &defaultFrameBuffer);

    //生成帧缓冲区对象
    glGenFramebuffers(1, &m_FBO);
    // set the FBO for reading
    glBindFramebuffer(GL_FRAMEBUFFER, m_FBO);

    //生成 4 个纹理
    glGenTextures(ATTACHMENT_NUM, m_AttachTexIds);
    for (int i = 0; i < ATTACHMENT_NUM; ++i) {
        glBindTexture(GL_TEXTURE_2D, m_AttachTexIds[i]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        // Set the filtering mode
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        // 将相关纹理绑定到FBO
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, attachments[i], GL_TEXTURE_2D, m_AttachTexIds[i], 0);
    }
    // 为渲染指定颜色附着： 这里设置一个带有4个颜色输出的FBO
    glDrawBuffers(ATTACHMENT_NUM, attachments);

    if (GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus(GL_FRAMEBUFFER))
    {
        return false;
    }
    // Restore the original framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, defaultFrameBuffer);
    return true;
}
