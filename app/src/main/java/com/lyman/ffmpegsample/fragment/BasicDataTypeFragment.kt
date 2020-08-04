package com.lyman.ffmpegsample.fragment

import android.os.Bundle
import android.util.Log
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import com.lyman.ffmpegsample.R

class BasicDataTypeFragment: BaseFragment() {
    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        var view: View = inflater.inflate(R.layout.fragment_basic_data_type, container, false)
        list = listOf(DATA_TYPE.BASIC_DATA_RGB, DATA_TYPE.BASIC_DATA_YUV, DATA_TYPE.BASIC_DATA_PCM,
            DATA_TYPE.BASIC_DATA_H264, DATA_TYPE.BASIC_DATA_H265, DATA_TYPE.BASIC_DATA_AAC, DATA_TYPE.BASIC_DATA_FLV, DATA_TYPE.BASIC_DATA_MP4)
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type){
            DATA_TYPE.BASIC_DATA_RGB -> {
                showFragment(DATA_TYPE.BASIC_DATA_RGB)
            }
            else -> super.onItemClick(view, position)
        }
    }
}