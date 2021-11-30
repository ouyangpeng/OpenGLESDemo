//
// Created by OuyangPeng on 2021/10/25.
//

#include <cstdlib>
#include "MipMap2D.h"

MipMap2D::MipMap2D() {

}

MipMap2D::~MipMap2D() {

}

void MipMap2D::create() {
	GLUtils::printGLInfo();

	// Main Program
	VERTEX_SHADER = GLUtils::openTextFile(
			"vertex/vertex_shader_texture_mipmap_2d.glsl");
	FRAGMENT_SHADER = GLUtils::openTextFile(
			"fragment/fragment_shader_texture_mipmap_2d.glsl");

	mProgram = GLUtils::createProgram(&VERTEX_SHADER, &FRAGMENT_SHADER);

	if (!mProgram) {
		LOGD("Could not create program")
		return;
	}

	// Get the sampler location
	samplerLoc = glGetUniformLocation(mProgram, "s_texture");

	// Get the offset location
	offsetLoc = glGetUniformLocation(mProgram, "u_offset");

	// Load the texture  加载纹理
	textureId = CreateMipMappedTexture2D();

	// 设置清除颜色
	glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void MipMap2D::change(int width, int height) {
	mWidth = width;
	mHeight = height;
	LOGD("change() width = %d , height = %d\n", width, height);
	glViewport(0, 0, mWidth, mHeight);
}

void MipMap2D::shutdown() {
	// Delete texture object
	glDeleteTextures(1, &textureId);

	// Delete program object
	glDeleteProgram(mProgram);
}

void MipMap2D::draw() {
	GLfloat vVertices[] =
	{
		-0.5f,  0.5f, 0.0f, 1.5f,	// Position 0
		0.0f,  0.0f,				// TexCoord 0

		-0.5f, -0.5f, 0.0f, 0.75f,	// Position 1
		0.0f,  1.0f,				// TexCoord 1

		0.5f, -0.5f, 0.0f, 0.75f,	// Position 2
		1.0f,  1.0f,				// TexCoord 2

		0.5f,  0.5f, 0.0f, 1.5f,	// Position 3
		1.0f,  0.0f					// TexCoord 3
	};

    // 注意索引从0开始!
    GLushort indices[] = {
            0, 1, 2,		// 第一个三角形
            0, 2, 3			// 第二个三角形
    };

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(mProgram);

	// Load the vertex position
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), vVertices);
	// Load the textture coordinate
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), &vVertices[4]);

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);

	// Bind the texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureId);

	// Set the sampler texture unit to 0
	glUniform1i(samplerLoc, 0);

	//Draw quad with nearest sampling
	//GL_NEAREST（也叫邻近过滤，Nearest Neighbor Filtering）是OpenGL默认的纹理过滤方式。
	// 当设置为GL_NEAREST的时候，OpenGL会选择中心点最接近纹理坐标的那个像素。
	// 下图中你可以看到四个像素，加号代表纹理坐标。左上角那个纹理像素的中心距离纹理坐标最近，所以它会被选择为样本颜色：
	// 图片地址：https://segmentfault.com/img/remote/1460000037542100
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glUniform1f(offsetLoc, -0.6f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

	//Draw quad with trilinear sampling
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glUniform1f(offsetLoc, 0.6f);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);
}

