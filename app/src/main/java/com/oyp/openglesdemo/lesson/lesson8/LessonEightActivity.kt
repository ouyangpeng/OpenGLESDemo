package com.oyp.openglesdemo.lesson.lesson8

import android.opengl.GLSurfaceView
import android.util.DisplayMetrics
import com.oyp.openglesdemo.BaseGLActivity

class LessonEightActivity : BaseGLActivity() {

    override fun getSurfaceView(): GLSurfaceView {
        val mGLSurfaceView = LessonEightGLSurfaceView(this)

        val displayMetrics = DisplayMetrics()
        windowManager.defaultDisplay.getMetrics(displayMetrics)
        // Set the renderer to out demo renderer, define below
        mGLSurfaceView.setRenderer(renderer, displayMetrics.density)
        return mGLSurfaceView
    }

    override fun getRender(): GLSurfaceView.Renderer {
        return LessonEightNativeRenderer(this)
    }
}