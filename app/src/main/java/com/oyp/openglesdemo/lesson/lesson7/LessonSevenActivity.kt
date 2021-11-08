package com.oyp.openglesdemo.lesson.lesson7

import android.app.Activity
import android.app.ActivityManager
import android.os.Bundle
import android.util.DisplayMetrics
import android.util.Log
import android.view.View
import android.widget.Button
import com.oyp.openglesdemo.R

class LessonSevenActivity : Activity() {
    private var mGLSurfaceView: LessonSevenGLSurfaceView? = null
    private var mRenderer: Action? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.lesson_seven)

        mGLSurfaceView = findViewById<View>(R.id.gl_surface_view) as LessonSevenGLSurfaceView

        if(detectOpenGLES30() && mGLSurfaceView != null ){
            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.

            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.
            mGLSurfaceView!!.setEGLContextClientVersion(3)

            val displayMetrics = DisplayMetrics()
            windowManager.defaultDisplay.getMetrics(displayMetrics)

            // Set the renderer to out demo renderer, define below
            mRenderer = LessonSevenNativeRenderer(this, mGLSurfaceView!!)
            (mRenderer as LessonSevenNativeRenderer).init()
            mGLSurfaceView!!.setRenderer(mRenderer as LessonSevenNativeRenderer, displayMetrics.density)
        } else {
            Log.e("LessonSixActivity", "OpenGL ES 3.0 not supported on device.  Exiting...")
            return
        }

        findViewById<View>(R.id.button_decrease_num_cubes).setOnClickListener { decreaseCubeCount() }

        findViewById<View>(R.id.button_increase_num_cubes).setOnClickListener { increaseCubeCount() }

        findViewById<View>(R.id.button_switch_VBOs).setOnClickListener { toggleVBOs() }

        findViewById<View>(R.id.button_switch_stride).setOnClickListener { toggleStride() }
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

    override fun onDestroy() {
        super.onDestroy()
        mRenderer?.destroy()
    }

    private fun decreaseCubeCount() {
        mGLSurfaceView?.queueEvent { mRenderer?.decreaseCubeCount() }
    }

    private fun increaseCubeCount() {
        mGLSurfaceView?.queueEvent { mRenderer?.increaseCubeCount() }
    }

    private fun toggleVBOs() {
        mGLSurfaceView?.queueEvent { mRenderer?.toggleVBOs() }
    }

    private fun toggleStride() {
        mGLSurfaceView?.queueEvent { mRenderer?.toggleStride() }
    }

    // 这个方法最终会在jni层被调用
    fun updateVboStatus(usingVbos: Boolean) {
        runOnUiThread {
            if (usingVbos) {
                (findViewById<View>(R.id.button_switch_VBOs) as Button).setText(
                    R.string.lesson_seven_using_VBOs
                )
            } else {
                (findViewById<View>(R.id.button_switch_VBOs) as Button).setText(
                    R.string.lesson_seven_not_using_VBOs
                )
            }
        }
    }

    // 这个方法最终会在jni层被调用
    fun updateStrideStatus(useStride: Boolean) {
        runOnUiThread {
            if (useStride) {
                (findViewById<View>(R.id.button_switch_stride) as Button).setText(
                    R.string.lesson_seven_using_stride
                )
            } else {
                (findViewById<View>(R.id.button_switch_stride) as Button).setText(
                    R.string.lesson_seven_not_using_stride
                )
            }
        }
    }
}