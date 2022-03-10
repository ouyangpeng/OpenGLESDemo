package com.oyp.openglesdemo.activity

import android.Manifest
import android.app.Activity
import android.app.ActivityManager
import android.app.AlertDialog
import android.app.Dialog
import android.content.Context
import android.content.pm.PackageManager
import android.graphics.*
import android.hardware.Sensor
import android.hardware.SensorEvent
import android.hardware.SensorEventListener
import android.hardware.SensorManager
import android.opengl.GLES20
import android.opengl.GLSurfaceView
import android.os.Build
import android.os.Bundle
import android.util.DisplayMetrics
import android.util.Log
import android.view.View
import android.view.ViewGroup
import android.widget.RelativeLayout
import android.widget.Toast
import androidx.core.app.ActivityCompat
import com.oyp.openglesdemo.*
import com.oyp.openglesdemo.ImageFormat
import com.oyp.openglesdemo.audio.AudioCollector
import com.oyp.openglesdemo.render.MyNativeRenderer
import java.io.File
import java.io.IOException
import java.io.InputStream
import java.nio.ByteBuffer

class NativeRenderActivity : Activity(), AudioCollector.Callback, SensorEventListener {

    private var mMinSetting = -1
    private var mMagSetting = -1

    companion object {
        private const val MIN_DIALOG = 1
        private const val CONTEXT_CLIENT_VERSION = 3
        private const val MAG_DIALOG = 2
        private const val MIN_SETTING = "min_setting"
        private const val MAG_SETTING = "mag_setting"
        private const val TAG: String = "NativeRenderActivity"

        private val REQUEST_PERMISSIONS = arrayOf(
            Manifest.permission.WRITE_EXTERNAL_STORAGE,
            Manifest.permission.RECORD_AUDIO
        )
        private const val PERMISSION_REQUEST_CODE = 1
    }

    private var mRootView: ViewGroup? = null

    /**
     * Hold a reference to our GLSurfaceView
     */
    private var mGLSurfaceView: MyCustomerGLSurfaceView? = null

    private var renderer: MyNativeRenderer? = null

    var type = IMyNativeRendererType.SAMPLE_TYPE


    private var mAudioCollector: AudioCollector? = null

    private var mSensorManager: SensorManager? = null

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        if (!detectOpenGLES30()) {
            return
        }
        //定义接收数据的Intent
        val intent = intent
        type = intent.getIntExtra(
            IMyNativeRendererType.RENDER_TYPE,
            IMyNativeRendererType.SAMPLE_TYPE
        )

        // 初始化SensorManager
        if (type == IMyNativeRendererType.SAMPLE_TYPE_KEY_AVATAR) {
            mSensorManager = getSystemService(SENSOR_SERVICE) as SensorManager
        }

