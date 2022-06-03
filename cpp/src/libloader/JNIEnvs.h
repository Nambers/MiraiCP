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

    void initializeMiraiCPLoader();
} // namespace JNIEnvs


#endif //MIRAICP_PRO_JNIENVS_H
