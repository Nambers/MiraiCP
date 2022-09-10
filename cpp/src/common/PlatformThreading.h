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


#if MIRAICP_MSVC
#include <Windows.h>
#include <processthreadsapi.h>
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
#include <thread>

inline auto platform_thread_self() {
    return pthread_self();
}

inline void platform_set_thread_name(decltype(platform_thread_self()) id, const char *name) {
    pthread_setname_np(id, name);
}

inline void platform_get_thread_name(decltype(platform_thread_self()) id, char *buf, size_t bufsize) {
    pthread_getname_np(id, buf, bufsize);
}
#endif


#endif //MIRAICP_PRO_PLATFORMTHREADING_H
