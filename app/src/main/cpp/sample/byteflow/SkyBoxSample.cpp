//
// Created by OuyangPeng on 2021/12/14.
// 参考博客：
// NDK OpenGL ES 3.0 开发（十五）：立方体贴图（天空盒）
// https://blog.csdn.net/Kennethdroid/article/details/102991524
// 立方体贴图 https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/06%20Cubemaps/

// OpenGL ES 立方体贴图本质上还是纹理映射，是一种 3D 纹理映射。
// 立方体贴图所使的纹理称为立方图纹理，它是由 6 个单独的 2D 纹理组成，每个 2D 纹理是立方图的一个面。
//
// 立方图纹理的采样通过一个 3D 向量（s, t, r）作为纹理坐标，这个 3D 向量只作为方向向量使用，
// OpenGL ES 获取方向向量触碰到立方图表面上的纹理像素作为采样结果。
// 方向向量触碰到立方图表面对应的纹理位置作为采样点，要求立方图的中心必须位于原点。

// 立方图各个面的指定方法与 2D 纹理基本相同，且每个面必须为正方形（宽度和高度必须相同）。
//

#include "SkyBoxSample.h"

SkyBoxSample::SkyBoxSample() : m_CubeProgramObj(GL_NONE) {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_SkyBoxVaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_ModelMatrix = glm::mat4(0.0f);

    memset(m_pSkyBoxRenderImg, 0, sizeof(NativeImage) * 6);
}

SkyBoxSample::~SkyBoxSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);

    for (NativeImage nativeImage : m_pSkyBoxRenderImg) {
        NativeImageUtil::FreeNativeImage(&nativeImage);
    }
}

void SkyBoxSample::create() {
    // 编译链接用于普通渲染的着色器程序
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_skybox.glsl");
    // 用于普通渲染的片段着色器脚本，简单纹理映射
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_skybox.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (mProgram == GL_NONE) {
        LOGE("SkyBoxSample::create()  mProgram == GL_NONE")
        return;
    }
    m_SamplerLoc = glGetUniformLocation(mProgram, "s_SkyBox");
    m_MVPMatLoc = glGetUniformLocation(mProgram, "u_MVPMatrix");


    const char *vCubeShaderStr = GLUtils::openTextFile(
            "vertex/vertex_shader_skybox_cube.glsl");
    const char *fCubeShaderStr = GLUtils::openTextFile(
            "fragment/fragment_shader_skybox_cube.glsl");
    m_CubeProgramObj = GLUtils::createProgram(&vCubeShaderStr, &fCubeShaderStr);
    if (m_CubeProgramObj == GL_NONE) {
        LOGE("SkyBoxSample::create()  m_CubeProgramObj == GL_NONE")
        return;
    }
    m_CubeSamplerLoc = glGetUniformLocation(m_CubeProgramObj, "s_SkyBox");
    m_CubeMVPMatLoc = glGetUniformLocation(m_CubeProgramObj, "u_MVPMatrix");
    m_CubeModelMatLoc = glGetUniformLocation(m_CubeProgramObj, "u_ModelMatrix");
    m_ViewPosLoc = glGetUniformLocation(m_CubeProgramObj, "u_cameraPos");


    // Generate SkyBox VBO Ids and load the VBOs with data
    glGenBuffers(1, &m_SkyBoxVboId);
    glBindBuffer(GL_ARRAY_BUFFER, m_SkyBoxVboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW);

    // Generate SkyBox VAO Id
    glGenVertexArrays(1, &m_SkyBoxVaoId);
    glBindVertexArray(m_SkyBoxVaoId);
    glBindBuffer(GL_ARRAY_BUFFER, m_SkyBoxVboId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);



    // Generate Cube VBO Ids and load the VBOs with data
    glGenBuffers(1, &m_CubeVboId);
    glBindBuffer(GL_ARRAY_BUFFER, m_CubeVboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxCubeVertices), skyboxCubeVertices, GL_STATIC_DRAW);

    // Generate Cube VAO Id
    glGenVertexArrays(1, &m_CubeVaoId);
    glBindVertexArray(m_CubeVaoId);
    glBindBuffer(GL_ARRAY_BUFFER, m_CubeVboId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat),
                          (GLvoid *) (3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void SkyBoxSample::draw() {
    LOGD("SkyBoxSample::Draw()")

    if (mProgram == GL_NONE || m_CubeProgramObj == GL_NONE) return;

    // 如果这句话不写，直接会黑屏。当使用 GL_DEPTH_TEST的时候，要记得调用下面这句话，搭配使用。
    // 黑屏的Bug，参考博客
    // 【我的OpenGL学习进阶之旅】关于OpenGL ES 开启深度测试，直接黑屏的问题的解决方法
    // https://ouyangpeng.blog.csdn.net/article/details/121867616

    // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 启用深度测试
    glEnable(GL_DEPTH_TEST);

    // ========================================== draw SkyBox ==========================================

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, 1.0, (float) mWidth / (float) mHeight);

    if (!m_TextureId) {
        //create RGBA texture
        // 生成一个纹理，激活相应纹理单元，然后绑定到 GL_TEXTURE_CUBE_MAP类型纹理。
        glGenTextures(1, &m_TextureId);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);

        // 由于立方图包含 6 个纹理，每个面对应一个纹理，需要调用glTexImage2D函数 6 次，
        // OpenGL ES 为立方图提供了 6 个不同的纹理目标，对应立方图的 6 个面，且 6 个纹理目标按顺序依次增 1。
        // GL_TEXTURE_CUBE_MAP_POSITIVE_X	右
        //  GL_TEXTURE_CUBE_MAP_NEGATIVE_X	左
        //  GL_TEXTURE_CUBE_MAP_POSITIVE_Y	上
        //  GL_TEXTURE_CUBE_MAP_NEGATIVE_Y	下
        //  GL_TEXTURE_CUBE_MAP_POSITIVE_Z	后
        //  GL_TEXTURE_CUBE_MAP_NEGATIVE_Z	前
        for (int i = 0; i < sizeof(m_pSkyBoxRenderImg) / sizeof(NativeImage); ++i) {
            //  将立方图的 6 个面对应的图像数据加载到纹理，其中m_pSkyBoxRenderImg为图像数据的数组：
            glTexImage2D(
                    GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0,
                    GL_RGBA, m_pSkyBoxRenderImg[i].width, m_pSkyBoxRenderImg[i].height, 0, GL_RGBA,
                    GL_UNSIGNED_BYTE, m_pSkyBoxRenderImg[i].ppPlane[0]
            );
        }
        // 立方体贴图和其它纹理没什么不同，我们也需要设定它的环绕和过滤方式
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

        // 我们将环绕方式设置为GL_CLAMP_TO_EDGE，
        // 这是因为正好处于两个面之间的纹理坐标可能不能击中一个面（由于一些硬件限制），
        // 所以通过使用GL_CLAMP_TO_EDGE，OpenGL将在我们对两个面之间采样的时候，永远返回它们的边界值。
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        //不要被GL_TEXTURE_WRAP_R吓到，它仅仅是为纹理的R坐标设置了环绕方式，它对应的是纹理的第三个维度（和位置的z一样）。
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }

    // draw SkyBox
    glUseProgram(mProgram);
    glBindVertexArray(m_SkyBoxVaoId);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR()


    // ========================================== draw Cube ==========================================
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, 0.4f, (float) mWidth / (float) mHeight);

    glUseProgram(m_CubeProgramObj);
    glBindVertexArray(m_CubeVaoId);
    glUniformMatrix4fv(m_CubeMVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniformMatrix4fv(m_CubeModelMatLoc, 1, GL_FALSE, &m_ModelMatrix[0][0]);
    glUniform3f(m_ViewPosLoc, 0.0f, 0.0f, 1.8f);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_TextureId);
    glUniform1i(m_CubeSamplerLoc, 0);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR()
}

