//
// Created by OuyangPeng on 2021/10/25.
//

#include <cstdlib>
#include "TextureWrap.h"

TextureWrap::TextureWrap() {

}

TextureWrap::~TextureWrap() {

}

void TextureWrap::create() {
    GLUtils::printGLString("Version", GL_VERSION);
    GLUtils::printGLString("Vendor", GL_VENDOR);
    GLUtils::printGLString("Renderer", GL_RENDERER);
    GLUtils::printGLString("Extensions", GL_EXTENSIONS);

    const char* VERTEX_SHADER_TRIANGLE =
            "#version 300 es                            \n"
            "uniform float u_offset;                    \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec2 a_texCoord;   \n"
            "out vec2 v_texCoord;                       \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   gl_Position.x += u_offset;              \n"
            "   v_texCoord = a_texCoord;                \n"
            "}                                          \n";

    const char* FRAGMENT_SHADER_TRIANGLE =
            "#version 300 es                                     \n"
            "precision mediump float;                            \n"
            "in vec2 v_texCoord;                                 \n"
            "layout(location = 0) out vec4 outColor;             \n"
            "uniform sampler2D s_texture;                        \n"
            "void main()                                         \n"
            "{                                                   \n"
            "   outColor = texture( s_texture, v_texCoord );     \n"
            "}                                                   \n";

    programObject = GLUtils::createProgram(&VERTEX_SHADER_TRIANGLE, &FRAGMENT_SHADER_TRIANGLE);
    if (!programObject) {
        LOGD("Could not create program");
        return;
    }
    // Get the sampler location
    samplerLoc = glGetUniformLocation(programObject, "s_texture");

    // Get the offset location
    offsetLoc = glGetUniformLocation(programObject, "u_offset");

    // Load the texture  加载纹理
    textureId = CreateTexture2D();

    // 设置清除颜色
    glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
}

void TextureWrap::change(int width, int height) {
    mWidth = width;
    mHeight = height;
    LOGD("change() width = %d , height = %d\n", width, height);

    // Set the viewport
    glViewport(0, 0, mWidth, mHeight);
}

void TextureWrap::draw() {
    GLfloat vVertices[] = {
            -0.3f,  0.3f, 0.0f, 1.0f,   // Position 0
            -1.0f,  -1.0f,              // TexCoord 0

            -0.3f, -0.3f, 0.0f, 1.0f,   // Position 1
            -1.0f,  2.0f,               // TexCoord 1

            0.3f, -0.3f, 0.0f, 1.0f,    // Position 2
            2.0f,  2.0f,                // TexCoord 2

            0.3f,  0.3f, 0.0f, 1.0f,    // Position 3
            2.0f,  -1.0f                // TexCoord 3
    };
    GLushort indices[] = { 0, 1, 2, 0, 2, 3 };


    // Clear the color buffer
    glClear(GL_COLOR_BUFFER_BIT);

    // Use the program object
    glUseProgram(programObject);

    // Load the vertex position
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), vVertices);

    // Load the textture coordinate
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), &vVertices[4]);

    //启用顶点的句柄
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Bind the texture
    //激活指定纹理单元
    glActiveTexture(GL_TEXTURE0);
    //绑定纹理ID到纹理单元
    glBindTexture(GL_TEXTURE_2D, textureId);

    // Set the sampler texture unit to 0
    glUniform1i(samplerLoc, 0);

    // 配置边缘过渡参数
    // 第9章 纹理  第9.18节 纹理坐标包装
    // https://segmentfault.com/a/1190000037542097
    // Draw left quad with repeat wrap mode
    // GL_REPEAT模式渲染。在这种模式下，纹理只是在[0,1]区间之外重复，造成倾斜的图案
    // 对纹理的默认行为。重复纹理图像
    // 效果如： https://segmentfault.com/img/bVcHGht

    // 纹理坐标的范围通常是从[0, 0]到[1, 1]，超过[0.0, 1.0]的范围是允许的，
    // 而对与超出范围的内容要如何显示，这就取决于纹理的环绕方式(Wrapping mode)。
    // 在OpenGL默认的行为是重复这个纹理图像(GL_REPEAT)。
    //    环绕方式(Wrapping)	                描述
    //    GL_REPEAT	                对纹理的默认行为。重复纹理图像
    //    GL_MIRRORED_REPEAT	    但每次重复图片是镜像放置的
    //    GL_CLAMP_TO_EDGE	        纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glUniform1f(offsetLoc, -0.7f);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices);

    // Draw quad with clamp to edge wrap mode
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
    glUniform1f ( offsetLoc, 0.0f );
    glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );

    // Draw quad with mirrored repeat
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT );
    glUniform1f ( offsetLoc, 0.7f );
    glDrawElements ( GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, indices );
}

