package com.lyman.ffmpegsample.controller

class BasicFFMpegJNI {
    companion object {
        init {
            System.loadLibrary("basicffmpeg")
        }
    }


    external fun getConfiguration(): String
}