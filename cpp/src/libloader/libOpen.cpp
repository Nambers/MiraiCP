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

#include "libOpen.h"


#if MIRAICP_WINDOWS
#include "utf8.h"
#include <cstdio>
#include "WindowsMin.h"
#define OPEN_LIBRARY(path) LoadLibrary(TEXT(path.c_str()))
#define CLOSE_LIBRARY(handle) FreeLibrary((HMODULE) handle)
#define GET_SYMBOL(handle, symbol) GetProcAddress((HINSTANCE) handle, symbol)
#else
#include <dlfcn.h>
#define OPEN_LIBRARY(path) dlopen(path.c_str(), RTLD_LAZY)
#define CLOSE_LIBRARY(handle) dlclose(handle)
#define GET_SYMBOL(handle, symbol) dlsym(handle, symbol)
#endif

namespace LibLoader::LoaderApi {
    plugin_handle libOpen(const std::string &path) {
        return OPEN_LIBRARY(path);
    }

    plugin_func_ptr libSymbolLookup(void *handle, const char *symbol) {
        return (plugin_func_ptr) GET_SYMBOL(handle, symbol);
    }

    int libClose(void *handle) {
        return CLOSE_LIBRARY(handle);
    }

    std::string libError() {
#if MIRAICP_WINDOWS
        // https://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
        //Returns the last Win32 error, in string format. Returns an empty string if there is no error.

        //Get the error message ID, if any.
        DWORD errorMessageID = ::GetLastError();
        if (errorMessageID == 0) {
            return {}; //No error message has been recorded
        }

        LPWSTR messageBuffer = nullptr;

        //Ask Win32 to give us the string version of that message ID.
        //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
        size_t size = FormatMessageW(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                     nullptr, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPWSTR) &messageBuffer, 0, nullptr);

        //Copy the error message into a std::string.
        std::wstring message(messageBuffer, size);

        //Free the Win32's string's buffer.
        LocalFree(messageBuffer);

        // Translate to utf8 encoded std::string
        std::string ans;
        utf8::utf16to8(message.begin(), message.end(), std::back_inserter(ans));

        return ans;
#else
        return dlerror();
#endif
    }
} // namespace LibLoader::LoaderApi
