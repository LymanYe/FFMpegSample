package com.lyman.ffmpegsample

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import com.lyman.ffmpegsample.fragment.BaseFragment
import com.lyman.ffmpegsample.fragment.MainFragment
import com.lyman.ffmpegsample.fragment.basicdatatype.*
import com.lyman.ffmpegsample.fragment.basicffmpeg.BasicFFMpegAvcodecFragment
import com.lyman.ffmpegsample.fragment.basicffmpeg.BasicFFMpegFragment

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        showFragment(MainFragment.newInstance(), MainFragment.NAME_TAG)
    }


    private fun showFragment(fragment: BaseFragment, tag: String) {
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
}
