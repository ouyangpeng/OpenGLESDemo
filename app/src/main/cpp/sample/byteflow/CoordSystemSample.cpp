//
// Created by OuyangPeng on 2021/12/10.
//

// 可以参考下面的博客理解代码
// NDK OpenGL ES 3.0 开发（八）：坐标系统
// https://blog.csdn.net/Kennethdroid/article/details/100898155

#include <glm/ext.hpp>
#include "CoordSystemSample.h"

CoordSystemSample::CoordSystemSample() {

    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;
}

CoordSystemSample::~CoordSystemSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}


void CoordSystemSample::create() {
    // create RGBA texture
    glGenTextures(1, &m_TextureId);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 编译链接用于普通渲染的着色器程序
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_coord_system.glsl");
    // 用于普通渲染的片段着色器脚本，简单纹理映射
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_texture_map.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (mProgram == GL_NONE) {
        LOGE("CoordSystemSample::Init mProgram == GL_NONE")
        return;
    }
    m_MVPMatLoc = glGetUniformLocation(mProgram, "u_MVPMatrix");
    m_SamplerLoc = glGetUniformLocation(mProgram, "s_TextureMap");

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(3, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cs_vVertices), cs_vVertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cs_vTexCoors), cs_vTexCoors, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cs_indices), cs_indices, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(CS_VERTEX_POS_INDX);
    glVertexAttribPointer(CS_VERTEX_POS_INDX, 3, GL_FLOAT, GL_FALSE,
                          3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(CS_TEXTURE_POS_INDX);
    glVertexAttribPointer(CS_TEXTURE_POS_INDX, 2, GL_FLOAT, GL_FALSE,
                          2 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_VboIds[2]);

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

void CoordSystemSample::draw() {

    if (mProgram == GL_NONE || m_TextureId == GL_NONE) return;

    // Clear the color buffer
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 旋转角度变换，更新变换矩阵
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) mWidth / (float) mHeight);

    // Use the program object
    glUseProgram(mProgram);

    glBindVertexArray(m_VaoId);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    glUniform1i(m_SamplerLoc, 0);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void CoordSystemSample::shutdown() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteTextures(1, &m_TextureId);
    }
}

