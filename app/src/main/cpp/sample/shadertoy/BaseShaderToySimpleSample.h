//
// Created by OuyangPeng on 2021/12/15.
//

#ifndef OPENGLESDEMO_BASESHADERTOYSIMPLESAMPLE_H
#define OPENGLESDEMO_BASESHADERTOYSIMPLESAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

class BaseShaderToySimpleSample : public GLBaseSample {

public:
    BaseShaderToySimpleSample(const char *fragmentShaderPath);

    virtual ~BaseShaderToySimpleSample() = default;

    virtual void create();

    virtual void draw();

    virtual void shutdown();

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

private:
    const char *mFragmentShaderPath;

    GLint m_SamplerLoc;
    GLint iTime;
    GLint iResolution;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[2];
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};


#endif //OPENGLESDEMO_BASESHADERTOYSIMPLESAMPLE_H
