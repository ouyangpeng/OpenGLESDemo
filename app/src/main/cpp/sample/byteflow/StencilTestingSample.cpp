/**
 * Created by OuyangPeng on 2021/12/13.
 *
 * OpenGL ES 模板测试
 * 参考链接：NDK OpenGL ES 3.0 开发（十一）：模板测试   https://blog.csdn.net/Kennethdroid/article/details/102533260
 * 也可以参考《OPENGL ES 3.0编程指南》 第11章第2节第2小节
 *
 * https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/02%20Stencil%20testing/
 *
  模板测试与深度测试类似，主要作用是利用模板缓冲区(Stencil Buffer)所保存的模板值决定当前片段是否被丢弃，且发生于深度测试之前。
  模板测试一般步骤：
        1. 启用模板测试，开启模板缓冲写入glStencilMask(0xFF)；
        2. 执行渲染操作，更新模板缓冲区；
        3. 关闭模板缓冲写入glStencilMask(0x00)；
        4. 执行渲染操作，利用模板缓冲区所保存的模板值确定是否丢弃特定片段。


物体轮廓所能做的事情正如它名字所描述的那样。我们将会为每个（或者一个）物体在它的周围创建一个很小的有色边框。
当你想要在策略游戏中选中一个单位进行操作的，想要告诉玩家选中的是哪个单位的时候，这个效果就非常有用了。

为物体创建轮廓的步骤如下：
1. 在绘制（需要添加轮廓的）物体之前，将模板函数设置为GL_ALWAYS，每当物体的片段被渲染时，将模板缓冲更新为1。
2. 渲染物体。
3. 禁用模板写入以及深度测试。
4. 将每个物体缩放一点点。
5. 使用一个不同的片段着色器，输出一个单独的（边框）颜色。
6. 再次绘制物体，但只在它们片段的模板值不等于1时才绘制。
7. 再次启用模板写入和深度测试。
 */

#include "StencilTestingSample.h"

static GLfloat stencilTestingVertices[] = {
        //position            //texture coord  //normal
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,      0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 0.0f,      0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,      0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,   0.0f, 1.0f,      0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 0.0f,      0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f, 0.5f,    0.0f, 0.0f,      0.0f,  0.0f,  1.0f,
        0.5f, -0.5f, 0.5f,    1.0f, 0.0f,      0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, 0.5f,    1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
        0.5f,  0.5f, 0.5f,    1.0f, 1.0f,      0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f, 0.5f,    0.0f, 1.0f,      0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, 0.5f,    0.0f, 0.0f,      0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,   1.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,     -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,     -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,   1.0f, 0.0f,     -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,   1.0f, 0.0f,      1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,   1.0f, 1.0f,      1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,   0.0f, 0.0f,      1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,   1.0f, 0.0f,      1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,   1.0f, 1.0f,      0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,   1.0f, 0.0f,      0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,   0.0f, 0.0f,      0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,   0.0f, 1.0f,      0.0f, -1.0f,  0.0f,

        -0.5f, 0.5f, -0.5f,    0.0f, 1.0f,      0.0f,  1.0f,  0.0f,
        0.5f, 0.5f, -0.5f,    1.0f, 1.0f,      0.0f,  1.0f,  0.0f,
        0.5f, 0.5f,  0.5f,    1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
        0.5f, 0.5f,  0.5f,    1.0f, 0.0f,      0.0f,  1.0f,  0.0f,
        -0.5f, 0.5f,  0.5f,    0.0f, 0.0f,      0.0f,  1.0f,  0.0f,
        -0.5f, 0.5f, -0.5f,    0.0f, 1.0f,      0.0f,  1.0f,  0.0f,
};

StencilTestingSample::StencilTestingSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;
    m_ModelMatrixLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ModelMatrix = glm::mat4(0.0f);
}

