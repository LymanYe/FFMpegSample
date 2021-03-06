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

class BasicFFMpegAvcodecFragment : BaseFragment() {
    // storage/emulated/0/Android/data/com.lyman.ffmpegsample/files/Movies/BasicFFMpeg/AVCODEC
    private val AVCODEC_ROOT_PATH by lazy {
        activity?.getExternalFilesDir(Environment.DIRECTORY_MOVIES)?.absolutePath + File.separator + "BasicFFMpeg" + File.separator + "AVCODEC"
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
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_DECODE_VIDEO,
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_DECODE_AAC,
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_DECODE_MP3,
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_ENCODE_GENERATE_YUV420P2H264,
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_ENCODE_YUV420P2H264,
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_ENCODE_PCM16_SINGLE_TONE2MP2,
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_ENCODE_PCM2AAC,
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_ENCODE_PCM2MP3
        )
        initView(view)
        return view
    }


    override fun onItemClick(view: View, position: Int) {
        var type = mRecyclerAdapter.getItem(position)
        when(type) {
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_DECODE_VIDEO -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("output.h264")
                var rootPath = File(AVCODEC_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                var yuvDir = File(AVCODEC_ROOT_PATH + File.separator + "decode_yuv")
                if(!yuvDir.exists())
                    yuvDir.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicFFMpegJNI.decodeVideoData2YUV420P(bArrays, AVCODEC_ROOT_PATH, yuvDir.absolutePath)
                        activity?.runOnUiThread {
                            showToast("File save to $rootPath")
                        }

                    }).start()
                }
            }
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_DECODE_AAC -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("output.aac")
                var rootPath = File(AVCODEC_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                var pcmDir = File(AVCODEC_ROOT_PATH + File.separator + "decode_pcm")
                if(!pcmDir.exists())
                    pcmDir.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicFFMpegJNI.decodeAACData2PCM(bArrays, AVCODEC_ROOT_PATH, pcmDir.absolutePath)
                        activity?.runOnUiThread {
                            showToast("File save to $rootPath")
                        }

                    }).start()
                }
            }
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_DECODE_MP3 -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("output.mp3")
                var rootPath = File(AVCODEC_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                var pcmDir = File(AVCODEC_ROOT_PATH + File.separator + "decode_pcm")
                if(!pcmDir.exists())
                    pcmDir.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicFFMpegJNI.decodeMP3Data2PCM(bArrays, AVCODEC_ROOT_PATH, pcmDir.absolutePath)
                        activity?.runOnUiThread {
                            showToast("File save to $rootPath")
                        }

                    }).start()
                }
            }
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_ENCODE_YUV420P2H264 -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("yuv420p.yuv")
                var rootPath = File(AVCODEC_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                var h264Dir = File(AVCODEC_ROOT_PATH + File.separator + "encode_h264")
                if(!h264Dir.exists())
                    h264Dir.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicFFMpegJNI.encodeYUV420PData2H264(bArrays, AVCODEC_ROOT_PATH, h264Dir.absolutePath, 500, 500)
                        activity?.runOnUiThread {
                            showToast("File save to $rootPath")
                        }

                    }).start()
                }
            }
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_ENCODE_GENERATE_YUV420P2H264 -> {
                var h264Dir = File(AVCODEC_ROOT_PATH + File.separator + "encode_h264")
                if(!h264Dir.exists())
                    h264Dir.mkdirs()
                Thread( Runnable {
                    mBasicFFMpegJNI.encodeYUV420PSingleData2H264(h264Dir.absolutePath)
                    activity?.runOnUiThread {
                        showToast("File save to ${h264Dir.absolutePath}")
                    }

                }).start()
            }
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_ENCODE_PCM16_SINGLE_TONE2MP2 -> {
                var pcmDir = File(AVCODEC_ROOT_PATH + File.separator + "encode_pcm")
                if(!pcmDir.exists())
                    pcmDir.mkdirs()
                Thread( Runnable {
                    mBasicFFMpegJNI.encodeS16leSingleToneData2MP2(pcmDir.absolutePath)
                    activity?.runOnUiThread {
                        showToast("File save to $pcmDir")
                    }

                }).start()
            }
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_ENCODE_PCM2AAC -> {
                var bArrays: ByteArray? = readAssetsFileToByteArray("fltp.pcm")
                var rootPath = File(AVCODEC_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                var pcmDir = File(AVCODEC_ROOT_PATH + File.separator + "encode_pcm")
                if(!pcmDir.exists())
                    pcmDir.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicFFMpegJNI.encodePCMData2AAC(bArrays, AVCODEC_ROOT_PATH, pcmDir.absolutePath)
                        activity?.runOnUiThread {
                            showToast("File save to $rootPath")
                        }

                    }).start()
                }
            }
            DATA_TYPE.BASIC_FFMPEG_AVCODEC_ENCODE_PCM2MP3 -> {
                // 传入的是fltp还是s16le采样的pcm要做区分设置不同的采样格式
                var bArrays: ByteArray? = readAssetsFileToByteArray("fltp.pcm")
                var rootPath = File(AVCODEC_ROOT_PATH)
                if(!rootPath.exists())
                    rootPath.mkdirs()
                var pcmDir = File(AVCODEC_ROOT_PATH + File.separator + "encode_pcm")
                if(!pcmDir.exists())
                    pcmDir.mkdirs()
                if(bArrays != null) {
                    Thread( Runnable {
                        mBasicFFMpegJNI.encodePCMData2MP3(bArrays, AVCODEC_ROOT_PATH, pcmDir.absolutePath)
                        activity?.runOnUiThread {
                            showToast("File save to $rootPath")
                        }

                    }).start()
                }
            }
            else -> super.onItemClick(view, position)
        }
    }
}