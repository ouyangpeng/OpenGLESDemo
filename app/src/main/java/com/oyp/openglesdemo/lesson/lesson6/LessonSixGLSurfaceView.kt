package com.oyp.openglesdemo.lesson.lesson6

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.view.MotionEvent

class LessonSixGLSurfaceView : GLSurfaceView {
    private var mRenderer: Action? = null
    private var mPreviousX = 0f
    private var mPreviousY = 0f
    private var mDensity = 0f

    constructor(context: Context?) : super(context) {}

    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs) {}

    override fun onTouchEvent(event: MotionEvent): Boolean {
        if (event != null) {
            val x = event.x
            val y = event.y
            if (event.action == MotionEvent.ACTION_MOVE) {
                if (mRenderer != null) {
                    val deltaX = (x - mPreviousX) / mDensity / 2
                    val deltaY = (y - mPreviousY) / mDensity / 2
                    mRenderer!!.setDelta(deltaX, deltaY)
                }
            }
            mPreviousX = x
            mPreviousY = y
            return true
        }
        return super.onTouchEvent(event)
    }

    // Hides superclass method.
    fun setRenderer(renderer: Renderer?, density: Float) {
        mRenderer = renderer as Action?
        mDensity = density
        super.setRenderer(renderer)
    }
}