//
// Created by OuyangPeng on 2021/12/20.
//

#ifndef OPENGLESDEMO_SCRATCHCARDSAMPLE_H
#define OPENGLESDEMO_SCRATCHCARDSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>
#include <vector>

#define TRIANGLE_NUM  43
#define EFFECT_RADIUS 0.03

using namespace glm;

class ScratchCardSample : public GLBaseSample {

public:
    ScratchCardSample();

    virtual ~ScratchCardSample();

    virtual void create();

    virtual void draw();

    virtual void shutdown();

    virtual void LoadImage(NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    virtual void SetTouchLocation(float x, float y);

    void UpdateMVPMatrix(mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

    void CalculateMesh(vec2 pre, vec2 cur);
private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[2];
    NativeImage m_RenderImage;
    mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
    vec3 m_pVtxCoords[TRIANGLE_NUM * 3];
    vec2 m_pTexCoords[TRIANGLE_NUM * 3];

    std::vector<vec4> m_PointVector;

    vec2 m_CurTouchPoint;
    vec2 m_PreTouchPoint;
    bool m_bReset;
};


#endif //OPENGLESDEMO_SCRATCHCARDSAMPLE_H
