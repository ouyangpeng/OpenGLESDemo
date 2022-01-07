//
// Created by OuyangPeng on 2021/12/20.
// 参考下面博客：
// OpenGL ES 实现头部形变和头部晃动效果
// https://blog.csdn.net/Kennethdroid/article/details/105208054
//

#include "RotaryHeadSample.h"


// 对头部区域进行形变就需要知道头部区域的关键点，头部区域的关键点可以通过 AI 算法来获得。
// 这里为了展示方便，将头部区域的关键点简化为 9 个，其中 8 个点位于头部边缘，一个点位于头部中心位置。
// 下面的头部区域关键点是写死的，实际项目中需要通过AI算法配合实时检测出对应的关键点
static float KEY_POINTS[KEY_POINTS_COUNT * 2] =
        {
                // 0-7 是 头部边缘   8是头部中心
                146, 101,//0
                174, 36, //1
                232, 6,  //2
                307, 38, //3
                335, 105,//4
                305, 218,//5
                250, 267,//6
                191, 218,//7
                237, 160,//8
        };

/**
 * DotProduct 函数表示计算两个向量的点积
 * @param a 第一个向量
 * @param b 第二个向量
 * @return 两个向量的点积
 */
static float DotProduct(vec2 a, vec2 b) {
    return a.x * b.x + a.y * b.y;
}

RotaryHeadSample::RotaryHeadSample() {

    m_SamplerLoc = GL_NONE;
    m_MVPMatLoc = GL_NONE;

    m_TextureId = GL_NONE;

    m_AngleX = 0;
    m_AngleY = 0;

    m_ScaleX = 1.0f;
    m_ScaleY = 1.0f;

    m_FrameIndex = 0;
}

RotaryHeadSample::~RotaryHeadSample() {
    NativeImageUtil::FreeNativeImage(&m_RenderImage);

}

void RotaryHeadSample::Create() {
    // 顶点着色器
    VERTEX_SHADER = GLUtils::openTextFile(
            "vertex/vertex_shader_big_head.glsl");
    // 片段着色器脚本
    FRAGMENT_SHADER = GLUtils::openTextFile(
            "fragment/fragment_shader_big_head.glsl");

    m_ProgramObj = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
    if (!m_ProgramObj) {
        LOGE("RotaryHeadSample::Init Create program fail")
        return;
    }

    m_SamplerLoc = glGetUniformLocation(m_ProgramObj, "s_TextureMap");
    m_MVPMatLoc = glGetUniformLocation(m_ProgramObj, "u_MVPMatrix");

    CalculateMesh(0);

    //=========================================== VBO和VAO相关===========================================
    // Generate VBO Ids and load the VBOs with data
    glGenBuffers(2, m_VboIds);
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_Vertices), m_Vertices, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_TexCoords), m_TexCoords, GL_STATIC_DRAW);

    // Generate VAO Id
    glGenVertexArrays(1, &m_VaoId);
    glBindVertexArray(m_VaoId);

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    GO_CHECK_GL_ERROR()

    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[1]);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), nullptr);
    glBindBuffer(GL_ARRAY_BUFFER, GL_NONE);
    GO_CHECK_GL_ERROR()

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

void RotaryHeadSample::Draw() {
    if (m_ProgramObj == GL_NONE) {
        LOGE("FaceSlenderSample::Draw() m_ProgramObj == GL_NONE return")
        return;
    }
    // 清除缓存
    glClear(GL_STENCIL_BUFFER_BIT | GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use the program object
    glUseProgram(m_ProgramObj);

    //变换矩阵
    UpdateMVPMatrix(m_MVPMatrix, m_AngleX, m_AngleY, (float) m_Width / (float) m_Height);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

    m_FrameIndex++;
    // 强度
    float ratio = (float) (m_FrameIndex % 100) * 1.0f / 100;

    // 计算新的网格
    CalculateMesh(static_cast<float>(ratio * 2 * MATH_PI));

    // 更新顶点数组
    glBindBuffer(GL_ARRAY_BUFFER, m_VboIds[0]);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_Vertices), m_Vertices);

    glBindVertexArray(m_VaoId);
    glUniformMatrix4fv(m_MVPMatLoc, 1, GL_FALSE, &m_MVPMatrix[0][0]);

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

    GLUtils::setFloat(m_ProgramObj, "u_type", 0);
    glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_COUNT * 3);

    GLUtils::setFloat(m_ProgramObj, "u_type", 1);
}

void RotaryHeadSample::Shutdown() {
    GLBaseSample::Shutdown();

    LOGD("RotaryHeadSample::Shutdown()")
    glDeleteBuffers(2, m_VboIds);
    glDeleteVertexArrays(1, &m_VaoId);
    glDeleteTextures(1, &m_TextureId);

}