        // Set the renderer to out demo renderer, define below
        renderer = MyNativeRenderer(this)
        renderer?.let { myNativeRenderer ->
            myNativeRenderer.setRenderType(IMyNativeRendererType.SAMPLE_TYPE, type)
            if (type == IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_SIX) {
                specialInitGLSurfaceViewForLesson6(myNativeRenderer, savedInstanceState)
            } else {
                setContentView(R.layout.activity_native_render)
                mRootView = findViewById<View>(R.id.rootView) as ViewGroup

                // Tell the surface view we want to create an OpenGL ES 3.0-compatible context,
                // and set an OpenGL ES 3.0-compatible renderer.
                mGLSurfaceView =
                    MyCustomerGLSurfaceView(this, myNativeRenderer, CONTEXT_CLIENT_VERSION)

                mGLSurfaceView?.let {
                    val lp = RelativeLayout.LayoutParams(
                        ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT
                    )
                    lp.addRule(RelativeLayout.CENTER_IN_PARENT)
                    mRootView!!.addView(it, lp)

                    if (mRootView!!.width != it.width
                        || mRootView!!.height != it.height
                    ) {
                        it.setAspectRatio(mRootView!!.width, mRootView!!.height)
                    }
                    // 设置渲染模式
                    setRenderMode(it)
                    // 加载图片
                    loadImageToGLSurfaceView()
                    // 申请重新绘制
                    it.requestRender()
                }
            }
        }
    }

    private fun loadImageToGLSurfaceView() {
        when (type) {
            IMyNativeRendererType.SAMPLE_TYPE_KEY_BASE_LIGHT,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_MULTI_LIGHT,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_INSTANCING,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_STENCIL_TESTING,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_COORD_SYSTEM,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TEXTURE_MAP,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_PBO,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_TIME_TUNNEL,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_FBO -> {
                // 从res目录加载图片
                // loadRGBAImageFromRes(R.mipmap.yangchaoyue)
                // 从assets目录加载图片
                loadRGBAImageFromAssets("texture/yangchaoyue.png")
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_TIME_WATERMARK_STICKER -> {
                loadRGBAImageFromRes(R.mipmap.yifei)
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_STICKER -> {
                loadRGBAImageFromAssets("texture/yangchaoyue.png")
                loadRGBAImageFromResWithIndex(R.mipmap.sticker_c_plus, 0)
                loadRGBAImageFromResWithIndex(R.mipmap.sticker_b_plus, 1)
                loadRGBAImageFromResWithIndex(R.mipmap.sticker_a_minute, 2)
                loadRGBAImageFromResWithIndex(R.mipmap.sticker_a_plus, 3)
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_AIR_HOCKEY->{
                val bitmap = loadRGBAImageFromRes(R.drawable.air_hockey_surface)
                bitmap?.let {
                    mGLSurfaceView?.setAspectRatio(it.width, it.height)
                }
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_SCRATCH_CARD,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_FACE_SLENDER,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_BIG_EYES -> {
                val bitmap = loadRGBAImageFromRes(R.mipmap.yifei)
                bitmap?.let {
                    mGLSurfaceView?.setAspectRatio(it.width, it.height)
                }
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_RATARY_HEAD,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_BIG_HEAD -> {
                val bitmap = loadRGBAImageFromRes(R.mipmap.huge)
                bitmap?.let {
                    mGLSurfaceView?.setAspectRatio(it.width, it.height)
                }
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_AVATAR -> {
                // 素材图里的背景层是每个像素透明度均为最大值的 JPG 图
                val bitmap = loadRGBAImageFromResWithIndex(R.mipmap.avatar_a, 0)
                bitmap?.let {
                    mGLSurfaceView?.setAspectRatio(it.width, it.height)
                }
                // 素材图里的人像层和外层是部分区域透明的 PNG 图
                loadRGBAImageFromResWithIndex(R.mipmap.avatar_b, 1)
                loadRGBAImageFromResWithIndex(R.mipmap.avatar_c, 2)
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_RGB2YUV,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_UBO,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_FBO_BLIT,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_MRT2,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_MULTI_THREAD_RENDER,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_SHOCK_WAVE -> {
                val bitmap = loadRGBAImageFromRes(R.mipmap.lye)
                bitmap?.let {
                    mGLSurfaceView?.setAspectRatio(it.width, it.height)
                }
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_STAY_COLOR -> {
                loadGrayImageFromAssetsWithIndex("gray/lye_1280x800.Gray", 0, 1280, 800)
                val b5 = loadRGBAImageFromRes(R.mipmap.lye2)
                b5?.let {
                    mGLSurfaceView?.setAspectRatio(it.width, it.height)
                }
                loadRGBAImageFromResWithIndex(R.drawable.ascii_mapping, 1)
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_FBO_LEG -> {
                // 从assets目录加载图片
                loadRGBAImageFromAssets("texture/leg.jpg")
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_YUV_RENDER -> {
                loadNV21ImageFromAssets("yuv/YUV_Image_840x1074.NV21", 840, 1074)
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_BLENDING -> {
                loadRGBAImageFromResWithIndex(R.mipmap.board_texture, 0)
                loadRGBAImageFromResWithIndex(R.mipmap.floor, 1)
                loadRGBAImageFromResWithIndex(R.mipmap.window, 2)
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_MAIN_SEQUENCE_STAR -> {
                loadRGBAImageFromResWithIndex(R.mipmap.main_sequence_start_bg0, 0)
                loadRGBAImageFromResWithIndex(R.mipmap.main_sequence_start_bg1, 1)
            }


            IMyNativeRendererType.SAMPLE_TYPE_KEY_PARTICLE_SYSTEM2 -> {
                loadRGBAImageFromRes(R.mipmap.board_texture)
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_SKYBOX -> {
                loadRGBAImageFromResWithIndex(R.mipmap.direction_right, 0)
                loadRGBAImageFromResWithIndex(R.mipmap.direction_left, 1)
                loadRGBAImageFromResWithIndex(R.mipmap.direction_top, 2)
                loadRGBAImageFromResWithIndex(R.mipmap.direction_bottom, 3)
                loadRGBAImageFromResWithIndex(R.mipmap.direction_back, 4)
                loadRGBAImageFromResWithIndex(R.mipmap.direction_front, 5)
            }

            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_1,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_2,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_3,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_4,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_5,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_6,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_7,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_8,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_9,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_10,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_11,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_12,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_13,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_14,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_15,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_16,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_17,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_18,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_19,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_20,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_21 -> {
                loadRGBAImageFromResWithIndex(R.mipmap.yangchaoyue1, 0)
                loadRGBAImageFromResWithIndex(R.mipmap.yangchaoyue2, 1)
                loadRGBAImageFromResWithIndex(R.mipmap.yangchaoyue3, 2)
                loadRGBAImageFromResWithIndex(R.mipmap.yangchaoyue4, 3)
                loadRGBAImageFromResWithIndex(R.mipmap.yangchaoyue5, 4)
                loadRGBAImageFromResWithIndex(R.mipmap.yangchaoyue6, 5)
                loadRGBAImageFromResWithIndex(R.mipmap.yangchaoyue7, 6)
            }
        }
    }

    /**
     * GLSurfaceView会在一个单独的线程中调用渲染器的方法。
     * 默认情况下，GLSurfaceView 会以显示设备的刷新频率不断地渲染，当然，它也可以配置为按请求渲染，
     * 只需要用 GLSurfaceView.RENDERMODE_WHEN_DIRTY作为参数调用GLSurfaceView.setRenderMode() 即可。
     *
     *  GLSurfaceView.RENDERMODE_WHEN_DIRTY
     *  GLSurfaceView.RENDERMODE_CONTINUOUSLY
     */
    private fun setRenderMode(it: MyCustomerGLSurfaceView) {
        // 默认渲染模式设置为RENDERMODE_WHEN_DIRTY
        it.renderMode = GLSurfaceView.RENDERMODE_WHEN_DIRTY

        when (type) {
            // 这几个类型需要不停绘制，所以渲染模式设置为RENDERMODE_CONTINUOUSLY
            IMyNativeRendererType.SAMPLE_TYPE_KEY_CUBE_SIMPLE_VERTEX_SHADER,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_PARTICLE_SYSTEM,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_PARTICLE_SYSTEM_TRANSFORM_FEEDBACK,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_ONE,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_TWO,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_THREE,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_FOUR,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_FIVE,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_LESSON_SEVEN,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_PBO,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_BEATING_HEART,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_CLOUD,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_BEZIER_CURVE,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_ATMOSPHERE_SYSTEM_TEST,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_SKY_PATH,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_A_DAY,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_MAIN_SEQUENCE_STAR,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_SHADER_TOY_TIME_TUNNEL,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_FACE_SLENDER,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_BIG_EYES,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_BIG_HEAD,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_RATARY_HEAD,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_VISUALIZE_AUDIO,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_AVATAR,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_MULTI_THREAD_RENDER,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_SHOCK_WAVE,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_STAY_COLOR,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_PARTICLE_SYSTEM2,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_1,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_2,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_3,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_4,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_5,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_6,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_7,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_8,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_9,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_10,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_11,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_12,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_13,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_14,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_15,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_16,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_17,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_18,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_19,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_20,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TRANSITIONS_21,
            IMyNativeRendererType.SAMPLE_TYPE_KEY_TIME_WATERMARK_STICKER-> {
                // 这几个类型需要不停绘制，所以渲染模式设置为RENDERMODE_CONTINUOUSLY
                it.renderMode = GLSurfaceView.RENDERMODE_CONTINUOUSLY
            }
        }
    }

    private fun specialInitGLSurfaceViewForLesson6(
        myNativeRenderer: MyNativeRenderer,
        savedInstanceState: Bundle?
    ) {
        setContentView(R.layout.lesson_six)
        mGLSurfaceView = findViewById<View>(R.id.gl_surface_view) as MyCustomerGLSurfaceView

        mGLSurfaceView?.let {
            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.

            // Tell the surface view we want to create an OpenGL ES 3.0-compatible
            // context, and set an OpenGL ES 3.0-compatible renderer.
            it.setEGLContextClientVersion(CONTEXT_CLIENT_VERSION)

            val displayMetrics = DisplayMetrics()
            windowManager.defaultDisplay.getMetrics(displayMetrics)

            it.setRenderer(myNativeRenderer, displayMetrics.density)

            configLessonSix(savedInstanceState)
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
        val reqGlEsVersion = info.reqGlEsVersion

        // 有些GPU模拟部分有缺陷，为了使代码在模拟器上正常工作，写下面的代码
        // 这段代码判断当前设备是不是模拟器，如果是，就假定它支持OpenGL ES 3.0
        // 要确保程序能运行，模拟器一定要配置OpenGL ES 3.0
        val isSupportedOpenGLES30 = reqGlEsVersion >= 0x30000
                || (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR2
                && (Build.FINGERPRINT.startsWith("generic")
                || Build.FINGERPRINT.startsWith("unknown")
                || Build.MODEL.contains("google_sdk")
                || Build.MODEL.contains("Emulator")
                || Build.MODEL.contains("Android SDK built for x86")))


        if (!isSupportedOpenGLES30) {
            Log.e(
                TAG,
                "OpenGL ES 3.0 not supported on device. The device's reqGlEsVersion is $reqGlEsVersion, Exiting..."
            )
            Toast.makeText(
                this,
                "当前设备不支持OpenGL ES 3.0 ，当前设备的GlEs版本是$reqGlEsVersion",
                Toast.LENGTH_LONG
            ).show()
        }
        return isSupportedOpenGLES30
    }

    override fun onResume() {
        // The activity must call the GL surface view's onResume() on activity onResume().
        super.onResume()

        mGLSurfaceView?.onResume()

        if (type == IMyNativeRendererType.SAMPLE_TYPE_KEY_VISUALIZE_AUDIO
            || type == IMyNativeRendererType.SAMPLE_TYPE_KEY_RGB2YUV
            || type == IMyNativeRendererType.SAMPLE_TYPE_KEY_TEXT_RENDER
            || type == IMyNativeRendererType.SAMPLE_TYPE_KEY_3D_MODEL
            || type == IMyNativeRendererType.SAMPLE_TYPE_KEY_3D_MODEL2
        ) {
            if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
                ActivityCompat.requestPermissions(
                    this,
                    REQUEST_PERMISSIONS,
                    PERMISSION_REQUEST_CODE
                )
            } else {
                if (mAudioCollector == null) {
                    mAudioCollector = AudioCollector()
                    mAudioCollector!!.addCallback(this@NativeRenderActivity)
                    mAudioCollector!!.init()
                }

                //  这个目录在C层会被调用，写入到这个目录
                //  /data/data/com.oyp.openglesdemo/cache
                val fileDir = cacheDir!!.absolutePath
                File(fileDir).let {
                    if (!it.exists()) it.mkdirs()
                }
                // 复制assets目录文件到sdcard
                CommonUtils.copyAssetsDirToSDCard(this, "fonts", fileDir)
                CommonUtils.copyAssetsDirToSDCard(this, "poly", fileDir)
                CommonUtils.copyAssetsDirToSDCard(this, "objects", fileDir)
            }
        }

        // 注册SensorManager 监听
        if (type == IMyNativeRendererType.SAMPLE_TYPE_KEY_AVATAR) {
            mSensorManager?.let {
                it.registerListener(
                    this,
                    it.getDefaultSensor(Sensor.TYPE_GRAVITY),
                    SensorManager.SENSOR_DELAY_FASTEST
                )
            }
        }

    }

    private fun hasPermissionsGranted(permissions: Array<String>): Boolean {
        for (permission in permissions) {
            if (ActivityCompat.checkSelfPermission(this, permission)
                != PackageManager.PERMISSION_GRANTED
            ) {
                return false
            }
        }
        return true
    }

    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<String?>,
        grantResults: IntArray
    ) {
        if (requestCode == PERMISSION_REQUEST_CODE) {
            if (!hasPermissionsGranted(REQUEST_PERMISSIONS)) {
                Toast.makeText(
                    this,
                    "We need the permission: RECORD_AUDIO && WRITE_EXTERNAL_STORAGE",
                    Toast.LENGTH_SHORT
                ).show()
            }
        } else {
            super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        }
    }


    override fun onPause() {
        // The activity must call the GL surface view's onPause() on activity onPause().
        super.onPause()

        mGLSurfaceView?.onPause()

        if (type == IMyNativeRendererType.SAMPLE_TYPE_KEY_VISUALIZE_AUDIO) {
            if (mAudioCollector != null) {
                mAudioCollector!!.unInit()
                mAudioCollector = null
            }
        }

        // 注销SensorManager 监听
        if (type == IMyNativeRendererType.SAMPLE_TYPE_KEY_AVATAR) {
            mSensorManager?.unregisterListener(this)
        }
    }

    override fun onDestroy() {
        super.onDestroy()
        renderer?.onDestroy()
    }

    private fun setMinSetting(item: Int) {
        mMinSetting = item
        mGLSurfaceView?.queueEvent {
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
            renderer?.setMinFilter(filter)
        }
    }

    private fun setMagSetting(item: Int) {
        mMagSetting = item
        mGLSurfaceView?.queueEvent {
            val filter: Int = if (item == 0) {
                GLES20.GL_NEAREST
            } else {
                // if (item == 1)
                GLES20.GL_LINEAR
            }
            renderer?.setMagFilter(filter)
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
        renderer?.let {
            val inputStream = this.resources.openRawResource(resId)
            val bitmap: Bitmap?
            try {
                bitmap = BitmapFactory.decodeStream(inputStream)
                setBitmap(bitmap, it)
            } finally {
                try {
                    inputStream.close()
                } catch (e: IOException) {
                    Log.e(TAG, e.stackTraceToString())
                }
            }
            return bitmap
        }
        return null
    }

    private fun loadGrayImageFromAssetsWithIndex(fileName: String, index: Int, width: Int, height: Int) {
        renderer?.let {
            var inputStream: InputStream? = null
            try {
                inputStream = assets.open(fileName)
                val length = inputStream.available()
                val buffer = ByteArray(length)
                inputStream.read(buffer)
                it.setImageDataWithIndex(
                    index,
                    ImageFormat.IMAGE_FORMAT_GARY,
                    width,
                    height,
                    buffer
                )
            } catch (e: IOException) {
                e.printStackTrace()
            } finally {
                try {
                    inputStream!!.close()
                } catch (e: IOException) {
                    Log.e(TAG, e.stackTraceToString())
                }
            }
        }
    }

    private fun setBitmap(
        bitmap: Bitmap?,
        renderer: MyNativeRenderer
    ) {
        if (bitmap != null) {
            val bytes = bitmap.byteCount
            val buf = ByteBuffer.allocate(bytes)
            bitmap.copyPixelsToBuffer(buf)
            val byteArray = buf.array()
            renderer.setImageData(
                ImageFormat.IMAGE_FORMAT_RGBA,
                bitmap.width,
                bitmap.height,
                byteArray
            )
        }
    }

    private fun loadRGBAImageFromResWithIndex(resId: Int, index: Int): Bitmap? {
        renderer?.let {
            var inputStream: InputStream? = null
            var bitmap: Bitmap? = null
            try {
                inputStream = resources.openRawResource(resId)
                bitmap = BitmapFactory.decodeStream(inputStream)
                if (bitmap != null) {
                    val bytes = bitmap.byteCount
                    val buf = ByteBuffer.allocate(bytes)
                    bitmap.copyPixelsToBuffer(buf)
                    val byteArray = buf.array()
                    it.setImageDataWithIndex(
                        index,
                        ImageFormat.IMAGE_FORMAT_RGBA,
                        bitmap.width,
                        bitmap.height,
                        byteArray
                    )
                }
            } finally {
                try {
                    inputStream?.close()
                } catch (e: IOException) {
                    Log.e(TAG, e.stackTraceToString())
                }
            }
            return bitmap
        }
        return null
    }

    private fun loadRGBAImageFromAssets(imagePath: String): Bitmap? {
        renderer?.let {
            var inputStream: InputStream? = null
            var bitmap: Bitmap? = null
            try {
                inputStream = assets.open(imagePath)
                bitmap = BitmapFactory.decodeStream(inputStream)
                if (bitmap != null) setBitmap(bitmap, it)
            } catch (e: IOException) {
                Log.e(TAG, e.stackTraceToString())
            } finally {
                try {
                    inputStream?.close()
                } catch (e: IOException) {
                    Log.e(TAG, e.stackTraceToString())
                }
            }
            return bitmap
        }
        return null
    }


    private fun loadNV21ImageFromAssets(fileName: String, width: Int, height: Int) {
        renderer?.let {
            var inputStream: InputStream? = null
            try {
                inputStream = assets.open(fileName)
                val length = inputStream.available()
                val buffer = ByteArray(length)
                inputStream.read(buffer)
                it.setImageData(ImageFormat.IMAGE_FORMAT_NV21, width, height, buffer)
            } catch (e: IOException) {
                Log.e(TAG, e.stackTraceToString())
            } finally {
                try {
                    inputStream!!.close()
                } catch (e: IOException) {
                    Log.e(TAG, e.stackTraceToString())
                }
            }
        }
    }

    override fun onAudioBufferCallback(buffer: ShortArray?) {
        if (type == IMyNativeRendererType.SAMPLE_TYPE_KEY_VISUALIZE_AUDIO) {
            Log.e(TAG, "onAudioBufferCallback() called with: buffer[0] = [" + buffer!![0] + "]")
            renderer?.setAudioData(buffer)
        }
    }

    override fun onSensorChanged(event: SensorEvent?) {
        if (event!!.sensor.type == Sensor.TYPE_GRAVITY) {
            Log.d(
                TAG, "onSensorChanged() called with TYPE_GRAVITY: " +
                        "[x,y,z] = [" + event.values[0] + ", " + event.values[1] + ", " + event.values[2] + "]"
            )
            if (type == IMyNativeRendererType.SAMPLE_TYPE_KEY_AVATAR) {
                renderer?.setGravityXY(event.values[0], event.values[1])
            }
        }
    }

    override fun onAccuracyChanged(sensor: Sensor?, accuracy: Int) {
    }
}