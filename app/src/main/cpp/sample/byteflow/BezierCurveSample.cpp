/**
 * 参考下面博客：
 * OpenGL ES 绘制贝塞尔曲线  https://blog.csdn.net/Kennethdroid/article/details/104721096
 */

#include "BezierCurveSample.h"

// 定义是绘制单纯的贝塞尔曲线 还是  曲线扇形
//#define DRAW_POINTS

//取 256 个点
#define POINTS_NUM           256

#define POINTS_PRE_TRIANGLES        3

BezierCurveSample::BezierCurveSample() {

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

BezierCurveSample::~BezierCurveSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);
}

void BezierCurveSample::create() {
    // 编译链接用于普通渲染的着色器程序
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_bezier_curve.glsl");
    // 用于普通渲染的片段着色器脚本，简单纹理映射
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_bezier_curve.glsl");
    mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!mProgram) {
        LOGE("BezierCurveSample::Init create program fail")
    }

    // 获取 t 的取值数组，实际上就是对 t∈[0，1] 区间进行等间隔取值：
    int tDataSize = POINTS_NUM * POINTS_PRE_TRIANGLES;
    auto *p_tData = new float[tDataSize];

    for (int i = 0; i < tDataSize; i += POINTS_PRE_TRIANGLES) {
#ifdef DRAW_POINTS
        // 获取 t 的取值数组，实际上就是对 t∈[0，1] 区间进行等间隔取值：
        float t0 = (float) i / tDataSize;
        float t1 = (float) (i + 1) / tDataSize;
        float t2 = (float) (i + 2) / tDataSize;

        p_tData[i] = t0;
        p_tData[i + 1] = t1;
        p_tData[i + 2] = t2;
#else
        // 接下来我们基于贝塞尔曲线去绘制曲边扇形（填充曲线与 x 轴之间的区域），
        // 则需要 OpenGL 绘制三角形实现，
        // 还要重新输入 t 的取值数组，使得每输出 3 个点包含一个原点，类似于绘制扇形。
        float t = (float) i / (float) tDataSize;
        float t1 = (float) (i + 3) / (float) tDataSize;

        p_tData[i] = t;
        p_tData[i + 1] = t1;
        p_tData[i + 2] = -1;
#endif
    }

    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(1, &m_VboId);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * tDataSize, p_tData, GL_STATIC_DRAW);

    delete[] p_tData;

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboId);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 1, GL_FLOAT, GL_FALSE, sizeof(GLfloat), nullptr);

    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    glBindVertexArray(GL_NONE);

    // 设置清除颜色
    glClearColor(0.1, 0.1, 0.1, 0.1);
}


