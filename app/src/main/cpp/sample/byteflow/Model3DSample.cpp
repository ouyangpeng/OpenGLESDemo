//
// Created by OuyangPeng on 2021/12/28.
// 参考博客：
// NDK OpenGL ES 3.0 开发（二十）：3D 模型
// https://blog.csdn.net/Kennethdroid/article/details/103771970

// NDK OpenGL ES 3.0 开发（二十一）：3D 模型加载和渲染
// https://blog.csdn.net/Kennethdroid/article/details/103825593
//

#include "Model3DSample.h"

Model3DSample::Model3DSample() {
    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_pModel = nullptr;
    m_pShader = nullptr;
}

void Model3DSample::Create() {
    // "/data/data/com.oyp.openglesdemo/cache"
    std::string path(DEFAULT_OGL_ASSETS_DIR);

    //初始化，加载模型
    m_pModel = new Model(path + "/poly/Apricot_02_hi_poly.obj");
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_model_3d.glsl");
    if (m_pModel->ContainsTextures()) {
        // 片段着色器
        FRAGMENT_SHADER = GLUtils::openTextFile(
                "fragment/fragment_shader_model_3d.glsl");
        m_pShader = new Shader(VERTEX_SHADER, FRAGMENT_SHADER);
    } else {
        const char *fNoTextureShaderStr = GLUtils::openTextFile(
                "fragment/fragment_shader_model_3d_no_texture.glsl");
        m_pShader = new Shader(VERTEX_SHADER, fNoTextureShaderStr);
    }
    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
}

void Model3DSample::Draw() {
    if (m_pModel == nullptr || m_pShader == nullptr) return;
    LOGD("Model3DSample::Draw()")

    // 如果这句话不写，直接会黑屏。当使用 GL_DEPTH_TEST的时候，要记得调用下面这句话，搭配使用。
    // 黑屏的Bug，参考博客
    // 【我的OpenGL学习进阶之旅】关于OpenGL ES 开启深度测试，直接黑屏的问题的解决方法
    // https://ouyangpeng.blog.csdn.net/article/details/121867616
    // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) m_Width / (float) m_Height);

    m_pShader->use();
    m_pShader->setMat4("u_MVPMatrix", m_MVPMatrix);
    m_pShader->setMat4("u_ModelMatrix", m_ModelMatrix);
    m_pShader->setVec3("lightPos", glm::vec3(0, 0, m_pModel->GetMaxViewDistance()));
    m_pShader->setVec3("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
    m_pShader->setVec3("viewPos", glm::vec3(0, 0, m_pModel->GetMaxViewDistance()));
    m_pModel->Draw((*m_pShader));
}

void Model3DSample::Shutdown() {
    LOGD("Model3DSample::Destroy")
    GLBaseSample::Shutdown();
    // 销毁对象
    if (m_pModel != nullptr) {
        m_pModel->Destroy();
        delete m_pModel;
        m_pModel = nullptr;
    }

    if (m_pShader != nullptr) {
        m_pShader->Destroy();
        delete m_pShader;
        m_pShader = nullptr;
    }
}

void
Model3DSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void Model3DSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) {
    LOGD("Model3DSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY, ratio);
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);

    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.1f, 100.0f);
    glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 1.0f, m_pModel->GetMaxViewDistance() * 4);
    //glm::mat4 Projection = glm::perspective(45.0f,ratio, 0.1f,100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, m_pModel->GetMaxViewDistance() * 1.8f), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, -m_pModel->GetAdjustModelPosVec());
    m_ModelMatrix = Model;
    mvpMatrix = Projection * View * Model;
}
