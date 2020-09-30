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

/**
 * 封装格式处理
 */
class BasicFFMpegEncapsulationFormatFragment : BaseFragment() {
    companion object {
        val NAME_TAG = "BasicFFMpegEncapsulationFormatFragment"
    }


    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Movies/BasicFFMpeg/SWSCALE
    private val ENCAPSULATION_FORMAT_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_MOVIES)?.absolutePath + File.separator + "BasicFFMpeg" + File.separator + "ENCAPSULATION_FORMAT"
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
            DATA_TYPE.BASIC_FFMPELG_ENCAPSULATION_FORMAT_SAMPLE_DEMUXER,
            DATA_TYPE.BASIC_FFMPELG_ENCAPSULATION_FORMAT_DEMUXER,
            DATA_TYPE.BASIC_FFMPELG_ENCAPSULATION_FORMAT_MUXER,
            DATA_TYPE.BASIC_FFMPELG_ENCAPSULATION_FORMAT_REMUXER,
            DATA_TYPE.BASIC_FFMPELG_ENCAPSULATION_FORMAT_ENCODE_AND_MUXER
        )
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type) {
            DATA_TYPE.BASIC_FFMPELG_ENCAPSULATION_FORMAT_SAMPLE_DEMUXER -> {
                var demuxuerDir = File(ENCAPSULATION_FORMAT_ROOT_PATH + File.separator + "demuxer")
                if(!demuxuerDir.exists()){
                    demuxuerDir.mkdirs()
                }

                var byteArray: ByteArray? = readAssetsFileToByteArray("h264_mp3.mp4")
                var inputFilePath = demuxuerDir.absolutePath + File.separator + "h264_mp3.mp4"
                writeByteArray2File(byteArray, inputFilePath)

                Thread( Runnable {
                    mBasicFFMpegJNI.encapsulationFormatSampleDeMuxer(inputFilePath, demuxuerDir.absolutePath)
                    activity?.runOnUiThread {
                        showToast("File save to ${demuxuerDir.absolutePath}")
                    }

                }).start()
            }
            DATA_TYPE.BASIC_FFMPELG_ENCAPSULATION_FORMAT_DEMUXER -> {
                var demuxuerDir = File(ENCAPSULATION_FORMAT_ROOT_PATH + File.separator + "demuxer")
                if(!demuxuerDir.exists()){
                    demuxuerDir.mkdirs()
                }

                var byteArray: ByteArray? = readAssetsFileToByteArray("h264_aac.mp4")
                var inputFilePath = demuxuerDir.absolutePath + File.separator + "h264_aac.mp4"
                writeByteArray2File(byteArray, inputFilePath)

                Thread( Runnable {
                    mBasicFFMpegJNI.encapsulationFormatDeMuxer(inputFilePath, demuxuerDir.absolutePath)
                    activity?.runOnUiThread {
                        showToast("File save to ${demuxuerDir.absolutePath}")
                    }

                }).start()
            }
            DATA_TYPE.BASIC_FFMPELG_ENCAPSULATION_FORMAT_MUXER -> {
                Thread( Runnable {
                    var muxuerDir = File(ENCAPSULATION_FORMAT_ROOT_PATH + File.separator + "muxer")
                    if(!muxuerDir.exists()){
                        muxuerDir.mkdirs()
                    }

                    var byteArray: ByteArray? = readAssetsFileToByteArray("video_stream.h264")
                    var inputVideoFilePath = muxuerDir.absolutePath + File.separator + "video_stream.h264"
                    writeByteArray2File(byteArray, inputVideoFilePath)

                    byteArray = readAssetsFileToByteArray("audio_stream.aac")
                    var inputAudioFilePath = muxuerDir.absolutePath + File.separator + "audio_stream.aac"
                    writeByteArray2File(byteArray, inputAudioFilePath)

                    var outputFilePath = muxuerDir.absolutePath + File.separator + "output.mp4"

                    mBasicFFMpegJNI.encapsulationFormatMuxer(inputVideoFilePath, inputAudioFilePath, outputFilePath)
                    activity?.runOnUiThread {
                        showToast("File save to $outputFilePath")
                    }
                }).start()
            }
            DATA_TYPE.BASIC_FFMPELG_ENCAPSULATION_FORMAT_REMUXER -> {
                Thread( Runnable {
                    var muxuerDir = File(ENCAPSULATION_FORMAT_ROOT_PATH + File.separator + "muxer")
                    if(!muxuerDir.exists()){
                        muxuerDir.mkdirs()
                    }

                    var byteArray: ByteArray? = readAssetsFileToByteArray("output.mp4")
                    var inputVideoFilePath = muxuerDir.absolutePath + File.separator + "remuxing_input.mp4"
                    writeByteArray2File(byteArray, inputVideoFilePath)

                    var outputFilePath = muxuerDir.absolutePath + File.separator + "remuxing_output.flv"

                    mBasicFFMpegJNI.encapsulationFormatReMuxer(inputVideoFilePath, outputFilePath)
                    activity?.runOnUiThread {
                        showToast("File save to $outputFilePath")
                    }
                }).start()
            }
            DATA_TYPE.BASIC_FFMPELG_ENCAPSULATION_FORMAT_ENCODE_AND_MUXER -> {
                Thread( Runnable {
                    var muxuerDir = File(ENCAPSULATION_FORMAT_ROOT_PATH + File.separator + "muxer")
                    if(!muxuerDir.exists()){
                        muxuerDir.mkdirs()
                    }

                    var outputFilePath = muxuerDir.absolutePath + File.separator + "encode_muxing_output.mp4"

                    mBasicFFMpegJNI.encapsulationFormatEncodeAndMuxer(outputFilePath)
                    activity?.runOnUiThread {
                        showToast("File save to $outputFilePath")
                    }
                }).start()
            }
            else -> super.onItemClick(view, position)
        }
    }
}