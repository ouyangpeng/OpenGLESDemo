# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# Declares and names the project.

project("openglesdemo")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        opengles-lesson-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).

        # utils.graphics
        utils/graphics/Matrix.h
        utils/graphics/Matrix.cpp

        # utils.log
        utils/log/LogUtils.h

        # utils.time
        utils/time/TimeUtils.h

        # utils
        utils/esTransform.cpp
        utils/esTransform.h

        utils/esShapes.cpp
        utils/esShapes.h

        utils/GLUtils.h
        utils/GLUtils.cpp

        # glesdemo.lesson1
        glesdemo/lesson1/Native1Lesson.cpp
        glesdemo/lesson1/Native1Lesson.h

        # glesdemo.triangle
        glesdemo/triangle/NativeTriangle.cpp
        glesdemo/triangle/NativeTriangle.h

        glesdemo/triangle/NativeTriangle2.cpp
        glesdemo/triangle/NativeTriangle2.h

        glesdemo/triangle/NativeTriangle3.cpp
        glesdemo/triangle/NativeTriangle3.h

        glesdemo/triangle/NativeTriangleMapBuffers.cpp
        glesdemo/triangle/NativeTriangleMapBuffers.h

        glesdemo/triangle/NativeTriangleVertextArrayObject.cpp
        glesdemo/triangle/NativeTriangleVertextArrayObject.h

        glesdemo/triangle/NativeTriangleVertextBufferObject.cpp
        glesdemo/triangle/NativeTriangleVertextBufferObject.h

        glesdemo/cube/NativeCubeSimpleVertexShader.cpp
        glesdemo/cube/NativeCubeSimpleVertexShader.h

        # texture
        glesdemo/texture/SimpleTexture2D.cpp
        glesdemo/texture/SimpleTexture2D.h

        glesdemo/texture/SimpleTextureCubemap.cpp
        glesdemo/texture/SimpleTextureCubemap.h

        glesdemo/texture/MipMap2D.cpp
        glesdemo/texture/MipMap2D.h

        glesdemo/texture/TextureWrap.cpp
        glesdemo/texture/TextureWrap.h
       )

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        opengles-lesson-lib

        EGL
#        GLESv2  # 把opengl库文件添加进来，GLESv2
        GLESv3   # 把opengl库文件添加进来，GLESv3
        android

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})