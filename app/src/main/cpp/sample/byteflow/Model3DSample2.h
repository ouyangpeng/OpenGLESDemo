//
// Created by OuyangPeng on 2021/12/28.
//

#ifndef OPENGLESDEMO_MODEL3DSAMPLE2_H
#define OPENGLESDEMO_MODEL3DSAMPLE2_H

#include <GLBaseSample.h>
#include <shader.h>
#include <model.h>

class Model3DSample2 : public GLBaseSample {
public:
    Model3DSample2();

    virtual ~Model3DSample2() = default;

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio);

private:
    glm::mat4 m_MVPMatrix;
    glm::mat4 m_ModelMatrix;
    Shader *m_pShader;
    Model *m_pModel;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;
};


#endif //OPENGLESDEMO_MODEL3DSAMPLE2_H
