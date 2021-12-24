//
// Created by OuyangPeng on 2021/12/24.
//

#ifndef OPENGLESDEMO_SHAREDEGLCONTEXTSAMPLE_H
#define OPENGLESDEMO_SHAREDEGLCONTEXTSAMPLE_H

#include <glm/gtc/matrix_transform.hpp>
#include <GLBaseSample.h>
#include <thread>
#include <GLRenderLooper.h>

using namespace std;

class SharedEGLContextSample : public GLBaseSample {

public:
    SharedEGLContextSample();

    virtual ~SharedEGLContextSample();

    virtual void Create();

    virtual void Draw();

    virtual void Shutdown();

    virtual void LoadImage(NativeImage *pImage);

    static void OnAsyncRenderDone(void *callback, int fboTexId);

private:
    GLuint m_ImageTextureId;
    GLuint m_FboTextureId;
    GLuint m_VaoId;
    GLuint m_VboIds[4];
    NativeImage m_RenderImage;
    GLuint m_FboProgramObj;

    mutex m_Mutex;
    condition_variable m_Cond;
    GLEnv m_GLEnv;
};


#endif //OPENGLESDEMO_SHAREDEGLCONTEXTSAMPLE_H
