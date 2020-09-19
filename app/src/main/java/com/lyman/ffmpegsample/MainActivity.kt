package com.lyman.ffmpegsample

import android.os.Bundle
import android.util.Log
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import com.lyman.ffmpegsample.fragment.BaseFragment
import com.lyman.ffmpegsample.fragment.MainFragment
import com.lyman.ffmpegsample.fragment.basicdatatype.*
import com.lyman.ffmpegsample.fragment.basicffmpeg.*

class MainActivity : AppCompatActivity() {
    companion object {
        var TAG: String = "MainActivity"
    }
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        showFragment(MainFragment.newInstance(), MainFragment.NAME_TAG)
    }


    private fun showFragment(fragment: BaseFragment, tag: String) {
        if(fragment != null)
            supportFragmentManager.beginTransaction().replace(R.id.content, fragment, tag).addToBackStack(null).commit()

    }


    fun showFragment(type: BaseFragment.DATA_TYPE, tag: String) {
        var fragment: Fragment? = null
        when(type){
            BaseFragment.DATA_TYPE.BASIC_FFMPEG_AVDEVICE -> fragment =
                BasicFFMpegAvdeviceFragment()
        }
        if(fragment != null)
            supportFragmentManager.beginTransaction().replace(R.id.content, fragment, tag).addToBackStack(null).commit()
    }


    fun showFragment(type: BaseFragment.DATA_TYPE) {
        var fragment: Fragment? = null
        when(type){
            BaseFragment.DATA_TYPE.BASIC_DATA -> fragment =
                BasicDataTypeFragment()
            BaseFragment.DATA_TYPE.BASIC_DATA_RGB -> fragment =
                BasicDataTypeRGBFragment()
            BaseFragment.DATA_TYPE.BASIC_DATA_YUV -> fragment =
                BasicDataTypeYUVFragment()
            BaseFragment.DATA_TYPE.BASIC_DATA_PCM -> fragment =
                BasicDataTypePCMFragment()
            BaseFragment.DATA_TYPE.BASIC_DATA_FORMAT_CONVERSION -> fragment =
                BasicDataTypeFormatConversionFragment()
            BaseFragment.DATA_TYPE.BASIC_FFMPEG -> fragment =
                BasicFFMpegFragment()
            BaseFragment.DATA_TYPE.BASIC_FFMPEG_AVCODEC -> fragment =
                BasicFFMpegAvcodecFragment()
            BaseFragment.DATA_TYPE.BASIC_FFMPEG_AVFILTER -> fragment =
                BasicFFMpegAvFilterFragment()
            BaseFragment.DATA_TYPE.BASIC_FFMPEG_SWSCALE -> fragment =
                BasicFFMpegSwscaleFragment()
            BaseFragment.DATA_TYPE.BASIC_FFMPEG_SWRESAMPLE -> fragment =
                BasicFFMpegSwresampleFragment()
            BaseFragment.DATA_TYPE.BASIC_FFMPELG_ENCAPSULATION_FORMAT -> fragment =
                BasicFFMpegEncapsulationFormatFragment()
        }
        if(fragment != null)
            supportFragmentManager.beginTransaction().replace(R.id.content, fragment).addToBackStack(null).commit()
    }


    override fun onBackPressed() {
        if(supportFragmentManager.backStackEntryCount == 1)
            finish()
        else
            supportFragmentManager.popBackStack()
    }


    override fun onRequestPermissionsResult(
        requestCode: Int,
        permissions: Array<out String>,
        grantResults: IntArray
    ) {
        var fragment: Fragment? = supportFragmentManager.findFragmentByTag(BasicFFMpegAvdeviceFragment.NAME_TAG)
        if(fragment != null)
            fragment.onRequestPermissionsResult(requestCode, permissions, grantResults)
        else
            super.onRequestPermissionsResult(requestCode, permissions, grantResults)
    }


    override fun onResume() {
        super.onResume()
        Log.d(TAG, "onResume")
    }


    override fun onPause() {
        super.onPause()
        Log.d(TAG, "onPause")
    }


    override fun onStart() {
        super.onStart()
        Log.d(TAG, "onStart")
    }


    override fun onStop() {
        super.onStop()
        Log.d(TAG, "onStop")
    }


    override fun onDestroy() {
        super.onDestroy()
        Log.d(TAG, "onDestroy")
    }
}
