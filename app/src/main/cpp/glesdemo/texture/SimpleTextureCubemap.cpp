//
// Created by OuyangPeng on 2021/10/24.
//

#include "SimpleTextureCubemap.h"

namespace NAMESPACE_SimpleTextureCubemap {
    // 顶点着色器
    const char* VERTEX_SHADER_TRIANGLE =
            "#version 300 es                            \n"
            "layout(location = 0) in vec4 a_position;   \n"
            "layout(location = 1) in vec3 a_normal;     \n"
            "out vec3 v_normal;                         \n"
            "void main()                                \n"
            "{                                          \n"
            "   gl_Position = a_position;               \n"
            "   v_normal = a_normal;                    \n"
            "}                                          \n";

    // 片段着色器
    const char* FRAGMENT_SHADER_TRIANGLE =
            "#version 300 es                                     \n"
            "precision mediump float;                            \n"
            "in vec3 v_normal;                                   \n"
            "layout(location = 0) out vec4 outColor;             \n"
            "uniform samplerCube s_texture;                      \n"
            "void main()                                         \n"
            "{                                                   \n"
            "   outColor = texture( s_texture, v_normal );       \n"
            "}                                                   \n";

    SimpleTextureCubemap::SimpleTextureCubemap() {

    }

    SimpleTextureCubemap::~SimpleTextureCubemap() {

    }

    void SimpleTextureCubemap::create() {
        GLUtils::printGLString("Version", GL_VERSION);
        GLUtils::printGLString("Vendor", GL_VENDOR);
        GLUtils::printGLString("Renderer", GL_RENDERER);
        GLUtils::printGLString("Extensions", GL_EXTENSIONS);

        programObject = GLUtils::createProgram(&VERTEX_SHADER_TRIANGLE, &FRAGMENT_SHADER_TRIANGLE);
        if (!programObject) {
            LOGD("Could not create program");
            return;
        }
        // Get the sampler location
        samplerLoc = glGetUniformLocation(programObject, "s_texture");

        // Load the texture  加载纹理
        textureId = CreateSimpleTextureCubemap();

        numIndices = esGenSphere ( 20, 0.75f, &vertices, &normals,
                                             nullptr, &indices );

        // 设置清除颜色
        glClearColor(1.0f, 1.0f, 1.0f, 0.0f);
    }


    void SimpleTextureCubemap::draw() {
        // Set the viewport
        glViewport ( 0, 0, mWidth, mHeight );

        // Clear the color buffer
        glClear(GL_COLOR_BUFFER_BIT);

        glCullFace(GL_BACK);
        glEnable(GL_CULL_FACE);

        // Use the program object
        glUseProgram(programObject);

        // Load the vertext position
        glVertexAttribPointer(0,3,GL_FLOAT,
                GL_FALSE, 0, vertices);

        // Load the normal
        glVertexAttribPointer(1,3, GL_FLOAT,
                GL_FALSE, 0, normals);

        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);

        // Bind the texture
        // 激活当前纹理单元，以便后续的glBindTexture调用将纹理绑定到当前活动单元
        glActiveTexture(GL_TEXTURE0);
        // 纹理绑定到纹理单元0
        glBindTexture(GL_TEXTURE_CUBE_MAP,textureId);

        // Set the sampler texture unit to 0
        // 采样器设置为纹理单元0
        glUniform1i(samplerLoc,0);

        glDrawElements(GL_TRIANGLES,numIndices,GL_UNSIGNED_INT,indices);
    }

    void SimpleTextureCubemap::change(int width, int height) {
        mWidth = width;
        mHeight = height;
        LOGD("change() width = %d , height = %d\n", width, height);
    }

    // Create a simple cubemap with a 1x1 face with a different color for each face
    GLuint SimpleTextureCubemap::CreateSimpleTextureCubemap()
    {
        GLuint textureId;

        // Six 1x1 RGB faces
        GLubyte cubePixels[6][3] =
                {
                        // Face 0 - Red
                        255, 0, 0,
                        // Face 1 - Green,
                        0, 255, 0,
                        // Face 2 - Blue
                        0, 0, 255,
                        // Face 3 - Yellow
                        255, 255, 0,
                        // Face 4 - Purple
                        255, 0, 255,
                        // Face 5 - White
                        255, 255, 255
                };

        // Generate a texture object
        glGenTextures ( 1, &textureId );

        // Bind the texture object
        glBindTexture ( GL_TEXTURE_CUBE_MAP, textureId );

        // Load the cube face - Positive X
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, 1, 1, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[0] );

        // Load the cube face - Negative X
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, 1, 1, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[1] );

        // Load the cube face - Positive Y
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, 1, 1, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[2] );

        // Load the cube face - Negative Y
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, 1, 1, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[3] );

        // Load the cube face - Positive Z
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, 1, 1, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[4] );

        // Load the cube face - Negative Z
        glTexImage2D ( GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, 1, 1, 0,
                       GL_RGB, GL_UNSIGNED_BYTE, &cubePixels[5] );

        // Set the filtering mode
        glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST );
        glTexParameteri ( GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST );

        return textureId;
    }
}

///////////////////////////////////////////////////////////////////////////////////////////

static NAMESPACE_SimpleTextureCubemap::SimpleTextureCubemap * simpleTextureCubemap;

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_SimpleTextureCubemapRenderer_00024Companion_nativeSurfaceCreate(
        JNIEnv *env, jobject thiz) {
    simpleTextureCubemap = new  NAMESPACE_SimpleTextureCubemap::SimpleTextureCubemap();
    if(simpleTextureCubemap != nullptr){
        simpleTextureCubemap->create();
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_SimpleTextureCubemapRenderer_00024Companion_nativeSurfaceChange(
        JNIEnv *env, jobject thiz, jint width, jint height) {
    if(simpleTextureCubemap != nullptr){
        simpleTextureCubemap->change(width,height);
    }
}

extern "C"
JNIEXPORT void JNICALL
Java_com_oyp_openglesdemo_texture_SimpleTextureCubemapRenderer_00024Companion_nativeDrawFrame(
        JNIEnv *env, jobject thiz) {
    if(simpleTextureCubemap != nullptr){
        simpleTextureCubemap->draw();
    }
}