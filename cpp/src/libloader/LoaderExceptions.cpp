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
        : errorMessage(std::move(info)),
          filename(std::move(_filename)),
          lineNum(_lineNum) {}

    void LoaderBaseException::raise() const noexcept {
        LibLoader::logger.error(errorMessage);
    }

    void LoaderBaseException::warningRaise() const noexcept {
        LibLoader::logger.warning(errorMessage);
    }

    PluginLoadException::PluginLoadException(const string &_pluginPath, string _filename, int _lineNum)
        : LoaderExceptionCRTP(_pluginPath + " 加载失败", std::move(_filename), _lineNum) {}

    PluginAlreadyLoadedException::PluginAlreadyLoadedException(const LoaderBaseException::string &id, LoaderBaseException::string _filename, int _lineNum)
        : LoaderExceptionCRTP("尝试加载已经被加载的插件：" + id, std::move(_filename), _lineNum) {}

    void PluginAlreadyLoadedException::raise() const noexcept {
        warningRaise();
    }

    PluginAlreadyEnabledException::PluginAlreadyEnabledException(const LoaderBaseException::string &id, LoaderBaseException::string _filename, int _lineNum)
        : LoaderExceptionCRTP("尝试启用已经启用的插件：" + id, std::move(_filename), _lineNum) {}

    void PluginAlreadyEnabledException::raise() const noexcept {
        warningRaise();
    }

    PluginNotLoadedException::PluginNotLoadedException(const LoaderBaseException::string &path,
                                                       LoaderBaseException::string _filename, int _lineNum)
            : LoaderExceptionCRTP("尝试启用未加载的插件，路径：" + path, std::move(_filename), _lineNum) {}

    PluginIdDuplicateException::PluginIdDuplicateException(const string &id, const string &conflictPluginPathOld,
                                                           const string &conflictPluginPathNew, string _filename,
                                                           int _lineNum)
            : LoaderExceptionCRTP(
            "插件ID " + id + " 重复：已经被位于 " + conflictPluginPathOld + " 的插件使用；位于 " + conflictPluginPathNew +
            " 的插件将取消加载。请修改插件id后重新编译", std::move(_filename), _lineNum) {}

    IllegalStateException::IllegalStateException(LibLoader::LoaderBaseException::string _filename, int _lineNum)
            : LoaderExceptionCRTP("到达未预料的位置(filename=" + _filename + ", num=" + std::to_string(_lineNum) + ")",
                                  std::move(_filename), std::move(_lineNum)) {}
} // namespace LibLoader