// Create a mipmapped 2D texture image
GLuint MipMap2D::CreateMipMappedTexture2D() {
	// Texture object handle
	GLuint textureId;
	int width = 256;
	int height = 256;
	int level;

	GLubyte* pixels;
	GLubyte* preImage;
	GLubyte* newImage;

	pixels = GenCheckImage(width, height, 8);

	if (pixels == nullptr) {
		return 0;
	}

	// Generate a texture object
	// 生成纹理对象
	// @param n 指定要生成的纹理对象数量
	// @param textures 一个保存n个纹理对象ID的无符号整数数组
	glGenTextures(1, &textureId);

	// Bind the texture object
	// 一旦用glGenTextures生成了纹理对象的ID，应用程序就必须绑定纹理对象进行操作。
	// 绑定纹理对象之后，后续的操作（如glTexImage2D和glTexParameter)将影响绑定的纹理对象
	// @param  target  将纹理对象绑定到GL_TEXTURE_2D、GL_TEXTURE_3D、GL_TEXTURE_2D_ARRAY或者GL_TEXTURE_CUBE_MAP目标
	// @param texture  要绑定的纹理对象句柄
	glBindTexture(GL_TEXTURE_2D, textureId);

	// Load mipmap level 0
	// 用于加载2D和立方图纹理
	// @param  target  将纹理对象绑定到GL_TEXTURE_2D、GL_TEXTURE_3D、GL_TEXTURE_2D_ARRAY或者GL_TEXTURE_CUBE_MAP目标
	// @param  level 指定要加载的mip级别。第一个级别为0，后续的mip贴图级别递增
	// @param  internalformat 纹理存储的内部格式。可以是未确定大小的基本内部格式，或者是确定大小的内部格式。
	// 未确定大小的内部格式可以为：  GL_ALPHA、GL_RGB、GL_RGBA、GL_LUMINANCE、GL_LUMINANCE_ALPHA
	// 确定大小的内部格式可以为：GL_R8、GL_R8UI、GL_R8_SNORM、GL_R16UI等一系列值
	// @param  width  图像的像素宽度
	// @param  height 图像的像素高度
	// @param  border 这个参数在OpenGL ES中被忽略，保留它是为了与桌面的OpenGL接口兼容；应该为0
	// @param  format 输出的纹理格式；可以为：
	// GL_RED、GL_RED_INTEGER、
	// GL_RG、 GL_RG_INTEGER
	// GL_RGB、GL_RGB_INTEGER、
	// GL_RGBA、GL_RGBA_INTEGER
	// GL_DEPTH_COMPONENT、GL_DEPTH_STENCIL
	// GL_ALPHA
	// @param  type 像素数据的类型
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB,
		width, height, 0,
		GL_RGB, GL_UNSIGNED_BYTE, pixels);

	level = 1;
	preImage = &pixels[0];

	// 当缩小和放大过滤设置为GL_NEAREST时，会发送这样的情况：
	// 一个纹素在提供的纹理坐标位置上读取。这称作 点采样或者最近采样。
	// 但是，最近采样可能产生严重的视觉伪像，这是因为三角形在屏幕空间中变得较小，
	// 在不同像素的插值中，纹理坐标有很大的跳跃。结果是：从一个大的纹理贴图中取得少量样本，造成巨大的性能损失。
	// OpenGL ES 中解决这类伪像的方案被称作 mip贴图(mipmapping)。

	// mip贴图的思路是构建一个图像链------mip贴图链。
	// mip贴图链始于原来指定的图像，后续的每个图像在每个维度上是前一个图像的一半，一直持续到最后达到链底部的1X1纹理。
	// mip贴图级别可以编程生成，一个mip级别中的每个像素通常根据上一级别中相同位置的4个像素的平均值计算(盒式过滤)

	while (width > 1 && height > 1)
	{
		int newWidth, newHeight;

        // 加载一个2D mip贴图链
		//Generate the next mipmap level
		GenMipMap2D(preImage, &newImage, width, height, &newWidth, &newHeight);

		//Load the mipmap level
		glTexImage2D(GL_TEXTURE_2D, level, GL_RGB,
			newWidth, newHeight, 0,
			GL_RGB, GL_UNSIGNED_BYTE, newImage);

		// Free the previous image
		free(preImage);

		// Set the previous image for the next iteration
		preImage = newImage;
		level++;

		//Half the width and height
		width = newWidth;
		height = newHeight;
	}

	free(newImage);

	// 加载mip贴图链之后，便可以设置过滤模式，以使用mip贴图。
	// 结果是我们实现了屏幕像素和纹理像素间的更好比率，从而减少了锯齿伪像。
	// 图像的锯齿也减少了，这是因为mip贴图链中的每个图像连续进行过滤，使得高频元素随着贴图链的下移而越来越少。

	// Set the filtering mode
	// 关于 纹理过滤的区别  可以参考链接：  https://segmentfault.com/a/1190000037542097

	// GL_NEAREST_MIPMAP_NEAREST 从所选的最近的mip级别中取得单点样本

	// GL_NEAREST_MIPMAP_NEAREST 使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
	// GL_LINEAR_MIPMAP_NEAREST	 使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
	// GL_NEAREST_MIPMAP_LINEAR	 在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
	// GL_LINEAR_MIPMAP_LINEAR	 在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);

	//GL_NEAREST（也叫邻近过滤，Nearest Neighbor Filtering）是OpenGL默认的纹理过滤方式。
	// 当设置为GL_NEAREST的时候，OpenGL会选择中心点最接近纹理坐标的那个像素。
	// 下图中你可以看到四个像素，加号代表纹理坐标。左上角那个纹理像素的中心距离纹理坐标最近，所以它会被选择为样本颜色：
	// 效果如 https://segmentfault.com/img/remote/1460000037542100

	// GL_LINEAR 从最靠近纹理坐标的纹理中取得一个双线性样本
	// GL_LINEAR（也叫线性过滤，(Bi)linear Filtering）它会基于纹理坐标附近的纹理像素，计算出一个插值，近似出这些纹理像素之间的颜色。
	// 一个纹理像素的中心距离纹理坐标越近，那么这个纹理像素的颜色对最终的样本颜色的贡献越大。下图中你可以看到返回的颜色是邻近像素的混合色
	// 效果如 https://segmentfault.com/img/remote/1460000037542101

	// 效果如 https://image-static.segmentfault.com/299/310/2993106777-7089e5b2270ff024_fix732  对比了GL_NEAREST 和 GL_LINEAR
	// GL_NEAREST产生了颗粒状的图案，我们能够清晰看到组成纹理的像素，而GL_LINEAR能够产生更平滑的图案，很难看出单个的纹理像素。
	// GL_LINEAR可以产生更真实的输出，但有些开发者更喜欢8-bit风格，所以他们会用GL_NEAREST选项。
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	return textureId;
}


