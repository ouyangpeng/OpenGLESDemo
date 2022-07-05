//
// Created by OuyangPeng on 2022/7/5.
//

#ifndef OPENGLESDEMO_IMAGELOADER_H
#define OPENGLESDEMO_IMAGELOADER_H

#include <string.h>
#include <string>

class ImageLoader{
public:
    static unsigned int TextureFromFile(const char *path, const std::string &directory, bool gamma = false) ;
};

#endif //OPENGLESDEMO_IMAGELOADER_H
