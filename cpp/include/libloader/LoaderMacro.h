//
// Created by 60168 on 2022-10-29.
//

#ifndef MIRAICP_PRO_LOADERMACRO_H
#define MIRAICP_PRO_LOADERMACRO_H

#include "MiraiCPMacros.h"
// -----------------------


#ifdef LOADER_NATIVE
#define JRETURNTYPE void
#define JRETURNVAL
#define JSTRING const char *
#define J_TO_STD_STRING std::string
#define SET_ENV(x)
#define LOADER_EXPORT
#define DECL_EXPORT_NAME(x) x##Impl
#define J_ARGS
#define J_ARGS_AND_STRING JSTRING
#else
#include <jni.h>
#define JRETURNTYPE jobject
#define JRETURNVAL nullptr
#define JSTRING jstring
#define J_TO_STD_STRING LibLoader::jstring2str
#define SET_ENV(x) JNIEnvManager::setEnv(x)
#define LOADER_EXPORT JNIEXPORT
#define DECL_EXPORT_NAME(x) x
#define J_ARGS JNIEnv *, jobject
#define J_ARGS_AND_STRING JNIEnv *, jobject, JSTRING
#endif

#if MIRAICP_TERMUX || MIRAICP_ANDROID
#define JNIPPTR
#define MIRAICP_JVER 0x00010008
#else
#define MIRAICP_JNIPPTR (void **)
#define MIRAICP_JVER JNI_VERSION_1_8
#endif

#endif //MIRAICP_PRO_LOADERMACRO_H
