#include <cstdlib>
#include "GLUtils.h"

typedef AAsset esFile;

// 注意点：保证内存是连续的，不然读取错误  使用  #pragma pack(1)  或者  __attribute__ ( ( packed ) ) 都可以
// 【我的C语言学习进阶之旅】关于C/C++内存对齐读取文件产生的问题以及解决方法  https://blog.csdn.net/qq446282412/article/details/121539330
// C/C++内存对齐详解  https://zhuanlan.zhihu.com/p/30007037
// #pragma的常用方法讲解   https://blog.csdn.net/weixin_39640298/article/details/84503428
//#pragma pack(push,x1)                            // Byte alignment (8-bit)
//#pragma pack(1)           // 如果前面加上#pragma pack(1)，那么此时有效对齐值为1字节
typedef struct
//  C语言__attribute__的使用  https://blog.csdn.net/qlexcel/article/details/92656797
//  使用该属性对struct 或者union 类型进行定义，设定其类型的每一个变量的内存约束。
//  就是告诉编译器取消结构在编译过程中的优化对齐（使用1字节对齐）,按照实际占用字节数进行对齐，是GCC特有的语法。
//  这个功能是跟操作系统没关系，跟编译器有关，gcc编译器不是紧凑模式的
    __attribute__ (( packed )) {
    unsigned char IdSize,
            MapType,
            ImageType;
    unsigned short PaletteStart,
            PaletteSize;
    unsigned char PaletteEntryDepth;
    unsigned short X,
            Y,
            Width,
            Height;
    unsigned char ColorDepth,
            Descriptor;
} TGA_HEADER;
//#pragma pack(pop,x1)


static JNIEnv *sEnv = nullptr;
static jobject sAssetManager = nullptr;

void GLUtils::setEnvAndAssetManager(JNIEnv *env, jobject assetManager) {
    sEnv = env;
    sAssetManager = assetManager;
}

static AAsset *loadAsset(const char *path) {
    AAssetManager *nativeManager = AAssetManager_fromJava(sEnv, sAssetManager);
    if (nativeManager == nullptr) {
        return nullptr;
    }
    return AAssetManager_open(nativeManager, path, AASSET_MODE_UNKNOWN);
}

//
// File open
//
static esFile *esFileOpen(const char *fileName) {
    esFile *file;
    FUN_BEGIN_TIME("GLUtils::esFileOpen")
        AAssetManager *nativeManager = AAssetManager_fromJava(sEnv, sAssetManager);
        if (nativeManager == nullptr) {
            return nullptr;
        }
        file = AAssetManager_open(nativeManager, fileName, AASSET_MODE_BUFFER);
    FUN_END_TIME("GLUtils::esFileOpen")
    return file;
}

//
// File close
//
static void esFileClose(esFile *pFile) {
    FUN_BEGIN_TIME("GLUtils::esFileClose")
        if (pFile != nullptr) {
            AAsset_close(pFile);
        }
    FUN_END_TIME("GLUtils::esFileClose")
}

//
// File read
//
static int esFileRead(esFile *pFile, int bytesToRead, void *buffer) {
    int bytesRead = 0;
    FUN_BEGIN_TIME("GLUtils::esFileRead")
        if (pFile == nullptr) {
            return bytesRead;
        }
        bytesRead = AAsset_read(pFile, buffer, bytesToRead);
    FUN_END_TIME("GLUtils::esFileRead")
    return bytesRead;
}

/**
 * Loads the given source code as a shader of the given type.
 *
 * 负责 加载着色器源代码、编译并检查错误。他返回一个着色器对象
 */
static GLuint loadShader(GLenum shaderType, const char** source) {
    // Create the shader object
    GLuint shader;
    FUN_BEGIN_TIME("GLUtils::loadShader")
        GLint compiled;
        // Create the shader object
        // shaderType 可以是  GL_VERTEX_SHADER  或者  GL_FRAGMENT_SHADER
        shader = glCreateShader(shaderType);
        if (shader == 0) {
            return 0;
        }

        // Load the shader source
        glShaderSource(shader, 1, source, nullptr);

        // Compile the shader
        glCompileShader(shader);

        // Check the compile status
        glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);

        if (!compiled) {
            GLint infoLen = 0;

            glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

            if (infoLen > 1) {
                char *infoLog = (char *) malloc(sizeof(char) * infoLen);
                if (infoLog) {
                    // 检索信息日志
                    glGetShaderInfoLog(shader, infoLen, nullptr, infoLog);
                    LOGE("GLUtils::loadShader error compiling shader:\n%s\n", infoLog)

                    free(infoLog);
                }
                // 删除Shader
                glDeleteShader(shader);
                return 0;
            }
        FUN_END_TIME("GLUtils::loadShader")
        return shader;
    }
}

