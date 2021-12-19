//
// Created by OuyangPeng on 2021/10/25.
//

#include <cstdlib>
#include "MRT.h"

void MRT::create() {
	GLUtils::printGLInfo();

	// Main Program
	// 顶点着色器
	VERTEX_SHADER = GLUtils::openTextFile(
			"vertex/vertex_shader_mrt.glsl");
	// 片段着色器
	FRAGMENT_SHADER = GLUtils::openTextFile(
			"fragment/fragment_shader_mrt.glsl");

	mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

	if (!mProgram) {
		LOGD("Could not create program")
		return;
	}

	initFBO();

	// 设置清除颜色
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

// 设置多重渲染目标
void MRT::initFBO() {
	int i ;
	GLint defaultFramebuffer = 0;
	const GLenum attachments[4] =
	{
		GL_COLOR_ATTACHMENT0,
		GL_COLOR_ATTACHMENT1,
		GL_COLOR_ATTACHMENT2,
		GL_COLOR_ATTACHMENT3
	};

	glGetIntegerv( GL_FRAMEBUFFER_BINDING , &defaultFramebuffer);

	// Setup FBO  初始化帧缓冲区对象 FBO
	glGenFramebuffers(1, &fbo);
	glBindFramebuffer( GL_FRAMEBUFFER, fbo);

	// Setup four output buffers and attach to FBO
	textureHeight = textureWidth = 400;
	glGenTextures(4, &colorTexId[0]);
	for(i = 0; i < 4; ++ i)
	{
        // 初始化纹理
		glBindTexture( GL_TEXTURE_2D, colorTexId[i]);

		glTexImage2D( GL_TEXTURE_2D, 0 , GL_RGBA,
				textureWidth, textureHeight,
				0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

		// Set the filtering mode
		glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri( GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// 将相关纹理绑定到FBO
		glFramebufferTexture2D( GL_DRAW_FRAMEBUFFER , attachments[i],
								GL_TEXTURE_2D,colorTexId[i],0);
	}

	// 为渲染指定颜色附着： 这里设置一个带有4个颜色输出的FBO
	glDrawBuffers(4,attachments);

	if( GL_FRAMEBUFFER_COMPLETE != glCheckFramebufferStatus( GL_FRAMEBUFFER))
	{
		return;
	}
	// Restore the original framebuffer
	glBindFramebuffer( GL_FRAMEBUFFER, defaultFramebuffer);
}

void MRT::shutdown() {
	// Delete texture objects
	glDeleteTextures ( 4, colorTexId );

	// Delete fbo
	glDeleteFramebuffers ( 1, &fbo );

	// Delete program object
	    // Delete program object
    GLUtils::DeleteProgram(mProgram);
}

void MRT::draw() {
	GLint defaultFramebuffer = 0;
	const GLenum attachments[4] =
			{
					GL_COLOR_ATTACHMENT0,
					GL_COLOR_ATTACHMENT1,
					GL_COLOR_ATTACHMENT2,
					GL_COLOR_ATTACHMENT3
			};
	glGetIntegerv( GL_FRAMEBUFFER_BINDING , &defaultFramebuffer);

	// FIRST: use MRTs to output four colors to four buffers
	glBindFramebuffer( GL_FRAMEBUFFER , fbo);

	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glDrawBuffers(4,attachments);

	drawGeometry();

	// SECOND: copy the four output buffers into four window quadrants
	// with framebuffer blits

	// Restore the default framebuffer
	glBindFramebuffer ( GL_DRAW_FRAMEBUFFER, defaultFramebuffer );

	blitTextures ();
}

void MRT::drawGeometry() {
	GLfloat vVertices[] = { -1.0f,  1.0f, 0.0f,		// 右上角
							-1.0f, -1.0f, 0.0f,		// 右下角
							1.0f, -1.0f, 0.0f,		// 左下角
							1.0f,  1.0f, 0.0f,		// 左上角
	};
	// 注意索引从0开始!
	GLushort indices[] = {
			0, 1, 2,		// 第一个三角形
			0, 2, 3			// 第二个三角形
	};

	// Clear the color buffer
	glClear ( GL_COLOR_BUFFER_BIT );

	// Use the program object
	glUseProgram ( mProgram );

	// Load the vertex position
	glVertexAttribPointer(0,3, GL_FLOAT, 
			GL_FALSE,3*sizeof(GLfloat),vVertices);

	glEnableVertexAttribArray(0);

	// Draw a quad
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

}

//
// Copy MRT output buffers to screen
//
void MRT::blitTextures() {

	// set the FBO for reading
	glBindFramebuffer( GL_READ_FRAMEBUFFER, fbo);
	
	// copy the output red buffer to lower left quadrant
	glReadBuffer( GL_COLOR_ATTACHMENT0 );
	glBlitFramebuffer( 0, 0, textureWidth, textureHeight,
					   0, 0, mWidth/2, mHeight/2,
					   GL_COLOR_BUFFER_BIT, GL_LINEAR);

	// Copy the output green buffer to lower right quadrant
	glReadBuffer ( GL_COLOR_ATTACHMENT1 );
	glBlitFramebuffer ( 0, 0, textureWidth, textureHeight,
						mWidth/2, 0, mWidth, mHeight/2,
						GL_COLOR_BUFFER_BIT, GL_LINEAR );

	// Copy the output blue buffer to upper left quadrant
	glReadBuffer ( GL_COLOR_ATTACHMENT2 );
	glBlitFramebuffer ( 0, 0, textureWidth, textureHeight,
						0, mHeight/2, mWidth/2, mHeight,
						GL_COLOR_BUFFER_BIT, GL_LINEAR );

	// Copy the output gray buffer to upper right quadrant
	glReadBuffer ( GL_COLOR_ATTACHMENT3 );
	glBlitFramebuffer ( 0, 0, textureWidth, textureHeight,
						mWidth/2, mHeight/2, mWidth, mHeight,
						GL_COLOR_BUFFER_BIT, GL_LINEAR );
}
