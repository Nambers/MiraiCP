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

#ifndef MIRAICP_PRO_KTINTERFACE_H
#define MIRAICP_PRO_KTINTERFACE_H


#include "LoaderMacro.h"


// EXPORT functions
LOADER_EXPORT JRETURNTYPE DECL_EXPORT_NAME(Verify)(J_ARGS, JSTRING, JSTRING);

LOADER_EXPORT JRETURNTYPE DECL_EXPORT_NAME(Event)(J_ARGS, JSTRING);

LOADER_EXPORT JRETURNTYPE DECL_EXPORT_NAME(PluginDisable)(J_ARGS);

#ifdef LOADER_NATIVE
namespace LoaderAPIs {
    using LogFunc = void (*)(const char *, int);
    using OperFunc = const char *(*)(const char *);
    extern LogFunc log;
    extern OperFunc oper;
};
#else
int registerMethods(JNIEnv *env, const char *className, const JNINativeMethod *gMethods, int numMethods);

const JNINativeMethod method_table[]{
        {(char *) "Verify", (char *) "(Ljava/lang/String;Ljava/lang/String;)Ljava/lang/Void;", (jstring *) Verify},
        {(char *) "Event", (char *) "(Ljava/lang/String;)Ljava/lang/Void;", (jstring *) Event},
        {(char *) "PluginDisable", (char *) "()Ljava/lang/Void;", (jobject *) PluginDisable}};
#endif

#endif //MIRAICP_PRO_KTINTERFACE_H
