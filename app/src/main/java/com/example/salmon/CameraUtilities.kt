package com.example.salmon

import android.Manifest.permission.CAMERA
import android.hardware.camera2.CameraCaptureSession
import android.hardware.camera2.CameraCharacteristics
import android.hardware.camera2.CameraDevice
import android.hardware.camera2.CameraManager
import android.hardware.camera2.params.OutputConfiguration
import android.hardware.camera2.params.SessionConfiguration
import android.media.ImageReader
import android.util.Size
import android.view.Surface
import androidx.annotation.RequiresPermission
import java.util.concurrent.Executors

@RequiresPermission(CAMERA)
fun openCamera(cameraManager: CameraManager, cameraId: String, onOpened: (CameraDevice, List<Size>) -> Unit) {
    val supportedSizes = cameraManager
            .getCameraCharacteristics(cameraId)
            .get(CameraCharacteristics.SCALER_STREAM_CONFIGURATION_MAP)!!
            .getOutputSizes(ImageReader::class.java)
            .sortedWith(compareByDescending<Size> { it.width }.thenByDescending { it.height })
    cameraManager.openCamera(
            cameraId,
            object : CameraDevice.StateCallback() {
                override fun onOpened(camera: CameraDevice) {
                    onOpened(camera, supportedSizes)
                }

                override fun onDisconnected(camera: CameraDevice) {
                    camera.close()
                }

                override fun onError(camera: CameraDevice, error: Int) {
                    camera.close()
                }
            },
            null
    )
}

fun createNewCaptureSession(cameraDevice: CameraDevice, surfaces: List<Surface>) {
    cameraDevice.createCaptureSession(
            SessionConfiguration(
                    SessionConfiguration.SESSION_REGULAR,
                    surfaces.map { OutputConfiguration(it) },
                    Executors.newSingleThreadExecutor(),
                    object : CameraCaptureSession.StateCallback() {
                        override fun onConfigured(session: CameraCaptureSession) {
                            try {
                                session.setRepeatingRequest(
                                        cameraDevice
                                                .createCaptureRequest(CameraDevice.TEMPLATE_PREVIEW)
                                                .apply {
                                                    surfaces.forEach { addTarget(it) }
                                                }
                                                .build(),
                                        null,
                                        null
                                )
                            } catch (e: IllegalStateException) {
                                return
                            }
                        }

                        override fun onConfigureFailed(session: CameraCaptureSession) {}
                    }
            )
    )
}