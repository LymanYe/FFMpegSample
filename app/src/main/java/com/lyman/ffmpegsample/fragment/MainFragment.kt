package com.lyman.ffmpegsample.fragment

import android.os.Bundle
import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import androidx.recyclerview.widget.LinearLayoutManager
import com.lyman.ffmpegsample.R
import com.lyman.ffmpegsample.adapter.MyRecyclerViewAdapter

class MainFragment: BaseFragment(), MyRecyclerViewAdapter.ItemClickListener {
    companion object {
        val NAME_TAG = "MainFragment"
        fun newInstance() : MainFragment{
            return MainFragment()
        }
    }


    override fun onCreateView(
        inflater: LayoutInflater,
        container: ViewGroup?,
        savedInstanceState: Bundle?
    ): View? {
        var view: View = inflater.inflate(R.layout.fragment_main, container, false)
        list = listOf(DATA_TYPE.BASIC_DATA, DATA_TYPE.BASIC_FFMPEG)
        initView(view)
        return view
    }

}