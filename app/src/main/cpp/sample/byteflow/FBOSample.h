//
// Created by OuyangPeng on 2021/12/1.
//

#ifndef OPENGLESDEMO_FBOSAMPLE_H
#define OPENGLESDEMO_FBOSAMPLE_H

#include <BaseGLSample.h>

#define FBO_VERTEX_POS_INDX  0
#define FBO_TEXTURE_POS_INDX 1

#define FBO_VERTEX_POS_SIZE  3
#define FBO_TEXTURE_POS_SIZE 2


class FBOSample :public BaseGLSample{

public:
    FBOSample();

    virtual ~FBOSample();

    virtual void LoadImage(NativeImage *pImage);

    virtual void create();

    virtual void draw();

    virtual void shutdown();

private:
    GLuint m_ImageTextureId;
    GLuint m_FboTextureId;
    GLuint m_FboId;
    GLuint m_VaoIds[2];
    GLuint m_VboIds[4];
    GLint m_SamplerLoc;

    NativeImage m_RenderImage;

    GLuint m_FboProgramObj;
    GLint m_FboSamplerLoc;

    bool CreateFrameBufferObj();
};

//顶点坐标
static GLfloat fbo_vVertices[] = {
        -1.0f, -1.0f, 0.0f,
        1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        1.0f,  1.0f, 0.0f,
};

//正常纹理坐标
static GLfloat fbo_vTexCoors[] = {
        0.0f, 1.0f,
        1.0f, 1.0f,
        0.0f, 0.0f,
        1.0f, 0.0f,
};

//fbo 纹理坐标与正常纹理方向不同，原点位于左下角
static GLfloat fbo_vFboTexCoors[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        0.0f, 1.0f,
        1.0f, 1.0f,
};

// 注意索引从0开始!
static GLushort fbo_indices[] = {
        0, 1, 2,        // 第一个三角形
        1, 3, 2       // 第二个三角形
};

#endif //OPENGLESDEMO_FBOSAMPLE_H
