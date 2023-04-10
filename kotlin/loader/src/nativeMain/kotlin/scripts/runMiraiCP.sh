#!/bin/bash
#
# Copyright (c) 2020 - 2023. Eritque arcus and contributors.
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU Affero General Public License as
# published by the Free Software Foundation, either version 3 of the
# License, or any later version(in your opinion).
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Affero General Public License for more details.
#
# You should have received a copy of the GNU Affero General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.
#
#

executable="./MiraiCP-loader-v2.15.0-RC.kexe"
grep_re=$(LD_LIBRARY_PATH=./ ldd $executable | grep "not found")
if [ "$grep_re" != "" ]; then
  echo "检测到以下依赖缺失"
  echo "Missing dependencies!"
  echo "$grep_re"
else
  LD_LIBRARY_PATH=./ $executable
fi