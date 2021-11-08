package com.oyp.openglesdemo.lesson

import android.opengl.GLSurfaceView
import com.oyp.openglesdemo.BaseGLActivity


class LessonFourActivity : BaseGLActivity() {
    override fun getSurfaceView(): GLSurfaceView {
        return GLSurfaceView(this)
    }

    override fun getRender(): GLSurfaceView.Renderer {
        return LessonFourNativeRenderer(this)
    }
}