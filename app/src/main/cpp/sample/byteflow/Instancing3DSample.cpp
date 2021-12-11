//
// Created by OuyangPeng on 2021/12/11 0011.
// 可以参考下面的博客：
// NDK OpenGL ES 3.0 开发（十三）：实例化（Instancing）
// https://blog.csdn.net/Kennethdroid/article/details/102770813

// OpenGL ES 实例化（Instancing）是一种只调用一次渲染函数就能绘制出很多物体的技术，
// 可以实现将数据一次性发送给 GPU ，告诉 OpenGL ES 使用一个绘制函数，将这些数据绘制成多个物体。
// 实例化（Instancing）避免了 CPU 多次向 GPU 下达渲染命令
// （避免多次调用 glDrawArrays 或 glDrawElements 等绘制函数），
// 节省了绘制多个物体时 CPU 与 GPU 之间的通信时间，提升了渲染性能。

// 相对于普通绘制，实例化绘制多了一个参数 instancecount，
// 表示需要渲染的实例数量，调用完实例化绘制函数后，
// 我们便将绘制数据一次性发送给 GPU，然后告诉它该如何使用一个函数来绘制这些实例。

// 实例化（Instancing）的目标并不是实现将同一物体绘制多次，
// 而是能基于某一物体绘制出位置、大小、形状或者颜色不同的多个物体。
// OpenGL ES 着色器中有一个与实例化绘制相关的内建变量 gl_InstanceID。

// gl_InstanceID 表示当前正在绘制实例的 ID ，每个实例对应一个唯一的 ID ，
// 通过这个 ID 可以轻易实现基于一个物体而绘制出位置、大小、形状或者颜色不同的多个物体（实例）。
// 利用内建变量 gl_InstanceID 在 3D 空间绘制多个位于不同位置的立方体，
// 利用 u_offsets[gl_InstanceID] 对当前实例的位置进行偏移，对应的着色器脚本：
//

#include "Instancing3DSample.h"

Instancing3DSample::Instancing3DSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;
    m_ModelMatrixLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 2.0f;
    m_ScaleY = 2.0f;

    m_ModelMatrix = glm::mat4(0.0f);
}

