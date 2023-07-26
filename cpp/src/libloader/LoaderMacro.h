//
// Created by 60168 on 2022-10-29.
//

#ifndef MIRAICP_PRO_LOADERMACRO_H
#define MIRAICP_PRO_LOADERMACRO_H

#include "MiraiCPMacros.h"
// -----------------------


#include <jni.h>
#define JSTRING jstring
#define J_TO_STD_STRING LibLoader::jstring2str
#define SET_ENV(x) JNIEnvManager::setEnv(x)
#define J_ARGS JNIEnv *, jobject

#if MIRAICP_TERMUX || MIRAICP_ANDROID
#define MIRAICP_JNIPPTR
#define MIRAICP_JVER 0x00010008
#else
#define MIRAICP_JNIPPTR (void **)
#define MIRAICP_JVER JNI_VERSION_1_8
#endif

#endif //MIRAICP_PRO_LOADERMACRO_H
