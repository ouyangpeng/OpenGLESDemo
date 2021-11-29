#ifndef OPENGLLESSON_NATIVE7LESSON_H
#define OPENGLLESSON_NATIVE7LESSON_H

#include "../../../utils/GLUtils.h"

class Native7Lesson {
public:
	Native7Lesson();

	~Native7Lesson();

	void create();

	void change(int width, int height);

	void draw();

	static void decreaseCubeCount();

	static void increaseCubeCount();

	void setDelta(float x, float y);

	static void toggleStride();

	static void toggleVBOs();

	static void updateVboStatus(bool useVbos);

	static void updateStrideStatus(bool useStride);

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
