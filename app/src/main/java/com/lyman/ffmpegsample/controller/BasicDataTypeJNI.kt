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


    external fun splitPCM16LE(byteArray: ByteArray, saveRootPath: String)


    external fun halfPCMLeftVolume(byteArray: ByteArray, saveRootPath: String)


    external fun convertPCM16LEToWAVE(byteArray: ByteArray, saveRootPath: String)


    external fun analysisH264Format(byteArray: ByteArray, saveRootPath: String)


    external fun analysisH265Format(byteArray: ByteArray, saveRootPath: String)


    external fun analysisAACFormat(byteArray: ByteArray, saveRootPath: String)


    external fun analysisFLVFormat(byteArray: ByteArray, saveRootPath: String)


    external fun analysisMP4Format(byteArray: ByteArray, saveRootPath: String)
}