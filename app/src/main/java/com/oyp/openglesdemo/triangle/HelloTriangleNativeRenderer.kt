package com.oyp.openglesdemo.triangle

import android.app.Activity
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class HelloTriangleNativeRenderer (activity: Activity): GLSurfaceView.Renderer {
    private var mActivity: Activity = activity

    external fun nativeSurfaceCreate(assetManager: AssetManager)
    external fun nativeSurfaceChange(width: Int, height: Int)
    external fun nativeDrawFrame()

    init {
        System.loadLibrary("opengles-lesson-lib")
    }

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        val assetManager: AssetManager = mActivity.assets
        nativeSurfaceCreate(assetManager)
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        nativeSurfaceChange(width, height)
    }

    override fun onDrawFrame(gl: GL10) {
        nativeDrawFrame()
    }
}