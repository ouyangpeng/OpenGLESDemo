//
// Created by OuyangPeng on 2021/12/10.
//

#ifndef OPENGLESDEMO_COORDSYSTEMSAMPLE_H
#define OPENGLESDEMO_COORDSYSTEMSAMPLE_H

#include <GLBaseSample.h>
#include <glm/detail/type_mat4x4.hpp>

#define CS_VERTEX_POS_INDX  0
#define CS_TEXTURE_POS_INDX 1

class CoordSystemSample : public GLBaseSample {
public:
    CoordSystemSample();

    virtual ~CoordSystemSample();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadImage(NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[3];
    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};

//顶点坐标
static GLfloat cs_vVertices[] = {
        -1.0f, 1.0f, 0.0f,  // Position 0
        -1.0f, -1.0f, 0.0f,  // Position 1
        1.0f, -1.0f, 0.0f,  // Position 2
        1.0f, 1.0f, 0.0f,  // Position 3
};

//正常纹理坐标
static GLfloat cs_vTexCoors[] = {
        0.0f, 0.0f,        // TexCoord 0
        0.0f, 1.0f,        // TexCoord 1
        1.0f, 1.0f,        // TexCoord 2
        1.0f, 0.0f         // TexCoord 3
};

// 注意索引从0开始!
static GLushort cs_indices[] = {
        0, 1, 2,        // 第一个三角形
        0, 2, 3         // 第二个三角形
};
#endif //OPENGLESDEMO_COORDSYSTEMSAMPLE_H
