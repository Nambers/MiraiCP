//
// Created by 60168 on 2022-10-29.
//

#ifndef MIRAICP_PRO_LOADERMACRO_H
#define MIRAICP_PRO_LOADERMACRO_H

#ifdef LOADER_NATIVE
#define JRETURNTYPE void
#define JRETURNVAL
#define JSTRING const char *
#define J_TO_STD_STRING std::string
#define SET_ENV(x)
#define LOADER_EXPORT
#define DECL_EXPORT_NAME(x) x##Impl
#define J_ARGS
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
#endif

#endif //MIRAICP_PRO_LOADERMACRO_H
