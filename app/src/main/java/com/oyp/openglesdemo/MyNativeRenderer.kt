package com.oyp.openglesdemo

import android.app.Activity
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MyNativeRenderer(activity: Activity): GLSurfaceView.Renderer,RenderAction{
    private var mActivity: Activity = activity
    var mSampleType = 0

    external fun nativeSurfaceCreate(assetManager: AssetManager)
    external fun nativeSurfaceChange(width: Int, height: Int)
    external fun nativeDrawFrame()
    private external fun nativeSetParamsInt(paramType: Int, value0: Int, value1: Int)
    private external fun nativeOnDestroy()
    private external fun nativeSwitchBlendingMode()

    init {
        System.loadLibrary("opengles-lesson-lib")
    }

    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        val assetManager: AssetManager = mActivity.assets
        nativeSurfaceCreate(assetManager)
    }

    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        nativeSurfaceChange(width, height)
    }

    override fun onDrawFrame(gl: GL10) {
        nativeDrawFrame()
    }

    fun setParamsInt(sampleType: Int, type: Int, i: Int) {
        if (sampleType == IMyNativeRendererType.SAMPLE_TYPE) {
            mSampleType = type
        }
        nativeSetParamsInt(sampleType, type, i)
    }

    fun onDestroy(){
        nativeOnDestroy()
    }

    override fun switchBlendingMode() {
        nativeSwitchBlendingMode()
    }

}