void SkyBoxSample::shutdown() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        glDeleteVertexArrays(1, &m_SkyBoxVaoId);
        glDeleteBuffers(1, &m_SkyBoxVboId);
        glDeleteTextures(1, &m_TextureId);
        mProgram = GL_NONE;
    }

    if (m_CubeProgramObj) {
        glDeleteProgram(m_CubeProgramObj);
        glDeleteVertexArrays(1, &m_CubeVaoId);
        glDeleteBuffers(1, &m_CubeVboId);
        m_CubeProgramObj = GL_NONE;
    }
}

void SkyBoxSample::LoadImage(NativeImage *pImage) {
    LOGD("SkyBoxSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void SkyBoxSample::LoadMultiImageWithIndex(int index, NativeImage *pImage) {
    LOGD("SkyBoxSample::LoadMultiImageWithIndex pImage = %p, index=%d", pImage->ppPlane[0], index)
    if (pImage) {
        if (m_pSkyBoxRenderImg[index].ppPlane[0]) {
            NativeImageUtil::FreeNativeImage(&m_pSkyBoxRenderImg[index]);
        }

        m_pSkyBoxRenderImg[index].width = pImage->width;
        m_pSkyBoxRenderImg[index].height = pImage->height;
        m_pSkyBoxRenderImg[index].format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_pSkyBoxRenderImg[index]);
    }
}

void SkyBoxSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void SkyBoxSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float scale,
                                   float ratio) {
    LOGD("SkyBoxSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX,
         angleY, ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::mat4 View = glm::lookAt(
            glm::vec3(0, 0, 1.8), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, -1), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scale, scale, scale));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, glm::vec3(0.0f, 0.0f, 0.0f));

    m_ModelMatrix = Model;

    mvpMatrix = Projection * View * Model;
}
