package com.lyman.ffmpegsample.fragment.basicffmpeg

import android.os.Bundle
import android.os.Environment
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.lyman.ffmpegsample.R
import com.lyman.ffmpegsample.controller.BasicFFMpegJNI
import com.lyman.ffmpegsample.fragment.BaseFragment
import java.io.File

class BasicFFMpegAvcodecFragment : BaseFragment() {
    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Movies/BasicFFMpeg/AVCODEC
    private val AVCODEC_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_MOVIES)?.absolutePath + File.separator + "BasicFFMpeg" + File.separator + "AVCODEC"
    }
    private val mBasicFFMpegJNI: BasicFFMpegJNI by lazy {
        BasicFFMpegJNI()
    }


    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        var view: View = inflater.inflate(R.layout.fragment_basic_data_type, container, false)
        list = listOf(
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_DECODE_VIDEO
        )
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type) {
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_DECODE_VIDEO -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("output.h264")
                var rootPath = File(AVCODEC_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                var yuvDir = File(AVCODEC_ROOT_PATH + File.separator + "decode_yuv")
                if(!yuvDir.exists())
                    yuvDir.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicFFMpegJNI.decodeVideoData2YUV420P(bArrays, AVCODEC_ROOT_PATH, yuvDir.absolutePath)
                        activity?.runOnUiThread {
                            showToast("File save to $rootPath")
                        }

                    }).start()
                }
            }
            else -> super.onItemClick(view, position)
        }
    }


}