Instancing3DSample::~Instancing3DSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void Instancing3DSample::create() {
    if (mProgram) {
        return;
    }
    //create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 编译链接用于普通渲染的着色器程序
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_instance_3d.glsl");
    // 用于普通渲染的片段着色器脚本，简单纹理映射
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_multi_light.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    if (mProgram == GL_NONE) {
        LOGE("BasicLightingSample::Init mProgram == GL_NONE")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(mProgram, "s_TextureMap");
    GO_CHECK_GL_ERROR()
    m_MVPMatLoc = glGetUniformLocation(mProgram, "u_MVPMatrix");
    GO_CHECK_GL_ERROR()
    m_ModelMatrixLoc = glGetUniformLocation(mProgram, "u_ModelMatrix");
    GO_CHECK_GL_ERROR()
    m_ViewPosLoc = glGetUniformLocation(mProgram, "viewPos");
    GO_CHECK_GL_ERROR()


    m_LightPosition = glGetUniformLocation(mProgram, "light.position");
    m_LightColor = glGetUniformLocation(mProgram, "light.color");
    m_LightDirection = glGetUniformLocation(mProgram, "light.direction");
    m_LightCutOff = glGetUniformLocation(mProgram, "light.cutOff");
    m_LightOuterCutOff = glGetUniformLocation(mProgram, "light.outerCutOff");
    m_LightConstant = glGetUniformLocation(mProgram, "light.constant");
    m_LightLinear = glGetUniformLocation(mProgram, "light.linear");
    m_LightQuadratic = glGetUniformLocation(mProgram, "light.quadratic");


    // 在 3D 空间中产生 125 个偏移量（offset）：
    glm::vec3 translations[125];
    int index = 0;
    GLfloat offset = 0.2f;
    for (GLint y = -10; y < 10; y += 4) {
        for (GLint x = -10; x < 10; x += 4) {
            for (GLint z = -10; z < 10; z += 4) {
                glm::vec3 translation;
                translation.x = (GLfloat) x / 10.0f + offset;
                translation.y = (GLfloat) y / 10.0f + offset;
                translation.z = (GLfloat) z / 10.0f + offset;
                translations[index++] = translation;
            }
        }
    }

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(2, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(i3d_vertices), i3d_vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * 125, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), nullptr);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (const void *) (3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat),
                          (const void *) (5 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);


    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glVertexAttribDivisor(3, 1); // Tell OpenGL this is an instanced vertex attribute.
    glBindVertexArray(GL_NONE);

    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void Instancing3DSample::draw() {
    LOGD("Instancing3DSample::Draw()")

    if (mProgram == GL_NONE || m_TextureId == GL_NONE) return;
    // 如果这句话不写，直接会黑屏。当使用 GL_DEPTH_TEST的时候，要记得调用下面这句话，搭配使用。
    // 黑屏的Bug，参考博客
    // 【我的OpenGL学习进阶之旅】关于OpenGL ES 开启深度测试，直接黑屏的问题的解决方法
    // https://ouyangpeng.blog.csdn.net/article/details/121867616

    // Clear the color buffer
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    float ratio = (float) mWidth / (float) mHeight;

    // Use the program object
    glUseProgram(mProgram);

    glBindVertexArray(m_VaoId);

    glUniform3f(m_ViewPosLoc, 0.0f, 0.0f, 3.0f);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

    // 设置光源的位置、颜色和方向
    glUniform3f(m_LightPosition, 0.0f, 0.0f, 3.0f);
    glUniform3f(m_LightColor, 1.0f, 1.0f, 1.0f);
    glUniform3f(m_LightDirection, 0.0f, 0.0f, -1.0f);

    // 用于计算边缘的过度，cutOff 表示内切光角，outerCutOff 表示外切光角
    glUniform1f(m_LightCutOff, glm::cos(glm::radians(10.5f)));
    glUniform1f(m_LightOuterCutOff,glm::cos(glm::radians(11.5f)));

    // 衰减系数,常数项 constant，一次项 linear 和二次项 quadratic。
    glUniform1f(m_LightConstant, 1.0f);
    glUniform1f(m_LightLinear, 0.09);
    glUniform1f(m_LightQuadratic, 0.032);

    UpdateMatrix(m_MVPMatrix, m_ModelMatrix, m_AngleX + 10, m_AngleY + 10,
                 m_ScaleX > m_ScaleY ? m_ScaleY : m_ScaleX, glm::vec3(0.0f, 0.0f, 0.0f), ratio);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniformMatrix4fv(m_ModelMatrixLoc, 1, GL_FALSE, &m_ModelMatrix[0][0]);

    // 实例化渲染
    // 相对于普通绘制，实例化绘制多了一个参数 instancecount，
    // 表示需要渲染的实例数量，调用完实例化绘制函数后，
    // 我们便将绘制数据一次性发送给 GPU，然后告诉它该如何使用一个函数来绘制这些实例。
    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, 125);
    glBindVertexArray(0);
}

void Instancing3DSample::shutdown() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        glDeleteBuffers(2, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteTextures(1, &m_TextureId);
        mProgram = GL_NONE;
        m_VaoId = GL_NONE;
        m_TextureId = GL_NONE;
    }
}

void Instancing3DSample::LoadImage(NativeImage *pImage) {
    LOGD("Instancing3DSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void
Instancing3DSample::UpdateMatrix(glm::mat4 &mvpMatrix, glm::mat4 &modelMatrix, int angleXRotate,
                                 int angleYRotate, float scale, glm::vec3 transVec3, float ratio) {
    LOGD("Instancing3DSample::UpdateMatrix angleX = %d, angleY = %d, ratio = %f", angleXRotate,
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
            glm::vec3(0, 0, 3), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scale, scale, scale));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    //Model = glm::translate(Model, transVec3);

    modelMatrix = Model;

    mvpMatrix = Projection * View * Model;
}

void Instancing3DSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX,
                                               float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = 2 * scaleX;
    m_ScaleY = 2 * scaleY;
}
