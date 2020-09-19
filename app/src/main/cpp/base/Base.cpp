//
// Created by Lyman on 2020-08-18.
//

#include "Base.h"

char* jstring2cchar(JNIEnv *env, jstring jStr){
    char* rtn = NULL;
    jclass clsstring = env->FindClass("java/lang/String");
    jstring strencode = env->NewStringUTF("utf-8");
    jmethodID mid = env->GetMethodID(clsstring, "getBytes", "(Ljava/lang/String;)[B");
    jbyteArray barr= (jbyteArray)env->CallObjectMethod(jStr, mid, strencode);
    jsize alen = env->GetArrayLength(barr);
    jbyte* ba = env->GetByteArrayElements(barr, JNI_FALSE);
    if (alen > 0)
    {
        rtn = (char*)malloc(alen + 1);
        memcpy(rtn, ba, alen);
        rtn[alen] = 0;
    }
    env->ReleaseByteArrayElements(barr, ba, 0);
    return rtn;
}


jbyte * jbyteArray2cbyte(JNIEnv *env, jbyteArray jArrays){
    jbyte * bytearray = env->GetByteArrayElements(jArrays, 0);
    env->ReleaseByteArrayElements(jArrays, bytearray, 0);
    return bytearray;
}