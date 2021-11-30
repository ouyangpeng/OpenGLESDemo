package com.oyp.openglesdemo.lesson.lesson8

import android.app.Activity
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class LessonEightNativeRenderer(activity: Activity) : GLSurfaceView.Renderer, Action {
    private val mActivity: Activity = activity

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        nativeSurfaceCreate(mActivity.assets)
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        nativeSurfaceChange(width, height)
    }

    override fun onDrawFrame(gl: GL10) {
        nativeDrawFrame()
    }

    override fun setDelta(deltaX: Float, deltaY: Float) {
        nativeSetDelta(deltaX, deltaY)
    }

    private external fun nativeSurfaceCreate(assetManager: AssetManager?)
    private external fun nativeSurfaceChange(width: Int, height: Int)
    private external fun nativeDrawFrame()
    private external fun nativeSetDelta(x: Float, y: Float)

    init {
        System.loadLibrary("ouyangpeng-opengles-lib")
    }
}