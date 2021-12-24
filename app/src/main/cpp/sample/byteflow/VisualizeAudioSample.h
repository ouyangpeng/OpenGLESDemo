//
// Created by OuyangPeng on 2021/12/20.
//

#ifndef OPENGLESDEMO_VISUALIZEAUDIOSAMPLE_H
#define OPENGLESDEMO_VISUALIZEAUDIOSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>
#include <mutex>

#define MAX_AUDIO_LEVEL 2000
#define RESAMPLE_LEVEL  112

using namespace glm;

class VisualizeAudioSample : public GLBaseSample {
public:
    VisualizeAudioSample();

    virtual ~VisualizeAudioSample();


    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadAudioData(short *buffer, int len);

    virtual void UpdateTransformMatrix(float rotateX, float rotateY, float scaleX, float scaleY);

    void UpdateMVPMatrix(mat4 &mvpMatrix, int angleX, int angleY, float ratio) const;

    void UpdateMesh();

private:
    GLint m_SamplerLoc;
    GLint m_MVPMatLoc;
    GLuint m_VaoId;
    GLuint m_VboIds[2];
    glm::mat4 m_MVPMatrix;

    int m_AngleX;
    int m_AngleY;
    float m_ScaleX;
    float m_ScaleY;

    short *m_pCurAudioData;
    short *m_pAudioBuffer;
    int m_AudioDataSize;
    std::mutex m_Mutex;
    std::condition_variable m_Cond;

    vec3 *m_pVerticesCoords;
    vec2 *m_pTextureCoords;
    int m_RenderDataSize;

    int m_FrameIndex;
    volatile bool m_bAudioDataReady;
};


#endif //OPENGLESDEMO_VISUALIZEAUDIOSAMPLE_H
