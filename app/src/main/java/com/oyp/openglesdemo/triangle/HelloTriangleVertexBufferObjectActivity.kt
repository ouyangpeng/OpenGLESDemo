package com.oyp.openglesdemo.triangle

import android.app.Activity
import android.app.ActivityManager
import android.opengl.GLSurfaceView
import android.os.Bundle
import android.util.Log

class HelloTriangleVertexBufferObjectActivity : Activity() {
    /**
     * Hold a reference to our GLSurfaceView
     */
    private var mGLSurfaceView: GLSurfaceView? = null

    private val CONTEXT_CLIENT_VERSION = 3

    public override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        mGLSurfaceView = GLSurfaceView(this)

        if(detectOpenGLES30()){
            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.
            mGLSurfaceView!!.setEGLContextClientVersion(CONTEXT_CLIENT_VERSION)
            mGLSurfaceView!!.setRenderer(HelloTriangleVertexBufferObjectNativeRenderer())
        } else {
            Log.e("HelloTriangle", "OpenGL ES 3.0 not supported on device.  Exiting...")
            return;
        }
        setContentView(mGLSurfaceView)
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