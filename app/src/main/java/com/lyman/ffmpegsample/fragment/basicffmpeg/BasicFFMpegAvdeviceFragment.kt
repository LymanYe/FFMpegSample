package com.lyman.ffmpegsample.fragment.basicffmpeg

import android.content.pm.PackageManager
import android.os.Bundle
import android.os.Environment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.lyman.ffmpegsample.R
import com.lyman.ffmpegsample.controller.BasicFFMpegJNI
import com.lyman.ffmpegsample.fragment.BaseFragment
import java.io.File

class BasicFFMpegAvdeviceFragment : BaseFragment() {
    companion object {
        val NAME_TAG = "BasicFFMpegAvdeviceFragment"
        val REQUEST_CAMERA_PERMISSION = 1
    }


    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Movies/BasicFFMpeg/AVCODEC
    private val AVDEVICE_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_MOVIES)?.absolutePath + File.separator + "BasicFFMpeg" + File.separator + "AVDEVICE"
    }
    private val mBasicFFMpegJNI: BasicFFMpegJNI by lazy {
        BasicFFMpegJNI()
    }


    override fun onActivityCreated(savedInstanceState: Bundle?) {
        super.onActivityCreated(savedInstanceState)
        checkCameraPermission()
    }


    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        var view: View = inflater.inflate(R.layout.fragment_basic_data_type, container, false)
        list = listOf(
            DATA_TYPE.BASIC_FFMPEG_AVDEVICE_NATIVE_CAMERA
        )
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type) {
            DATA_TYPE.BASIC_FFMPEG_AVDEVICE_NATIVE_CAMERA -> {
                var rootPath = File(AVDEVICE_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()

                Thread( Runnable {
                    mBasicFFMpegJNI.openNativeCamera(rootPath.absolutePath)
                    activity?.runOnUiThread {
                        showToast("File save to $rootPath")
                    }
                }).start()
            }
            else -> super.onItemClick(view, position)
        }
    }


    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        when (requestCode) {
            REQUEST_CAMERA_PERMISSION -> {
                if (grantResults.isEmpty() || grantResults[0] != PackageManager.PERMISSION_GRANTED) {
                    showToast("请开启相机权限")
                    activity?.onBackPressed()
                }
            }
            else -> super.onRequestPermissionsResult(requestCode, permissions, grantResults)
        }
    }


    private fun checkCameraPermission() {
        if (context?.checkSelfPermission(android.Manifest.permission.CAMERA) != PackageManager.PERMISSION_GRANTED) {
            requestPermissions(arrayOf(android.Manifest.permission.CAMERA), REQUEST_CAMERA_PERMISSION)
        }
    }
}