//
// Created by OuyangPeng on 2021/12/13.

// Native5Lesson 中也运用了混合 技术

// 参考博客 :
// NDK OpenGL ES 3.0 开发（十二）：混合
// https://blog.csdn.net/Kennethdroid/article/details/102630858

// Android OpenGLES2.0（十八）——轻松搞定Blend颜色混合
// https://blog.csdn.net/junzia/article/details/76580379

// OpenGL ES 混合
// OpenGL ES 混合本质上是将 2 个片元的颜色进行调和，产生一个新的颜色。
// OpenGL ES 混合发生在片元通过各项测试之后，
// 准备进入帧缓冲区的片元和原有的片元按照特定比例加权计算出最终片元的颜色值，
// 不再是新（源）片元直接覆盖缓冲区中的（目标）片元。

#include "BlendingSample.h"

BlendingSample::BlendingSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;
}

BlendingSample::~BlendingSample() {
    for (auto &m_RenderImage : m_RenderImages) {
        NativeImageUtil::FreeNativeImage(&m_RenderImage);
    }
}

void BlendingSample::create() {
    //create RGBA textures
    glGenTextures(RENDER_IMG_NUM, m_TextureIds);
    for (unsigned int m_TextureId : m_TextureIds) {
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_blending.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!mProgram) {
        LOGD("Could not create program")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(mProgram, "s_TextureMap");
    GO_CHECK_GL_ERROR()
    m_MVPMatLoc = glGetUniformLocation(mProgram, "u_MVPMatrix");
    GO_CHECK_GL_ERROR()


    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(flatVertices), flatVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertices), windowVertices, GL_STATIC_DRAW);

    // Generate VAO Ids
    glGenVertexArrays(3, m_VaoIds);

    glBindVertexArray(m_VaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (const void *) (3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    glBindVertexArray(m_VaoIds[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (const void *) (3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    glBindVertexArray(m_VaoIds[2]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (const void *) (3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    //upload RGBA image data
    for (int i = 0; i < RENDER_IMG_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImages[i].width, m_RenderImages[i].height,
                     0, GL_RGBA,
                     GL_UNSIGNED_BYTE, m_RenderImages[i].ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    windowsTrans.emplace_back(-1.5f, 0.0f, -0.48f);
    windowsTrans.emplace_back(1.5f, 0.0f, 0.51f);
    windowsTrans.emplace_back(0.0f, 0.0f, 0.7f);
    windowsTrans.emplace_back(-0.3f, 0.0f, -2.3f);
    windowsTrans.emplace_back(0.5f, 0.0f, -0.6f);

    for (auto &windowsTran : windowsTrans) {
        GLfloat distance = std::sqrt(std::pow(windowsTran.x - 0.5f, 2.0f) +
                                     std::pow(windowsTran.y - 1.0f, 2.0f) +
                                     std::pow(windowsTran.z - 3.0f, 2.0f));
        sorted[distance] = windowsTran;
    }
}

void BlendingSample::LoadMultiImageWithIndex(int index, NativeImage *pImage) {
    GLBaseSample::LoadMultiImageWithIndex(index, pImage);
    LOGD("BlendingSample::LoadImage index=%d, pImage = %p", index, pImage->ppPlane[0])
    if (pImage) {
        m_RenderImages[index].width = pImage->width;
        m_RenderImages[index].height = pImage->height;
        m_RenderImages[index].format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImages[index]);
    }
}

void BlendingSample::draw() {
    LOGD("BlendingSample::Draw()")

    if (mProgram == GL_NONE) return;

    glUseProgram(mProgram);

    float ratio = (float) mWidth / (float) mHeight;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 启用 OpenGL ES 深度测试
    glEnable(GL_DEPTH_TEST);
    // 启用 OpenGL ES 混合使用
    glEnable(GL_BLEND);

    // 设置混合因子
    // 设置混合的方式，其中 sfactor 表示源因子，dfactor 表示目标因子。
    // GL_SRC_ALPHA 表示源因子取值为源颜色的 alpha
    // GL_ONE_MINUS_SRC_ALPHA 表示目标因子取值为 1- alpha（源颜色的 alpha）

    // 操作符默认为 GL_FUNC_ADD ，即加权相加。
    // 混合公式变成了 源颜色向量 × alpha + 目标颜色向量 × （1- alpha）
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //绘制箱子（不透明）
    glBindVertexArray(m_VaoIds[0]);
    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
    glUniform1i(m_SamplerLoc, 0);

    UpdateMatrix(m_MVPMatrix, 0, 0, 1.0, glm::vec3(-1.0f, 0.0f, -1.0f), ratio);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    UpdateMatrix(m_MVPMatrix, 0, 0, 1.0, glm::vec3(2.0f, 0.0f, 0.0f), ratio);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(0);


    //绘制地板（不透明）
    glBindVertexArray(m_VaoIds[1]);
    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[1]);
    glUniform1i(m_SamplerLoc, 0);

    UpdateMatrix(m_MVPMatrix, 0, 0, 1.0, glm::vec3(0.0f, 0.0f, 0.0f), ratio);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(0);


    //绘制窗户（透明）
    glBindVertexArray(m_VaoIds[2]);
    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[2]);
    glUniform1i(m_SamplerLoc, 0);


    //容器 sorted 根据窗户距观察者的距离进行排序
    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
        UpdateMatrix(m_MVPMatrix, 0, 0, 1.0, it->second, ratio);
        glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    GO_CHECK_GL_ERROR()
    glBindVertexArray(0);

    // 禁用 OpenGL ES 深度测试
    glDisable(GL_DEPTH_TEST);
    // 禁用 OpenGL ES 混合使用
    glDisable(GL_BLEND);
}

void BlendingSample::shutdown() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(3, m_VaoIds);
        glDeleteTextures(3, m_TextureIds);
        mProgram = GL_NONE;
    }
}

void
BlendingSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
}

void BlendingSample::UpdateMatrix(glm::mat4 &mvpMatrix, int angleXRotate, int angleYRotate,
                                  float scale, glm::vec3 transVec3, float ratio) {
    LOGD("BlendingSample::UpdateMatrix angleX = %d, angleY = %d, ratio = %f", angleXRotate,
         angleYRotate, ratio)
    angleXRotate = angleXRotate % 360;
    angleYRotate = angleYRotate % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleXRotate);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleYRotate);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0.5, 1, 3), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scale, scale, scale));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, transVec3);

    mvpMatrix = Projection * View * Model;
}
