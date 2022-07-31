#!/usr/bin/python3

#  Copyright (c) 2021. Eritque arcus and contributors.
#
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU Affero General Public License as
#  published by the Free Software Foundation, either version 3 of the
#  License, or any later version(in your opinion).
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU Affero General Public License for more details.
#
#  You should have received a copy of the GNU Affero General Public License
#  along with this program.  If not, see <http://www.gnu.org/licenses/>.
#

import io
import os

ss = """// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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
#ifndef MIRAICP_HPP_H
#define MIRAICP_HPP_H\n\n
"""

fnames = os.listdir('include')
fnames.sort()

for filename in fnames:
    if filename.endswith('.h'):
        ss += "#include \"{}\"\n".format(filename)

fnames = os.listdir('common')
fnames.sort()

for filename in fnames:
    if filename.endswith('.h'):
        ss += f"#include \"{filename}\"\n"

ss += "\n#endif //MIRAICP_HPP_H"

with open('include/MiraiCP.hpp', 'w', encoding='utf-8') as f:
    f.write(ss)

print("header file generated successfully")