void CoordSystemSample::LoadImage(NativeImage *pImage) {
    LOGD("CoordSystemSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void
CoordSystemSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    m_AngleX = (int) rotateX;
    m_AngleY = (int) rotateY;
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}
/**
 * OpenGL 坐标系中每个顶点的 x，y，z 坐标都应该在 -1.0 到 1.0 之间，超出这个坐标范围的顶点都将不可见。
    将一个物体（图像）渲染到屏幕上，通常经过将物体坐标转换为标准化设备坐标，然后再将标准化设备坐标转化为屏幕坐标的过程。
    该过程通常涉及多个坐标系统的变换，将所有顶点转换为片段之前，顶点需要处于不同的坐标系统进行计算，对我们来说比较重要的有 5 个坐标系统：
            局部空间(Local Space，或者物体空间(Object Space))
            世界空间(World Space)
            观察空间(View Space，
            裁剪空间(Clip Space)
            屏幕空间(Screen Space)

局部空间
局部空间 (Local Space) 是指对象所在的坐标空间，坐标原点由你自己指定，模型的所有顶点相对于你的对象来说都是局部的。

世界空间
在世界空间（World Space）主要实现对象的平移、缩放、旋转变换，将它们放在我们指定的位置，这些变换是通过模型矩阵(Model Matrix)实现的。
在 C/C++ 中可以利用 GLM 构建模型矩阵:
        glm::mat4 Model = glm::mat4(1.0f); //单位矩阵
        Model = glm::scale(Model, glm::vec3(2.0f, 2.0f, 2.0f)); //缩放
        Model = glm::rotate(Model, MATH_PI/2, glm::vec3(1.0f, 0.0f, 0.0f)); //沿 x 轴旋转 90 度
        Model = glm::translate(Model, glm::vec3(0.0f, 1.0f, 0.0f)); //沿 y 轴正方向平移一个单位

观察空间
观察空间(View Space)也被称为 OpenGL 相机空间，即从摄像机的角度观察到的空间，
 它将对象的世界空间的坐标转换为观察者视野前面的坐标，
 这通常是由一系列的平移和旋转的组合来平移和旋转场景从而使得特定的对象被转换到摄像机前面，
 这些组合在一起的转换通常存储在一个**观察矩阵(View Matrix)**里。
在 C/C++ 中可以利用 GLM 构建观察矩阵:
        // View matrix
        glm::mat4 View = glm::lookAt(
			glm::vec3(0, 0, 3), // Camera is at (0,0,1), in World Space 相机位置
			glm::vec3(0, 0, 0), // and looks at the origin 观察点坐标
			glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down) 相机 up 方向，即相机头部朝向
);

裁剪空间
裁剪空间(Clip Space)是用来裁剪观察对象的空间，在一个顶点着色器运行的最后，OpenGL 期望所有的坐标都能落在一个给定的范围内，且任何在这个范围之外的点都应该被裁剪掉。**投影矩阵(Projection Matrix)**用来将顶点坐标从观察空间转换到裁剪空间。

投影矩阵一般分为两种：正交投影（Orthographic Projection）和透视投影（Perspective Projection）。

 正交投影是一种平行投影，投影点与原顶点的连线相互平行，且物体不产生“近大远小”的视觉效果。
 在 C/C++ 中可以利用 GLM 构建正交投影矩阵:
    glm::mat4 Projection = glm::ortho(-ratio, ratio, -1.0f, 1.0f, 0.0f, 100.0f); //ratio 一般表示视口的宽高比，width/height
    前两个参数指定了平截头体的左右坐标，第三和第四参数指定了平截头体的底部和上部。通过这四个参数我们定义了近平面和远平面的大小，
    然后第五和第六个参数则定义了近平面和远平面的距离。这个指定的投影矩阵将处于这些 x，y，z 范围之间的坐标转换到标准化设备坐标系中。

 透视投影的投影线相交于一点，可以用来模拟真实世界“近大远小”的视觉效果。
 在 C/C++ 中可以利用 GLM 构建透视投影矩阵:
    glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f); //ratio 一般表示视口的宽高比，width/height,
    它的第一个参数定义了 fov 的值，它表示的是视野(Field of View)，并且设置了观察空间的大小。
    对于一个真实的观察效果，它的值经常设置为 45.0，但想要看到更多结果你可以设置一个更大的值。
    第二个参数设置了宽高比，由视口的高除以宽。第三和第四个参数设置了平截头体的近和远平面。
    我们经常设置近距离为 0.1 而远距离设为 100.0 。
    所有在近平面和远平面的顶点且处于平截头体内的顶点都会被渲染。

    最后整个坐标系统的变换矩阵可以用一个矩阵表示 MVPMatrix = Projection * View * Model;。
 */

/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void CoordSystemSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY,
                                        float ratio) const {
    LOGD("CoordSystemSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY,
         ratio)
    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    //glm::mat4 mProjection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 mProjection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    glm::mat4 mProjection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

    // View matrix
    glm::mat4 mView = glm::lookAt(
            glm::vec3(0, 0, 4), // Camera is at (0,0,1), in World Space
            glm::vec3(0, 0, 0), // and looks at the origin
            glm::vec3(0, 1, 0)  // Head is up (set to 0,-1,0 to look upside-down)
    );

    // Model matrix
    glm::mat4 mModel = glm::mat4(1.0f);
    mModel = glm::scale(mModel, glm::vec3(m_ScaleX, m_ScaleY, 1.0f));
    mModel = glm::rotate(mModel, radiansX, glm::vec3(1.0f, 0.0f, 0.0f));
    mModel = glm::rotate(mModel, radiansY, glm::vec3(0.0f, 1.0f, 0.0f));
    mModel = glm::translate(mModel, glm::vec3(0.0f, 0.0f, 0.0f));

    mvpMatrix = mProjection * mView * mModel;
}




