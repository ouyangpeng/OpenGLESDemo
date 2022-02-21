package com.oyp.openglesdemo

import android.content.Context
import android.graphics.*
import android.util.Log
import java.io.File
import java.io.FileOutputStream
import java.lang.Exception
import java.text.SimpleDateFormat
import java.util.*

object CommonUtils {
    private const val TAG = "CommonUtils"
    fun copyAssetsDirToSDCard(context: Context, assetsDirName: String, sdCardPath: String) {
        var path = sdCardPath
        Log.d(
            TAG,
            "copyAssetsDirToSDCard() called with: context = [$context], assetsDirName = [$assetsDirName], sdCardPath = [$path]"
        )
        try {
            val list = context.assets.list(assetsDirName)
            if (list!!.isEmpty()) {
                val inputStream = context.assets.open(assetsDirName)
                val mByte = ByteArray(1024)
                var bt = 0
                val file = File(
                    path + File.separator
                            + assetsDirName.substring(assetsDirName.lastIndexOf('/'))
                )
                if (!file.exists()) {
                    file.createNewFile()
                } else {
                    return
                }
                val fos = FileOutputStream(file)
                while (inputStream.read(mByte).also { bt = it } != -1) {
                    fos.write(mByte, 0, bt)
                }
                fos.flush()
                inputStream.close()
                fos.close()
            } else {
                var subDirName = assetsDirName
                if (assetsDirName.contains("/")) {
                    subDirName = assetsDirName.substring(assetsDirName.lastIndexOf('/') + 1)
                }
                path = path + File.separator + subDirName
                val file = File(path)
                if (!file.exists()) file.mkdirs()
                for (s in list) {
                    copyAssetsDirToSDCard(context, assetsDirName + File.separator + s, path)
                }
            }
        } catch (e: Exception) {
            e.printStackTrace()
        }
    }

    fun getTimeWaterBitmap(): Bitmap {
        // 更新 mBitmap
        val mBitmap : Bitmap
        val aText = SimpleDateFormat("HH:mm:ss.SSS").format(Date())
        val aFontSize = 60f
        val textPaint = Paint()
        textPaint.textSize = aFontSize
        textPaint.isFakeBoldText = false
        textPaint.isAntiAlias = true
        textPaint.setARGB(255, 255, 255, 255)
        // If a hinting is available on the platform you are developing, you should enable it (uncomment the line below).
        //textPaint.setHinting(Paint.HINTING_ON);
        // If a hinting is available on the platform you are developing, you should enable it (uncomment the line below).
        //textPaint.setHinting(Paint.HINTING_ON);
        textPaint.isSubpixelText = true
        textPaint.xfermode = PorterDuffXfermode(PorterDuff.Mode.SCREEN)
        textPaint.setShadowLayer(2f, 2f, 2f, Color.BLACK)

        val realTextWidth = textPaint.measureText(aText)

        // Creates a new mutable bitmap, with 128px of width and height
        val bitmapWidth = (realTextWidth + 2.0f).toInt()
        val bitmapHeight = aFontSize.toInt() + 2
        mBitmap = Bitmap.createBitmap(bitmapWidth, bitmapHeight, Bitmap.Config.ARGB_8888)
        mBitmap.eraseColor(Color.argb(0, 255, 0, 0))
        // Creates a new canvas that will draw into a bitmap instead of rendering into the screen
        // Creates a new canvas that will draw into a bitmap instead of rendering into the screen
        val bitmapCanvas = Canvas(mBitmap)
        // Set start drawing position to [1, base_line_position]
        // The base_line_position may vary from one font to another but it usually is equal to 75% of font size (height).
        // Set start drawing position to [1, base_line_position]
        // The base_line_position may vary from one font to another but it usually is equal to 75% of font size (height).
        bitmapCanvas.drawText(aText, 1f, 1.0f + aFontSize * 0.75f, textPaint)
        return mBitmap
    }
}