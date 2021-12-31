# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

# 设置 jnilibs 目录的值，后面会引用这个变量${jnilibs}
set(jnilibs "${CMAKE_SOURCE_DIR}/../jniLibs")

# Declares and names the project.

project("openglesdemo")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

#设置包含的目录
include_directories(
        inc
#       inc/assimp
        inc/freetype_2_9_1
        inc/opencv_3_0_0

        include

        jni

        model

        render
        render/egl
        render/looper

        sample
        sample/airhockey
        sample/byteflow
        sample/cube
        sample/lesson
        sample/lesson/lesson7
        sample/lesson/lesson8
        sample/shadertoy
        sample/texture
        sample/triangle

        utils
        utils/graphics
        utils/log
        utils/time
)

#自定义搜索规则
file(GLOB src-files
        ${CMAKE_SOURCE_DIR}/*.cpp
        ${CMAKE_SOURCE_DIR}/*.h

        ${CMAKE_SOURCE_DIR}/*/*.cpp
        ${CMAKE_SOURCE_DIR}/*/*.h

        ${CMAKE_SOURCE_DIR}/*/*/*.cpp
        ${CMAKE_SOURCE_DIR}/*/*/*.h

        ${CMAKE_SOURCE_DIR}/*/*/*/*.cpp
        ${CMAKE_SOURCE_DIR}/*/*/*/*.h

        ${CMAKE_SOURCE_DIR}/*/*/*/*/*.cpp
        ${CMAKE_SOURCE_DIR}/*/*/*/*/*.h

        ${CMAKE_SOURCE_DIR}/*/*/*/*/*/*.cpp
        ${CMAKE_SOURCE_DIR}/*/*/*/*/*/*.h

        ${CMAKE_SOURCE_DIR}/*/*/*/*/*/*/*.cpp
        ${CMAKE_SOURCE_DIR}/*/*/*/*/*/*/*.h
)


add_library( # Sets the name of the library.
        ouyangpeng-opengles-lib

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        ${src-files}
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

########################## 第三方库 相关配置  开始################################
add_library(freetype STATIC IMPORTED)
set_target_properties(freetype
        PROPERTIES
        IMPORTED_LOCATION
        ${jnilibs}/${CMAKE_ANDROID_ARCH_ABI}/libfreetype.a)

add_library(assimp SHARED IMPORTED )
set_target_properties(assimp
        PROPERTIES
        IMPORTED_LOCATION
        ${jnilibs}/${CMAKE_ANDROID_ARCH_ABI}/libassimp.so)

add_library(opencv_java3 SHARED IMPORTED )
set_target_properties(opencv_java3
        PROPERTIES
        IMPORTED_LOCATION
        ${jnilibs}/${CMAKE_ANDROID_ARCH_ABI}/libopencv_java3.so)

set(third-party-libs
    freetype
    assimp
    opencv_java3
)
########################## 第三方库 相关配置  结束################################

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        ouyangpeng-opengles-lib

        EGL
        GLESv3   # 把opengl库文件添加进来，GLESv3
        android

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib}
        # 第三方库
        ${third-party-libs}
        )