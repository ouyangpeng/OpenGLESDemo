package com.oyp.openglesdemo

interface RenderAction {
    fun switchBlendingMode()

    fun setMinFilter(filter: Int)
    fun setMagFilter(filter: Int)
    fun setDelta(deltaX: Float, deltaY: Float)

    fun setImageData(format: Int, width: Int, height: Int, imageData: ByteArray)

    fun setImageDataWithIndex(index: Int, format: Int, width: Int, height: Int, imageData: ByteArray)
}