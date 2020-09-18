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

class BasicFFMpegAvFilterFragment : BaseFragment() {
    companion object {
        val NAME_TAG = "BasicFFMpegAvdeviceFragment"
        val REQUEST_CAMERA_PERMISSION = 1
    }


    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Movies/BasicFFMpeg/AVFILTER
    private val AVFILTER_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_MOVIES)?.absolutePath + File.separator + "BasicFFMpeg" + File.separator + "AVFILTER"
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
            DATA_TYPE.BASIC_FFMPEG_AVFILTER_BASIC,
            DATA_TYPE.BASIC_FFMPEG_AVFILTER_DECODE_AUDIO_AND_SEND_TO_FILTER,
            DATA_TYPE.BASIC_FFMPEG_AVFILTER_YUV420P_ADD_FILTER
        )
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type) {
            DATA_TYPE.BASIC_FFMPEG_AVFILTER_BASIC -> {
                Thread( Runnable {
                    mBasicFFMpegJNI.basicFilterPCMData(10)
                    activity?.runOnUiThread {
                        showToast("finish")
                    }
                }).start()
            }
            DATA_TYPE.BASIC_FFMPEG_AVFILTER_DECODE_AUDIO_AND_SEND_TO_FILTER -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("output.mp4")
                var rootPath = File(AVFILTER_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicFFMpegJNI.decodeAudioAndFillFilter(bArrays, rootPath.absolutePath)
                        activity?.runOnUiThread {
                            showToast("finish")
                        }
                    }).start()
                }

            }
            DATA_TYPE.BASIC_FFMPEG_AVFILTER_YUV420P_ADD_FILTER -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("yuv420p.yuv")
                var rootPath = File(AVFILTER_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicFFMpegJNI.yuv420PAndFilter(bArrays, 500, 500, rootPath.absolutePath)
                        activity?.runOnUiThread {
                            showToast("File save to ${rootPath.absoluteFile}")
                        }
                    }).start()
                }

            }
            else -> super.onItemClick(view, position)
        }
    }
}