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

class BasicFFMpegSwresampleFragment : BaseFragment() {
    companion object {
        val NAME_TAG = "BasicFFMpegSwresampleFragment"
    }


    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Movies/BasicFFMpeg/SWSRESAMPLE
    private val SWRESAMPLE_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_MOVIES)?.absolutePath + File.separator + "BasicFFMpeg" + File.separator + "SWSRESAMPLE"
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
            DATA_TYPE.BASIC_FFMPEG_SWRESAMPLE_PCM_DBL2S16
        )
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type) {
            DATA_TYPE.BASIC_FFMPEG_SWRESAMPLE_PCM_DBL2S16 -> {
                var resampleDir = File(SWRESAMPLE_ROOT_PATH)
                if(!resampleDir.exists()){
                    resampleDir.mkdirs()
                }
                var resampleFile = File(SWRESAMPLE_ROOT_PATH + File.separator + "output_s16.pcm")
                if(!resampleFile.exists())
                    resampleFile.createNewFile()
                Thread( Runnable {
                    mBasicFFMpegJNI.resampleDBL2S16PCM(resampleFile.absolutePath)
                    activity?.runOnUiThread {
                        showToast("File save to ${resampleFile.absolutePath}")
                    }
                }).start()
            }
            else -> super.onItemClick(view, position)
        }
    }
}