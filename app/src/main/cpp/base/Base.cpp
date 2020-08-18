//
// Created by Lyman on 2020-08-18.
//

#include "Base.h"

std::string jstring2string(JNIEnv *env, jstring jStr){
    const char *cstr = env->GetStringUTFChars(jStr, NULL);
    std::string str = std::string(cstr);
    env->ReleaseStringUTFChars(jStr, cstr);
    return str;
}