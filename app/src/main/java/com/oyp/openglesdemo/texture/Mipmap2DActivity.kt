package com.oyp.openglesdemo.texture

import android.opengl.GLSurfaceView
import com.oyp.openglesdemo.BaseGLActivity

class Mipmap2DActivity : BaseGLActivity() {

    override fun getSurfaceView(): GLSurfaceView {
        return GLSurfaceView(this)
    }

    override fun getRender(): GLSurfaceView.Renderer {
        return Mipmap2DRenderer()
    }
}