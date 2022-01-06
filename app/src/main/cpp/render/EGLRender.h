//
// Created by OuyangPeng on 2021/11/26.
//

#ifndef OPENGLESDEMO_EGLRender_H
#define OPENGLESDEMO_EGLRender_H

#include "stdint.h"
#include <GLES3/gl3.h>
#include <ImageDef.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
#include <GLUtils.h>

#define EGL_FEATURE_NUM 20

#define EGL_VERTEX_POS_LOC  0
#define EGL_TEXTURE_POS_LOC 1

#define EGL_VERTEX_POS_SIZE  3
#define EGL_TEXTURE_POS_SIZE 2

#define PARAM_TYPE_SHADER_INDEX    200

class EGLRender
{
public:
	EGLRender();
	~EGLRender();

    void Init(JNIEnv *env, jobject assetManager);

    int CreateGlesEnv();

    //void CreateProgramWithFeedback(const char *vShaderStr, const char *fShaderStr);

    void SetImageData(uint8_t *pData, int width, int height);

    void SetFragmentShaderType(int paramType, int param);

    void Draw();

    void DestroyGlesEnv();

    void UnInit();

    static EGLRender* GetInstance()
	{
		if (m_Instance == nullptr)
		{
			m_Instance = new EGLRender();
		}

		return m_Instance;
	}

    static void DestroyInstance()
	{
		if (m_Instance)
		{
			delete m_Instance;
			m_Instance = nullptr;
		}

	}

private:
	static EGLRender *m_Instance;
	GLuint m_ImageTextureId;
	GLuint m_FboTextureId;
	GLuint m_FboId;
	GLuint m_VaoIds[1] = {GL_NONE};;
	GLuint m_VboIds[3] = {GL_NONE};;
	GLint m_SamplerLoc;
	GLint m_TexSizeLoc;
	NativeImage m_RenderImage;

	GLuint m_ProgramObj;
	const char *vertexShader;
	// 正常纹理
	const char *fShaderStr_normal;
	// 马赛克
	const char *fShaderStr_mosaic;
	// 马赛克
	const char *fShaderStr_mosaic2;
	// 马赛克
	const char *fShaderStr_mosaic3;
	// 网格
	const char *fShaderStr_grid;
	// 旋转
	const char *fShaderStr_rotate;
	// 边缘
	const char *fShaderStr_edge;
	// 放大
	const char *fShaderStr_enlarge;
	// 型变
	const char *fShaderStr_reshape;
	// 型变2
	const char *fShaderStr_reshape2;
	// 灰度
	const char *fShaderStr_gray;
	// 图像颠倒
	const char *fShaderStr_upside_down;

	EGLConfig  m_eglConf;
	EGLSurface m_eglSurface;
	EGLContext m_eglCtx;
	EGLDisplay m_eglDisplay;
	bool       m_IsGLContextReady;
	const char*m_fShaderStrs[EGL_FEATURE_NUM];
	int        m_ShaderIndex;
};

//顶点坐标
const GLfloat egl_vVertices[] = {
		-1.0f, -1.0f, 0.0f, // bottom left
		1.0f, -1.0f, 0.0f, // bottom right
		-1.0f,  1.0f, 0.0f, // top left
		1.0f,  1.0f, 0.0f, // top right
};

//正常纹理坐标
const GLfloat egl_vTexCoors[] = {
		0.0f, 1.0f, // bottom left
		1.0f, 1.0f, // bottom right
		0.0f, 0.0f, // top left
		1.0f, 0.0f, // top right
};

// 注意索引从0开始!
static GLushort egl_indices[] = {
		0, 1, 2,        // 第一个三角形
		1, 3, 2         // 第二个三角形
};

#endif //OPENGLESDEMO_EGLRender_H
