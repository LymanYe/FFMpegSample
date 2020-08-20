package com.lyman.ffmpegsample.controller

class BasicFFMpegJNI {
    companion object {
        init {
            System.loadLibrary("basicffmpeg")
        }
    }


    external fun getConfiguration(): String


    external fun openNativeCamera(saveYUVDir: String)


    external fun decodeVideoData2YUV420P(byteArray: ByteArray, saveRootPath: String, saveYUVDir: String)


    external fun decodeMP3Data2PCM(byteArray: ByteArray, saveRootPath: String, savePCMDir: String)


    external fun decodeAACData2PCM(byteArray: ByteArray, saveRootPath: String, savePCMDir: String)


    external fun encodePCMData2AAC(byteArray: ByteArray, saveRootPath: String, savePCMDir: String)


    external fun encodePCMData2MP3(byteArray: ByteArray, saveRootPath: String, savePCMDir: String)


    external fun encodeYUV420PData2H264(byteArray: ByteArray, saveRootPath: String, savePCMDir: String)
}