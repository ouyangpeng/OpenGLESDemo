// 专门用来打印日志的

#ifndef OPEN_GL_LESSON_NATIVE_LOG_UTILS_H_
#define OPEN_GL_LESSON_NATIVE_LOG_UTILS_H_

#include <android/log.h>
#define LOG_TAG "[OPENGL_LESSON]"
#define LOGI(fmt, args...) __android_log_print(ANDROID_LOG_INFO, LOG_TAG, fmt, ##args)
#define LOGD(fmt, args...) __android_log_print(ANDROID_LOG_DEBUG, LOG_TAG, fmt, ##args)
#define LOGE(fmt, args...) __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, fmt, ##args)

#endif //OPEN_GL_LESSON_NATIVE_LOG_UTILS_H_
