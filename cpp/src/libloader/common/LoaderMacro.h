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
#else
#define JRETURNTYPE jobject
#define JRETURNVAL nullptr
#define JSTRING jstring
#define J_TO_STD_STRING LibLoader::jstring2str
#define SET_ENV(x) JNIEnvManager::setEnv(x)
#endif

#endif //MIRAICP_PRO_LOADERMACRO_H
