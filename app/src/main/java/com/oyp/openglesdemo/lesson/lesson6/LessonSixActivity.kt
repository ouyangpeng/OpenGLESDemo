package com.oyp.openglesdemo.lesson.lesson6

import android.app.Activity
import android.app.ActivityManager
import android.app.AlertDialog
import android.app.Dialog
import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.util.DisplayMetrics
import android.util.Log
import android.view.View
import com.oyp.openglesdemo.R

class LessonSixActivity : Activity() {
    private var mGLSurfaceView: LessonSixGLSurfaceView? = null
    private var mRenderer: Action? = null
    private var mMinSetting = -1
    private var mMagSetting = -1

    companion object {
        private const val MIN_DIALOG = 1
        private const val CONTEXT_CLIENT_VERSION = 3
        private const val MAG_DIALOG = 2
        private const val MIN_SETTING = "min_setting"
        private const val MAG_SETTING = "mag_setting"
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        setContentView(R.layout.lesson_six)

        mGLSurfaceView = findViewById<View>(R.id.gl_surface_view) as LessonSixGLSurfaceView

        if(detectOpenGLES30()){
            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.

            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.
            mGLSurfaceView!!.setEGLContextClientVersion(CONTEXT_CLIENT_VERSION)

            val displayMetrics = DisplayMetrics()
            windowManager.defaultDisplay.getMetrics(displayMetrics)

            // Set the renderer to out demo renderer, define below

            // Set the renderer to out demo renderer, define below
            mRenderer = LessonSixNativeRenderer(this)

            mGLSurfaceView!!.setRenderer(
                mRenderer as GLSurfaceView.Renderer?,
                displayMetrics.density
            )
        } else {
            Log.e("LessonSixActivity", "OpenGL ES 3.0 not supported on device.  Exiting...")
            return
        }


        findViewById<View>(R.id.button_set_min_filter).setOnClickListener {
            showDialog(
                Companion.MIN_DIALOG
            )
        }

        findViewById<View>(R.id.button_set_mag_filter).setOnClickListener {
            showDialog(
                MAG_DIALOG
            )
        }

        // Restore previous settings

        // Restore previous settings
        if (savedInstanceState != null) {
            mMinSetting = savedInstanceState.getInt(MIN_SETTING, -1)
            mMagSetting = savedInstanceState.getInt(MAG_SETTING, -1)
            if (mMinSetting != -1) {
                setMinSetting(mMinSetting)
            }
            if (mMagSetting != -1) {
                setMagSetting(mMagSetting)
            }
        }
    }

    override fun onSaveInstanceState(outState: Bundle) {
        outState.putInt(MIN_SETTING, mMinSetting)
        outState.putInt(MAG_SETTING, mMagSetting)
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

    private fun setMinSetting(item: Int) {
        mMinSetting = item
        mGLSurfaceView!!.queueEvent {
            val filter: Int = when (item) {
                0 -> {
                    GLES20.GL_NEAREST
                }
                1 -> {
                    GLES20.GL_LINEAR
                }
                2 -> {
                    GLES20.GL_NEAREST_MIPMAP_NEAREST
                }
                3 -> {
                    GLES20.GL_NEAREST_MIPMAP_LINEAR
                }
                4 -> {
                    GLES20.GL_LINEAR_MIPMAP_NEAREST
                }
                else  // if (item == 5)
                -> {
                    GLES20.GL_LINEAR_MIPMAP_LINEAR
                }
            }
            mRenderer!!.setMinFilter(filter)
        }
    }

    private fun setMagSetting(item: Int) {
        mMagSetting = item
        mGLSurfaceView!!.queueEvent {
            val filter: Int = if (item == 0) {
                GLES20.GL_NEAREST
            } else {
                // if (item == 1)
                GLES20.GL_LINEAR
            }
            mRenderer!!.setMagFilter(filter)
        }
    }

    override fun onCreateDialog(id: Int): Dialog? {
        var dialog: Dialog? = null
        dialog = when (id) {
            Companion.MIN_DIALOG -> {
                val builder = AlertDialog.Builder(this)
                builder.setTitle(getText(R.string.lesson_six_set_min_filter_message))
                builder.setItems(
                    resources.getStringArray(R.array.lesson_six_min_filter_types)
                ) { _, item -> setMinSetting(item) }
                builder.create()
            }
            MAG_DIALOG -> {
                val builder = AlertDialog.Builder(this)
                builder.setTitle(getText(R.string.lesson_six_set_mag_filter_message))
                builder.setItems(
                    resources.getStringArray(R.array.lesson_six_mag_filter_types)
                ) { _, item -> setMagSetting(item) }
                builder.create()
            }
            else -> null
        }
        return dialog
    }
}