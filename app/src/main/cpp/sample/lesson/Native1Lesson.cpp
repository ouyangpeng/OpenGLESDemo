#include "Native1Lesson.h"


// This triangle is red, green, and blue.
GLfloat triangle1VerticesData[] = {
	// X, Y, Z,
	// R, G, B, A
	-0.5f, -0.25f, 0.0f,
	1.0f, 0.0f, 0.0f, 1.0f,

	0.5f, -0.25f, 0.0f,
	0.0f, 0.0f, 1.0f, 1.0f,

	0.0f, 0.559016994f, 0.0f,
	0.0f, 1.0f, 0.0f, 1.0f };

// This triangle is yellow, cyan, and magenta.
GLfloat triangle2VerticesData[] = {
	// X, Y, Z,
	// R, G, B, A
	-0.5f, -0.25f, 0.0f,
	1.0f, 1.0f, 0.0f, 1.0f,

	0.5f, -0.25f, 0.0f,
	0.0f, 1.0f, 1.0f, 1.0f,

	0.0f, 0.559016994f, 0.0f,
	1.0f, 0.0f, 1.0f, 1.0f };

// This triangle is white, gray, and black.
GLfloat triangle3VerticesData[] = {
	// X, Y, Z,
	// R, G, B, A
	-0.5f, -0.25f, 0.0f,
	1.0f, 1.0f, 1.0f, 1.0f,

	0.5f, -0.25f, 0.0f,
	0.5f, 0.5f, 0.5f, 1.0f,

	0.0f, 0.559016994f, 0.0f,
	0.0f, 0.0f, 0.0f, 1.0f };

// Index buffer data
GLushort indices[3] = { 0, 1, 2 };

Native1Lesson::Native1Lesson() {
	mModelMatrix = nullptr;
	mMVPMatrix = nullptr;
	mProjectionMatrix = nullptr;
	mViewMatrix = nullptr;
}

Native1Lesson::~Native1Lesson() = default;

void Native1Lesson::create() {
	GLUtils::printGLInfo();

	// Main Program
	VERTEX_SHADER = GLUtils::openTextFile(
			"vertex/vertex_shader_lesson_1.glsl");
	FRAGMENT_SHADER = GLUtils::openTextFile(
			"fragment/fragment_shader_lesson_1.glsl");

	mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);
	if (!mProgram) {
		LOGD("Could not create program")
		return;
	}

	mModelMatrix = new Matrix();
	mMVPMatrix = new Matrix();

	// Position the eye in front of the origin.
	float eyeX = 0.0f;
	float eyeY = 0.0f;
	float eyeZ = 1.5f;

	// We are looking at the origin
	float centerX = 0.0f;
	float centerY = 0.0f;
	float centerZ = 0.0f;

	// Set our up vector.
	float upX = 0.0f;
	float upY = 1.0f;
	float upZ = 0.0f;

	// Set the view matrix.
	mViewMatrix = Matrix::newLookAt(eyeX, eyeY, eyeZ, centerX, centerY, centerZ, upX, upY, upZ);
}

void Native1Lesson::change(int width, int height) {

	glViewport(0, 0, width, height);

	// Create a new perspective projection matrix. The height will stay the same
	// while the width will vary as per aspect ratio.
	float ratio = (float)width / height;
	float left = -ratio;
	float right = ratio;
	float bottom = -0.8f;
	float top = 0.8f;
	float near = 0.5f;
	float far = 2.0f;

	mProjectionMatrix = Matrix::newFrustum(left, right, bottom, top, near, far);
}

void Native1Lesson::draw() {
	glClearColor(0.5F, 0.5F, 0.5F, 0.5F);
	glClear(GL_COLOR_BUFFER_BIT);
	GLUtils::checkGlError("glClear");

	glUseProgram(mProgram);

	// 返回统一变量的位置
	mMVPMatrixHandle = (GLuint)glGetUniformLocation(mProgram, "u_MVPMatrix");

	long time = TimeUtils::currentTimeMillis() % 10000L;
	float angleInDegrees = (360.0f / 10000.0f) * ((int)time);

	// Draw the triangle facing straight on.
	mModelMatrix->identity();
	mModelMatrix->rotate(angleInDegrees, 0.0f, 0.0f, 1.0f);
	drawTriangle(triangle1VerticesData,sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
				 3, indices);

	// Draw one translated a bit down and rotated to be flat on the ground.
	mModelMatrix->identity();
	mModelMatrix->translate(0.0f, -1.0f, 0.0f);
	mModelMatrix->rotate(90.0f, 1.0f, 0.0f, 0.0f);
	mModelMatrix->rotate(angleInDegrees, 0.0f, 0.0f, 1.0f);
	drawTriangle(triangle2VerticesData,sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
				 3, indices);

	// Draw one translated a bit to the right and rotated to be facing to the left.
	mModelMatrix->identity();
	mModelMatrix->translate(1.0f, 0.0f, 0.0f);
	mModelMatrix->rotate(90.0f, 0.0f, 1.0f, 0.0f);
	mModelMatrix->rotate(angleInDegrees, 0.0f, 0.0f, 1.0f);
	drawTriangle(triangle3VerticesData,sizeof(GLfloat) * (VERTEX_POS_SIZE + VERTEX_COLOR_SIZE),
				 3, indices);
}

void Native1Lesson::drawTriangle(GLfloat* vertices,
								 GLint vtxStride, GLint numIndices,
								 GLushort* mIndices) {

	GLfloat* vtxBuf = vertices;
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	glEnableVertexAttribArray(VERTEX_POS_INDX);
	glEnableVertexAttribArray(VERTEX_COLOR_INDX);

	glVertexAttribPointer(
		VERTEX_POS_INDX,
		VERTEX_POS_SIZE,
		GL_FLOAT,
		GL_FALSE,
		vtxStride,
		vtxBuf
	);

	vtxBuf += VERTEX_POS_SIZE;
	glVertexAttribPointer(
		VERTEX_COLOR_INDX,
		VERTEX_COLOR_SIZE,
		GL_FLOAT,
		GL_FALSE,
		vtxStride,
		vtxBuf
	);

	// model * view
	mMVPMatrix->multiply(*mViewMatrix, *mModelMatrix);

	// model * view * projection
	mMVPMatrix->multiply(*mProjectionMatrix, *mMVPMatrix);

	glUniformMatrix4fv(mMVPMatrixHandle, 1, GL_FALSE, mMVPMatrix->mData);

	glDrawElements(GL_TRIANGLES, numIndices, GL_UNSIGNED_SHORT, mIndices);

	GLUtils::checkGlError("glDrawArrays");

	glDisableVertexAttribArray(VERTEX_POS_INDX);
	glDisableVertexAttribArray(VERTEX_COLOR_INDX);
}

void Native1Lesson::shutdown() {
	if(mModelMatrix){
		delete mModelMatrix;
		mModelMatrix = nullptr;
	}

	if(mMVPMatrix){
		delete mMVPMatrix;
		mMVPMatrix = nullptr;
	}
	if(mProjectionMatrix){
		delete mProjectionMatrix;
		mProjectionMatrix = nullptr;
	}
	if(mViewMatrix){
		delete mViewMatrix;
		mViewMatrix = nullptr;
	}
}