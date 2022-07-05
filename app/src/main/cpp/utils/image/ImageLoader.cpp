//
// Created by OuyangPeng on 2022/7/5.
//

#include "ImageLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>
#include <GLUtils.h>

unsigned int
ImageLoader::TextureFromFile(const char *path, const std::string &directory, bool gamma) {
    std::string filename = std::string(path);
    filename = directory + '/' + filename;

    unsigned int textureID;
    glGenTextures(1, &textureID);

    // load the texture using stb_image
    LOGD("TextureFromFile Loading texture %s", filename.c_str())

    int width, height, nrComponents;
    unsigned char *data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        if (nrComponents == 1)
            format = GL_LUMINANCE;
        else if(nrComponents == 2)
            format = GL_LUMINANCE_ALPHA;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;
        else
            format = GL_RGB;

        LOGD("nrComponents = %d",nrComponents)

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    } else {
        LOGD("Texture failed to load at path: %s", path)
        stbi_image_free(data);
    }
    return textureID;
}