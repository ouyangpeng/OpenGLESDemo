#include "OffscreenSurface.h"

OffscreenSurface::OffscreenSurface(EglCore *eglCore, int width, int height)
        : EglSurfaceBase(eglCore) {
    createOffscreenSurface(width, height);
}

void OffscreenSurface::release() {
    releaseEglSurface();
}


