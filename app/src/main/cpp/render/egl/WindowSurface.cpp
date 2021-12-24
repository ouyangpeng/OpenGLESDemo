#include <cassert>
#include <LogUtils.h>
#include "WindowSurface.h"

WindowSurface::WindowSurface(EglCore *eglCore, ANativeWindow *window, bool releaseSurface)
        : EglSurfaceBase(eglCore) {
    mSurface = window;
    createWindowSurface(mSurface);
    mReleaseSurface = releaseSurface;
}

WindowSurface::WindowSurface(EglCore *eglCore, ANativeWindow *window)
        : EglSurfaceBase(eglCore) {
    createWindowSurface(window);
    mSurface = window;
}

void WindowSurface::release() {
    releaseEglSurface();
    if (mSurface != nullptr) {
        ANativeWindow_release(mSurface);
        mSurface = nullptr;
    }

}

void WindowSurface::recreate(EglCore *eglCore) {
    assert(mSurface != nullptr);
    if (mSurface == nullptr) {
        LOGD("not yet implemented ANativeWindow")
        return;
    }
    mEglCore = eglCore;
    createWindowSurface(mSurface);
}