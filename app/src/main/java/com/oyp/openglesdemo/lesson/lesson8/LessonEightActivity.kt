package com.oyp.openglesdemo.lesson.lesson8

import android.app.Activity
import android.app.ActivityManager
import android.os.Bundle
import android.util.DisplayMetrics
import android.util.Log

class LessonEightActivity : Activity() {
    private var mGLSurfaceView: LessonEightGLSurfaceView? = null
    private var mRenderer: Action? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        mGLSurfaceView = LessonEightGLSurfaceView(this)
        setContentView(mGLSurfaceView)

        if(detectOpenGLES30() && mGLSurfaceView != null ){
            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.

            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.
            mGLSurfaceView!!.setEGLContextClientVersion(3)

            val displayMetrics = DisplayMetrics()
            windowManager.defaultDisplay.getMetrics(displayMetrics)

            // Set the renderer to out demo renderer, define below
            mRenderer = LessonEightNativeRenderer(this)
            mGLSurfaceView!!.setRenderer(mRenderer as LessonEightNativeRenderer, displayMetrics.density)
        } else {
            Log.e("LessonSixActivity", "OpenGL ES 3.0 not supported on device.  Exiting...")
            return
        }
    }

    private fun detectOpenGLES30(): Boolean {
        val am = getSystemService(ACTIVITY_SERVICE) as ActivityManager
        val info = am.deviceConfigurationInfo
        return info.reqGlEsVersion >= 0x30000
    }

    override fun onResume() {
        // The activity must call the GL surface view's onResume() on activity onResume().
        super.onResume()
        mGLSurfaceView!!.onResume()
    }

    override fun onPause() {
        // The activity must call the GL surface view's onPause() on activity onPause().
        super.onPause()
        mGLSurfaceView!!.onPause()
    }
}