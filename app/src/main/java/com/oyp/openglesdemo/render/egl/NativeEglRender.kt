package com.oyp.openglesdemo.render.egl

import android.app.Activity
import android.content.res.AssetManager

class NativeEglRender (activity: Activity) {
    init {
        System.loadLibrary("ouyangpeng-opengles-lib")
    }
    private var mActivity: Activity = activity

    fun eglRenderInit(){
        val assetManager: AssetManager = mActivity.assets
        nativeEglRenderInit(assetManager)
    }

    private external fun nativeEglRenderInit(assetManager: AssetManager)
    external fun nativeEglRenderSetImageData(data: ByteArray?, width: Int, height: Int)
    external fun nativeEglRenderSetFragmentShaderType(paramType: Int, fShaderType: Int)
    external fun nativeEglRenderDraw()
    external fun nativeEglRenderUnInit()
}