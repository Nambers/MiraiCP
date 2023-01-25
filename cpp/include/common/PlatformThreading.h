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

#ifndef MIRAICP_PRO_PLATFORMTHREADING_H
#define MIRAICP_PRO_PLATFORMTHREADING_H

#include "MiraiCPMacros.h"
// -----------------------
// TODO(Antares): 删掉这些实现。改成不依赖pthread或msvc的的线程识别方案

#if MIRAICP_MSVC
#include <Windows.h>
#include <processthreadsapi.h>
#include <string>

inline bool checkPthreadLen(const char *ptr) {
    size_t count = 0;
    while ((*ptr) && count <= 80) {
        ptr++;
        ++count;
    }
    return count <= 80;
}

inline void *platform_thread_self() {
    return GetCurrentThread();
}

inline void platform_set_thread_name(void *platform_thread_self, const char *name) {
    size_t len = strlen(name);
    std::wstring wst(len + 1, '#');
    size_t num_convert;
    mbstowcs_s(&num_convert, &wst[0], len + 1, name, len + 1);
    SetThreadDescription(platform_thread_self, &wst[0]);
}

inline void platform_get_thread_name(void *platform_thread_self, char *buf, size_t bufsize) {
    wchar_t *wbuf;
    GetThreadDescription(platform_thread_self, &wbuf);
    size_t num_convert;
    wcstombs_s(&num_convert, buf, bufsize, wbuf, bufsize);
    LocalFree(wbuf);
}
#else


#if (MIRAICP_ANDROID || MIRAICP_TERMUX) && !defined(TERMUX_THREAD_SUPPORTED)

inline void platform_set_thread_name(decltype(pthread_self()) id, const char *name) {}
inline void platform_get_thread_name(decltype(pthread_self()) id, char *buf, size_t bufsize) {
    buf[0] = 0;
}

#else

#include "pthread.h"
#include <string>

#if (MIRAICP_ANDROID || MIRAICP_TERMUX) && defined(TERMUX_THREAD_SUPPORTED)

int pthread_setname_np(pthread_t pthread, const char *__name);
int pthread_getname_np(pthread_t pthread, char *buf, size_t n);

#endif

inline auto platform_thread_self() {
    return pthread_self();
}

inline bool checkPthreadLen(const char *ptr) {
    size_t count = 0;
    while ((*ptr) && count <= 15) {
        ptr++;
        ++count;
    }
    return count <= 15;
}

inline void platform_set_thread_name(decltype(platform_thread_self()) id, const char *name) {
    if (checkPthreadLen(name)) {
        pthread_setname_np(id, name);
    } else {
        std::string tNewName;
        tNewName.reserve(15);
        tNewName.append(std::string_view(name, 15));
        pthread_setname_np(id, tNewName.c_str());
    }
}

inline void platform_get_thread_name(decltype(platform_thread_self()) id, char *buf, size_t bufsize) {
    pthread_getname_np(id, buf, bufsize);
}
#endif
#endif


#endif //MIRAICP_PRO_PLATFORMTHREADING_H
