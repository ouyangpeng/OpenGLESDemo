package com.oyp.openglesdemo

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.view.MotionEvent
import com.oyp.openglesdemo.render.IMyNativeRendererType
import com.oyp.openglesdemo.render.MyNativeRenderer

class MyCustomerGLSurfaceView : GLSurfaceView {
    private var mRenderer: MyNativeRenderer? = null
    private var mPreviousX = 0f
    private var mPreviousY = 0f
    private var mDensity = 0f

    constructor(context: Context?) : super(context) {}

    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs) {}

    constructor(context: Context?, glRender: MyNativeRenderer?, eglContextVersion: Int) : this(
        context,
        null,
        glRender,
        eglContextVersion
    )

    constructor(
        context: Context?,
        attrs: AttributeSet?,
        glRender: MyNativeRenderer?,
        eglContextVersion: Int
    ) : super(context, attrs) {
        setEGLContextClientVersion(eglContextVersion)
        mRenderer = glRender
        setRenderer(mRenderer)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        if (event != null) {
            val x = event.x
            val y = event.y
            if (event.action == MotionEvent.ACTION_DOWN) {
                if (mRenderer != null) {
                    // 如果是SAMPLE_TYPE_KEY_LESSON_FIVE，则执行mRenderer?.switchBlendingMode()
                    if (mRenderer!!.mSampleType == IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_FIVE) {
                        // Ensure we call switchMode() on the OpenGL thread.
                        // queueEvent() is a method of GLSurfaceView that will do this for us.
                        queueEvent { mRenderer?.switchBlendingMode() }
                        return true
                    }
                }
            } else if (event.action == MotionEvent.ACTION_MOVE) {
                if (mRenderer != null) {
                    val deltaX = (x - mPreviousX) / mDensity / 2
                    val deltaY = (y - mPreviousY) / mDensity / 2
                    // 如果是SAMPLE_TYPE_KEY_LESSON_FIVE，则执行mRenderer?.switchBlendingMode()
                    if (mRenderer!!.mSampleType == IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_SIX) {
                        mRenderer!!.setDelta(deltaX, deltaY)
                    }
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
        mRenderer = renderer as MyNativeRenderer?
        mDensity = density
        super.setRenderer(renderer)
    }
}