StencilTestingSample::~StencilTestingSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void StencilTestingSample::Create() {
//Create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_stencil_testing.glsl");
    // 片段着色器
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_multi_light.glsl");
    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    const char *fFrameShaderStr = GLUtils::openTextFile(
            "fragment/fragment_shader_hello_triangle.glsl");
    m_FrameProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &fFrameShaderStr);

    if (m_ProgramObj == GL_NONE || m_FrameProgramObj == GL_NONE) {
        LOGE("StencilTestingSample::Create()  m_ProgramObj == GL_NONE || m_FrameProgramObj == GL_NONE")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    GO_CHECK_GL_ERROR()
    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    GO_CHECK_GL_ERROR()
    m_ModelMatrixLoc = glGetUniformLocation(m_ProgramObj, "u_ModelMatrix");
    GO_CHECK_GL_ERROR()
    m_ViewPosLoc = glGetUniformLocation(m_ProgramObj, "viewPos");
    GO_CHECK_GL_ERROR()

    m_LightPosition = glGetUniformLocation(m_ProgramObj, "light.position");
    GO_CHECK_GL_ERROR()
    m_LightColor = glGetUniformLocation(m_ProgramObj, "light.color");
    GO_CHECK_GL_ERROR()
    m_LightDirection = glGetUniformLocation(m_ProgramObj, "light.direction");
    GO_CHECK_GL_ERROR()
    m_LightCutOff = glGetUniformLocation(m_ProgramObj, "light.cutOff");
    GO_CHECK_GL_ERROR()
    m_LightOuterCutOff = glGetUniformLocation(m_ProgramObj, "light.outerCutOff");
    GO_CHECK_GL_ERROR()
    m_LightConstant = glGetUniformLocation(m_ProgramObj, "light.constant");
    GO_CHECK_GL_ERROR()
    m_LightLinear = glGetUniformLocation(m_ProgramObj, "light.linear");
    GO_CHECK_GL_ERROR()
    m_LightQuadratic = glGetUniformLocation(m_ProgramObj, "light.quadratic");
    GO_CHECK_GL_ERROR()

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(1, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(stencilTestingVertices), stencilTestingVertices,
                 GL_STATIC_DRAW);

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

    glBindVertexArray(GL_NONE);

    //upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImage.width, m_RenderImage.height, 0, GL_RGBA,
                 GL_UNSIGNED_BYTE, m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void StencilTestingSample::Draw() {
    LOGD("StencilTestingSample::Draw()")

    if (m_ProgramObj == GL_NONE || m_TextureId == GL_NONE) return;

    float ratio = (float) m_Width / (float) m_Height;

    // 如果这句话不写，直接会黑屏。当使用 GL_DEPTH_TEST的时候，要记得调用下面这句话，搭配使用。
    // 黑屏的Bug，参考博客
    // 【我的OpenGL学习进阶之旅】关于OpenGL ES 开启深度测试，直接黑屏的问题的解决方法
    // https://ouyangpeng.blog.csdn.net/article/details/121867616

   // 清空缓冲区: STENCIL_BUFFER、COLOR_BUFFER、DEPTH_BUFFER
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 结合模板和深度测试，一个片段可能有3种结果：
    // 1. 片段无法通过模板测试。如果是这样，则不对该片段进行任何进一步的测试（也就是深度测试）
    // 2. 片段通过模板测试，但是无法通过深度测试
    // 3. 片段既通过模板测试，又通过深度测试

    // 启用深度测试
    glEnable(GL_DEPTH_TEST);
    // 启用模板测试
    glEnable(GL_STENCIL_TEST);

    GO_CHECK_GL_ERROR()

    // ======================================开始模板测试============================================================
    // 和深度测试一样，我们对模板缓冲应该通过还是失败，以及它应该如何影响模板缓冲，也是有一定控制的。
    // 一共有两个函数能够用来配置模板测试：glStencilFunc和glStencilOp。
    //
    // glStencilFunc(GLenum func, GLint ref, GLuint mask)一共包含三个参数：
    //  func：设置模板测试操作。这个测试操作应用到已经储存的模板值和 glStencilFunc的 ref 值上，可用的选项是：
    //          GL_NEVER、GL_LEQUAL、GL_GREATER、GL_GEQUAL、GL_EQUAL、GL_NOTEQUAL、GL_ALWAYS ;
    //  ref：指定模板测试的引用值。模板缓冲区中的模板值会与这个值对比;
    //  mask：指定一个遮罩，在模板测试对比引用值和储存的模板值前，对它们进行按位与（and）操作，初始设置为 1 。
    glStencilFunc(GL_ALWAYS, 1, 0xFF);  // 所有的片段都应该更新模板缓冲
    GO_CHECK_GL_ERROR()

    // 但是glStencilFunc仅仅描述了OpenGL应该对模板缓冲内容做什么，而不是我们应该如何更新缓冲。这就需要glStencilOp这个函数了。
    // glStencilOp 主要用于控制更新模板缓冲区的方式。
    //      fail： 如果模板测试失败将如何更新模板值;
    //      zfail： 如果模板测试通过，但是深度测试失败时将如何更新模板值;
    //      zpass： 如果深度测试和模板测试都通过，将如何更新模板值。

    // 参数可选操作如下：
    // 操作	                        描述
    //  GL_KEEP	            保持现有的模板值
    //  GL_ZERO	            将模板值置为 0
    //  GL_REPLACE	        将模板值设置为用 glStencilFunc 函数设置的ref值
    //  GL_INCR	            如果模板值不是最大值就将模板值 +1
    //  GL_INCR_WRAP	    与 GL_INCR 一样将模板值 +1 ，如果模板值已经是最大值则设为 0
    //  GL_DECR	            如果模板值不是最小值就将模板值 -1
    //  GL_DECR_WRAP	    与 GL_DECR 一样将模板值 -1 ，如果模板值已经是最小值则设为最大值
    //  GL_INVERT	        按位反转当前模板缓冲区的值
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
    GO_CHECK_GL_ERROR()

//    glStencilMask允许我们设置一个位掩码(Bitmask)，它会与将要写入缓冲的模板值进行与(AND)运算。
//    默认情况下设置的位掩码所有位都为1，不影响输出，
//    但如果我们将它设置为0x00，写入缓冲的所有模板值最后都会变成0.
//    这与深度测试中的glDepthMask(GL_FALSE)是等价的。
//
//    glStencilMask(0xFF); // 每一位写入模板缓冲时都保持原样
//    glStencilMask(0x00); // 每一位在写入模板缓冲时都会变成0（禁用写入）

    // 0xFF == 0b11111111
    // 模板值与它进行按位与运算结果是模板值，模板缓冲可写
    glStencilMask(0xFF);    // 启用模板缓冲写入
    GO_CHECK_GL_ERROR()

    // ================================下面开始绘制物体============================================
    drawContainer(ratio);
    // ====================================== 结束绘制物体============================================

    // 通过使用GL_ALWAYS模板测试函数，我们保证了箱子的每个片段都会将模板缓冲的模板值更新为1。
    // 因为片段永远会通过模板测试，在绘制片段的地方，模板缓冲会被更新为参考值。
    // 现在模板缓冲在箱子被绘制的地方都更新为1了，我们将要绘制放大的箱子，但这次要禁用模板缓冲的写入
    // 我们将模板函数设置为GL_NOTEQUAL，它会保证我们只绘制箱子上模板值不为1的部分，
    // 即只绘制箱子在之前绘制的箱子之外的部分。注意我们也禁用了深度测试，让放大的箱子，即边框，不会被地板所覆盖。
    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);
    //禁用模板写入和深度测试
    // 0x00 == 0b00000000 == 0
    // 模板值与它进行按位与运算结果是0，模板缓冲不可写
    glStencilMask(0x00);    // 禁止模板缓冲的写入
    glDisable(GL_DEPTH_TEST);
    // ======================================结束模板测试============================================================

    // ================================下面开始绘制物体轮廓 绘制红线============================================
    drawScaledUpContainer(ratio);
    // ====================================== 结束绘制物体轮廓 绘制红线============================================
    // 记得要在完成之后重新启用深度缓冲。
    //开启模板写入和深度测试
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
}