void RotaryHeadSample::LoadImage(NativeImage *pImage) {
    LOGD("RotaryHeadSample::LoadImage pImage = %p", pImage->ppPlane[0])
    if (pImage) {
        m_RenderImage.width = pImage->width;
        m_RenderImage.height = pImage->height;
        m_RenderImage.format = pImage->format;
        NativeImageUtil::CopyNativeImage(pImage, &m_RenderImage);
    }
}

void
RotaryHeadSample::UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY) {
    GLBaseSample::UpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY);
    m_AngleX = static_cast<int>(rotateX);
    m_AngleY = static_cast<int>(rotateY);
    m_ScaleX = scaleX;
    m_ScaleY = scaleY;
}

void RotaryHeadSample::UpdateMVPMatrix(mat4 &mvpMatrix, int angleX, int angleY, float ratio) const {
    LOGD("RotaryHeadSample::UpdateMVPMatrix angleX = %d, angleY = %d, ratio = %f", angleX, angleY,
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

void RotaryHeadSample::CalculateMesh(float rotaryAngle) {
    vec2 centerPoint(KEY_POINTS[16] / (float)m_RenderImage.width, KEY_POINTS[17] /  (float)m_RenderImage.height);
//    centerPoint = RotaryKeyPoint(centerPoint, rotaryAngle);
    m_KeyPointsInts[8] = centerPoint;
    m_KeyPoints[8] = RotaryKeyPoint(centerPoint, rotaryAngle);
    for (int i = 0; i < KEY_POINTS_COUNT - 1; ++i) {
        vec2 inputPoint(KEY_POINTS[i * 2] /  (float)m_RenderImage.width, KEY_POINTS[i * 2 + 1] /  (float)m_RenderImage.height);
//        inputPoint = RotaryKeyPoint(inputPoint, rotaryAngle);
        m_KeyPoints[i] = RotaryKeyPoint(inputPoint, rotaryAngle);
        m_KeyPointsInts[i] = CalculateIntersection(inputPoint, centerPoint);
        LOGD("RotaryHeadSample::CalculateMesh index=%d, input[x,y]=[%f, %f], interscet[x, y]=[%f, %f]", i,
                m_KeyPoints[i].x, m_KeyPoints[i].y, m_KeyPointsInts[i].x, m_KeyPointsInts[i].y)
    }

    for (int i = 0; i < KEY_POINTS_COUNT - 1; ++i) {
        m_MeshPoints[i * 2] = m_KeyPointsInts[i];
        m_MeshPoints[i * 2 + 1] = m_KeyPoints[i];
    }
    m_MeshPoints[16] = m_KeyPointsInts[0];
    m_MeshPoints[17] = m_KeyPoints[0];

    //周围
    for (int i = 2; i < KEY_POINTS_COUNT * 2; ++i) {
        m_TexCoords[(i-2) * 3] = m_MeshPoints[i-2];
        m_TexCoords[(i-2) * 3 + 1] = m_MeshPoints[i-1];
        m_TexCoords[(i-2) * 3 + 2] = m_MeshPoints[i];
    }

    //中心
    for (int i = 0; i < KEY_POINTS_COUNT - 2; ++i) {
        m_TexCoords[(i + 16) * 3] = m_KeyPoints[i];
        m_TexCoords[(i + 16) * 3 + 1] = m_KeyPoints[i+1];
        m_TexCoords[(i + 16) * 3 + 2] = m_KeyPoints[8];
    }
    m_TexCoords[23 * 3 + 2] = m_KeyPoints[7];
    m_TexCoords[23 * 3 + 1] = m_KeyPoints[8];
    m_TexCoords[23 * 3 + 0] = m_KeyPoints[0];

    //处理四个角
    m_TexCoords[24 * 3]     = vec2(0,0);
    m_TexCoords[24 * 3 + 1] = m_KeyPointsInts[0];
    m_TexCoords[24 * 3 + 2] = m_KeyPointsInts[1];

    m_TexCoords[25 * 3]     = vec2(0,0);
    m_TexCoords[25 * 3 + 1] = m_KeyPointsInts[1];
    m_TexCoords[25 * 3 + 2] = m_KeyPointsInts[2];

    m_TexCoords[26 * 3]     = vec2(1,0);
    m_TexCoords[26 * 3 + 1] = m_KeyPointsInts[2];
    m_TexCoords[26 * 3 + 2] = m_KeyPointsInts[3];

    m_TexCoords[27 * 3]     = vec2(1,0);
    m_TexCoords[27 * 3 + 1] = m_KeyPointsInts[3];
    m_TexCoords[27 * 3 + 2] = m_KeyPointsInts[4];

    m_TexCoords[28 * 3]     = vec2(1,1);
    m_TexCoords[28 * 3 + 1] = m_KeyPointsInts[4];
    m_TexCoords[28 * 3 + 2] = m_KeyPointsInts[5];

    m_TexCoords[29 * 3]     = vec2(1,1);
    m_TexCoords[29 * 3 + 1] = m_KeyPointsInts[5];
    m_TexCoords[29 * 3 + 2] = m_KeyPointsInts[6];

    m_TexCoords[30 * 3]     = vec2(0,1);
    m_TexCoords[30 * 3 + 1] = m_KeyPointsInts[6];
    m_TexCoords[30 * 3 + 2] = m_KeyPointsInts[7];

    m_TexCoords[31 * 3]     = vec2(0,1);
    m_TexCoords[31 * 3 + 1] = m_KeyPointsInts[7];
    m_TexCoords[31 * 3 + 2] = m_KeyPointsInts[0];

    for (int i = 0; i < TRIANGLE_COUNT * 3; ++i) {
        //LOGD("m_TexCoords[%d]=(%f, %f)", i, m_TexCoords[i].x, m_TexCoords[i].y);
        m_Vertices[i] = vec3( m_TexCoords[i].x * 2 - 1, 1 - 2 * m_TexCoords[i].y, 0);
    }
}

/**
 * 直线 x=1、y=1 和纹理坐标轴连成了一个矩形，每个头部边缘的关键点和头部中心点确定一条直线，
 * 该直线会与矩形的边存在交点，我们用这些交点和头部关键点来构建这个呈辐射状的网格。
 * 如图所示：https://imgconvert.csdnimg.cn/aHR0cHM6Ly91cGxvYWQtaW1hZ2VzLmppYW5zaHUuaW8vdXBsb2FkX2ltYWdlcy8zMjM5OTMzLTQzMjFmMjdhZjBjM2RjMzYuanBn
 * 如上图所示，每个头部边缘关键点和头部中心点确定一条直线，这条直线可以用二元一次方程来表示，它与上述矩形边的交点，可以通过求解二元一次方程得出。
 * @param inputPoint    头部边缘关键点
 * @param centerPoint   头部中心点
 * @return 计算出来的交点
 */
vec2 RotaryHeadSample::CalculateIntersection(vec2 inputPoint, vec2 centerPoint) {
    vec2 outputPoint;

    //直线与 y 轴平行
    if (inputPoint.x == centerPoint.x) {
        vec2 pointA(inputPoint.x, 0);
        vec2 pointB(inputPoint.x, 1);

        float dA = distance(inputPoint, pointA);
        float dB = distance(inputPoint, pointB);
        outputPoint = dA > dB ? pointB : pointA;
        return outputPoint;
    }

    //直线与 x 轴平行
    if (inputPoint.y == centerPoint.y) {
        vec2 pointA(0, inputPoint.y);
        vec2 pointB(1, inputPoint.y);

        float dA = distance(inputPoint, pointA);
        float dB = distance(inputPoint, pointB);
        outputPoint = dA > dB ? pointB : pointA;
        return outputPoint;
    }

    // 二元一次方程  y = a*x + c
    float a, c;

    a = (inputPoint.y - centerPoint.y) / (inputPoint.x - centerPoint.x);

    c = inputPoint.y - a * inputPoint.x;

    //x=0, x=1, y=0, y=1 四条线交点

    //x=0
    vec2 point_0(0, c);
    float d0 = DotProduct((centerPoint - inputPoint), (centerPoint - point_0));

    if (c >= 0 && c <= 1 && d0 > 0)
        outputPoint = point_0;

    //x=1
    vec2 point_1(1, a + c);
    float d1 = DotProduct((centerPoint - inputPoint), (centerPoint - point_1));

    if ((a + c) >= 0 && (a + c) <= 1 && d1 > 0)
        outputPoint = point_1;

    //y=0
    vec2 point_2(-c / a, 0);
    float d2 = DotProduct((centerPoint - inputPoint), (centerPoint - point_2));

    if ((-c / a) >= 0 && (-c / a) <= 1 && d2 > 0)
        outputPoint = point_2;

    //y=1
    vec2 point_3((1 - c) / a, 1);
    float d3 = DotProduct((centerPoint - inputPoint), (centerPoint - point_3));

    if (((1 - c) / a) >= 0 && ((1 - c) / a) <= 1 && d3 > 0)
        outputPoint = point_3;

    return outputPoint;
}

/**
 * 如何实现头部晃动的效果呢？答案还是控制头部关键点的位置。
 * 简而言之就是，控制头部所有关键点统一按照某一圆的轨迹进行移动，我们这里指的头部关键点是在屏幕坐标系中纹理坐标所对应的点。
 *
 * 实现关键点按照某一圆的轨迹进行移动的函数
 * @param input 为头部关键点
 * @param rotaryAngle   为转动角度
 * @return
 */
vec2 RotaryHeadSample::RotaryKeyPoint(vec2 input, float rotaryAngle) {
    // 0.02f 表示圆的半径
    return input + vec2(cos(rotaryAngle), sin(rotaryAngle)) * 0.02f;
}

