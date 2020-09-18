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


    external fun encodeS16leSingleToneData2MP2(saveRootPath: String)


    external fun encodePCMData2AAC(byteArray: ByteArray, saveRootPath: String, savePCMDir: String)


    external fun encodePCMData2MP3(byteArray: ByteArray, saveRootPath: String, savePCMDir: String)


    external fun encodeYUV420PData2H264(byteArray: ByteArray, saveRootPath: String, savePCMDir: String, width: Int, height: Int)


    external fun encodeYUV420PSingleData2H264(saveRootPath: String)


    external fun basicFilterPCMData(duration: Int)


    external fun decodeAudioAndFillFilter(byteArray: ByteArray, saveRootPath: String)


    external fun yuv420PAndFilter(byteArray: ByteArray, width: Int, height: Int, saveRootPath: String)


    external fun swscaleGenerateYUV420P2RGB24(saveFilePath: String)


    external fun resampleDBL2S16PCM(saveFilePath: String)
}