void BezierCurveSample::draw() {
    LOGD("BezierCurveSample::Draw()")

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (mProgram == GL_NONE) return;

    // Use the program object
    glUseProgram(mProgram);

    // 启动混合模式
    glEnable(GL_BLEND);
    // glBlendFuncSeparate (GLenum sfactorRGB, GLenum dfactorRGB, GLenum sfactorAlpha, GLenum dfactorAlpha);
    // 参数说明：
    // sfactorRGB   指定输入片段 红、绿和蓝色分量的混合系数
    // dfactorRGB   指定目标像素 红、绿和蓝色分量的混合系数
    // sfactorAlpha 指定输入片段 Alpha值的混合系数
    // dfactorAlpha 指定目标像素 Alpha值的混合系数

    // 混合系数枚举值                    RGB混合因子                Alpha混合因子
    // GL_ZERO                          (0,0,0)                          0
    // GL_ONE                           (1,1,1)                          1
    // GL_SRC_COLOR                     (Rs,Gs,Bs)                       As
    // GL_ONE_MINUS_SRC_COLOR           (1-Rs,1-Gs,1-Bs)                1-As

    // 其他的 不介绍了
    //    #define GL_SRC_ALPHA                      0x0302
    //    #define GL_ONE_MINUS_SRC_ALPHA            0x0303
    //    #define GL_DST_ALPHA                      0x0304
    //    #define GL_ONE_MINUS_DST_ALPHA            0x0305
    //    #define GL_DST_COLOR                      0x0306
    //    #define GL_ONE_MINUS_DST_COLOR            0x0307
    //    #define GL_SRC_ALPHA_SATURATE             0x0308
    //    #define GL_CONSTANT_COLOR                 0x8001
    //    #define GL_ONE_MINUS_CONSTANT_COLOR       0x8002
    //    #define GL_CONSTANT_ALPHA                 0x8003
    //    #define GL_ONE_MINUS_CONSTANT_ALPHA       0x8004
    glBlendFuncSeparate(GL_ONE, GL_ONE_MINUS_SRC_COLOR,
                        GL_ONE, GL_ONE_MINUS_SRC_ALPHA);

    // glBlendEquationSeparate (GLenum modeRGB, GLenum modeAlpha);
    // 参数说明：
    // modeRGB          为 红、绿和蓝颜色分量  指定混合运算符
    // modeAlpha        为 Alpha分量  指定混合运算符
    // 有效值为：GL_FUNC_ADD、GL_FUNC_SUBTRACT、GL_FUNC_REVERSE_SUBTRACT、GL_MIN、GL_MAX
    glBlendEquationSeparate(GL_FUNC_ADD, GL_FUNC_ADD);

    glBindVertexArray(m_VaoId);

    m_FrameIndex++;

    // ==============================绘制第一个==============================
    //绘制 一条 贝塞尔曲线
    GLUtils::setVec4(mProgram, "u_StartEndPoints", glm::vec4(-1, 0, 1, 0));
    GLUtils::setVec4(mProgram, "u_ControlPoints", glm::vec4(-0.04f, 0.99f, 0.0f, 0.99f));
    GLUtils::setVec4(mProgram, "u_Color", glm::vec4(1.0f, 0.3f, 0.0f, 1.0f));

    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) mWidth / (float) mHeight);
    GLUtils::setMat4(mProgram, "u_MVPMatrix", m_MVPMatrix);

    float offset = (float) (m_FrameIndex % 100) * 1.0f / 100;
    offset = (m_FrameIndex / 100) % 2 == 1 ? (1 - offset) : offset;
    GLUtils::setFloat(mProgram, "u_Offset", offset);
    // 绘制
    DrawArray();

    //旋转 180 度后再绘制 一条 贝塞尔曲线
    UpdateMVPMatrix(m_MVPMatrix, 180, m_AngleY, (float) mWidth / (float) mHeight);
    GLUtils::setMat4(mProgram, "u_MVPMatrix", m_MVPMatrix);
    // 绘制
    DrawArray();

    // ==============================绘制第二个==============================
    int newIndex = m_FrameIndex + 33;
    offset = (float) (newIndex % 100) * 1.0f / 100;
    offset = (newIndex / 100) % 2 == 1 ? (1 - offset) : offset;
    GLUtils::setFloat(mProgram, "u_Offset", offset);
    GLUtils::setVec4(mProgram, "u_Color", glm::vec4(0.0f, 0.3f, 0.8f, 1.0f));
    GLUtils::setVec4(mProgram, "u_ControlPoints", glm::vec4(-0.8f, 0.99f,
                                                            0.0f, 0.0f));
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) mWidth / (float) mHeight);
    GLUtils::setMat4(mProgram, "u_MVPMatrix", m_MVPMatrix);

    DrawArray();

    UpdateMVPMatrix(m_MVPMatrix, 180, m_AngleY, (float) mWidth / (float) mHeight);
    GLUtils::setMat4(mProgram, "u_MVPMatrix", m_MVPMatrix);

    DrawArray();

    // ==============================绘制第三个==============================
    newIndex = newIndex + 33;
    offset = (float) (newIndex % 100) * 1.0f / 100;
    offset = (newIndex / 100) % 2 == 1 ? (1 - offset) : offset;
    GLUtils::setFloat(mProgram, "u_Offset", offset);
    GLUtils::setVec4(mProgram, "u_Color", glm::vec4(0.1f, 0.6f, 0.3f, 1.0f));
    GLUtils::setVec4(mProgram, "u_ControlPoints", glm::vec4(0.0f, 0.0f, 0.8f, 0.99f));
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) mWidth / (float) mHeight);
    GLUtils::setMat4(mProgram, "u_MVPMatrix", m_MVPMatrix);

    DrawArray();

    UpdateMVPMatrix(m_MVPMatrix, 180, m_AngleY, (float) mWidth / (float) mHeight);
    GLUtils::setMat4(mProgram, "u_MVPMatrix", m_MVPMatrix);

    DrawArray();

    // ==============================绘制第四个==============================
    newIndex = newIndex + 33;
    offset = (float) (newIndex % 100) * 1.0f / 100;
    offset = (newIndex / 100) % 2 == 1 ? (1 - offset) : offset;
    GLUtils::setFloat(mProgram, "u_Offset", offset);
    GLUtils::setVec4(mProgram, "u_Color", glm::vec4(1.0f, 0.0f, 0.3f, 1.0f));
    GLUtils::setVec4(mProgram, "u_ControlPoints", glm::vec4(-0.2f, 0.99f, 0.0f, 0.0f));
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) mWidth / (float) mHeight);
    GLUtils::setMat4(mProgram, "u_MVPMatrix", m_MVPMatrix);

    DrawArray();

    UpdateMVPMatrix(m_MVPMatrix, 180, m_AngleY, (float) mWidth / (float) mHeight);
    GLUtils::setMat4(mProgram, "u_MVPMatrix", m_MVPMatrix);

    DrawArray();

    // ==============================绘制第五个==============================
    newIndex = newIndex + 33;
    offset = (float) (newIndex % 100) * 1.0f / 100;
    offset = (newIndex / 100) % 2 == 1 ? (1 - offset) : offset;
    GLUtils::setFloat(mProgram, "u_Offset", offset);
    GLUtils::setVec4(mProgram, "u_Color", glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
    GLUtils::setVec4(mProgram, "u_ControlPoints", glm::vec4(0.0f, 0.0f, 0.2f, 0.99f));
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) mWidth / (float) mHeight);
    GLUtils::setMat4(mProgram, "u_MVPMatrix", m_MVPMatrix);

    DrawArray();

    UpdateMVPMatrix(m_MVPMatrix, 180, m_AngleY, (float) mWidth / (float) mHeight);
    GLUtils::setMat4(mProgram, "u_MVPMatrix", m_MVPMatrix);

    DrawArray();
    // ==============================绘制完毕==============================

    glDisable(GL_BLEND);
}