GLuint GLUtils::createProgram(const char** vertexSource, const char** fragmentSource) {
    GLuint program = 0;
    FUN_BEGIN_TIME("GLUtils::createProgram")
        // Load the Vertex shader
        GLuint vertexShader = loadShader(GL_VERTEX_SHADER, vertexSource);
        if (vertexShader == 0) {
            return 0;
        }
        // Load the Fragment shader
        GLuint fragmentShader = loadShader(GL_FRAGMENT_SHADER, fragmentSource);
        if (fragmentShader == 0) {
            return 0;
        }

        // Create the program object
        program = glCreateProgram();
        if (program == 0) {
            return 0;
        }

        // Bind the vertex shader to the program
        glAttachShader(program, vertexShader);
        checkGlError("glAttachShader");
        // Bind the fragment shader to the program.
        glAttachShader(program, fragmentShader);
        checkGlError("glAttachShader");
        // Link the program
        glLinkProgram(program);

        // Check the link status
        GLint linkStatus;
        glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

        if (!linkStatus) {
            // Retrieve compiler error message when linking fails
            GLint infoLen = 0;
            glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLen);
            if (infoLen > 1) {
                char *infoLog = (char *) malloc(sizeof(char) * infoLen);
                if (infoLog) {
                    //获取信息
                    glGetProgramInfoLog(program, infoLen, nullptr, infoLog);
                    LOGE("GLUtils::createProgram error linking program:\n%s\n", infoLog)
                    free(infoLog);
                }
            }
            // 删除程序对象
            glDeleteProgram(program);
            return 0;
        }
        // Free up no longer needed shader resources
        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);
    FUN_END_TIME("GLUtils::createProgram")
    return program;
}

/**
 * Loads a 8-bit, 24-bit or 32-bit TGA image from a file
 * 从文件中加载一个  8-bit, 24-bit or 32-bit TGA图像
 * @param fileName  文件名
 * @param width		以像素表示的加载图像宽度
 * @param height	以像素表示的加载图像高度
 * @return	指向加载图像的指针；失败时返回NULL
 */
char *esLoadTGA(const char *fileName, int *width, int *height) {
    FUN_BEGIN_TIME("GLUtils::esLoadTGA")
        char *buffer;
        esFile *fp;
        TGA_HEADER Header;
        int bytesRead;

        // Open the file for reading
        fp = esFileOpen(fileName);

        if (fp == nullptr) {
            // Log error as 'error in opening the input file from apk'
            LOGE ("esLoadTGA FAILED to load : { %s }\n", fileName)
            return nullptr;
        }
        LOGD ("sizeof ( TGA_HEADER ) : { %d }\n", sizeof(TGA_HEADER))
        bytesRead = esFileRead(fp, sizeof(TGA_HEADER), &Header);

        *width = Header.Width;
        *height = Header.Height;

        if (Header.ColorDepth == 8 ||
            Header.ColorDepth == 24 || Header.ColorDepth == 32) {
            int bytesToRead = sizeof(char) * (*width) * (*height) * Header.ColorDepth / 8;

            // Allocate the image data buffer
            buffer = (char *) malloc(bytesToRead);

            if (buffer) {
                bytesRead = esFileRead(fp, bytesToRead, buffer);
                esFileClose(fp);

                return (buffer);
            }
        }
    FUN_END_TIME("GLUtils::esLoadTGA")
    return (nullptr);
}

//
// Load texture from disk
//
GLuint GLUtils::loadTgaTexture(const char *fileName) {
    GLuint texId;
    FUN_BEGIN_TIME("GLUtils::loadTgaTexture")
        int width, height;
        char *buffer = esLoadTGA(fileName, &width, &height);

        if (buffer == nullptr) {
            LOGI ("Error loading (%s) image.\n", fileName)
            return 0;
        }

        glGenTextures(1, &texId);
        glBindTexture(GL_TEXTURE_2D, texId);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, width, height, 0, GL_ALPHA, GL_UNSIGNED_BYTE,
                     buffer);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

        free(buffer);
    FUN_END_TIME("GLUtils::loadTgaTexture")
    return texId;
}


GLuint GLUtils::loadTexture(const char *path) {
    GLuint textureId = 0;
    FUN_BEGIN_TIME("GLUtils::loadTexture")
        jclass utilsClass = sEnv->FindClass(
                "com/oyp/openglesdemo/Utils");  //com.oyp.openglesdemo.Utils类
        if (utilsClass == nullptr) {
            LOGE("Couldn't find utils class")
            return (GLuint) -1;
        }
        jmethodID loadTexture = sEnv->GetStaticMethodID(utilsClass, "loadTexture",
                                                        "(Landroid/content/res/AssetManager;Ljava/lang/String;)I");
        if (loadTexture == nullptr) {
            LOGE("Couldn't find loadTexture method")
            return (GLuint) -1;
        }
        jstring pathStr = sEnv->NewStringUTF(path);
        textureId = (GLuint) sEnv->CallStaticIntMethod(utilsClass, loadTexture, sAssetManager,
                                                       pathStr);
        sEnv->DeleteLocalRef(pathStr);
    FUN_END_TIME("GLUtils::loadTexture")
    return textureId;
}

char *GLUtils::openTextFile(const char *path) {
    char *buffer;
    FUN_BEGIN_TIME("GLUtils::openTextFile")
        AAsset *asset = loadAsset(path);
        if (asset == nullptr) {
            LOGE("Couldn't load %s", path)
            return nullptr;
        }
        off_t length = AAsset_getLength(asset);
        buffer = new char[length + 1];
        int num = AAsset_read(asset, buffer, length);
        AAsset_close(asset);
        if (num != length) {
            LOGE("Couldn't read %s", path)
            delete[] buffer;
            return nullptr;
        }
        buffer[length] = '\0';
    FUN_END_TIME("GLUtils::openTextFile")
    return buffer;
}

void GLUtils::checkGlError(const char* pGLOperation) {
    for (GLint error = glGetError(); error; error = glGetError())
    {
        LOGE("GLUtils::CheckGLError after GL Operation %s() glError (0x%x)\n", pGLOperation, error);
    }
}