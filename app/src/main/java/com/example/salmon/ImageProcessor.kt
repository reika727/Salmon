package com.example.salmon

import android.app.AlertDialog
import android.content.Context
import android.content.res.Configuration
import android.graphics.SurfaceTexture
import android.media.Image
import android.view.TextureView
import android.view.WindowManager

class ImageProcessor(private val context: Context) {
    private var _process = ::noFilter

    fun process(image: Image?, textureView: TextureView) {
        image ?: return
        val isLandscape = context.resources.configuration.orientation == Configuration.ORIENTATION_LANDSCAPE
        textureView.apply {
            val magnification =
                    if (isLandscape)
                        (image.width / width.toDouble()).coerceAtLeast(image.height / height.toDouble())
                    else
                        (image.width / height.toDouble()).coerceAtLeast(image.height / width.toDouble())
            surfaceTexture.setDefaultBufferSize(kotlin.math.ceil(width * magnification).toInt(), kotlin.math.ceil(height * magnification).toInt())
        }
        _process(image, textureView.surfaceTexture, isLandscape)
    }

    fun changeFilter() {
        val filters = arrayOf(
                "一次微分フィルタ" to ::differentialFilter,
                "プレヴィットフィルタ" to ::prewittFilter,
                "ソーベルフィルタ" to ::sobelFilter,
                "ラプラシアンフィルタ" to ::laplacianFilter,
                "ネガフィルム" to ::negativeFilm,
                "擬色" to ::pseudoColor,
                "なし" to ::noFilter
        )
        AlertDialog
                .Builder(context)
                .setTitle("Filter")
                .setItems(filters.map { it.first }.toTypedArray(), null)
                .create()
                .apply {
                    listView.setOnItemClickListener { _, _, position, _ ->
                        _process = filters[position].second
                        dismiss()
                    }
                    window?.addFlags(WindowManager.LayoutParams.FLAG_NOT_FOCUSABLE or WindowManager.LayoutParams.FLAG_WATCH_OUTSIDE_TOUCH)
                }
                .show()
    }

    private external fun differentialFilter(image: Image, surfaceTexture: SurfaceTexture, isLandscape: Boolean)
    private external fun prewittFilter(image: Image, surfaceTexture: SurfaceTexture, isLandscape: Boolean)
    private external fun sobelFilter(image: Image, surfaceTexture: SurfaceTexture, isLandscape: Boolean)
    private external fun laplacianFilter(image: Image, surfaceTexture: SurfaceTexture, isLandscape: Boolean)
    private external fun negativeFilm(image: Image, surfaceTexture: SurfaceTexture, isLandscape: Boolean)
    private external fun pseudoColor(image: Image, surfaceTexture: SurfaceTexture, isLandscape: Boolean)
    private external fun noFilter(image: Image, surfaceTexture: SurfaceTexture, isLandscape: Boolean)

    companion object {
        init {
            System.loadLibrary("image_processing-lib")
        }
    }
}