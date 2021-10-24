#pragma once
#include "../utils/GLUtils.h"
#include "../graphics/Matrix.h"
namespace NAMESPACE_NativeTriangle2{
	class NativeTriangle {

	public:
		NativeTriangle();

		~NativeTriangle();

		void create();

		void change(int width, int height);

		void draw();

	private:
		GLuint mProgram;
		int mWidth;
		int mHeight;
	};
}