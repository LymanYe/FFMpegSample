package com.lyman.ffmpegsample.controller

class BasicFFMpegJNI {
    companion object {
        init {
            System.loadLibrary("basicffmpeg")
        }
    }


    external fun getConfiguration(): String


    external fun decodeVideoData2YUV420P(byteArray: ByteArray, saveRootPath: String, saveYUVDir: String)
}