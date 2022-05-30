//
// Created by antares on 5/25/22.
//

#ifndef MIRAICP_PRO_JNIENVS_H
#define MIRAICP_PRO_JNIENVS_H

#include <jni.h>


class LoaderLogger;


namespace JNIEnvs {
    extern jclass volatile Class_cpplib;
    extern LoaderLogger logger;

    // functions
    JNIEnv *getEnv();

    void setJNIVersion();

    void loadConfigClass();

    void loggerInit();

    void initializeMiraiCPLoader();
} // namespace JNIEnvs


#endif //MIRAICP_PRO_JNIENVS_H
