package com.oyp.openglesdemo.activity

import androidx.appcompat.app.AppCompatActivity
import android.os.Bundle
import com.oyp.openglesdemo.R
import android.graphics.Bitmap
import android.graphics.BitmapFactory
import android.opengl.GLES30
import android.opengl.GLException
import android.util.Log
import android.view.Menu
import android.view.MenuItem
import android.widget.Button
import android.widget.ImageView
import com.oyp.openglesdemo.render.egl.NativeEglRender
import java.io.IOException
import java.nio.ByteBuffer
import java.nio.IntBuffer

class EGLActivity : AppCompatActivity() {
    private lateinit var mImageView: ImageView
    private lateinit var mBtn: Button
    private var mBgRender: NativeEglRender? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_egl)
        mImageView = findViewById(R.id.imageView)
        mBtn = findViewById(R.id.button)
        mBgRender = NativeEglRender(this)
        mBgRender!!.eglRenderInit()

        mBtn.setOnClickListener {
            if (mBtn.text == this@EGLActivity.resources.getString(R.string.btn_txt_reset)) {
                mImageView.setImageResource(R.mipmap.yangchaoyue)
                mBtn.setText(R.string.bg_render_txt)
            } else {
                startBgRender()
                mBtn.setText(R.string.btn_txt_reset)
            }
        }

        supportActionBar?.let{
            it.setHomeButtonEnabled(true)
            it.setDisplayHomeAsUpEnabled(true)
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        mBgRender!!.nativeEglRenderUnInit()
    }

    override fun onCreateOptionsMenu(menu: Menu): Boolean {
        // Inflate the menu; this adds items to the action bar if it is present.
        menuInflater.inflate(R.menu.menu_egl, menu)
        return true
    }

    override fun onOptionsItemSelected(item: MenuItem): Boolean {
        val id = item.itemId
        var shaderIndex = 0
        when (id) {
            R.id.action_shader0 -> shaderIndex = 0
            R.id.action_shader1 -> shaderIndex = 1
            R.id.action_shader2 -> shaderIndex = 2
            R.id.action_shader3 -> shaderIndex = 3
            R.id.action_shader4 -> shaderIndex = 4
            R.id.action_shader5 -> shaderIndex = 5
            R.id.action_shader6 -> shaderIndex = 6
            R.id.action_shader7 -> shaderIndex = 7
            R.id.action_shader8 -> shaderIndex = 8
            R.id.action_shader9 -> shaderIndex = 9
            R.id.action_shader10 -> shaderIndex = 10
            R.id.action_shader11 -> shaderIndex = 11
            R.id.action_shader12 -> shaderIndex = 12
            R.id.action_shader13 -> shaderIndex = 13
            R.id.action_shader14 -> shaderIndex = 14
            R.id.action_shader15 -> shaderIndex = 15
            R.id.action_shader16 -> shaderIndex = 16
            R.id.action_shader17 -> shaderIndex = 17
            R.id.action_shader18 -> shaderIndex = 18
            R.id.action_shader19 -> shaderIndex = 19
            R.id.action_shader20 -> shaderIndex = 20
            R.id.action_shader21 -> shaderIndex = 21
            R.id.action_shader22 -> shaderIndex = 22
            R.id.action_shader23 -> shaderIndex = 23
            R.id.action_shader24 -> shaderIndex = 24
            // 处理返回按钮
            android.R.id.home-> finish()
            else -> {
            }
        }
        if (mBgRender != null) {
            mBgRender!!.nativeEglRenderSetFragmentShaderType(PARAM_TYPE_SHADER_INDEX, shaderIndex)
            startBgRender()
            mBtn.setText(R.string.btn_txt_reset)
        }
        return true
    }

    private fun startBgRender() {
        val bitmap = loadRGBAImage(R.mipmap.yangchaoyue, mBgRender)
        mBgRender!!.nativeEglRenderDraw()
        if (bitmap != null) {
            mImageView.setImageBitmap(createBitmapFromGLSurface(0, 0, bitmap.width, bitmap.height))
        }
    }

    private fun loadRGBAImage(resId: Int, render: NativeEglRender?) : Bitmap?{
        val inputStream = this.resources.openRawResource(resId)
        val bitmap: Bitmap?
        try {
            bitmap = BitmapFactory.decodeStream(inputStream)
            if (bitmap != null) {
                val bytes = bitmap.byteCount
                val buf = ByteBuffer.allocate(bytes)
                bitmap.copyPixelsToBuffer(buf)
                val byteArray = buf.array()
                render!!.nativeEglRenderSetImageData(byteArray, bitmap.width, bitmap.height)
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

    private fun createBitmapFromGLSurface(x: Int, y: Int, w: Int, h: Int): Bitmap? {
        val bitmapBuffer = IntArray(w * h)
        val bitmapSource = IntArray(w * h)
        val intBuffer = IntBuffer.wrap(bitmapBuffer)
        intBuffer.position(0)
        try {
            GLES30.glReadPixels(
                x, y, w, h, GLES30.GL_RGBA, GLES30.GL_UNSIGNED_BYTE,
                intBuffer
            )
            var offset1: Int
            var offset2: Int
            for (i in 0 until h) {
                offset1 = i * w
                offset2 = (h - i - 1) * w
                for (j in 0 until w) {
                    val texturePixel = bitmapBuffer[offset1 + j]
                    //  int blue = (texturePixel >> 16) & 0xff;
                    val blue = texturePixel shr 16 and 0xff
                    //  int red = (texturePixel << 16) & 0x00ff0000;
                    val red = texturePixel shl 16 and 0x00ff0000
                    //  int pixel = (texturePixel & 0xff00ff00) | red | blue;
                    val pixel = texturePixel and -0xff0100 or red or blue
                    bitmapSource[offset2 + j] = pixel
                }
            }
        } catch (e: GLException) {
            Log.e(TAG,e.stackTraceToString())
            return null
        }
        return Bitmap.createBitmap(bitmapSource, w, h, Bitmap.Config.ARGB_8888)
    }

    companion object {
        private const val TAG = "EGLActivity"
        const val PARAM_TYPE_SHADER_INDEX = 200
    }
}