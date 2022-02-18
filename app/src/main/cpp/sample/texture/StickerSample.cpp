//
// Created by OuyangPeng on 2021/12/13.

#include "StickerSample.h"

// 渲染坐标系或OpenGLES 坐标系
// 4 个纹理坐标对于的顶点坐标分别为
// V0（-1，0.5），V1（-1，-0.5），V2（1，-0.5），V3（1，0.5）
// 可以参考图片： https://img-blog.csdnimg.cn/2019072219143123.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0tlbm5ldGhkcm9pZA==,size_16,color_FFFFFF,t_70#pic_center
static GLfloat verticesCoords[] = {
        -1.0f, 1.0f, 0.0f,  // Position 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        1.0f, -1.0f, 0.0f,  // Position 2
        1.0f, 1.0f, 0.0f,  // Position 3
};

// 纹理坐标系
// 4 个纹理坐标分别为
// T0（0，0），T1（0，1），T2（1，1），T3（1，0）。
// 可以参考图片： https://img-blog.csdnimg.cn/20190722191345500.png?x-oss-process=image/watermark,type_ZmFuZ3poZW5naGVpdGk,shadow_10,text_aHR0cHM6Ly9ibG9nLmNzZG4ubmV0L0tlbm5ldGhkcm9pZA==,size_16,color_FFFFFF,t_70#pic_center
static GLfloat textureCoords[] = {
        0.0f, 0.0f,        // TexCoord 0
        0.0f, 1.0f,        // TexCoord 1
        1.0f, 1.0f,        // TexCoord 2
        1.0f, 0.0f         // TexCoord 3
};

// 由于 OpenGLES 绘制是以三角形为单位的，设置绘制的 2 个三角形为 V0V1V2 和 V0V2V3 。
// 当我们调整纹理坐标的顺序顶点坐标顺序不变，如 T0T1T2T3 -> T1T2T3T0 ，绘制后将得到一个顺时针旋转 90 度的纹理贴图。
// 所以调整纹理坐标和顶点坐标的对应关系可以实现纹理图简单的旋转。

// 注意索引从0开始!
static GLushort indices[] = {
        0, 1, 2,        // 第一个三角形
        0, 2, 3         // 第二个三角形
};

StickerSample::StickerSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

StickerSample::~StickerSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);

    for (auto &m_StickerRenderImage : m_StickerRenderImages) {
        NativeImageUtil::FreeNativeImage(&m_StickerRenderImage);
    }
}