// 加载一个2D mip贴图链
//  From an RGB8 source image, generate the next level mipmap
GLboolean MipMap2D::GenMipMap2D(const GLubyte* src, GLubyte** dst, int srcWidth, int srcHeight, int* dstWidth, int* dstHeight)
{
	int x, y;
	int texelSize = 3;

	*dstWidth = srcWidth / 2;

	if (*dstWidth <= 0)
	{
		*dstWidth = 1;
	}

	*dstHeight = srcHeight / 2;

	if (*dstHeight <= 0)
	{
		*dstHeight = 1;
	}

	*dst = (GLubyte*)malloc(sizeof(GLubyte) * texelSize * (*dstWidth) * (*dstHeight));

	if (*dst == nullptr)
	{
		return GL_FALSE;
	}

	for (y = 0; y < *dstHeight; y++)
	{
		for (x = 0; x < *dstWidth; x++)
		{
			int srcIndex[4];
			float r = 0.0f,
				g = 0.0f,
				b = 0.0f;
			int sample;

			// Compute the offsets for 2x2 grid of pixels in previous
			// image to perform box filter
			srcIndex[0] = (((y * 2) * srcWidth) + (x * 2)) * texelSize;
			srcIndex[1] = (((y * 2) * srcWidth) + (x * 2 + 1)) * texelSize;
			srcIndex[2] = ((((y * 2) + 1) * srcWidth) + (x * 2)) * texelSize;
			srcIndex[3] = ((((y * 2) + 1) * srcWidth) + (x * 2 + 1)) * texelSize;

			// Sum all pixels
			for (sample = 0; sample < 4; sample++)
			{
				r += src[srcIndex[sample]];
				g += src[srcIndex[sample] + 1];
				b += src[srcIndex[sample] + 2];
			}

			// Average results
			r /= 4.0;
			g /= 4.0;
			b /= 4.0;

			// Store resulting pixels
			(*dst)[(y * (*dstWidth) + x) * texelSize] = (GLubyte)(r);
			(*dst)[(y * (*dstWidth) + x) * texelSize + 1] = (GLubyte)(g);
			(*dst)[(y * (*dstWidth) + x) * texelSize + 2] = (GLubyte)(b);
		}
	}
	return GL_TRUE;
}

//  Generate an RGB8 checkerboard image
GLubyte* MipMap2D::GenCheckImage(int width, int height, int checkSize)
{
	int x, y;
	GLubyte* pixels = (GLubyte*)malloc(width * height * 3);

	if (pixels == nullptr)
	{
		return nullptr;
	}

	for (y = 0; y < height; y++)
		for (x = 0; x < width; x++)
		{
			GLubyte rColor = 0;
			GLubyte bColor = 0;

			if ((x / checkSize) % 2 == 0)
			{
				rColor = 255 * ((y / checkSize) % 2);
				bColor = 255 * (1 - ((y / checkSize) % 2));
			}
			else
			{
				bColor = 255 * ((y / checkSize) % 2);
				rColor = 255 * (1 - ((y / checkSize) % 2));
			}

			pixels[(y * width + x) * 3] = rColor;
			pixels[(y * width + x) * 3 + 1] = 0;
			pixels[(y * width + x) * 3 + 2] = bColor;
		}

	return pixels;
}
