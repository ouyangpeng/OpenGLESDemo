//
// Created by OuyangPeng on 2021/12/6.
//

#ifndef OPENGLESDEMO_FBOLEGLENGTHENSAMPLE_H
#define OPENGLESDEMO_FBOLEGLENGTHENSAMPLE_H

#include <BaseGLSample.h>

#include <glm/detail/type_mat4x4.hpp>


#define VERTICAL_STRETCH_8_POINTS          0x10
#define VERTICAL_STRETCH_TOP_6_POINTS      0x11
#define VERTICAL_STRETCH_BOTTOM_6_POINTS   0x12
#define VERTICAL_STRETCH_4_POINTS          0x13

#define HORIZONTAL_STRETCH_8_POINTS        0x20
#define HORIZONTAL_STRETCH_LEFT_6_POINTS   0x21
#define HORIZONTAL_STRETCH_RIGHT_6_POINTS  0x22
#define HORIZONTAL_STRETCH_4_POINTS        0x23

class FBOLegLengthenSample : public BaseGLSample{
public:
    FBOLegLengthenSample();

    virtual ~FBOLegLengthenSample();

    virtual void LoadImage(NativeImage *pImage);

    virtual void create();

    virtual void draw();

    virtual void shutdown();

    bool CreateFrameBufferObj();

private:
    GLuint m_ImageTextureId;
    GLuint m_FboTextureId;
    GLuint m_FboId;
    GLuint m_VaoIds[2] = {GL_NONE};;
    GLuint m_VboIds[6] = {GL_NONE};;
    GLint m_SamplerLoc;
    NativeImage m_RenderImage;
    GLuint m_FboProgramObj;
    GLint m_FboSamplerLoc;

    glm::mat4 m_FboMVPMatrix;
    GLint m_FboMVPMatLoc;

    glm::mat4 m_MVPMatrix;
    GLint m_MVPMatLoc;

    GLfloat m_dt;
    bool    m_isgo;

    RectF m_StretchRect;
    int   m_StretchMode;

    bool  m_bIsVerticalMode;
};


#endif //OPENGLESDEMO_FBOLEGLENGTHENSAMPLE_H
