package com.oyp.openglesdemo.render

interface RenderAction {
    fun switchBlendingMode()

    fun setMinFilter(filter: Int)
    fun setMagFilter(filter: Int)
    fun setDelta(deltaX: Float, deltaY: Float)

    fun setImageData(format: Int, width: Int, height: Int, imageData: ByteArray)

    fun setImageDataWithIndex(index: Int, format: Int, width: Int, height: Int, imageData: ByteArray)

    fun updateTransformMatrix(rotateX: Float, rotateY: Float, scaleX: Float, scaleY: Float)
}