#ifndef OPENGLLESSON_NATIVE7LESSON_H
#define OPENGLLESSON_NATIVE7LESSON_H

#include <GLUtils.h>

class Native7Lesson {
public:
	Native7Lesson();

	~Native7Lesson();

	void Create();

	void Change(int width, int height);

	void Draw();

	static void DecreaseCubeCount();

	static void IncreaseCubeCount();

	void SetDelta(float x, float y);

	static void ToggleStride();

	static void ToggleVBOs();

	static void UpdateVboStatus(bool useVbos);

	static void UpdateStrideStatus(bool useStride);

private:

	// model/view/projection matrix
	Matrix* mModelMatrix;
	Matrix* mViewMatrix;
	Matrix* mProjectionMatrix;
	Matrix* mMVPMatrix;

	//
	Matrix* mAccumulatedRotationMatrix;
	Matrix* mCurrentRotationMatrix;
	Matrix* mLightModelMatrix;

	//
	GLuint mMVPMatrixHandle;
	GLuint mMVMatrixHandle;
	GLuint mLightPosHandle;
	GLuint mTextureUniformHandle;

	GLuint mProgramHandle;
	GLuint mPointProgramHandle;

	GLuint mAndroidDataHandle;

	float mLightPosInModelSpace[4];
	float mLightPosInWorldSpace[4];
	float mLightPosInEyeSpace[4];

	float mDeltaX;
	float mDeltaY;
};


#endif
