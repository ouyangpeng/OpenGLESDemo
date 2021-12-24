package com.oyp.openglesdemo.render

import android.app.Activity
import android.content.res.AssetManager
import android.opengl.GLSurfaceView
import com.oyp.openglesdemo.IMyNativeRendererType
import javax.microedition.khronos.egl.EGLConfig
import javax.microedition.khronos.opengles.GL10

class MyNativeRenderer(activity: Activity) : GLSurfaceView.Renderer, RenderAction {
    private var mActivity: Activity = activity
    var mSampleType = 0

    init {
        System.loadLibrary("ouyangpeng-opengles-lib")
    }

    ////////////////////////////////// Native 方法///////////////////////////////////////
    // 通用的
    private external fun nativeSurfaceCreate(assetManager: AssetManager)
    private external fun nativeSurfaceChange(width: Int, height: Int)
    private external fun nativeDrawFrame()
    private external fun nativeSetRenderType(sampleCategoryType: Int, renderSampleType: Int)
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

    private external fun nativeUpdateTransformMatrix(
        rotateX: Float,
        rotateY: Float,
        scaleX: Float,
        scaleY: Float
    )

    private external fun nativeSetAudioData(audioData: ShortArray)

    private external fun nativeSetTouchLocation(x: Float, y: Float)

    private external fun nativeSetGravityXY(x: Float, y: Float)

    ////////////////////////////////// Java 方法///////////////////////////////////////

    /**
     * 当Surface被创建的时候，GLSurfaceView 会调用这个方法。
     * 这发送在应用程序第一次运行的时候，并且，当设备被唤醒或者用户从其他Activity切换换来时，
     * 这个方法也可能被调用。在实践中，这意味着，当应用程序运行时，本方法可能会被调用多次。
     *
     *
     * 为什么会有一个未被使用的参数类型GL10呢？
     * 它是OpenGL ES 1.0的API遗留下来的。如果要编写OpenGL ES 1.0的渲染器，就要用这个参数。
     * 但是，对应OpenGL ES 3.0，GLES20/GLEL30类提供了静态方法来读取。
     */
    override fun onSurfaceCreated(gl: GL10, config: EGLConfig) {
        val assetManager: AssetManager = mActivity.assets
        nativeSurfaceCreate(assetManager)
    }

    /**
     * 当Surface被创建以后，每次Surface尺寸变化时，这个方法都会被 GLSurfaceView 调用到。
     * 在横屏、竖屏来回切换的时候，Surface尺寸会发生变化
     */
    override fun onSurfaceChanged(gl: GL10, width: Int, height: Int) {
        nativeSurfaceChange(width, height)
    }

    /**
     * 当绘制一帧时，这个方法会被 GLSurfaceView 调用。
     * 在这个方法中，我们一定要绘制一些东西，即使只是清空屏幕。
     * 因为，在这个方法返回后，渲染缓冲区会被交换并显示在屏幕上，
     * 如果什么都没画，可能会看到糟糕的闪烁效果。
     */
    override fun onDrawFrame(gl: GL10) {
        nativeDrawFrame()
    }

    fun setRenderType(sampleCategoryType: Int, renderSampleType: Int) {
        if (sampleCategoryType == IMyNativeRendererType.SAMPLE_TYPE) {
            mSampleType = renderSampleType
        }
        nativeSetRenderType(sampleCategoryType, renderSampleType)
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

    override fun updateTransformMatrix(
        rotateX: Float,
        rotateY: Float,
        scaleX: Float,
        scaleY: Float
    ) {
        nativeUpdateTransformMatrix(rotateX, rotateY, scaleX, scaleY)
    }

    override fun setAudioData(audioData: ShortArray) {
        nativeSetAudioData(audioData)
    }

    override fun setTouchLocation(x: Float, y: Float) {
        nativeSetTouchLocation(x,y)
    }

    override fun setGravityXY(x: Float, y: Float){
        nativeSetGravityXY(x,y)
    }
}