void BezierCurveSample::shutdown() {
    if (mProgram) {
        glDeleteProgram(mProgram);
        glDeleteBuffers(1, &m_VboId);
        glDeleteVertexArrays(1, &m_VaoId);
    }
}


/**
 * @param angleX 绕X轴旋转度数
 * @param angleY 绕Y轴旋转度数
 * @param ratio 宽高比
 * */
void BezierCurveSample::UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY,
                                        float ratio) const {
    LOGD("BezierCurveSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f",
         angleX, angleY, ratio)

    angleX = angleX % 360;
    angleY = angleY % 360;

    //转化为弧度角
    auto radiansX = static_cast<float>(MATH_PI / 180.0f * angleX);
    auto radiansY = static_cast<float>(MATH_PI / 180.0f * angleY);


    // Projection matrix
    glm::mat4 Projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.1f, 100.0f);
    //glm::mat4 Projection = glm::frustum(-ratio, ratio, -1.0f, 1.0f, 4.0f, 100.0f);
    //glm::mat4 Projection = glm::perspective(45.0f, ratio, 0.1f, 100.f);

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

void
BezierCurveSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void BezierCurveSample::DrawArray() {
#ifdef DRAW_POINTS // 绘制纯粹的贝塞尔曲线
    glDrawArrays(GL_POINTS, 0, POINTS_NUM * POINTS_PRE_TRIANGLES);
#else // 绘制曲边扇形
    glDrawArrays(GL_TRIANGLES, 0, POINTS_NUM * POINTS_PRE_TRIANGLES);
    glDrawArrays(GL_LINES, 0, POINTS_NUM * POINTS_PRE_TRIANGLES);
#endif
}
