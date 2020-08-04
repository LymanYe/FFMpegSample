package com.lyman.ffmpegsample.fragment

import android.graphics.Rect
import android.view.View
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.lyman.ffmpegsample.MainActivity
import com.lyman.ffmpegsample.R
import com.lyman.ffmpegsample.adapter.MyRecyclerViewAdapter

open class BaseFragment: Fragment(), MyRecyclerViewAdapter.ItemClickListener {
    lateinit var mRecyclerView: RecyclerView
    lateinit var mRecyclerAdapter: MyRecyclerViewAdapter
    lateinit var list : List<DATA_TYPE>
    enum class DATA_TYPE(val text: String){
        BASIC_DATA("基础数据格式"),
        BASIC_DATA_RGB("RGB格式"),
        BASIC_DATA_RGB_SPLIT("RGB24分离R、G、B分量"),
        BASIC_DATA_RGB_TO_BMP("RGB24封装为BMP"),
        BASIC_DATA_YUV("YUV格式"),
        BASIC_DATA_PCM("PCM格式"),
        BASIC_DATA_H264("H264视频编码格式"),
        BASIC_DATA_H265("H265视频编码格式"),
        BASIC_DATA_AAC("AAC音频编码格式"),
        BASIC_DATA_FLV("FLV封装格式"),
        BASIC_DATA_MP4("MP4封装格式"),
        BASIC_FFMPEG("FFMpeg基础"),
        BASIC_FFMPEG_INTEGRATED("FFMpeg集成"),
        ENCODE("编码"),
        DECODER("解码")
    }
    inner class SpacesItemDecoration(private var space: Int): RecyclerView.ItemDecoration() {
        override fun getItemOffsets(
            outRect: Rect,
            view: View,
            parent: RecyclerView,
            state: RecyclerView.State
        ) {
            outRect?.let {
                it.left = space
                it.right = space
                it.bottom = space
                if(parent.getChildLayoutPosition(view) == 0)
                    it.top = space
            }
        }
    }

    fun initView(view: View) {
        if(list != null) {
            mRecyclerView = view.findViewById(R.id.recycler_view)
            mRecyclerView.layoutManager = LinearLayoutManager(context)
            mRecyclerView.addItemDecoration(SpacesItemDecoration(30))
            mRecyclerAdapter = MyRecyclerViewAdapter(list)
            mRecyclerAdapter.setOnClickListener(this)
            mRecyclerView.adapter = mRecyclerAdapter
        }
    }

    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type){
            DATA_TYPE.BASIC_DATA -> {
                showFragment(DATA_TYPE.BASIC_DATA)
            }
            DATA_TYPE.BASIC_FFMPEG -> {
                showFragment(DATA_TYPE.BASIC_FFMPEG)
            }
            DATA_TYPE.ENCODE -> {}
            DATA_TYPE.DECODER -> {}
        }
        Toast.makeText(context, "clicked" + type.text , Toast.LENGTH_SHORT).show()
    }

    fun showToast(text: String) {
        Toast.makeText(context, text, Toast.LENGTH_SHORT).show()
    }

    fun showFragment(dataType: DATA_TYPE) {
        var mainActivity: MainActivity = activity as MainActivity
        mainActivity.showFragment(dataType)
    }
}