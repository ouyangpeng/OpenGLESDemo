package com.oyp.openglesdemo.lesson.lesson6

import android.app.Activity
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class LessonSixNativeRenderer(activity: Activity) : GLSurfaceView.Renderer, Action {

    private var mActivity: Activity = activity

    external fun nativeSurfaceCreate(assetManager: AssetManager)
    external fun nativeSurfaceChange(width: Int, height: Int)
    external fun nativeDrawFrame()
    external fun nativeSetDelta(x: Float, y: Float)
    external fun nativeSetMinFilter(filter: Int)
    external fun nativeSetMagFilter(filter: Int)

    init {
        System.loadLibrary("opengles-lesson-lib")
    }

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        nativeSurfaceCreate(mActivity.assets)
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        nativeSurfaceChange(width, height)
    }

    override fun onDrawFrame(gl: GL10) {
        nativeDrawFrame()
    }

    override fun setMinFilter(filter: Int) {
        nativeSetMinFilter(filter)
    }

    override fun setMagFilter(filter: Int) {
        nativeSetMagFilter(filter)
    }

    override fun setDelta(deltaX: Float, deltaY: Float) {
        nativeSetDelta(deltaX, deltaY)
    }

}