void StencilTestingSample::drawScaledUpContainer(float ratio){
    glBindVertexArray(m_VaoId);
    glUseProgram(m_FrameProgramObj);
    //放大 1.05 倍
    UpdateMatrix(m_MVPMatrix, m_ModelMatrix, m_AngleX, m_AngleY, 1.05,
                 glm::vec3(0.0f, 0.0f, 0.0f), ratio);
    //glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    GLUtils::setMat4(m_FrameProgramObj, "u_MVPMatrix", m_MVPMatrix);
    //glUniformMatrix4fv(m_ModelMatrixLoc, 1, GL_FALSE, &m_ModelMatrix[0][0]);
    GLUtils::setMat4(m_FrameProgramObj, "u_ModelMatrix", m_ModelMatrix);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(0);
}

void StencilTestingSample::drawContainer(float ratio) {
    glBindVertexArray(m_VaoId);
    glUseProgram(m_ProgramObj);
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
    glUniform1f(m_LightCutOff, cos(glm::radians(10.5f)));
    glUniform1f(m_LightOuterCutOff, cos(glm::radians(11.5f)));

    // 衰减系数,常数项 constant，一次项 linear 和二次项 quadratic。
    glUniform1f(m_LightConstant, 1.0f);
    glUniform1f(m_LightLinear, 0.09);
    glUniform1f(m_LightQuadratic, 0.032);

    UpdateMatrix(m_MVPMatrix, m_ModelMatrix, m_AngleX, m_AngleY, 1.0, glm::vec3(0.0f, 0.0f, 0.0f),
                 ratio);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniformMatrix4fv(m_ModelMatrixLoc, 1, GL_FALSE, &m_ModelMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(0);
}

void StencilTestingSample::Shutdown() {
    GLBaseSample::Shutdown();

    glDeleteBuffers(1, m_VboIds);
    glDeleteVertexArrays(1, &m_VaoId);
    glDeleteTextures(1, &m_TextureId);
    m_ProgramObj = GL_NONE;
    m_VaoId = GL_NONE;
    m_TextureId = GL_NONE;
}

void StencilTestingSample::LoadImage(NativeImage *pImage) {
    LOGD("StencilTestingSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void StencilTestingSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX,
                                                 float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
}

void
StencilTestingSample::UpdateMatrix(glm::mat4 &mvpMatrix, glm::mat4 &modelMatrix, int angleXRotate,
                                   int angleYRotate, float scale, glm::vec3 transVec3,
                                   float ratio) {
    LOGD("StencilTestingSample::UpdateMatrix angleX = %d, angleY = %d, ratio = %f", angleXRotate,
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
    Model = glm::translate(Model, transVec3);

    modelMatrix = Model;

    mvpMatrix = Projection * View * Model;
}
