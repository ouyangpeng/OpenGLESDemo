//
// Created by OuyangPeng on 2021/12/21.
// 参考博客
// Android OpenGL ES 实现 3D 阿凡达（Avatar）效果
// https://blog.csdn.net/Kennethdroid/article/details/106423475

// 纹理映射，绘制图像；
// 图像坐标变换，坐标系统矩阵变换实现图像的位移和缩放；
// 监听手机传感器数据，利用传感器数据控制图像位移。
//

#include "AvatarSample.h"

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

AvatarSample::AvatarSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_TransX = 0.0f;
    m_TransY = 0.0f;

    memset(m_TextureIds, 0, sizeof(m_TextureIds));
    memset(m_RenderImages, 0, sizeof(m_RenderImages));

    m_FrameIndex = 0;
}

AvatarSample::~AvatarSample() {
    for (int i = 0; i < RENDER_IMG_NUM; ++i) {
        NativeImageUtil::FreeNativeImage(&m_RenderImages[i]);
    }
}

void AvatarSample::Create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_blending.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_avatar.glsl");
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (m_ProgramObj == GL_NONE) {
        LOGE("BasicLightingSample::Init m_ProgramObj == GL_NONE")
        return;
    }

    // 片段着色器
    const char *fBlurShader = GLUtils::openTextFile(
            "fragment/fragment_shader_avatar_blur.glsl");
    m_BlurProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &fBlurShader);
    if (m_BlurProgramObj == GL_NONE) {
        LOGE("BasicLightingSample::Init m_ProgramObj == GL_NONE")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");

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
    glGenTextures(RENDER_IMG_NUM, m_TextureIds);
    for (int i = 0; i < RENDER_IMG_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImages[i].width, m_RenderImages[i].height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImages[i].ppPlane[0]);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}


void AvatarSample::Draw() {
    LOGD("AvatarSample::Draw()")

    if (m_ProgramObj == GL_NONE) return;
    float dScaleLevel = m_FrameIndex % 200 * 1.0f / 1000 + 0.0001f;


    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_ProgramObj);
    glBindVertexArray(m_VaoId);

    // 素材图里的人像层和外层是部分区域透明的 PNG 图，而背景层是每个像素透明度均为最大值的 JPG 图。
    // 所以，在绘制 3 张图时，要先绘制背景层，然后依次是人像层、外层，为了防止遮挡，
    // 在绘制人像层、外层时需要利用片段着色器来丢弃透明度比较低的片元，这种操作俗称 alpha 测试。

    //===================================== 1. 背景层的绘制 =====================================
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
    glUniform1i(m_SamplerLoc, 0);

    //缩放控制
    auto scaleLevel = static_cast<float>(1.0f + dScaleLevel * pow(-1, m_FrameIndex / 200));
    scaleLevel = scaleLevel < 1.0 ? scaleLevel + 0.2f : scaleLevel;
    m_ScaleY = m_ScaleX = scaleLevel + 0.4f;
    GLUtils::setVec2(m_ProgramObj, "u_texSize",
                     glm::vec2(m_RenderImages[0].width, m_RenderImages[0].height));
    GLUtils::setFloat(m_ProgramObj, "u_needRotate", 1.0f); // u_needRotate == 1 开启形变
    GLUtils::setFloat(m_ProgramObj, "u_rotateAngle", m_TransX * 1.5f);

    //设置变换矩阵 m_TransX m_TransY 为 x,y 方向的重力传感器数据
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, m_TransX / 2, m_TransY / 2,
                    (float) m_Width / (float) m_Height);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    //绘制
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

    //===================================== 2. 人像层的绘制 =====================================
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[1]);
    glUniform1i(m_SamplerLoc, 1);

    //缩放控制 pow(-1, m_FrameIndex / 200 + 1) 控制人像层的缩放大小跟背景层和外层相反
    scaleLevel = static_cast<float>(1.0f + dScaleLevel * pow(-1, m_FrameIndex / 200 + 1));
    scaleLevel = scaleLevel < 1.0 ? scaleLevel + 0.2f : scaleLevel;
    m_ScaleY = m_ScaleX = scaleLevel + 0.4f;
    LOGD("AvatarSample::Draw() scaleLevel=%f", scaleLevel)

    //设置变换矩阵 m_TransX m_TransY 为 x,y 方向的重力传感器数据
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, m_TransX * 1.2f, m_TransY * 1.2f,
                    (float) m_Width / (float) m_Height);
    GLUtils::setVec2(m_ProgramObj, "u_texSize",
                     glm::vec2(m_RenderImages[0].width, m_RenderImages[0].height));
    GLUtils::setFloat(m_ProgramObj, "u_needRotate", 0.0f);// u_needRotate == 0 关闭形变
    GLUtils::setFloat(m_ProgramObj, "u_rotateAngle", m_TransX / 20);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    //绘制
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

    //===================================== 3. 外层的绘制 =====================================
    //开启混合
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_COLOR, GL_ONE_MINUS_SRC_ALPHA);
    //使用新的着色器程序
    glUseProgram(m_BlurProgramObj);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[2]);
    GLUtils::setFloat(m_BlurProgramObj, "s_TextureMap", 0);

    // 缩放控制
    scaleLevel = static_cast<float>(1.0f + dScaleLevel * pow(-1, m_FrameIndex / 200));
    scaleLevel = scaleLevel < 1.0 ? scaleLevel + 0.2f : scaleLevel;
    m_ScaleY = m_ScaleX = scaleLevel + 0.8f;

    //设置变换矩阵 m_TransX m_TransY 为 x,y 方向的重力传感器数据
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, m_TransX * 2.5f, m_TransY * 2.5f,
                    (float) m_Width / (float) m_Height);
    GLUtils::setMat4(m_BlurProgramObj, "u_MVPMatrix", m_MVPMatrix);
    //绘制
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
    //关闭混合
    glDisable(GL_BLEND);
    m_FrameIndex++;
}


void AvatarSample::LoadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGD("AvatarSample::LoadMultiImageWithIndex pImage = %p, index = %d", pImage, index)
    if (pImage->ppPlane[0]) {
        m_RenderImages[index].width = pImage->width;
        m_RenderImages[index].height = pImage->height;
        m_RenderImages[index].format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImages[index]);
    }
}

void AvatarSample::Shutdown() {
    GLBaseSample::Shutdown();
    glDeleteBuffers(3, m_VboIds);
    glDeleteVertexArrays(1, &m_VaoId);
    glDeleteTextures(RENDER_IMG_NUM, m_TextureIds);
}

void AvatarSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float transX,
                                   float transY, float ratio) const {
    LOGD("AvatarSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY,
         ratio)
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
    Model = glm::translate(Model, glm::vec3(transX, transY, 0.0f));

    mvpMatrix = Projection * View * Model;
}

void AvatarSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void AvatarSample::SetGravityXY(float x, float y) {
    m_TransX = -x / 60;
    m_TransY = -y / 60;
}
