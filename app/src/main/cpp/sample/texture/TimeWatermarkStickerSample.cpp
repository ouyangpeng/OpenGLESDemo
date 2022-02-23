//
// Created by OuyangPeng on 2021/12/13.

#include <PointUtils.h>
#include "TimeWatermarkStickerSample.h"

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
        0, 1, 2,        // 第一个三角形
        0, 2, 3         // 第二个三角形
};

//左脸颊关键点
static float LeftCheekKeyPoint[] = {211, 363};
//右脸颊关键点
static float RightCheekPoint[] = {471, 365};
//下巴关键点
static float ChinKeyPoint[] = {336, 565};

TimeWatermarkStickerSample::TimeWatermarkStickerSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

TimeWatermarkStickerSample::~TimeWatermarkStickerSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
    NativeImageUtil::FreeNativeImage(&m_StickerRenderImage);
}

void TimeWatermarkStickerSample::Create() {
    GLUtils::printGLInfo();

    // 顶点着色器
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
    glGenTextures(1, &m_TextureId);
    glGenTextures(1, &m_StickerTextureId);
    for(int i = 0 ; i <= 1 ; i ++){
        // 将纹理 m_TextureId 绑定到类型 GL_TEXTURE_2D 纹理
        glBindTexture(GL_TEXTURE_2D, i == 0 ? m_TextureId : m_StickerTextureId);
        //设置纹理 S 轴（横轴）的拉伸方式为截取
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        //设置纹理 T 轴（纵轴）的拉伸方式为截取
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //设置纹理采样方式
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }
    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void TimeWatermarkStickerSample::LoadImage(NativeImage *pImage) {
    LOGD("SkyBoxSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void TimeWatermarkStickerSample::LoadMultiImageWithIndex(int index, NativeImage *pImage) {
    GLBaseSample::LoadMultiImageWithIndex(index, pImage);
    LOGD("TimeWatermarkStickerSample::LoadImage index=%d, pImage = %p", index, pImage->ppPlane[0])
    if (pImage && index == 0) {
        m_StickerRenderImage.width = pImage->width;
        m_StickerRenderImage.height = pImage->height;
        m_StickerRenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_StickerRenderImage);
    }
}

void TimeWatermarkStickerSample::Draw() {
    LOGD("TimeWatermarkStickerSample::Draw()")

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
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_StickerTextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_StickerRenderImage.width,
                 m_StickerRenderImage.height,
                 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_StickerRenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);
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
    glBindTexture(GL_TEXTURE_2D, m_StickerTextureId);
    glUniform1i(m_SamplerLoc, 0);


    float scaleX=(RightCheekPoint[0] - LeftCheekKeyPoint[0]) / m_RenderImage.width;
    LOGD("TimeWatermarkStickerSample::Draw() scaleX=%f ", scaleX)

    float centerPointX = (RightCheekPoint[0] + LeftCheekKeyPoint[0]) / 2 / m_RenderImage.width;
    float centerPointY = (RightCheekPoint[1] + LeftCheekKeyPoint[1]) / 2 / m_RenderImage.height;
    float targetPointY = centerPointY - (ChinKeyPoint[1] - centerPointY ) * 1.2 ;
    float offsetTargetPointY =  (m_RenderImage.height / 2 - targetPointY) / (m_RenderImage.height / 2);
    LOGD("TimeWatermarkStickerSample::Draw()  centerPointX=%f , centerPointY = %f , "
         "targetPointY = %f, offsetTargetPointY = %f", centerPointX,centerPointY,targetPointY,offsetTargetPointY)

    //贴纸位移一点距离
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, scaleX,
                    glm::vec3(0.0f,offsetTargetPointY,0.0f),ratio);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    GO_CHECK_GL_ERROR()

    glBindVertexArray(0);

    // 禁用 OpenGL ES 混合使用
    glDisable(GL_BLEND);
}

void TimeWatermarkStickerSample::Shutdown() {
    GLBaseSample::Shutdown();
    glDeleteBuffers(3, m_VboIds);
    glDeleteVertexArrays(1, &m_VaoId);
    if (m_TextureId) {
        // 删除纹理
        glDeleteTextures(1, &m_TextureId);
        m_TextureId = GL_NONE;
    }
    if (m_StickerTextureId) {
        // 删除纹理
        glDeleteTextures(1, &m_StickerTextureId);
        m_StickerTextureId = GL_NONE;
    }
    m_ProgramObj = GL_NONE;
}

void
TimeWatermarkStickerSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
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
void TimeWatermarkStickerSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY,
                                    float scale, glm::vec3 transVec3, float ratio) const {
    LOGD("TimeWatermarkStickerSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY,
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

