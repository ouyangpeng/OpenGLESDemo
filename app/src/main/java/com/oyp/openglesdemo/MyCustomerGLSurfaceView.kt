package com.oyp.openglesdemo

import android.content.Context
import android.opengl.GLSurfaceView
import android.util.AttributeSet
import android.util.Log
import android.view.MotionEvent
import android.view.ScaleGestureDetector
import com.oyp.openglesdemo.render.MyNativeRenderer

class MyCustomerGLSurfaceView : GLSurfaceView, ScaleGestureDetector.OnScaleGestureListener {
    private lateinit var mRenderer: MyNativeRenderer
    private lateinit var mScaleGestureDetector: ScaleGestureDetector

    private var mPreviousX = 0f
    private var mPreviousY = 0f

    private var mXAngle = 0f
    private var mYAngle = 0f

    private var mRatioWidth = 0
    private var mRatioHeight = 0

    private var mPreScale = 1.0f
    private var mCurScale = 1.0f

    private var mDensity = 0f

    private var mLastMultiTouchTime: Long = 0

    constructor(context: Context?) : super(context) {}

    constructor(context: Context?, attrs: AttributeSet?) : super(context, attrs) {}

    constructor(context: Context?, glRender: MyNativeRenderer, eglContextVersion: Int) : this(
        context,
        null,
        glRender,
        eglContextVersion
    )

    constructor(
        context: Context?,
        attrs: AttributeSet?,
        glRender: MyNativeRenderer,
        eglContextVersion: Int
    ) : super(context, attrs) {
        setEGLContextClientVersion(eglContextVersion)
        mRenderer = glRender

        /*If no setEGLConfigChooser method is called,
        then by default the view will choose an RGB_888 surface with a depth buffer depth of at least 16 bits.*/
        // 最后 2 个参数表示分别配置 16 位的深度缓冲区和模板缓冲区
        setEGLConfigChooser(8, 8, 8, 8, 16, 8)
        setRenderer(mRenderer)
        mScaleGestureDetector = ScaleGestureDetector(context, this)
    }

    override fun onTouchEvent(event: MotionEvent): Boolean {
        Log.d(TAG, "onTouchEvent")
        if (event.pointerCount == 1) {
            Log.d(TAG, "event.pointerCount == 1")
            val currentTimeMillis = System.currentTimeMillis()
            if (currentTimeMillis - mLastMultiTouchTime > 200) {
                var x: Float = -1.0f
                var y: Float = -1.0f
                when (event.action) {
                    MotionEvent.ACTION_DOWN -> {
                        if (mRenderer.mSampleType == IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_FIVE) {
                            // Android的 GLSurfaceView 在后台线程中执行渲染，必须要小心，
                            // 只能在这个渲染线程中调用OpenGL，在Android主线程中使用UI(用户界面)相关的调用
                            // 两个线程之间的通信可以用如下方法：
                            // 在主线程中的 GLSurfaceView实例可以调用 queueEven() 方法传递一个Runnable给后台渲染线程
                            // 渲染线程可以调用Activity的runOnUIThread()来传递事件(event)给主线程

                            // Ensure we call switchMode() on the OpenGL thread.
                            // queueEvent() is a method of GLSurfaceView that will do this for us.
                            queueEvent { mRenderer.switchBlendingMode() }
                            return true
                        }
                    }

                    MotionEvent.ACTION_MOVE -> {
                        x = event.x
                        y = event.y
                        val deltaX = (x - mPreviousX) / mDensity / 2
                        val deltaY = (y - mPreviousY) / mDensity / 2
                        if (mRenderer.mSampleType == IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_SIX) {
                            mRenderer.setDelta(deltaX, deltaY)
                        } else if (mRenderer.mSampleType == IMyNativeRendererType.SAMPLE_TYPE_KEY_SCRATCH_CARD) {
                            mRenderer.setTouchLocation(x, y)
                            // 重新请求绘制
                            requestRender()
                        }

                        val dy = y - mPreviousY
                        val dx = x - mPreviousX
                        mYAngle += (dx * TOUCH_SCALE_FACTOR).toInt()
                        mXAngle += (dy * TOUCH_SCALE_FACTOR).toInt()
                    }

                    MotionEvent.ACTION_CANCEL -> {
                        x = -1.0f
                        y = -1.0f
                    }

                    MotionEvent.ACTION_UP->{
                        if (mRenderer.mSampleType == IMyNativeRendererType.SAMPLE_TYPE_KEY_SHOCK_WAVE) {
                            mRenderer.setTouchLocation(event.x, event.y)
                        }
                    }
                }

                mPreviousX = x
                mPreviousY = y

                when (mRenderer.mSampleType) {
                    IMyNativeRendererType.SAMPLE_TYPE_KEY_FBO_LEG,
                    IMyNativeRendererType.SAMPLE_TYPE_KEY_COORD_SYSTEM,
                    IMyNativeRendererType.SAMPLE_TYPE_KEY_BASE_LIGHT,
                    IMyNativeRendererType.SAMPLE_TYPE_KEY_MULTI_LIGHT,
                    IMyNativeRendererType.SAMPLE_TYPE_KEY_STENCIL_TESTING,
                    IMyNativeRendererType.SAMPLE_TYPE_KEY_PARTICLE_SYSTEM2,
                    IMyNativeRendererType.SAMPLE_TYPE_KEY_INSTANCING,
                    IMyNativeRendererType.SAMPLE_TYPE_KEY_PBO,
                    IMyNativeRendererType.SAMPLE_TYPE_KEY_UBO,
                    IMyNativeRendererType.SAMPLE_TYPE_KEY_SKYBOX -> {
                        Log.d(TAG, "updateTransformMatrix")
                        mRenderer.updateTransformMatrix(mXAngle, mYAngle, mCurScale, mCurScale)
                        requestRender()
                    }
                }

            }
        } else {
            Log.d(TAG, "event.pointerCount != 1")
            mScaleGestureDetector.onTouchEvent(event)
        }
        return true
    }


