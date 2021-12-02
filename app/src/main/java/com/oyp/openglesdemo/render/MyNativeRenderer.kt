package com.oyp.openglesdemo.render

import android.app.Activity
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MyNativeRenderer(activity: Activity) : GLSurfaceView.Renderer, RenderAction {
    private var mActivity: Activity = activity
    var mSampleType = 0

    init {
        System.loadLibrary("ouyangpeng-opengles-lib")
    }

    // 通用的
    private external fun nativeSurfaceCreate(assetManager: AssetManager)
    private external fun nativeSurfaceChange(width: Int, height: Int)
    private external fun nativeDrawFrame()
    private external fun nativeSetParamsInt(paramType: Int, value0: Int, value1: Int)
    private external fun nativeOnDestroy()

    // 特定的方法
    private external fun nativeSwitchBlendingMode()

    // 特定的方法
    private external fun nativeSetDelta(x: Float, y: Float)
    private external fun nativeSetMinFilter(filter: Int)
    private external fun nativeSetMagFilter(filter: Int)

    private external fun nativeSetImageData(
        format: Int,
        width: Int,
        height: Int,
        imageData: ByteArray?
    )

    private external fun nativeSetImageDataWithIndex(
        index: Int,
        format: Int,
        width: Int,
        height: Int,
        imageData: ByteArray?
    )


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

    fun onDestroy() {
        nativeOnDestroy()
    }

    override fun switchBlendingMode() {
        nativeSwitchBlendingMode()
    }

    override fun setMinFilter(filter: Int) {
        nativeSetMinFilter(filter)
    }

    override fun setMagFilter(filter: Int) {
        nativeSetMagFilter(filter)
    }

    override fun setDelta(deltaX: Float, deltaY: Float) {
        nativeSetDelta(deltaX, deltaY)
    }

    override fun setImageData(
        format: Int,
        width: Int,
        height: Int,
        imageData: ByteArray
    ) {
        nativeSetImageData(format, width, height, imageData)
    }

    override fun setImageDataWithIndex(
        index: Int,
        format: Int,
        width: Int,
        height: Int,
        imageData: ByteArray
    ) {
        nativeSetImageDataWithIndex(index, format, width, height, imageData)
    }
}