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


class BasicDataTypeRGBFragment: BaseFragment() {
    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Pictures/BasicDataType/RGB
    private val SPLIT_RGB_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_PICTURES)?.absolutePath + File.separator + "BasicDataType" + File.separator + "RGB"
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
            DATA_TYPE.BASIC_DATA_RGB_SPLIT,
            DATA_TYPE.BASIC_DATA_RGB_TO_BMP
        )
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type){
            DATA_TYPE.BASIC_DATA_RGB_SPLIT -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("pic.rgb")
                var rootPath = File(SPLIT_RGB_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                if(bArrays != null) {
                    mBasicDataTypeJNI.splitRGB24(bArrays, 500, 500, rootPath.toString())
                    showToast("File save to $rootPath")
                }
            }
            DATA_TYPE.BASIC_DATA_RGB_TO_BMP -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("pic.rgb")
                var rootPath = File(SPLIT_RGB_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                var saveFilePath = SPLIT_RGB_ROOT_PATH + File.separator + "pic.bmp"
                if(bArrays != null) {
                    mBasicDataTypeJNI.convertRGB24ToBMP(bArrays, 500, 500, saveFilePath)
                    showToast("File save to $saveFilePath")
                }
            }
            else -> super.onItemClick(view, position)
        }
    }
}