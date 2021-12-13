//
// Created by OuyangPeng on 2021/12/13.
//

#ifndef OPENGLESDEMO_PARTICLESSAMPLE2_H
#define OPENGLESDEMO_PARTICLESSAMPLE2_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>

#define MAX_PARTICLES 500

struct Particle {
    //offset 控制粒子的位置
    GLfloat dx, dy, dz;
    //speed 控制粒子的运动速度
    GLfloat dxSpeed, dySpeed, dzSpeed;
    //r,g,b,a 控制粒子的颜色
    GLubyte r, g, b, a;
    //控制粒子的生命值
    GLfloat life;
    GLfloat cameraDistance;

    Particle() {
        dx = 0.0f;
        dy = 0.0f;
        dz = 0.0f;

        r = static_cast<GLubyte>(1.0f);
        g = static_cast<GLubyte>(1.0f);
        b = static_cast<GLubyte>(1.0f);
        a = static_cast<GLubyte>(1.0f);

        dxSpeed = 1.0f;
        dySpeed = 1.0f;
        dzSpeed = 1.0f;

        life = 5.0f;
    }

    bool operator<(const Particle &that) const {
        // Sort in reverse order : far particles drawn first.
        return this->cameraDistance > that.cameraDistance;
    }
};


class ParticlesSample2 : public GLBaseSample {
public:
    ParticlesSample2();

    virtual ~ParticlesSample2();

    virtual void create();

    virtual void draw();

    virtual void shutdown();

    virtual void LoadImage(NativeImage *pImage);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(glm::mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

    int FindUnusedParticle();

    void SortParticles();

    int UpdateParticles();

    static void GenerateNewParticle(Particle &particle);

private:
    GLuint m_TextureId;
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;

    GLuint m_VaoId;
    GLuint m_ParticlesVertexVboId;
    GLuint m_ParticlesPosVboId;
    GLuint m_ParticlesColorVboId;

    NativeImage m_RenderImage;
    glm::mat4 m_MVPMatrix;

    // particles relation
    Particle m_ParticlesContainer[MAX_PARTICLES];
    GLfloat *m_pParticlesPosData;
    GLubyte *m_pParticlesColorData;
    int m_LastUsedParticle;

    int m_AngleX;
    int m_AngleY;

    float m_ScaleX;
    float m_ScaleY;
};

static const GLfloat g_vertex_buffer_data[] = {
        //position            //texture coord
        -0.05f, -0.05f, -0.05f, 0.0f, 0.0f,
        0.05f, -0.05f, -0.05f, 1.0f, 0.0f,
        0.05f, 0.05f, -0.05f, 1.0f, 1.0f,
        0.05f, 0.05f, -0.05f, 1.0f, 1.0f,
        -0.05f, 0.05f, -0.05f, 0.0f, 1.0f,
        -0.05f, -0.05f, -0.05f, 0.0f, 0.0f,
        -0.05f, -0.05f, 0.05f, 0.0f, 0.0f,
        0.05f, -0.05f, 0.05f, 1.0f, 0.0f,
        0.05f, 0.05f, 0.05f, 1.0f, 1.0f,
        0.05f, 0.05f, 0.05f, 1.0f, 1.0f,
        -0.05f, 0.05f, 0.05f, 0.0f, 1.0f,
        -0.05f, -0.05f, 0.05f, 0.0f, 0.0f,
        -0.05f, 0.05f, 0.05f, 1.0f, 0.0f,
        -0.05f, 0.05f, -0.05f, 1.0f, 1.0f,
        -0.05f, -0.05f, -0.05f, 0.0f, 1.0f,
        -0.05f, -0.05f, -0.05f, 0.0f, 1.0f,
        -0.05f, -0.05f, 0.05f, 0.0f, 0.0f,
        -0.05f, 0.05f, 0.05f, 1.0f, 0.0f,
        0.05f, 0.05f, 0.05f, 1.0f, 0.0f,
        0.05f, 0.05f, -0.05f, 1.0f, 1.0f,
        0.05f, -0.05f, -0.05f, 0.0f, 1.0f,
        0.05f, -0.05f, -0.05f, 0.0f, 1.0f,
        0.05f, -0.05f, 0.05f, 0.0f, 0.0f,
        0.05f, 0.05f, 0.05f, 1.0f, 0.0f,
        -0.05f, -0.05f, -0.05f, 0.0f, 1.0f,
        0.05f, -0.05f, -0.05f, 1.0f, 1.0f,
        0.05f, -0.05f, 0.05f, 1.0f, 0.0f,
        0.05f, -0.05f, 0.05f, 1.0f, 0.0f,
        -0.05f, -0.05f, 0.05f, 0.0f, 0.0f,
        -0.05f, -0.05f, -0.05f, 0.0f, 1.0f,
        -0.05f, 0.05f, -0.05f, 0.0f, 1.0f,
        0.05f, 0.05f, -0.05f, 1.0f, 1.0f,
        0.05f, 0.05f, 0.05f, 1.0f, 0.0f,
        0.05f, 0.05f, 0.05f, 1.0f, 0.0f,
        -0.05f, 0.05f, 0.05f, 0.0f, 0.0f,
        -0.05f, 0.05f, -0.05f, 0.0f, 1.0f,
};

#endif //OPENGLESDEMO_PARTICLESSAMPLE2_H
