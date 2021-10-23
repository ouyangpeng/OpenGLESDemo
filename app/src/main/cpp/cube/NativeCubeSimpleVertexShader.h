#pragma once
#include "../graphics/GLUtils.h"
#include "../graphics/Matrix.h"

#define VERTEX_POS_SIZE       3 // x, y and z
#define VERTEX_COLOR_SIZE     4 // r, g, b, and a

#define VERTEX_POS_INDX       0
#define VERTEX_COLOR_INDX     1

#define PI 3.1415926535897932384626433832795f

namespace TRIANGLE_SIMPLE_VERTEXT_SHADER{
	class NativeTriangle {
	public:
		NativeTriangle();

		~NativeTriangle();

		void create();

		void change(int width, int height);

		void draw();

	private:
        typedef struct
        {
            GLfloat  m[4][4];
        } ESMatrix;

		// Handle to a program object
		GLuint mProgram;

		// Uniform locations
		GLint mvpLoc;

		// Vertex daata
		GLfloat * vertices;
		GLuint * indices;

		int numIndices;

		// Rotation angle
		GLfloat angle;

        // MVP matrix
        ESMatrix  mvpMatrix;

		int mWidth;
		int mHeight;

		long mLastTime = 0;

        int esGenCube(float scale, GLfloat **vertices, GLfloat **normals, GLfloat **texCoords,
                      GLuint **indices);

		void update(float deltaTime);

		void esMatrixLoadIdentity(ESMatrix *result);

		void esPerspective(ESMatrix *result, float d, float fovy, float aspect, float nearZ);

		void esFrustum ( ESMatrix *result, float left, float right, float bottom, float top, float nearZ, float farZ );

		void esMatrixMultiply ( ESMatrix *result, ESMatrix *srcA, ESMatrix *srcB );

		void esTranslate ( ESMatrix *result, GLfloat tx, GLfloat ty, GLfloat tz );

		void esRotate ( ESMatrix *result, GLfloat angle, GLfloat x, GLfloat y, GLfloat z );

		float getDeltaTime();
	};
}


