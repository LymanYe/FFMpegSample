package com.lyman.ffmpegsample.fragment

import android.os.Bundle
import android.os.Environment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.lyman.ffmpegsample.R
import com.lyman.ffmpegsample.controller.BasicDataTypeJNI
import java.io.File


class BasicDataTypePCMFragment: BaseFragment() {
    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Pictures/BasicDataType/PCM
    private val SPLIT_PCM_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_PICTURES)?.absolutePath + File.separator + "BasicDataType" + File.separator + "PCM"
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
        list = listOf(DATA_TYPE.BASIC_DATA_PCM_SPLIT_16LE, DATA_TYPE.BASIC_DATA_PCM_HALF_VOLUME_LEFT, DATA_TYPE.BASIC_DATA_PCM_TO_WAVE)
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type){
            DATA_TYPE.BASIC_DATA_PCM_SPLIT_16LE -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("out.pcm")
                var rootPath = File(SPLIT_PCM_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                if(bArrays != null) {
                    mBasicDataTypeJNI.splitPCM16LE(bArrays, rootPath.toString())
                    showToast("File save to $rootPath")
                }
            }
            DATA_TYPE.BASIC_DATA_PCM_HALF_VOLUME_LEFT -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("out.pcm")
                var rootPath = File(SPLIT_PCM_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                if(bArrays != null) {
                    mBasicDataTypeJNI.halfPCMLeftVolume(bArrays, rootPath.toString())
                    showToast("File save to $rootPath")
                }
            }
            DATA_TYPE.BASIC_DATA_PCM_TO_WAVE -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("out.pcm")
                var rootPath = File(SPLIT_PCM_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                var saveFilePath = SPLIT_PCM_ROOT_PATH + File.separator + "out.wav"
                if(bArrays != null) {
                    mBasicDataTypeJNI.convertPCM16LEToWAVE(bArrays, saveFilePath)
                    showToast("File save to $saveFilePath")
                }
            }
            else -> super.onItemClick(view, position)
        }
    }
}