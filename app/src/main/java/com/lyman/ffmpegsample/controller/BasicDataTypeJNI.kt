package com.lyman.ffmpegsample.controller

class BasicDataTypeJNI {
    companion object{
        init {
            System.loadLibrary("basicdatatype")
        }
    }


    external fun convertRGB24ToBMP(byteArray: ByteArray, width: Int, height: Int, savePath: String)


    external fun convertRGB24ToYUV420P(byteArray: ByteArray, width: Int, height: Int, savePath: String)


    external fun setYUV420PGray(byteArray: ByteArray, width: Int, height: Int, savePath: String)


    external fun splitRGB24(byteArray: ByteArray, width: Int, height: Int, saveRootPath: String)


    external fun splitYUV420P(byteArray: ByteArray, width: Int, height: Int, saveRootPath: String)
}