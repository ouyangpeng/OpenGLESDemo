package com.oyp.openglesdemo.lesson1

import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class LessonOneNativeRenderer : GLSurfaceView.Renderer {
    companion object {
        external fun nativeSurfaceCreate()
        external fun nativeSurfaceChange(width: Int, height: Int)
        external fun nativeDrawFrame()

        init {
            System.loadLibrary("opengles-lesson-lib")
        }
    }

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        nativeSurfaceCreate()
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        nativeSurfaceChange(width, height)
    }

    override fun onDrawFrame(gl: GL10) {
        nativeDrawFrame()
    }
}