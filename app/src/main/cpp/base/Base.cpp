//
// Created by Lyman on 2020-08-18.
//

#include "Base.h"

char* jstring2cchar(JNIEnv *env, jstring jStr){
    char *cstr = const_cast<char *>(env->GetStringUTFChars(jStr, NULL));
    env->ReleaseStringUTFChars(jStr, cstr);
    return cstr;
}


jbyte * jbyteArray2cbyte(JNIEnv *env, jbyteArray jArrays){
    jbyte * bytearray = env->GetByteArrayElements(jArrays, 0);
    env->ReleaseByteArrayElements(jArrays, bytearray, 0);
    return bytearray;
}