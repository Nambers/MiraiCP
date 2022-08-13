// Copyright (c) 2022. Eritque arcus and contributors.
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

#ifndef MIRAICP_PRO_ENVMOCKER_H
#define MIRAICP_PRO_ENVMOCKER_H


#include "JNIEnvManager.h"


namespace EnvMocker {
    // forward declare the function for tests
    void force_reset_env();
} // namespace EnvMocker


// mock env in a local scope
class EnvScopeMocker {
    JNIEnv env;
    struct JNINativeInterface_ functions;

public:
    EnvScopeMocker() {
        constructEnv();
        JNIEnvManager::setEnv(&env);
    }

    ~EnvScopeMocker() {
        EnvMocker::force_reset_env();
    }

    void constructEnv() {
        env.functions = &functions;
        functions.CallStaticVoidMethodV = &EnvScopeMocker::callStaticVoidMethodV;
        functions.NewString = &EnvScopeMocker::NewString;
        // add more functions here if needed
    }

    static void callStaticVoidMethodV(JNIEnv *env, jclass cls, jmethodID methodID, va_list args) {}

    static jstring NewString(JNIEnv *env, const jchar *unicode, jsize len) { return nullptr; }

    // add more functions here if needed
};


#endif //MIRAICP_PRO_ENVMOCKER_H
