package com.lyman.ffmpegsample.adapter

import android.view.LayoutInflater
import android.view.View
import android.view.ViewGroup
import android.widget.TextView
import androidx.recyclerview.widget.RecyclerView
import com.lyman.ffmpegsample.R
import com.lyman.ffmpegsample.fragment.BaseFragment

class MyRecyclerViewAdapter(private val datas: List<BaseFragment.DATA_TYPE>) : RecyclerView.Adapter<MyRecyclerViewAdapter.MyRecyclerViewHolder>() {
    interface ItemClickListener{
        fun onItemClick(view: View, position: Int)
    }
    inner class MyRecyclerViewHolder(val textView: TextView) : RecyclerView.ViewHolder(textView) {
        init {
            textView?.setOnClickListener {
                itemClickListener?.onItemClick(it, adapterPosition)
            }
        }
    }
    private lateinit var itemClickListener: ItemClickListener


    override fun onCreateViewHolder(parent: ViewGroup, viewType: Int): MyRecyclerViewHolder {
        val textView = LayoutInflater.from(parent.context).inflate(R.layout.item_recyclerview, parent, false) as TextView
        return MyRecyclerViewHolder(textView)
    }


    override fun getItemCount(): Int {
        return datas.size
    }


    override fun onBindViewHolder(holder: MyRecyclerViewHolder, position: Int) {
        holder.textView.text = datas[position].text
    }


    fun setOnClickListener(listener: ItemClickListener){
        this.itemClickListener = listener
    }


    fun getItem(id: Int) : BaseFragment.DATA_TYPE {
        return datas[id]
    }
}