package com.oyp.openglesdemo.render.egl

class NativeEglRender {
    init {
        System.loadLibrary("ouyangpeng-opengles-lib")
    }

    external fun nativeEglRenderInit()
    external fun nativeEglRenderSetImageData(data: ByteArray?, width: Int, height: Int)
    external fun nativeEglRenderSetIntParams(paramType: Int, param: Int)
    external fun nativeEglRenderDraw()
    external fun nativeEglRenderUnInit()
}