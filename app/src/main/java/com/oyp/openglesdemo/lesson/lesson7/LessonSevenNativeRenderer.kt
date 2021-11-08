package com.oyp.openglesdemo.lesson.lesson7

import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class LessonSevenNativeRenderer(activity: LessonSevenActivity, glSurfaceView: GLSurfaceView)
    : GLSurfaceView.Renderer, Action {
    private val mActivity: LessonSevenActivity = activity
    private val mGlSurfaceView: GLSurfaceView = glSurfaceView

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        nativeSurfaceCreate(mActivity.assets)
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        nativeSurfaceChange(width, height)
    }

    override fun onDrawFrame(gl: GL10) {
        nativeDrawFrame()
    }

    override fun init() {
        nativeInit()
    }

    override fun destroy() {
        nativeDestroy()
    }

    override fun setDelta(deltaX: Float, deltaY: Float) {
        nativeSetDelta(deltaX, deltaY)
    }

    override fun decreaseCubeCount() {
        nativeDecreaseCubeCount()
    }

    override fun increaseCubeCount() {
        nativeIncreaseCubeCount()
    }

    override fun toggleVBOs() {
        nativeToggleVBOs()
    }

    override fun toggleStride() {
        nativeToggleStride()
    }
    
    // 这个方法最终会在jni层被调用
    fun updateVboStatus(useVbos: Boolean) {
        mActivity.updateVboStatus(useVbos)
    }
    
    // 这个方法最终会在jni层被调用
    fun updateStrideStatus(useStride: Boolean) {
        mActivity.updateStrideStatus(useStride)
    }

    private external fun nativeInit()
    private external fun nativeDestroy()
    private external fun nativeSurfaceCreate(assetManager: AssetManager?)
    private external fun nativeSurfaceChange(width: Int, height: Int)
    private external fun nativeDrawFrame()
    private external fun nativeSetDelta(x: Float, y: Float)
    private external fun nativeDecreaseCubeCount()
    private external fun nativeIncreaseCubeCount()
    private external fun nativeToggleVBOs()
    private external fun nativeToggleStride()

    init {
        System.loadLibrary("opengles-lesson-lib")
    }
}