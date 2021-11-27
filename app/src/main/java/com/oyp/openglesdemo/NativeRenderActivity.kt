package com.oyp.openglesdemo

import android.app.Activity
import android.app.ActivityManager
import android.content.Context
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.util.Log

class NativeRenderActivity : Activity() {
    /**
     * Hold a reference to our GLSurfaceView
     */
    var mGLSurfaceView: GLSurfaceView? = null

    var renderer: MyNativeRenderer? = null

    var type = IMyNativeRendererType.SAMPLE_TYPE

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        //定义接收数据的Intent
        val intent = intent
        type = intent.getIntExtra(IMyNativeRendererType.RENDER_TYPE, IMyNativeRendererType.SAMPLE_TYPE)

        renderer = MyNativeRenderer(this)
        renderer!!.setParamsInt(IMyNativeRendererType.SAMPLE_TYPE, type, 0)

        mGLSurfaceView = GLSurfaceView(this)
        if (detectOpenGLES30()) {
            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.
            mGLSurfaceView!!.setEGLContextClientVersion(3)
            mGLSurfaceView!!.setRenderer(renderer)
        } else {
            Log.e("HelloTriangle", "OpenGL ES 3.0 not supported on device.  Exiting...")
            return
        }
        setContentView(mGLSurfaceView)
    }

    private fun detectOpenGLES30(): Boolean {
        val am = getSystemService(Context.ACTIVITY_SERVICE) as ActivityManager
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

//    override fun onDestroy() {
//        super.onDestroy()
//        renderer?.onDestroy()
//    }
}