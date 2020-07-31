package com.lyman.ffmpegsample.fragment

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.lyman.ffmpegsample.R
import com.lyman.ffmpegsample.controller.BasicFFMpegJNI

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
        list = listOf(DATA_TYPE.BASIC_FFMPEG_INTEGRATED)
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
            else -> super.onItemClick(view, position)
        }
    }
}