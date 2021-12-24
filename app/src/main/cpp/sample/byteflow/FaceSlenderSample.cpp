//
// Created by OuyangPeng on 2021/12/19 0019.
//

#include "FaceSlenderSample.h"


//
// Created by OuyangPeng on 2021/12/19 0019.
// 参考博客：

// OpenGL ES 实现瘦脸大眼效果
// https://blog.csdn.net/Kennethdroid/article/details/104907763

// 在OpenGL中利用shader进行实时瘦脸大眼等脸型微调
// https://juejin.cn/post/6844903977176268813
// https://github.com/ymkil/MagicCamera
//
#include <PointUtils.h>
#include "FaceSlenderSample.h"

// 下面的关键点  是写死的
// 实际项目中需要使用  人脸关键点的检测技术，实时返回下面的关键点

//左脸颊关键点
float LeftCheekKeyPoint[] = {211, 363};
//右脸颊关键点
float RightCheekPoint[] = {471, 365};
//左侧控制点
float LeftSlenderCtlPoint[] = {211, 512};
//右侧控制点
float RightSlenderCtlPoint[] = {477, 509};
//下巴关键点
float ChinKeyPoint[] = {336, 565};


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

FaceSlenderSample::FaceSlenderSample() {
    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;
    m_VaoId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_FrameIndex = 0;
}

FaceSlenderSample::~FaceSlenderSample() {
    LOGD("FaceSlenderSample::~FaceSlenderSample()")
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void FaceSlenderSample::Create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_coord_system.glsl");
    // 片段着色器脚本
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_face_slender.glsl");

    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!m_ProgramObj) {
        LOGE("FaceSlenderSample::Init Create program fail")
        return;
    }

    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");
    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");

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
    // 生成一个纹理，将纹理 id 赋值给 m_TextureId
    glGenTextures(1, &m_TextureId);
    // 将纹理 m_TextureId 绑定到类型 GL_TEXTURE_2D 纹理
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    //设置纹理 S 轴（横轴）的拉伸方式为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    //设置纹理 T 轴（纵轴）的拉伸方式为截取
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //设置纹理采样方式
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // 设置清除颜色
    glClearColor(0.1, 0.1, 0.1, 0.1);
}

void FaceSlenderSample::Draw() {
    if (m_ProgramObj == GL_NONE) {
        LOGE("FaceSlenderSample::Draw() m_ProgramObj == GL_NONE return")
        return;
    }
    // 清除缓存
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the program object
    glUseProgram(m_ProgramObj);

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) m_Width / (float) m_Height);

    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);


    m_FrameIndex++;
    float ratio = (float) (m_FrameIndex % 100) * 1.0f / 100;
    ratio = (m_FrameIndex / 100) % 2 == 1 ? (1 - ratio) : ratio;

    float effectRadius = PointUtil::Distance(
            PointF(LeftCheekKeyPoint[0], LeftCheekKeyPoint[1]),
            PointF(ChinKeyPoint[0], ChinKeyPoint[1])
    ) / 2;
    LOGD("FaceSlenderSample::Draw() ratio=%f, effectRadius=%f", ratio, effectRadius)

    // 赋值给片段着色器
    GLUtils::setFloat(m_ProgramObj, "u_reshapeRatio", ratio);
    GLUtils::setFloat(m_ProgramObj, "u_reshapeRadius", effectRadius);
    GLUtils::setVec4(m_ProgramObj, "u_preCtrlPoints",
                     LeftSlenderCtlPoint[0] / (float) m_RenderImage.width,
                     LeftSlenderCtlPoint[1] / (float) m_RenderImage.height,
                     RightSlenderCtlPoint[0] / (float) m_RenderImage.width,
                     RightSlenderCtlPoint[1] / (float) m_RenderImage.height);

    PointF leftCurPoint = PointUtil::PointAdd(
            PointF(LeftCheekKeyPoint[0], LeftCheekKeyPoint[1]),
            PointF(ChinKeyPoint[0], ChinKeyPoint[1]));
    leftCurPoint = PointUtil::PointDivide(leftCurPoint, 2);

    PointF rightCurPoint = PointUtil::PointAdd(
            PointF(RightCheekPoint[0], RightCheekPoint[1]),
            PointF(ChinKeyPoint[0], ChinKeyPoint[1]));
    rightCurPoint = PointUtil::PointDivide(rightCurPoint, 2);

    GLUtils::setVec4(m_ProgramObj, "u_curCtrlPoints",
                     leftCurPoint.x / (float) m_RenderImage.width,
                     leftCurPoint.y / (float) m_RenderImage.height,
                     rightCurPoint.x / (float) m_RenderImage.width,
                     rightCurPoint.y / (float) m_RenderImage.height);
    GLUtils::setVec2(m_ProgramObj, "u_texSize",
                     (float) m_RenderImage.width,
                     (float) m_RenderImage.height);

    // Upload RGBA image data
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);
    //加载 RGBA 格式的图像数据
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA,
                 m_RenderImage.width, m_RenderImage.height,
                 0, GL_RGBA, GL_UNSIGNED_BYTE,
                 m_RenderImage.ppPlane[0]);
    glBindTexture(GL_TEXTURE_2D, GL_NONE);

    // Bind the RGBA map
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, m_TextureId);

    glUniform1i(m_SamplerLoc, 0);


    // Bind VAO
    glBindVertexArray(m_VaoId);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);
}

void FaceSlenderSample::Shutdown() {
    if (m_ProgramObj) {
        LOGD("FaceSlenderSample::Shutdown()")
        glDeleteProgram(m_ProgramObj);
        glDeleteBuffers(3, m_VboIds);
        glDeleteVertexArrays(1, &m_VaoId);
        glDeleteTextures(1, &m_TextureId);
    }
}

void FaceSlenderSample::LoadImage(NativeImage *pImage) {
    LOGD("FaceSlenderSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void
FaceSlenderSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void
FaceSlenderSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY,
                                   float ratio) const {
    LOGD("FaceSlenderSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY,
         ratio)
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

