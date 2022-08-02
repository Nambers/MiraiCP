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

#include "LoaderExceptions.h"
#include "commonTools.h"


namespace LibLoader {
    LoaderBaseException::LoaderBaseException(string info, string _filename, int _lineNum)
        : re(std::move(info)),
          filename(std::move(_filename)),
          lineNum(_lineNum) {}

    PluginLoadException::PluginLoadException(const string &_pluginPath, string _filename, int _lineNum)
        : LoaderExceptionCRTP(_pluginPath + " 加载失败", std::move(_filename), _lineNum) {}

    SymbolResolveException::SymbolResolveException(const string &_pluginPath, SymbolType Type, string _filename, int _lineNum)
        : LoaderExceptionCRTP(_pluginPath + " 寻找符号失败，类型：" + SymbolTypeToString(Type), std::move(_filename), _lineNum) {}

    LoaderBaseException::string SymbolResolveException::SymbolTypeToString(SymbolType Type) {
        switch (Type) {
            case Entrance:
                return "插件入口";
            case Exit:
                return "插件出口";
            case Event:
                return "事件函数";
            case Config:
                return "插件配置函数";
            default:
                throw LoaderException("无法到达的代码", MIRAICP_EXCEPTION_WHERE);
        }
    }
#ifdef WIN32
#include <windows.h>
    static EventHandlerPitch pitch = EventHandlerPitch();
    EventHandlerPitch::EventHandlerPitch() {
        preHandler = SetUnhandledExceptionFilter(EventHandlerPitch::eventHandler);
    }
    EventHandlerPitch::~EventHandlerPitch() {
        SetUnhandledExceptionFilter(preHandler);
    }
    long EventHandlerPitch::eventHandler(PEXCEPTION_POINTERS pExceptionPointers) {
        // todo(ea): add loader task and handle exceptionCode
        pExceptionPointers->ExceptionRecord->ExceptionCode;
        TerminateThread(GetCurrentThread(), 1);
        return EXCEPTION_CONTINUE_EXECUTION;
    }
#endif
} // namespace LibLoader
