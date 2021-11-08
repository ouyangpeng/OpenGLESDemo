package com.oyp.openglesdemo.cube

import android.opengl.GLSurfaceView
import com.oyp.openglesdemo.BaseGLActivity

class HelloCubeSimpleVertexShaderActivity : BaseGLActivity() {
    override fun getSurfaceView(): GLSurfaceView {
        return GLSurfaceView(this)
    }

    override fun getRender(): GLSurfaceView.Renderer {
       return HelloCubeSimpleVertexShaderNativeRenderer()
    }
}