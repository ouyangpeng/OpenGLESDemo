//
// Created by OuyangPeng on 2021/12/27.
// 参考博客：
// Android 图片转场和轮播特效，你想要的都在这了
// https://www.jianshu.com/p/46175128d1d9

// 转场特效
// https://gl-transitions.com/gallery
//

#include "GLTransitionExample.h"

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

static GLushort indices[] = {0, 1, 2, 0, 2, 3};

GLTransitionExample::GLTransitionExample(int renderSampleType) {
    mRenderSampleType = renderSampleType;

    m_MVPMatLoc = GL_NONE;

    for (unsigned int &m_TextureId : m_TextureIds) {
        m_TextureId = GL_NONE;
    }
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_frameIndex = 0;
    m_loopCount = 0;
}

GLTransitionExample::~GLTransitionExample() {
    for (auto &m_RenderImage : m_RenderImages) {
        NativeImageUtil::FreeNativeImage(&m_RenderImage);
    }
}

void GLTransitionExample::Create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_coord_system.glsl");

    switch(mRenderSampleType){
        case SAMPLE_TYPE_KEY_TRANSITIONS_1:
            mFragmentShaderPath = "fragment/fragment_shader_transition1.glsl";
            break;
        case SAMPLE_TYPE_KEY_TRANSITIONS_2:
            mFragmentShaderPath = "fragment/fragment_shader_transition2.glsl";
            break;
        case SAMPLE_TYPE_KEY_TRANSITIONS_3:
            mFragmentShaderPath = "fragment/fragment_shader_transition3.glsl";
            break;
        case SAMPLE_TYPE_KEY_TRANSITIONS_4:
            mFragmentShaderPath = "fragment/fragment_shader_transition4.glsl";
            break;
        default:
            break;
    }
    LOGD("BaseShaderToySimpleSample::Create() mRenderSampleType =%d, mFragmentShaderPath = %s",
         mRenderSampleType, mFragmentShaderPath)
    if (mFragmentShaderPath == GL_NONE) {
        LOGD("BaseShaderToySimpleSample::Create() mFragmentShaderPath == GL_NONE 直接return")
        return;
    }
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(mFragmentShaderPath);
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (m_ProgramObj == GL_NONE) {
        LOGD("PortraitStayColorExample::Init create program fail")
        return;
    }
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

    //===========================================  字节对齐相关===========================================

    // 另外需要注意 OpenGL 访问的图像内存默认是 4 字节对齐，
    // 这样灰度 Mask 图的宽度不是 4 的整数倍的话，会有花屏现象，这里需要取消对齐设置
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    //=========================================== 纹理相关===========================================

    //create RGBA texture
    for (unsigned int &m_TextureId : m_TextureIds) {
        glGenTextures(1, &m_TextureId);
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }
    //upload RGBA image data
    for (int i = 0; i < BF_IMG_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImages[i].width, m_RenderImages[i].height,
                     0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImages[i].ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }
}

void GLTransitionExample::LoadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGD("GLTransitionExample::LoadMultiImageWithIndex pImage = %p,[w=%d,h=%d,f=%d]",
         pImage->ppPlane[0], pImage->width, pImage->height, pImage->format)

    if (pImage && index >= 0 && index < BF_IMG_NUM) {
        m_RenderImages[index].width = pImage->width;
        m_RenderImages[index].height = pImage->height;
        m_RenderImages[index].format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImages[index]);
    }
}

void GLTransitionExample::Draw() {
    LOGD("GLTransitionExample::Draw()")

    if (m_ProgramObj == GL_NONE || m_TextureIds[0] == GL_NONE) return;

    m_frameIndex++;

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) m_Width / (float) m_Height);

    // Use the program object
    glUseProgram(m_ProgramObj);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

	float offset = (m_frameIndex % BF_LOOP_COUNT) * 1.0f / BF_LOOP_COUNT;

    if (m_frameIndex % BF_LOOP_COUNT == 0)
        m_loopCount++;

    // 设置纹理和变量
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[m_loopCount % BF_IMG_NUM]);
    GLUtils::setInt(m_ProgramObj, "u_texture0", 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[(m_loopCount + 1) % BF_IMG_NUM]);
    GLUtils::setInt(m_ProgramObj, "u_texture1", 1);

    GLUtils::setVec2(m_ProgramObj, "u_texSize", m_RenderImages[0].width, m_RenderImages[0].height);
    GLUtils::setFloat(m_ProgramObj, "u_offset", offset);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void GLTransitionExample::Shutdown() {
    if (m_ProgramObj) {
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteTextures(BF_IMG_NUM, m_TextureIds);
    }
}

void GLTransitionExample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX,
                                                float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void GLTransitionExample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY,
                                          float ratio) const {
    LOGD("GLTransitionExample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX,
         angleY, ratio);
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
