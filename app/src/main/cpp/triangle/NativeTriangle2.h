#pragma once
#include "../graphics/GLUtils.h"
#include "../graphics/Matrix.h"

class NativeTriangle2 {

public:
	NativeTriangle2();

	~NativeTriangle2();

	void create();

	void change(int width, int height);

	void draw();

private:
	GLuint mProgram;
	int mWidth;
	int mHeight;
};
