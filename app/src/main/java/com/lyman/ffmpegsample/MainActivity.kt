package com.lyman.ffmpegsample

import android.os.Bundle
import androidx.appcompat.app.AppCompatActivity
import androidx.fragment.app.Fragment
import com.lyman.ffmpegsample.fragment.BaseFragment
import com.lyman.ffmpegsample.fragment.BasicDataTypeFragment
import com.lyman.ffmpegsample.fragment.BasicFFMpegFragment
import com.lyman.ffmpegsample.fragment.MainFragment

class MainActivity : AppCompatActivity() {
    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)
        setContentView(R.layout.activity_main)
        showFragment(MainFragment.newInstance(), MainFragment.NAME_TAG)
    }

    fun showFragment(fragment: BaseFragment) {
        supportFragmentManager.beginTransaction().replace(R.id.content, fragment).addToBackStack(null).commit()
    }

    private fun showFragment(fragment: BaseFragment, tag: String) {
        supportFragmentManager.beginTransaction().replace(R.id.content, fragment, tag).addToBackStack(null).commit()
    }

    fun showFragment(type: BaseFragment.DATA_TYPE) {
        var fragment: Fragment? = null
        when(type){
            BaseFragment.DATA_TYPE.BASIC_DATA -> fragment = BasicDataTypeFragment()
            BaseFragment.DATA_TYPE.BASIC_FFMPEG -> fragment = BasicFFMpegFragment()
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
