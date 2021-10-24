//
// Created by OuyangPeng on 2021/10/24.
//

#ifndef OPENGLESDEMO_TIMEUTILS_H
#define OPENGLESDEMO_TIMEUTILS_H

#include <sys/time.h>

class TimeUtils{

public:
    static long currentTimeMillis() {
        struct timeval tv {};
        gettimeofday(&tv, (struct timezone*) nullptr);
        return tv.tv_sec * 1000 + tv.tv_usec / 1000;
    }
};

#endif //OPENGLESDEMO_TIMEUTILS_H
