//
// Created by OuyangPeng on 2021/12/13.

// Native5Lesson 中也运用了混合 技术

// 参考博客 :
// NDK OpenGL ES 3.0 开发（十二）：混合
// https://blog.csdn.net/Kennethdroid/article/details/102630858

// Android OpenGLES2.0（十八）——轻松搞定Blend颜色混合
// https://blog.csdn.net/junzia/article/details/76580379

// https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/03%20Blending/

// OpenGL ES 混合
// OpenGL ES 混合本质上是将 2 个片元的颜色进行调和，产生一个新的颜色。
// OpenGL ES 混合发生在片元通过各项测试之后，
// 准备进入帧缓冲区的片元和原有的片元按照特定比例加权计算出最终片元的颜色值，
// 不再是新（源）片元直接覆盖缓冲区中的（目标）片元。


//要想让混合在多个物体上工作，我们需要最先绘制最远的物体，最后绘制最近的物体。普通不需要混合的物体仍然可以使用深度缓冲正常绘制，所以它们不需要排序。但我们仍要保证它们在绘制（排序的）透明物体之前已经绘制完毕了。当绘制一个有不透明和透明物体的场景的时候，大体的原则如下：
//1. 先绘制所有不透明的物体。
//2. 对所有透明的物体排序。
//3. 按顺序绘制所有透明的物体。


#include "BlendingSample.h"

BlendingSample::BlendingSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;
}

BlendingSample::~BlendingSample() {
    for (auto &m_RenderImage : m_RenderImages) {
        NativeImageUtil::FreeNativeImage(&m_RenderImage);
    }
}

