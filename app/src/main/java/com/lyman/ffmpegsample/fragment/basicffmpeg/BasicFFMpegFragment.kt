package com.lyman.ffmpegsample.fragment.basicffmpeg

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.lyman.ffmpegsample.R
import com.lyman.ffmpegsample.controller.BasicFFMpegJNI
import com.lyman.ffmpegsample.fragment.BaseFragment

class BasicFFMpegFragment: BaseFragment() {
    private val mBasicFFMpegJNI: BasicFFMpegJNI by lazy {
        BasicFFMpegJNI()
    }


    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        var view: View = inflater.inflate(R.layout.fragment_basic_ffmpeg, container, false)
        list = listOf(
            DATA_TYPE.BASIC_FFMPEG_INTEGRATED,
            DATA_TYPE.BASIC_FFMPEG_AVCODEC,
            //DATA_TYPE.BASIC_FFMPEG_AVDEVICE,/*Not implement*/
            DATA_TYPE.BASIC_FFMPEG_AVFILTER,
            DATA_TYPE.BASIC_FFMPEG_AVFORMAT,
            DATA_TYPE.BASIC_FFMPEG_AVUTIL,
            DATA_TYPE.BASIC_FFMPEG_POSTPROC,
            DATA_TYPE.BASIC_FFMPEG_SWRESAMPLE,
            DATA_TYPE.BASIC_FFMPEG_SWSCALE
        )
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type){
            DATA_TYPE.BASIC_FFMPEG_INTEGRATED -> {
                var version = mBasicFFMpegJNI.getConfiguration()
                showToast("configuration is : $version")
            }
            DATA_TYPE.BASIC_FFMPEG_AVCODEC-> {
                showFragment(DATA_TYPE.BASIC_FFMPEG_AVCODEC)
            }
            DATA_TYPE.BASIC_FFMPEG_AVDEVICE-> {
                showFragment(DATA_TYPE.BASIC_FFMPEG_AVDEVICE, BasicFFMpegAvdeviceFragment.NAME_TAG)
            }
            DATA_TYPE.BASIC_FFMPEG_AVFILTER-> {
                showFragment(DATA_TYPE.BASIC_FFMPEG_AVFILTER)
            }
            else -> super.onItemClick(view, position)
        }
    }
}