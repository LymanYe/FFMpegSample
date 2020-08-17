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


class BasicDataTypeFormatConversionFragment: BaseFragment() {
    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Pictures/BasicDataType/
    private val FORMAT_CONVERSION_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_PICTURES)?.absolutePath + File.separator + "BasicDataType"
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
        list = listOf(DATA_TYPE.BASIC_DATA_FORMAT_CONVERSION_RGB_TO_YUV)
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type){
            DATA_TYPE.BASIC_DATA_FORMAT_CONVERSION_RGB_TO_YUV -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("pic.rgb")
                var rootPath = File(FORMAT_CONVERSION_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                var saveFilePath = FORMAT_CONVERSION_ROOT_PATH + File.separator + "pic.yuv"
                if(bArrays != null) {
                    mBasicDataTypeJNI.convertRGB24ToYUV420P(bArrays, 500, 500, saveFilePath)
                    showToast("File save to $saveFilePath")
                }
            }
            else -> super.onItemClick(view, position)
        }
    }
}