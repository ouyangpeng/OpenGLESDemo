#ifndef OPENGLLESSON_CUBESCLIENTSIDE_H
#define OPENGLLESSON_CUBESCLIENTSIDE_H

#include <GLES3/gl3.h>
#include "Cubes.h"

class CubesClientSide : public Cubes {

public:
    vector<float> *mCubePositions = nullptr;
    vector<float> *mCubeNormals = nullptr;
    vector<float> *mCubeTextureCoordinates = nullptr;

public:
    CubesClientSide(vector<float> *cubePositions, vector<float> *cubeNormals,
                    vector<float> *cubeTextureCoordinates, int generatedCubeFactor) {
        vector<vector<float> *> buffers = getBuffers(cubePositions, cubeNormals,
                                                     cubeTextureCoordinates, generatedCubeFactor);
        CubesClientSide::mCubePositions = buffers.at(0);
        CubesClientSide::mCubeNormals = buffers.at(1);
        CubesClientSide::mCubeTextureCoordinates = buffers.at(2);
    }

    virtual void renderer() override;

    virtual void release() override;
};


#endif
