package com.oyp.openglesdemo.texture

import android.opengl.GLSurfaceView
import com.oyp.openglesdemo.BaseGLActivity

class Noise3DActivity : BaseGLActivity() {
    override fun getSurfaceView(): GLSurfaceView {
        return GLSurfaceView(this)
    }

    override fun getRender(): GLSurfaceView.Renderer {
        return Noise3DRenderer(this)
    }
}