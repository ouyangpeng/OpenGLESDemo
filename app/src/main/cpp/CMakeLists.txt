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

#设置包含的目录
include_directories(
        glesdemo/cube
        glesdemo/lesson
        glesdemo/lesson/lesson7
        glesdemo/texture
        glesdemo/triangle

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
)


add_library( # Sets the name of the library.
        opengles-lesson-lib

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