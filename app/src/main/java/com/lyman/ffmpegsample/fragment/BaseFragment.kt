package com.lyman.ffmpegsample.fragment

import android.graphics.Rect
import android.text.TextUtils
import android.view.View
import android.widget.Toast
import androidx.fragment.app.Fragment
import androidx.recyclerview.widget.LinearLayoutManager
import androidx.recyclerview.widget.RecyclerView
import com.lyman.ffmpegsample.MainActivity
import com.lyman.ffmpegsample.R
import com.lyman.ffmpegsample.adapter.MyRecyclerViewAdapter
import java.io.InputStream

open class BaseFragment: Fragment(), MyRecyclerViewAdapter.ItemClickListener {
    var TAG: String = javaClass.simpleName
    lateinit var mRecyclerView: RecyclerView
    lateinit var mRecyclerAdapter: MyRecyclerViewAdapter
    lateinit var list : List<DATA_TYPE>


    enum class DATA_TYPE(val text: String){
        BASIC_DATA("基础数据格式"),
        BASIC_DATA_RGB("RGB格式"),
        BASIC_DATA_RGB_SPLIT("RGB24分离R、G、B分量"),
        BASIC_DATA_RGB_TO_BMP("RGB24封装为BMP"),
        BASIC_DATA_YUV("YUV格式"),
        BASIC_DATA_YUV_SPLIT("YUV420P分离Y、U、V分量"),
        BASIC_DATA_YUV_GRAY("YUV420P变为灰度图"),
        BASIC_DATA_PCM("PCM格式"),
        BASIC_DATA_PCM_SPLIT_16LE("分离PCM16LE格式"),
        BASIC_DATA_PCM_HALF_VOLUME_LEFT("降低PCM左声道音频音量"),
        BASIC_DATA_PCM_TO_WAVE("PCM16LE封装为WAVE"),
        BASIC_DATA_H264("H264视频编码格式解析"),
        BASIC_DATA_H265("H265视频编码格式解析"),
        BASIC_DATA_AAC("AAC音频编码格式解析"),
        BASIC_DATA_FLV("FLV封装格式解析"),
        BASIC_DATA_MP4("MP4封装格式解析"),
        BASIC_DATA_FORMAT_CONVERSION("格式转换"),
        BASIC_DATA_FORMAT_CONVERSION_RGB_TO_YUV("RGB24 转换为YUV420P"),
        BASIC_FFMPEG("FFMpeg基础"),
        BASIC_FFMPEG_INTEGRATED("FFMpeg集成"),
        BASIC_FFMPEG_AVCODEC("libavcodec示例"),
        BASIC_FFMPEG_AVCODEC_DECODE_VIDEO("视频数据H264解码为YUV420P"),
        BASIC_FFMPEG_AVCODEC_DECODE_AUDIO("音频数据AAC数据解码为PCM"),
        BASIC_FFMPEG_AVDEVICE("libavdevice示例"),
        BASIC_FFMPEG_AVDEVICE_NATIVE_CAMERA("native抓取Camera YUV数据"),
        BASIC_FFMPEG_AVFILTER("libavfilter示例"),
        BASIC_FFMPEG_AVFORMAT("libavformat示例"),
        BASIC_FFMPEG_AVUTIL("libavutil示例"),
        BASIC_FFMPEG_POSTPROC("libpostproc示例"),
        BASIC_FFMPEG_SWRESAMPLE("libswresample示例"),
        BASIC_FFMPEG_SWSCALE("libswscale示例"),
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


    fun readAssetsFileToByteArray(fileName: String): ByteArray? {
        if (TextUtils.isEmpty(fileName))
            return null

        try {
            var inputStream: InputStream? = activity?.assets?.open(fileName)
            return inputStream?.readBytes()
        } catch (error: Throwable) {
            error.printStackTrace()
        }
        return null
    }


    fun showToast(text: String) {
        Toast.makeText(context, text, Toast.LENGTH_SHORT).show()
    }


    fun showFragment(dataType: DATA_TYPE) {
        var mainActivity: MainActivity = activity as MainActivity
        mainActivity.showFragment(dataType)
    }


    fun showFragment(dataType: DATA_TYPE, tag: String) {
        var mainActivity: MainActivity = activity as MainActivity
        mainActivity.showFragment(dataType, tag)
    }
}