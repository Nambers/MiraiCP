// Copyright (c) 2022 - 2022. Antares, Eritque arcus and contributors.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or any later version(in your opinion).
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//

#ifndef LOADER_NATIVE

#include "JNIEnvs.h"
#include "LoaderLogger.h"


namespace LibLoader::JNIEnvs {
    // globals
    volatile jclass Class_cpplib = nullptr;
    volatile jmethodID koper = nullptr;
    // functions
    void loadConfigClass() {
        auto env = getEnv();
        Class_cpplib = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/shared/CPPLibMultiplatform")));
        koper = env->GetStaticMethodID(Class_cpplib, "KOperation", "(Ljava/lang/String;)Ljava/lang/String;");
    }

    void initializeMiraiCPLoader() {
        loadConfigClass();
        logger.init();
    }

    JNIEnv *getEnv() {
        return JNIEnvManager::getEnv();
    }
} // namespace LibLoader::JNIEnvs

#endif //LOADER_NATIVE