package com.oyp.openglesdemo.activity

import android.app.Activity
import android.app.ActivityManager
import android.app.AlertDialog
import android.app.Dialog
import android.content.Context
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.opengl.GLES20
import android.os.Bundle
import android.util.DisplayMetrics
import android.util.Log
import android.view.View
import com.oyp.openglesdemo.*
import com.oyp.openglesdemo.render.IMyNativeRendererType
import com.oyp.openglesdemo.render.MyNativeRenderer
import java.io.IOException
import java.io.InputStream
import java.nio.ByteBuffer

class NativeRenderActivity : Activity() {

    private var mMinSetting = -1
    private var mMagSetting = -1

    companion object {
        private const val MIN_DIALOG = 1
        private const val CONTEXT_CLIENT_VERSION = 3
        private const val MAG_DIALOG = 2
        private const val MIN_SETTING = "min_setting"
        private const val MAG_SETTING = "mag_setting"
        private const val TAG: String = "NativeRenderActivity"
    }

    /**
     * Hold a reference to our GLSurfaceView
     */
    private var mGLSurfaceView: MyCustomerGLSurfaceView? = null

    var renderer: MyNativeRenderer? = null

    var type = IMyNativeRendererType.SAMPLE_TYPE

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        if (!detectOpenGLES30()) {
            Log.e("HelloTriangle", "OpenGL ES 3.0 not supported on device.  Exiting...")
            return
        }
        //定义接收数据的Intent
        val intent = intent
        type = intent.getIntExtra(
            IMyNativeRendererType.RENDER_TYPE,
            IMyNativeRendererType.SAMPLE_TYPE
        )

        // Set the renderer to out demo renderer, define below
        renderer = MyNativeRenderer(this)
        renderer!!.setRenderType(IMyNativeRendererType.SAMPLE_TYPE, type)

        if (type == IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_SIX) {
            setContentView(R.layout.lesson_six)
            mGLSurfaceView = findViewById<View>(R.id.gl_surface_view) as MyCustomerGLSurfaceView

            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.

            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.
            mGLSurfaceView!!.setEGLContextClientVersion(CONTEXT_CLIENT_VERSION)

            val displayMetrics = DisplayMetrics()
            windowManager.defaultDisplay.getMetrics(displayMetrics)

            mGLSurfaceView!!.setRenderer(renderer, displayMetrics.density)

            configLessonSix(savedInstanceState)

        } else {
            when (type) {
                IMyNativeRendererType.SAMPLE_TYPE_KEY_TEXTURE_MAP,
                IMyNativeRendererType.SAMPLE_TYPE_KEY_FBO ->{
                    // 从res目录加载图片
                    // loadRGBAImageFromRes(R.mipmap.yangchaoyue)
                    // 从assets目录加载图片
                    loadRGBAImageFromAssets("texture/yangchaoyue.png")
                }

                IMyNativeRendererType.SAMPLE_TYPE_KEY_YUV_RENDER -> {
                    loadNV21ImageFromAssets("yuv/YUV_Image_840x1074.NV21",840,1074)
                }
            }
            // Tell the surface view we want to create an OpenGL ES 3.0-compatible context,
            // and set an OpenGL ES 3.0-compatible renderer.
            mGLSurfaceView = MyCustomerGLSurfaceView(this, renderer, CONTEXT_CLIENT_VERSION)
            setContentView(mGLSurfaceView)
        }
    }

    private fun configLessonSix(savedInstanceState: Bundle?) {
        findViewById<View>(R.id.button_set_min_filter).setOnClickListener {
            showDialog(
                MIN_DIALOG
            )
        }

        findViewById<View>(R.id.button_set_mag_filter).setOnClickListener {
            showDialog(
                MAG_DIALOG
            )
        }

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

    override fun onDestroy() {
        super.onDestroy()
        renderer?.onDestroy()
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
            renderer!!.setMinFilter(filter)
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
            renderer!!.setMagFilter(filter)
        }
    }

    override fun onCreateDialog(id: Int): Dialog? {
        val dialog: Dialog? = when (id) {
            MIN_DIALOG -> {
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


    private fun loadRGBAImageFromRes(resId: Int): Bitmap? {
        val inputStream = this.resources.openRawResource(resId)
        val bitmap: Bitmap?
        try {
            bitmap = BitmapFactory.decodeStream(inputStream)
            if (bitmap != null) {
                val bytes = bitmap.byteCount
                val buf = ByteBuffer.allocate(bytes)
                bitmap.copyPixelsToBuffer(buf)
                val byteArray = buf.array()
                renderer!!.setImageData(ImageFormat.IMAGE_FORMAT_RGBA, bitmap.width, bitmap.height, byteArray)
            }
        } finally {
            try {
                inputStream.close()
            } catch (e: IOException) {
                Log.e(TAG,e.stackTraceToString())
            }
        }
        return bitmap
    }

    private fun loadRGBAImageFromResWithIndex(resId: Int, index: Int): Bitmap? {
        val inputStream = this.resources.openRawResource(resId)
        val bitmap: Bitmap?
        try {
            bitmap = BitmapFactory.decodeStream(inputStream)
            if (bitmap != null) {
                val bytes = bitmap.byteCount
                val buf = ByteBuffer.allocate(bytes)
                bitmap.copyPixelsToBuffer(buf)
                val byteArray = buf.array()
                renderer!!.setImageDataWithIndex(
                    index,
                    ImageFormat.IMAGE_FORMAT_RGBA,
                    bitmap.width,
                    bitmap.height,
                    byteArray
                )
            }
        } finally {
            try {
                inputStream.close()
            } catch (e: IOException) {
                Log.e(TAG,e.stackTraceToString())
            }
        }
        return bitmap
    }

    private fun loadRGBAImageFromAssets(imagePath : String): Bitmap? {
        var inputStream : InputStream? = null
        var bitmap: Bitmap? = null
        try {
            inputStream  = assets.open(imagePath)
            bitmap = BitmapFactory.decodeStream(inputStream)
            if (bitmap != null) {
                val bytes = bitmap.byteCount
                val buf = ByteBuffer.allocate(bytes)
                bitmap.copyPixelsToBuffer(buf)
                val byteArray = buf.array()
                renderer!!.setImageData(ImageFormat.IMAGE_FORMAT_RGBA, bitmap.width, bitmap.height, byteArray)
            }
        } catch (e: IOException) {
            Log.e(TAG,e.stackTraceToString())
        } finally {
            try {
                inputStream!!.close()
            } catch (e: IOException) {
                Log.e(TAG,e.stackTraceToString())
            }
        }
        return bitmap
    }


    private fun loadNV21ImageFromAssets(fileName:String, width: Int, height: Int) {
        var inputStream: InputStream? = null
        var lenght = 0
        try {
            inputStream = assets.open(fileName)
            lenght = inputStream.available()
            val buffer = ByteArray(lenght)
            inputStream.read(buffer)
            renderer!!.setImageData(ImageFormat.IMAGE_FORMAT_NV21, width, height, buffer)
        } catch (e: IOException) {
            Log.e(TAG,e.stackTraceToString())
        } finally {
            try {
                inputStream!!.close()
            } catch (e: IOException) {
                Log.e(TAG,e.stackTraceToString())
            }
        }
    }
}