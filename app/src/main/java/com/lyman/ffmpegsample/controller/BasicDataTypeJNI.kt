package com.lyman.ffmpegsample.controller

class BasicDataTypeJNI {
    companion object{
        init {
            System.loadLibrary("basicdatatype")
        }
    }


    external fun splitRGB24(byteArray: ByteArray, width: Int, height: Int, saveRootPath: String)


    external fun convertRGB24ToBMP(byteArray: ByteArray, width: Int, height: Int, savePath: String)
}