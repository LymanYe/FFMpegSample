package com.lyman.ffmpegsample.fragment

import android.os.Bundle
import android.os.Environment
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.lyman.ffmpegsample.R
import com.lyman.ffmpegsample.controller.BasicDataTypeJNI
import java.io.File

class BasicDataTypeFragment: BaseFragment() {
    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Pictures/BasicDataType/H264
    private val H264_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_PICTURES)?.absolutePath + File.separator + "BasicDataType" + File.separator + "H264"
    }
    private val mBasicDataTypeJNI: BasicDataTypeJNI by lazy {
        BasicDataTypeJNI()
    }


    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        var view: View = inflater.inflate(R.layout.fragment_basic_data_type, container, false)
        list = listOf(DATA_TYPE.BASIC_DATA_RGB, DATA_TYPE.BASIC_DATA_YUV, DATA_TYPE.BASIC_DATA_PCM,
            DATA_TYPE.BASIC_DATA_H264, DATA_TYPE.BASIC_DATA_H265, DATA_TYPE.BASIC_DATA_AAC, DATA_TYPE.BASIC_DATA_FLV,
            DATA_TYPE.BASIC_DATA_MP4, DATA_TYPE.BASIC_DATA_FORMAT_CONVERSION)
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type){
            DATA_TYPE.BASIC_DATA_RGB -> {
                showFragment(DATA_TYPE.BASIC_DATA_RGB)
            }
            DATA_TYPE.BASIC_DATA_YUV -> {
                showFragment(DATA_TYPE.BASIC_DATA_YUV)
            }
            DATA_TYPE.BASIC_DATA_PCM -> {
                showFragment(DATA_TYPE.BASIC_DATA_PCM)
            }
            DATA_TYPE.BASIC_DATA_H264 -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("output.h264")
                var rootPath = File(H264_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                if(bArrays != null) {
                    Log.d(TAG, "analysisH264Format, start")
                    Thread( Runnable {
                        mBasicDataTypeJNI.analysisH264Format(bArrays, H264_ROOT_PATH)
                    }).start()
                    Log.d(TAG, "analysisH264Format, end")
                    showToast("File save to $rootPath")
                }
            }
            DATA_TYPE.BASIC_DATA_FORMAT_CONVERSION -> {
                showFragment(DATA_TYPE.BASIC_DATA_FORMAT_CONVERSION)
            }
            else -> super.onItemClick(view, position)
        }
    }
}