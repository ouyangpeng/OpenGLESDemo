package com.oyp.openglesdemo.lesson

import android.app.Activity
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class LessonFiveNativeRenderer(activity:Activity) : GLSurfaceView.Renderer,BlendingMode {

    private var mActivity: Activity = activity

    companion object {
        external fun nativeSurfaceCreate(assetManager: AssetManager)
        external fun nativeSurfaceChange(width: Int, height: Int)
        external fun nativeDrawFrame()
        external fun nativeSwitchMode()

        init {
            System.loadLibrary("opengles-lesson-lib")
        }
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

    override fun switchMode() {
        nativeSwitchMode()
    }
}