void TextureWrap::shutdown() {
    // Delete texture object
    glDeleteTextures ( 1, &textureId );

    // Delete program object
    glDeleteProgram ( programObject );
}

// Create a mipmapped 2D texture image
GLuint TextureWrap::CreateTexture2D() {
    // Texture object handle
    GLuint textureId;
    int width = 256, height = 256;
    GLubyte *pixels;

    pixels = GenCheckImage ( width, height, 64 );

    if ( pixels == nullptr )
    {
        return 0;
    }

    // Generate a texture object
    glGenTextures ( 1, &textureId );

    // Bind the texture object
    glBindTexture ( GL_TEXTURE_2D, textureId );

    // Load mipmap level 0
    //第一个参数     指定了纹理目标(Target)。设置为GL_TEXTURE_2D意味着会生成与当前绑定的纹理对象在同一个目标上的纹理（任何绑定到GL_TEXTURE_1D和GL_TEXTURE_3D的纹理不会受到影响）。
    //第二个参数     为纹理指定多级渐远纹理的级别，如果你希望单独手动设置每个多级渐远纹理的级别的话。这里我们填0，也就是基本级别。
    //第三个参数     告诉OpenGL我们希望把纹理储存为何种格式。我们的图像只有RGB值，因此我们也把纹理储存为RGB值。
    //第四个和第五个参数     设置最终的纹理的宽度和高度。我们之前加载图像的时候储存了它们，所以我们使用对应的变量。
    //下个参数      应该总是被设为0（历史遗留的问题）。
    //第七第八个     参数定义了源图的格式和数据类型。我们使用RGB值加载这个图像，并把它们储存为char(byte)数组，我们将会传入对应值。
    //最后一个参数    是真正的图像数据。

    //当调用glTexImage2D时，当前绑定的纹理对象就会被附加上纹理图像。
    glTexImage2D ( GL_TEXTURE_2D, 0, GL_RGB, width, height,
                   0, GL_RGB, GL_UNSIGNED_BYTE, pixels );

    // Set the filtering mode
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );

    return textureId;
}

//  Generate an RGB8 checkerboard image
GLubyte * TextureWrap::GenCheckImage(int width, int height, int checkSize) {
    int x, y;
    GLubyte *pixels = (GLubyte *) malloc ( width * height * 3 );

    if ( pixels == nullptr )
    {
        return nullptr;
    }

    for ( y = 0; y < height; y++ )
        for ( x = 0; x < width; x++ )
        {
            GLubyte rColor = 0;
            GLubyte bColor = 0;

            if ( ( x / checkSize ) % 2 == 0 )
            {
                rColor = 255 * ( ( y / checkSize ) % 2 );
                bColor = 255 * ( 1 - ( ( y / checkSize ) % 2 ) );
            }
            else
            {
                bColor = 255 * ( ( y / checkSize ) % 2 );
                rColor = 255 * ( 1 - ( ( y / checkSize ) % 2 ) );
            }

            pixels[ ( y * width + x ) * 3] = rColor;
            pixels[ ( y * width + x ) * 3 + 1] = 0;
            pixels[ ( y * width + x ) * 3 + 2] = bColor;
        }

    return pixels;
}

static TextureWrap * textureWrap;

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_TextureWrapRenderer_00024Companion_nativeSurfaceCreate(
        JNIEnv *env, jobject thiz) {
    textureWrap = new TextureWrap ();
    if(textureWrap != nullptr){
        textureWrap->create();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_TextureWrapRenderer_00024Companion_nativeSurfaceChange(
        JNIEnv *env, jobject thiz, jint width, jint height) {
    if(textureWrap != nullptr){
        textureWrap->change(width,height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_TextureWrapRenderer_00024Companion_nativeDrawFrame(
        JNIEnv *env, jobject thiz) {
    if(textureWrap != nullptr){
        textureWrap->draw();
    }
}
