package com.lyman.ffmpegsample.fragment.basicdatatype

import android.os.Bundle
import android.os.Environment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.lyman.ffmpegsample.R
import com.lyman.ffmpegsample.controller.BasicDataTypeJNI
import com.lyman.ffmpegsample.fragment.BaseFragment
import java.io.File

class BasicDataTypeFragment: BaseFragment() {
    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Pictures/BasicDataType/H264
    private val H264_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_PICTURES)?.absolutePath + File.separator + "BasicDataType" + File.separator + "H264"
    }
    private val H265_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_PICTURES)?.absolutePath + File.separator + "BasicDataType" + File.separator + "H265"
    }
    private val AAC_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_PICTURES)?.absolutePath + File.separator + "BasicDataType" + File.separator + "AAC"
    }
    private val FLV_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_PICTURES)?.absolutePath + File.separator + "BasicDataType" + File.separator + "FLV"
    }
    private val MP4_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_PICTURES)?.absolutePath + File.separator + "BasicDataType" + File.separator + "MP4"
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
        list = listOf(
            DATA_TYPE.BASIC_DATA_RGB,
            DATA_TYPE.BASIC_DATA_YUV,
            DATA_TYPE.BASIC_DATA_PCM,
            DATA_TYPE.BASIC_DATA_H264,
            DATA_TYPE.BASIC_DATA_H265,
            DATA_TYPE.BASIC_DATA_AAC,
            DATA_TYPE.BASIC_DATA_FLV,
            DATA_TYPE.BASIC_DATA_MP4,
            DATA_TYPE.BASIC_DATA_FORMAT_CONVERSION
        )
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
                    Thread( Runnable {
                        mBasicDataTypeJNI.analysisH264Format(bArrays, H264_ROOT_PATH)
                    }).start()
                    showToast("File save to $rootPath")
                }
            }
            DATA_TYPE.BASIC_DATA_H265 -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("output.h265")
                var rootPath = File(H265_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicDataTypeJNI.analysisH265Format(bArrays, H265_ROOT_PATH)
                    }).start()
                    showToast("File save to $rootPath")
                }
            }
            DATA_TYPE.BASIC_DATA_AAC -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("output.aac")
                var rootPath = File(AAC_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicDataTypeJNI.analysisAACFormat(bArrays, AAC_ROOT_PATH)
                    }).start()
                    showToast("File save to $rootPath")
                }
            }
            DATA_TYPE.BASIC_DATA_FLV -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("output.flv")
                var rootPath = File(FLV_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicDataTypeJNI.analysisFLVFormat(bArrays, FLV_ROOT_PATH)
                    }).start()
                    showToast("File save to $rootPath")
                }
            }
            DATA_TYPE.BASIC_DATA_MP4 -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("output.mp4")
                var rootPath = File(MP4_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                if(bArrays != null) {
                    //Thread( Runnable {
                        mBasicDataTypeJNI.analysisMP4Format(bArrays, MP4_ROOT_PATH)
                    //}).start()
                    // result see logs, TAG: MP4Data
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