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
#include "miraicp-core/core.h"
using namespace MiraiCP;
int main() {
    Core::loadCore(R"(D:\jdk\jdk-17.0.1\bin\server\jvm.dll)", R"(D:\Git\mirai\MiraiCP\kotlin\core\build\libs\MiraiCP-core-2.11.0-M1-all.jar)");
    return 0;
}