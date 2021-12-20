#!/usr/bin/python3
"""
Filename: MiraiCP/cpp/buildScript/addinclude.py
Path: MiraiCP/cpp/include
Author: 1930893235@qq.com
Copyright (c) 2021 Eritque arcus
"""
#  Copyright (c) 2021-2021. Eritque arcus and contributors.
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

import os
import re

license = """
// Copyright (c) 2021-2021. Eritque arcus and contributors.
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
"""

ss = license + "\n#include <MiraiCP.hpp>\n"
for filename in sorted(os.listdir('include')):
    if filename.endswith('.cpp'):
        reader = open("include/" + filename, 'r', encoding="utf-8")
        ss += "//from " + filename + "\n" \
              + re.sub(r"#include \"(.*)\"", "",
                       reader.read())
        reader.close()
writer = open('single_include/MiraiCP/MiraiCP.cpp', 'w', encoding='utf-8')
for a in ss.replace(license, "").split("\n"):
    if not a == "":
        writer.write(a + "\n")
writer.close()
reader = open("single_include/MiraiCP/MiraiCP.hpp", 'r', encoding='utf-8')
text = reader.read()
reader.close()
writer = open("single_include/MiraiCP/MiraiCP.hpp", 'w', encoding='utf-8')
for a in text.replace(license, "").split("\n"):
    if not a == "":
        writer.write(a + "\n")
writer.close()
print("header file generated successfully")
