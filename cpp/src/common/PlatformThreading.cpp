// Copyright (c) 2023. Eritque arcus and contributors.
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

#include "MiraiCPMacros.h"
// -----------------------

#if MIRAICP_MSVC

#include "WindowsMin.h"
#include <processthreadsapi.h>
#include <string>


bool checkPthreadLen(const char *ptr) {
    size_t count = 0;
    while ((*ptr) && count <= 80) {
        ptr++;
        ++count;
    }
    return count <= 80;
}

void platform_set_thread_name(const char *name) {
    size_t len = strlen(name);
    std::wstring wst(len + 1, '#');
    size_t num_convert;
    mbstowcs_s(&num_convert, &wst[0], len + 1, name, len + 1);
    SetThreadDescription(GetCurrentThread(), &wst[0]);
}

void platform_get_thread_name(char *buf, size_t bufsize) {
    wchar_t *wbuf;
    GetThreadDescription(GetCurrentThread(), &wbuf);
    size_t num_convert;
    wcstombs_s(&num_convert, buf, bufsize, wbuf, bufsize);
    LocalFree(wbuf);
}

#else

#include <string>
#include <thread>

bool checkPthreadLen(const char *ptr) {
    size_t count = 0;
    while ((*ptr) && count <= 15) {
        ptr++;
        ++count;
    }
    return count <= 15;
}

void platform_set_thread_name(const char *name) {
    if (checkPthreadLen(name)) {
        pthread_setname_np(pthread_self(), name);
    } else {
        std::string tNewName;
        tNewName.reserve(15);
        tNewName.append(std::string_view(name, 15));
        pthread_setname_np(pthread_self(), tNewName.c_str());
    }
}

void platform_get_thread_name(char *buf, size_t bufsize) {
    pthread_getname_np(pthread_self(), buf, bufsize);
}

#endif