/**
 * Created by OuyangPeng on 2021/12/13.
 *
 * OpenGL ES 模板测试
 * 参考链接：NDK OpenGL ES 3.0 开发（十一）：模板测试   https://blog.csdn.net/Kennethdroid/article/details/102533260
 * 也可以参考《OPENGL ES 3.0编程指南》 第11章第2节第2小节
 *
  模板测试与深度测试类似，主要作用是利用模板缓冲区(Stencil Buffer)所保存的模板值决定当前片段是否被丢弃，且发生于深度测试之前。
  模板测试一般步骤：
        1. 启用模板测试，开启模板缓冲写入glStencilMask(0xFF)；
        2. 执行渲染操作，更新模板缓冲区；
        3. 关闭模板缓冲写入glStencilMask(0x00)；
        4. 执行渲染操作，利用模板缓冲区所保存的模板值确定是否丢弃特定片段。
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

void StencilTestingSample::create() {
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
            "vertex/vertex_shader_stencil_testing.glsl");
    // 用于普通渲染的片段着色器脚本，简单纹理映射
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_multi_light.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

    const char *fFrameShaderStr = GLUtils::openTextFile(
            "fragment/fragment_shader_hello_triangle.glsl");
    m_FrameProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &fFrameShaderStr);

    if (mProgram == GL_NONE || m_FrameProgramObj == GL_NONE) {
        LOGE("StencilTestingSample::create()  mProgram == GL_NONE || m_FrameProgramObj == GL_NONE")
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
    GO_CHECK_GL_ERROR()
    m_LightColor = glGetUniformLocation(mProgram, "light.color");
    GO_CHECK_GL_ERROR()
    m_LightDirection = glGetUniformLocation(mProgram, "light.direction");
    GO_CHECK_GL_ERROR()
    m_LightCutOff = glGetUniformLocation(mProgram, "light.cutOff");
    GO_CHECK_GL_ERROR()
    m_LightOuterCutOff = glGetUniformLocation(mProgram, "light.outerCutOff");
    GO_CHECK_GL_ERROR()
    m_LightConstant = glGetUniformLocation(mProgram, "light.constant");
    GO_CHECK_GL_ERROR()
    m_LightLinear = glGetUniformLocation(mProgram, "light.linear");
    GO_CHECK_GL_ERROR()
    m_LightQuadratic = glGetUniformLocation(mProgram, "light.quadratic");
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

void StencilTestingSample::draw() {
    LOGD("StencilTestingSample::Draw()")

    if (mProgram == GL_NONE || m_TextureId == GL_NONE) return;

    float ratio = (float) mWidth / (float) mHeight;

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

    //  func：设置模板测试操作。这个测试操作应用到已经储存的模板值和 glStencilFunc的 ref 值上，可用的选项是：
    //          GL_NEVER、GL_LEQUAL、GL_GREATER、GL_GEQUAL、GL_EQUAL、GL_NOTEQUAL、GL_ALWAYS ;
    //  ref：指定模板测试的引用值。模板缓冲区中的模板值会与这个值对比;
    //  mask：指定一个遮罩，在模板测试对比引用值和储存的模板值前，对它们进行按位与（and）操作，初始设置为 1 。
    glStencilFunc(GL_ALWAYS, 1, 0xFF);
    GO_CHECK_GL_ERROR()

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

    // 0xFF == 0b11111111
    // 模板值与它进行按位与运算结果是模板值，模板缓冲可写
    glStencilMask(0xFF);
    GO_CHECK_GL_ERROR()

    // 下面开始绘制物体

    glBindVertexArray(m_VaoId);
    glUseProgram(mProgram);
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

    UpdateMatrix(m_MVPMatrix, m_ModelMatrix, m_AngleX, m_AngleY, 1.0, glm::vec3(0.0f, 0.0f, 0.0f),
                 ratio);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glUniformMatrix4fv(m_ModelMatrixLoc, 1, GL_FALSE, &m_ModelMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(0);

    glStencilFunc(GL_NOTEQUAL, 1, 0xFF);


    //禁用模板写入和深度测试
    // 0x00 == 0b00000000 == 0
    // 模板值与它进行按位与运算结果是0，模板缓冲不可写
    glStencilMask(0x00);
    glDisable(GL_DEPTH_TEST);
    // ======================================结束模板测试============================================================


    //绘制物体轮廓   绘制红线
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

    //开启模板写入和深度测试
    glStencilMask(0xFF);
    glEnable(GL_DEPTH_TEST);
}

void StencilTestingSample::shutdown() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        glDeleteBuffers(1, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteTextures(1, &m_TextureId);
        mProgram = GL_NONE;
        m_VaoId = GL_NONE;
        m_TextureId = GL_NONE;
    }

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
