// 专门用来打印日志的

#ifndef OPEN_GL_LESSON_NATIVE_LOG_UTILS_H_
#define OPEN_GL_LESSON_NATIVE_LOG_UTILS_H_

// 实现 C语言中打印log到android控制台
// 导入 头文件 android/log.h
#include <android/log.h>
#include <string.h>
#include <sys/time.h>

// 文件名
#define __FILENAME__ (strrchr(__FILE__, '/') + 1)

#define LOG_SWITCH_STATUS_ON

#ifdef LOG_SWITCH_STATUS_ON
// 定义LOG 标签
#define TAG "NDK_JNI_LOG_TAG"
// 定义几个打印日志的方法
#define LOGV(format, ...) __android_log_print(ANDROID_LOG_VERBOSE, TAG,\
        "[%s][%s][%d]: " format, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define LOGD(format, ...) __android_log_print(ANDROID_LOG_DEBUG, TAG,\
        "[%s][%s][%d]: " format, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define LOGI(format, ...) __android_log_print(ANDROID_LOG_INFO, TAG,\
        "[%s][%s][%d]: " format, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define LOGW(format, ...) __android_log_print(ANDROID_LOG_WARN, TAG,\
        "[%s][%s][%d]: " format, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#define LOGE(format, ...) __android_log_print(ANDROID_LOG_ERROR, TAG,\
        "[%s][%s][%d]: " format, __FILENAME__, __FUNCTION__, __LINE__, ##__VA_ARGS__);
#else
#define LOGV(format, ...);
#define LOGD(format, ...);
#define LOGI(format, ...);
#define LOGW(format, ...);
#define LOGE(format, ...);
#endif


#define FUN_BEGIN_TIME(FUN) {\
    LOGE("[%s] func start", FUN); \
    long long t0 = GetSysCurrentTime();

#define FUN_END_TIME(FUN) \
    long long t1 = GetSysCurrentTime(); \
    LOGE("[%s] func cost time %ldms", FUN, (long)(t1-t0));}

#define BEGIN_TIME(FUN) {\
    LOGE("[%s] func start", FUN); \
    long long t0 = GetSysCurrentTime();

#define END_TIME(FUN) \
    long long t1 = GetSysCurrentTime(); \
    LOGE("[%s] func cost time %ldms", FUN, (long)(t1-t0));}

static long long GetSysCurrentTime()
{
    struct timeval time;
    gettimeofday(&time, NULL);
    long long curTime = ((long long)(time.tv_sec))*1000+time.tv_usec/1000;
    return curTime;
}

#define GO_CHECK_GL_ERROR(...)   LOGD("CHECK_GL_ERROR %s glGetError = %d, line = %d, ",  __FUNCTION__, glGetError(), __LINE__)

#endif //OPEN_GL_LESSON_NATIVE_LOG_UTILS_H_
