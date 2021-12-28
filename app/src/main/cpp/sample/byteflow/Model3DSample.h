//
// Created by OuyangPeng on 2021/12/28.
//

#ifndef OPENGLESDEMO_MODEL3DSAMPLE_H
#define OPENGLESDEMO_MODEL3DSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>
#include <shader.h>
#include <model.h>

class Model3DSample : public GLBaseSample {
public:
    Model3DSample();

    virtual ~Model3DSample() = default;

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


#endif //OPENGLESDEMO_MODEL3DSAMPLE_H
