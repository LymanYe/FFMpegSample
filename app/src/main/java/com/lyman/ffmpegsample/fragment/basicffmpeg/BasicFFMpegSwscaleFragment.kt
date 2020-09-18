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

class BasicFFMpegSwscaleFragment : BaseFragment() {
    companion object {
        val NAME_TAG = "BasicFFMpegSwscaleFragment"
    }


    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Movies/BasicFFMpeg/SWSCALE
    private val SWSCALE_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_MOVIES)?.absolutePath + File.separator + "BasicFFMpeg" + File.separator + "SWSCALE"
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
            DATA_TYPE.BASIC_FFMPEG_SWSCALE_GENERATE_YUV420P2RGB24
        )
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type) {
            DATA_TYPE.BASIC_FFMPEG_SWSCALE_GENERATE_YUV420P2RGB24 -> {
                var scaleDir = File(SWSCALE_ROOT_PATH)
                if(!scaleDir.exists()){
                    scaleDir.mkdirs()
                }
                var scaleFile = File(SWSCALE_ROOT_PATH + File.separator + "ouput.rgb")
                if(!scaleFile.exists())
                    scaleFile.createNewFile()
                Thread( Runnable {
                    mBasicFFMpegJNI.swscaleGenerateYUV420P2RGB24(scaleFile.absolutePath)
                    activity?.runOnUiThread {
                        showToast("File save to ${scaleFile.absolutePath}")
                    }

                }).start()
            }
            else -> super.onItemClick(view, position)
        }
    }
}