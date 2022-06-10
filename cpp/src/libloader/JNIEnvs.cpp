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

#include "JNIEnvs.h"
#include "JNIEnvManager.h"
#include "LoaderLogger.h"


namespace LibLoader::JNIEnvs {
    // globals
    volatile jclass Class_cpplib = nullptr;

    void setJNIVersion() {
        JNIEnvManager::JNIVersion = getEnv()->GetVersion();
    }

    // functions
    void loadConfigClass() {
        auto env = getEnv();
        Class_cpplib = reinterpret_cast<jclass>(env->NewGlobalRef(env->FindClass("tech/eritquearcus/miraicp/shared/CPPLib")));
    }

    void initializeMiraiCPLoader() {
        loadConfigClass();
        logger.init();
    }

    JNIEnv *getEnv() {
        return JNIEnvManager::getEnv();
    }
} // namespace JNIEnvs