    override fun onMeasure(widthMeasureSpec: Int, heightMeasureSpec: Int) {
        super.onMeasure(widthMeasureSpec, heightMeasureSpec)
        val width = MeasureSpec.getSize(widthMeasureSpec)
        val height = MeasureSpec.getSize(heightMeasureSpec)
        if (0 == mRatioWidth || 0 == mRatioHeight) {
            setMeasuredDimension(width, height)
        } else {
            if (width < height * mRatioWidth / mRatioHeight) {
                setMeasuredDimension(width, width * mRatioHeight / mRatioWidth)
            } else {
                setMeasuredDimension(height * mRatioWidth / mRatioHeight, height)
            }
        }
    }

    fun setAspectRatio(width: Int, height: Int) {
        Log.d(TAG, "setAspectRatio() called with: width = [$width], height = [$height]")
        require(!(width < 0 || height < 0)) { "Size cannot be negative." }
        mRatioWidth = width
        mRatioHeight = height
        requestLayout()
    }

    // Hides superclass method.
    fun setRenderer(renderer: Renderer, density: Float) {
        mRenderer = renderer as MyNativeRenderer
        mDensity = density
        super.setRenderer(renderer)
    }

    override fun onScale(detector: ScaleGestureDetector?): Boolean {
        Log.d(TAG, "onScale")
        when (mRenderer.mSampleType) {
            IMyNativeRendererType.SAMPLE_TYPE_KEY_COORD_SYSTEM,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_BASE_LIGHT,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_INSTANCING -> {
                val preSpan = detector!!.previousSpan
                val curSpan = detector.currentSpan
                mCurScale = if (curSpan < preSpan) {
                    mPreScale - (preSpan - curSpan) / 200
                } else {
                    mPreScale + (curSpan - preSpan) / 200
                }
                mCurScale = 0.05f.coerceAtLeast(mCurScale.coerceAtMost(80.0f))
                mRenderer.updateTransformMatrix(mXAngle, mYAngle, mCurScale, mCurScale)
                requestRender()
            }
            else -> {}
        }

        return false
    }

    override fun onScaleBegin(detector: ScaleGestureDetector?): Boolean {
        Log.d(TAG, "onScaleBegin")
        return true
    }

    override fun onScaleEnd(detector: ScaleGestureDetector?) {
        Log.d(TAG, "onScaleEnd")
        mPreScale = mCurScale
        mLastMultiTouchTime = System.currentTimeMillis()
    }

    companion object {
        private const val TOUCH_SCALE_FACTOR = 180.0f / 320
        private const val TAG = "MyCustomerGLSurfaceView"
    }
}