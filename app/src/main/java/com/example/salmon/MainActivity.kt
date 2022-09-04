package com.example.salmon

import android.Manifest.permission.CAMERA
import android.annotation.SuppressLint
import android.content.Context
import android.content.pm.PackageManager
import android.graphics.Color
import android.graphics.ImageFormat
import android.graphics.drawable.ColorDrawable
import android.hardware.camera2.CameraDevice
import android.hardware.camera2.CameraManager
import android.media.ImageReader
import android.os.Bundle
import android.os.Handler
import android.os.HandlerThread
import android.util.Size
import android.view.View
import android.widget.SeekBar
import androidx.appcompat.app.AppCompatActivity
import com.example.salmon.databinding.ActivityMainBinding

class MainActivity : AppCompatActivity() {
    private val binding by lazy {
        ActivityMainBinding.inflate(layoutInflater)
    }
    private val imageProcessor = ImageProcessor(this)

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        requestPermissions(arrayOf(CAMERA), REQUEST_CAMERA_PERMISSION)
    }

    override fun onResume() {
        super.onResume()
        window.decorView.apply {
            systemUiVisibility = View.SYSTEM_UI_FLAG_HIDE_NAVIGATION or View.SYSTEM_UI_FLAG_FULLSCREEN
        }
    }

    @SuppressLint("MissingPermission")
    override fun onRequestPermissionsResult(requestCode: Int, permissions: Array<out String>, grantResults: IntArray) {
        when (requestCode) {
            REQUEST_CAMERA_PERMISSION -> {
                if (grantResults.isNotEmpty() && grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    setupTextureView()
                    val cameraManager = getSystemService(Context.CAMERA_SERVICE) as CameraManager
                    openCamera(cameraManager, cameraManager.cameraIdList[0]) { cameraDevice, supportedSizes ->
                        setupSeekBar(cameraDevice, supportedSizes)
                        createNewCaptureSession(cameraDevice, listOf(createMyImageReader(supportedSizes.last().width, supportedSizes.last().height).surface))
                    }
                    setContentView(binding.root)
                } else {
                    finishAndRemoveTask()
                }
            }
            else -> super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        }
    }

    private fun setupTextureView() {
        binding.textureView.setOnLongClickListener {
            imageProcessor.changeFilter()
            false
        }
    }

    private fun setupSeekBar(cameraDevice: CameraDevice, supportedSizes: List<Size>) {
        binding.seekBar.apply {
            max = supportedSizes.lastIndex
            progress = supportedSizes.lastIndex
            thumb.alpha = 0
            progressDrawable = ColorDrawable(Color.TRANSPARENT)
            setOnSeekBarChangeListener(
                    object : SeekBar.OnSeekBarChangeListener {
                        override fun onProgressChanged(seekBar: SeekBar?, progress: Int, fromUser: Boolean) {
                            if (!fromUser) return
                            val currentSize = supportedSizes[progress]
                            binding.textView.text = "$currentSize"
                            createNewCaptureSession(cameraDevice, listOf(createMyImageReader(currentSize.width, currentSize.height).surface))
                        }

                        override fun onStartTrackingTouch(seekBar: SeekBar?) {
                            seekBar ?: return
                            binding.textView.text = "${supportedSizes[seekBar.progress]}"
                        }

                        override fun onStopTrackingTouch(seekBar: SeekBar?) {
                            binding.textView.text = ""
                        }
                    }
            )
        }
    }

    private fun createMyImageReader(imageWidth: Int, imageHeight: Int) = ImageReader
            .newInstance(imageWidth, imageHeight, ImageFormat.YUV_420_888, 2)
            .apply {
                setOnImageAvailableListener(
                        { reader ->
                            reader?.acquireLatestImage().use {
                                imageProcessor.process(it, binding.textureView)
                            }
                        },
                        Handler(HandlerThread("ImageProcessingThread").apply { start() }.looper)
                )
            }

    companion object {
        private const val REQUEST_CAMERA_PERMISSION = 200
    }
}