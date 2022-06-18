// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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

#include "CPPPlugin.h"

namespace MiraiCP {
    // 静态区代码一般不会使用logger，虽是ub，但应该不会造成影响；
    // 静态区强行调用的话，因为loggerInterface尚未传入，若logger尚未初始化，访问Logger::logger将是ub
    // 可以考虑把logger做成纯静态实现？以及最好做成 Logger * const
    Logger *CPPPlugin::pluginLogger = &Logger::logger;

    std::unique_ptr<CPPPlugin> CPPPlugin::plugin = nullptr;
} // namespace MiraiCP
