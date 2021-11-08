package com.oyp.openglesdemo.texture

import android.opengl.GLSurfaceView
import com.oyp.openglesdemo.BaseGLActivity

class MultiTextureActivity : BaseGLActivity() {
    override fun getSurfaceView(): GLSurfaceView {
        return GLSurfaceView(this)
    }

    override fun getRender(): GLSurfaceView.Renderer {
        return MultiTextureRenderer(this)
    }
}