void StickerSample::Create() {
    GLUtils::printGLInfo();

    // 顶点着色器

//    VERTEX_SHADER = GLUtils::openTextFile(
//            "vertex/vertex_shader_texture_map.glsl");
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_blending.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!m_ProgramObj) {
        LOGD("Could not Create program")
        return;
    }
    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    GO_CHECK_GL_ERROR()
    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    GO_CHECK_GL_ERROR()

    // ======================= 设置 VAO VBO===================================

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
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

    glBindVertexArray(GL_NONE);

    // ======================= 设置纹理===================================

    // 原始纹理
    glGenTextures(1, &m_TextureId);
    // 将纹理 m_TextureId 绑定到类型 GL_TEXTURE_2D 纹理
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    //设置纹理 S 轴（横轴）的拉伸方式为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //设置纹理 T 轴（纵轴）的拉伸方式为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //设置纹理采样方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 贴图纹理
    glGenTextures(RENDER_IMG_NUM, m_StickerTextureIds);
    for (unsigned int m_StickerTextureId : m_StickerTextureIds) {
        glBindTexture(GL_TEXTURE_2D, m_StickerTextureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    windowsTrans.emplace_back(-0.5f, 0.0f, -0.48f);
    windowsTrans.emplace_back(0.5f, 0.4f, 0.51f);
    windowsTrans.emplace_back(-0.3f, 0.0f, -2.3f);
    windowsTrans.emplace_back(0.5f, 0.8f, -0.6f);

    for (auto &windowsTran : windowsTrans) {
        GLfloat distance = std::sqrt(std::pow(windowsTran.x - 0.5f, 2.0f) +
                                     std::pow(windowsTran.y - 1.0f, 2.0f) +
                                     std::pow(windowsTran.z - 3.0f, 2.0f));
        sorted[distance] = windowsTran;
    }

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void StickerSample::LoadImage(NativeImage *pImage) {
    LOGD("SkyBoxSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void StickerSample::LoadMultiImageWithIndex(int index, NativeImage *pImage) {
    GLBaseSample::LoadMultiImageWithIndex(index, pImage);
    LOGD("StickerSample::LoadImage index=%d, pImage = %p", index, pImage->ppPlane[0])
    if (pImage) {
        m_StickerRenderImages[index].width = pImage->width;
        m_StickerRenderImages[index].height = pImage->height;
        m_StickerRenderImages[index].format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_StickerRenderImages[index]);
    }
}

void StickerSample::Draw() {
    LOGD("StickerSample::Draw()")

    if (m_ProgramObj == GL_NONE) return;

    // 清空缓冲区
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    // 启用 OpenGL ES 混合使用
    glEnable(GL_BLEND);
    // 设置混合因子 混合公式变成了 源颜色向量 × alpha + 目标颜色向量 × （1- alpha）
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


    // ================================= 更新纹理 =================================
    // 更新原始纹理
    glActiveTexture(GL_TEXTURE0);
    // 将纹理绑定到当前活动单元
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    //加载 RGBA 格式的图像数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 更新贴纸纹理纹理
    for (int i = 0; i < RENDER_IMG_NUM; ++i) {
        glActiveTexture(GL_TEXTURE1 + i);
        glBindTexture(GL_TEXTURE_2D, m_StickerTextureIds[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_StickerRenderImages[i].width,
                     m_StickerRenderImages[i].height,
                     0, GL_RGBA,
                     GL_UNSIGNED_BYTE, m_StickerRenderImages[i].ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }
    // ================================= 更新纹理 =================================

    // Use the program object
    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    // 旋转角度变换，更新变换矩阵
    float ratio = (float) m_Width / (float) m_Height;
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, 1.0, glm::vec3(0.0f, 0.0f, 0.0f), ratio);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);
    // 绘制原始纹理
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    GO_CHECK_GL_ERROR()

    // 绘制贴纸纹理纹理
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_StickerTextureIds[0]);
    glUniform1i(m_SamplerLoc, 0);
    //容器 sorted 根据窗户距观察者的距离进行排序
    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
        UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, 0.1, it->second, ratio);
        glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    }

    glBindVertexArray(0);

    // 禁用 OpenGL ES 混合使用
    glDisable(GL_BLEND);
}

void StickerSample::Shutdown() {
    GLBaseSample::Shutdown();
    glDeleteBuffers(3, m_VboIds);
    glDeleteVertexArrays(1, &m_VaoId);
    if (m_TextureId) {
        // 删除纹理
        glDeleteTextures(1, &m_TextureId);
        m_TextureId = GL_NONE;
    }
    glDeleteTextures(3, m_StickerTextureIds);
    m_ProgramObj = GL_NONE;
}

void
StickerSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void StickerSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY,
                                    float scale, glm::vec3 transVec3, float ratio) const {
    LOGD("StickerSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY,
         ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 mProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 mProjection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 mProjection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    // 参考链接 https://learnopengl-cn.github.io/01%20Getting%20started/09%20Camera/ 去理解摄像机
    glm::mat4 mView = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 mModel = glm::mat4(1.0f);
    mModel = glm::scale(mModel, glm::vec3(scale, scale, scale));
    mModel = glm::rotate(mModel, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    mModel = glm::rotate(mModel, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    mModel = glm::translate(mModel, transVec3);

    mvpMatrix = mProjection * mView * mModel;
}

