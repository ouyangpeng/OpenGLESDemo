//
// Created by OuyangPeng on 2021/12/30.
//

#include "AirHockeySample.h"

// TABLE相关
#define TABLE_VERTEX_POS_INDX                  0
#define TABLE_TEXTURE_COORDINATES_POS_INDX     1

#define TABLE_POSITION_COMPONENT_COUNT                2       // x, y
#define TABLE_TEXTURE_COORDINATES_COMPONENT_COUNT     2       // s, t

static GLfloat tableVertices[] = {
        // Order of coordinates: X, Y,  S, T
        // Triangle Fan
        0.0f, 0.0f, 0.5f, 0.5f,
        -0.5f, -0.8f, 0.0f, 0.9f,
        0.5f, -0.8f, 1.0f, 0.9f,
        0.5f, 0.8f, 1.0f, 0.1f,
        -0.5f, 0.8f, 0.0f, 0.1f,
        -0.5f, -0.8f, 0.0f, 0.9f
};


// MALLET相关
#define MALLET_VERTEX_POS_INDX                  2
#define MALLET_VERTEX_COLOR_INDX                3

#define MALLET_POSITION_COMPONENT_COUNT         2       // x, y
#define MALLET_COLOR_COMPONENT_COUNT            3       // r,g,b

static GLfloat malletVertices[] = {
        // Order of coordinates: X, Y,      R, G, B
        0.0f, -0.4f, 0.0f, 0.0f, 1.0f,
        0.0f, 0.4f, 1.0f, 0.0f, 0.0f
};

void AirHockeySample::Create() {
    //==================================== TABLE =================================================//
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_air_hockey_table.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_air_hockey_table.glsl");

    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!m_ProgramObj) {
        LOGD("Could not Create m_ProgramObj")
        return;
    }
    m_TableMVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");

    GLint vtxStride = sizeof(GLfloat) *
                      (TABLE_POSITION_COMPONENT_COUNT + TABLE_TEXTURE_COORDINATES_COMPONENT_COUNT);
    glVertexAttribPointer(TABLE_VERTEX_POS_INDX, TABLE_POSITION_COMPONENT_COUNT,
                          GL_FLOAT, GL_FALSE,
                          vtxStride, tableVertices);

    glVertexAttribPointer(TABLE_TEXTURE_COORDINATES_POS_INDX,
                          TABLE_TEXTURE_COORDINATES_COMPONENT_COUNT,
                          GL_FLOAT, GL_FALSE,
                          vtxStride, tableVertices + TABLE_POSITION_COMPONENT_COUNT);
    glEnableVertexAttribArray(TABLE_VERTEX_POS_INDX);
    glEnableVertexAttribArray(TABLE_TEXTURE_COORDINATES_POS_INDX);

    // Create RGBA texture
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
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);


    //====================================  MALLET ===============================================//

    // 顶点着色器
    const char *MALLET_VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_air_hockey_mallet.glsl");
    // 片段着色器
    const char *MALLET_FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_air_hockey_mallet.glsl");
    m_MalletProgramObj = GLUtils::createProgram(&MALLET_VERTEX_SHADER, &MALLET_FRAGMENT_SHADER);
    if (!m_MalletProgramObj) {
        LOGD("Could not Create m_MalletProgramObj")
        return;
    }
    m_MalletMVPMatLoc = glGetUniformLocation(m_MalletProgramObj, "u_MVPMatrix");
    GLint malletVtxStride =
            sizeof(GLfloat) * (MALLET_POSITION_COMPONENT_COUNT + MALLET_COLOR_COMPONENT_COUNT);
    glVertexAttribPointer(MALLET_VERTEX_POS_INDX, MALLET_POSITION_COMPONENT_COUNT,
                          GL_FLOAT, GL_FALSE,
                          malletVtxStride, malletVertices);

    glVertexAttribPointer(MALLET_VERTEX_COLOR_INDX, MALLET_COLOR_COMPONENT_COUNT,
                          GL_FLOAT, GL_FALSE,
                          malletVtxStride, malletVertices + MALLET_POSITION_COMPONENT_COUNT);
    glEnableVertexAttribArray(MALLET_VERTEX_POS_INDX);
    glEnableVertexAttribArray(MALLET_VERTEX_COLOR_INDX);

    // 设置清除颜色为黑色
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void AirHockeySample::Change(int width, int height) {
    GLBaseSample::Change(width, height);
    aspectRatio = m_Width > m_Height
                  ? (float) m_Width / (float) m_Height
                  : (float) m_Height / (float) m_Width;
    LOGD("aspectRatio = %f", aspectRatio)

    // Projection matrix  投影矩阵
    // 用45度的视野创建一个透视投影。这个视锥体从z值为-1的位置开始，在z值为-10的位置结束
    glm::mat4 mProjection = glm::perspective(45.0f, aspectRatio, 1.0f, 10.f);

    // Model matrix
    // 单位矩阵
    glm::mat4 mModel = glm::mat4(1.0f);
    // 沿着Z轴平移 -2.5
    mModel = glm::translate(mModel, glm::vec3(0.0f, 0.0f, -2.5f));
    // 饶X轴旋转60度
    float angleX = -60.0f;
    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    mModel = glm::rotate(mModel, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    m_MVPMatrix = mProjection * mModel;
}

void AirHockeySample::Draw() {
    // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
//    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClear(GL_COLOR_BUFFER_BIT);

    //==================================== TABLE =================================================//
    glUseProgram(m_ProgramObj);
    // Assign the matrix
    glUniformMatrix4fv(m_TableMVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);
    // Draw the table.
    glDrawArrays(GL_TRIANGLE_FAN, 0, 6);

    glDisableVertexAttribArray(TABLE_VERTEX_POS_INDX);
    glDisableVertexAttribArray(TABLE_TEXTURE_COORDINATES_POS_INDX);

    //====================================  MALLET ===============================================//
    glUseProgram(m_MalletProgramObj);
    // Assign the matrix
    glUniformMatrix4fv(m_MalletMVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    // Draw the mallets.
    glDrawArrays(GL_POINTS, 0, 2);

    glDisableVertexAttribArray(MALLET_VERTEX_POS_INDX);
    glDisableVertexAttribArray(MALLET_VERTEX_COLOR_INDX);
}

void AirHockeySample::Shutdown() {
    // Delete program object
    GLUtils::DeleteProgram(m_ProgramObj);
    GLUtils::DeleteProgram(m_MalletProgramObj);
    glDeleteTextures(1, &m_TextureId);
}

void AirHockeySample::LoadImage(NativeImage *pImage) {
    LOGD("AirHockeySample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}