void BlendingSample::Create() {
    //Create RGBA textures
    glGenTextures(RENDER_IMG_NUM, m_TextureIds);
    for (unsigned int m_TextureId : m_TextureIds) {
        glBindTexture(GL_TEXTURE_2D, m_TextureId);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

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

    // Generate VAO Ids
    glGenVertexArrays(3, m_VaoIds);
    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, m_VboIds);

    glBindVertexArray(m_VaoIds[0]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(boxVertices), boxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (const void *) (3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    glBindVertexArray(m_VaoIds[1]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(flatVertices), flatVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (const void *) (3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    glBindVertexArray(m_VaoIds[2]);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(windowVertices), windowVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
                          (const void *) (3 * sizeof(GLfloat)));
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    //upload RGBA image data
    for (int i = 0; i < RENDER_IMG_NUM; ++i) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, m_TextureIds[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_RenderImages[i].width, m_RenderImages[i].height,
                     0, GL_RGBA,
                     GL_UNSIGNED_BYTE, m_RenderImages[i].ppPlane[0]);
        glBindTexture(GL_TEXTURE_2D, GL_NONE);
    }

//    当绘制一个有不透明和透明物体的场景的时候，大体的原则如下：
//    1. 先绘制所有不透明的物体。
//    2. 对所有透明的物体排序。
//    3. 按顺序绘制所有透明的物体。

//    排序透明物体的一种方法是，从观察者视角获取物体的距离。
//    接下来我们把距离和它对应的位置向量存储到一个STL库的map数据结构中。
//    map会自动根据键值(Key)对它的值排序，所以只要我们添加了所有的位置，
//    并以它的距离作为键，它们就会自动根据距离值排序了。

    // transparent window locations
    // --------------------------------
    std::vector<glm::vec3> windowsTrans
            {
                    glm::vec3(-1.5f, 0.0f, -0.48f),
                    glm::vec3( 1.5f, 0.0f, 0.51f),
                    glm::vec3( 0.0f, 0.0f, 0.7f),
                    glm::vec3(-0.3f, 0.0f, -2.3f),
                    glm::vec3( 0.5f, 0.0f, -0.6f)
            };
    // sort the transparent windows before rendering
    // ---------------------------------------------
    // 结果就是一个排序后的容器对象，它根据distance键值从低到高储存了每个窗户的位置。
    for (auto &windowsTran : windowsTrans) {
        glm::vec3 position = glm::vec3(2.0f,2.0f,2.0f);
        GLfloat distance = glm::length(position - windowsTran);
        sorted[distance] = windowsTran;
    }
}

void BlendingSample::LoadMultiImageWithIndex(int index, NativeImage *pImage) {
    GLBaseSample::LoadMultiImageWithIndex(index, pImage);
    LOGD("BlendingSample::LoadImage index=%d, pImage = %p", index, pImage->ppPlane[0])
    if (pImage) {
        m_RenderImages[index].width = pImage->width;
        m_RenderImages[index].height = pImage->height;
        m_RenderImages[index].format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImages[index]);
    }
}

void BlendingSample::Draw() {
    LOGD("BlendingSample::Draw()")

    if (m_ProgramObj == GL_NONE) return;

    glUseProgram(m_ProgramObj);

    float ratio = (float) m_Width / (float) m_Height;
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // 启用 OpenGL ES 深度测试
    glEnable(GL_DEPTH_TEST);
    // 启用 OpenGL ES 混合使用
    glEnable(GL_BLEND);
    // 设置混合因子
    // 设置混合的方式，其中 sfactor 表示源因子，dfactor 表示目标因子。
    // GL_SRC_ALPHA 表示源因子取值为源颜色的 alpha
    // GL_ONE_MINUS_SRC_ALPHA 表示目标因子取值为 1- alpha（源颜色的 alpha）

    // 操作符默认为 GL_FUNC_ADD ，即加权相加。
    // 混合公式变成了 源颜色向量 × alpha + 目标颜色向量 × （1- alpha）
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //绘制箱子（不透明）
    glBindVertexArray(m_VaoIds[0]);
    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[0]);
    glUniform1i(m_SamplerLoc, 0);

    UpdateMatrix(m_MVPMatrix, m_AngleX, m_AngleY, 1.0, glm::vec3(-1.0f, 0.0f, -1.0f), ratio);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);

    UpdateMatrix(m_MVPMatrix, m_AngleX, m_AngleY, 1.0, glm::vec3(2.0f, 0.0f, 0.0f), ratio);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(0);


    //绘制地板（不透明）
    glBindVertexArray(m_VaoIds[1]);
    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[1]);
    glUniform1i(m_SamplerLoc, 0);

    UpdateMatrix(m_MVPMatrix, m_AngleX, m_AngleY, 1.0, glm::vec3(0.0f, 0.0f, 0.0f), ratio);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    GO_CHECK_GL_ERROR()
    glBindVertexArray(0);


    //绘制窗户（透明）
    glBindVertexArray(m_VaoIds[2]);
    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureIds[2]);
    glUniform1i(m_SamplerLoc, 0);


    // 容器 sorted 根据窗户距观察者的距离进行排序
    // 之后，这次在渲染的时候，我们将以逆序（从远到近）从map中获取值，之后以正确的顺序绘制对应的窗户
    // 我们使用了map的一个反向迭代器(Reverse Iterator)，反向遍历其中的条目，
    // 并将每个窗户四边形位移到对应的窗户位置上。
    // 这是排序透明物体的一个比较简单的实现，它能够修复之前的问题，现在场景看起来是这样的：
    for (auto it = sorted.rbegin(); it != sorted.rend(); ++it) {
        UpdateMatrix(m_MVPMatrix, m_AngleX, m_AngleY, 1.0, it->second, ratio);
        glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
    GO_CHECK_GL_ERROR()
    glBindVertexArray(0);

    // 禁用 OpenGL ES 深度测试
    glDisable(GL_DEPTH_TEST);
    // 禁用 OpenGL ES 混合使用
    glDisable(GL_BLEND);
}

void BlendingSample::Shutdown() {
    GLBaseSample::Shutdown();
    glDeleteBuffers(3, m_VboIds);
    glDeleteVertexArrays(3, m_VaoIds);
    glDeleteTextures(3, m_TextureIds);
    m_ProgramObj = GL_NONE;
}

void
BlendingSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
}

void BlendingSample::UpdateMatrix(glm::mat4 &mvpMatrix, int angleXRotate, int angleYRotate,
                                  float scale, glm::vec3 transVec3, float ratio) {
    LOGD("BlendingSample::UpdateMatrix angleX = %d, angleY = %d, ratio = %f", angleXRotate,
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
            glm::vec3(0.5, 1, 3), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 Model = glm::mat4(1.0f);
    Model = glm::scale(Model, glm::vec3(scale, scale, scale));
    Model = glm::rotate(Model, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    Model = glm::rotate(Model, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    Model = glm::translate(Model, transVec3);

    mvpMatrix = Projection * View * Model;
}
