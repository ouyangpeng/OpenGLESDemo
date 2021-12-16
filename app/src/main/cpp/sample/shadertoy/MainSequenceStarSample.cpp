//
// Created by OuyangPeng on 2021/12/15.
// 着色器在ShaderToy 官网的地址： https://www.shadertoy.com/view/4dXGR4
// 主序星是什么 https://www.tanmizhi.com/html/9001.html
//

#include "MainSequenceStarSample.h"


MainSequenceStarSample::MainSequenceStarSample() {
    m_MVPMatLoc = GL_NONE;

    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

MainSequenceStarSample::~MainSequenceStarSample() {
    for (auto &m_RenderImage : m_RenderImages) {
        NativeImageUtil::FreeNativeImage(&m_RenderImage);
    }
}

void MainSequenceStarSample::create() {
    //create RGBA textures
    glGenTextures(MAIN_SEQUENCE_STAR_RENDER_IMG_NUM, m_TextureIds);
    for (unsigned int m_TextureId : m_TextureIds) {
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }


    // 编译链接用于普通渲染的着色器程序
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_beating_heart.glsl");
    // 用于普通渲染的片段着色器脚本，简单纹理映射
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_main_sequence_star.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (mProgram == GL_NONE) {
        LOGE("CoordSystemSample::Init mProgram == GL_NONE")
        return;
    }

    m_MVPMatLoc = glGetUniformLocation(mProgram, "u_MVPMatrix");
    m_SizeLoc = glGetUniformLocation(mProgram, "u_screenSize");
    m_TimeLoc = glGetUniformLocation(mProgram, "u_time");

    m_SamplerLocIChannels[0] = glGetUniformLocation(mProgram, "iChannel0");
    m_SamplerLocIChannels[1] = glGetUniformLocation(mProgram, "iChannel1");


    GLfloat verticesCoords[] = {
            -1.0f,  1.0f, 0.0f,  // Position 0
            -1.0f, -1.0f, 0.0f,  // Position 1
            1.0f,  -1.0f, 0.0f,  // Position 2
            1.0f,   1.0f, 0.0f,  // Position 3
    };

    GLfloat textureCoords[] = {
            0.0f,  0.0f,        // TexCoord 0
            0.0f,  1.0f,        // TexCoord 1
            1.0f,  1.0f,        // TexCoord 2
            1.0f,  0.0f         // TexCoord 3
    };

    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };

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

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void MainSequenceStarSample::draw() {
    LOGD("MainSequenceStarSample::Draw()")

    if (mProgram == GL_NONE) {
        LOGD("MainSequenceStarSample::Draw() mProgram == GL_NONE")
        return;
    }

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the program object
    glUseProgram(mProgram);

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) mWidth / (float) mHeight);

    //upload RGBA image data
    for (int i = 0; i < MAIN_SEQUENCE_STAR_RENDER_IMG_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImages[i].width, m_RenderImages[i].height,
                     0, GL_RGBA,
                     GL_UNSIGNED_BYTE, m_RenderImages[i].ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }


    glBindVertexArray(m_VaoId);

    static int sFrameIndex = 0;
    sFrameIndex++;

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    float time = sFrameIndex * 0.02f;
    LOGD("MainSequenceStarSample::Draw() time=%f", time)
    // 控制输入时间周期为 2000ms
    glUniform1f(m_TimeLoc, time);
    // 输入屏幕的尺寸
    glUniform2f(m_SizeLoc, mWidth, mHeight);

    //upload RGBA image data
    for (int i = 0; i < MAIN_SEQUENCE_STAR_RENDER_IMG_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glUniform1i(m_SamplerLocIChannels[i], i);
    }

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void MainSequenceStarSample::shutdown() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        glDeleteBuffers(2, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
    }
}

void MainSequenceStarSample::LoadMultiImageWithIndex(int index, NativeImage *pImage) {
    GLBaseSample::LoadMultiImageWithIndex(index, pImage);
    LOGD("BlendingSample::LoadImage index=%d, pImage = %p", index, pImage->ppPlane[0])
    if (pImage) {
        m_RenderImages[index].width = pImage->width;
        m_RenderImages[index].height = pImage->height;
        m_RenderImages[index].format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImages[index]);
    }
}

void
MainSequenceStarSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const {
    LOGD("MainSequenceStarSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio)
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


