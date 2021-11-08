package com.oyp.openglesdemo.lesson.lesson5

import android.content.Context
import android.opengl.GLSurfaceView
import android.view.MotionEvent
import com.oyp.openglesdemo.BaseGLActivity


class LessonFiveActivity : BaseGLActivity() {
    override fun getSurfaceView(): GLSurfaceView {
        return LessonFiveGLSurfaceView(this)
    }

    override fun getRender(): GLSurfaceView.Renderer {
        return LessonFiveNativeRenderer(this)
    }

    internal class LessonFiveGLSurfaceView(context: Context?) : GLSurfaceView(context) {

        private var mRenderer: BlendingMode? = null

        override fun onTouchEvent(event: MotionEvent): Boolean {
            if (event != null) {
                if (event.action == MotionEvent.ACTION_DOWN) {
                    if (mRenderer != null) {
                        // Ensure we call switchMode() on the OpenGL thread.
                        // queueEvent() is a method of GLSurfaceView that will do this for us.
                        queueEvent { mRenderer?.switchMode() }
                        return true
                    }
                }
            }
            return super.onTouchEvent(event)
        }

        // Hides superclass method.
        override fun setRenderer(renderer: Renderer) {
            if (renderer is BlendingMode) {
                mRenderer = renderer as BlendingMode
            }
            super.setRenderer(renderer)
        }
    }
}