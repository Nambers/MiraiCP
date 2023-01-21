#!/usr/bin/python3
"""
Filename: MiraiCP/cpp/buildScript/addinclude.py
Path: MiraiCP/cpp/include
Author: 1930893235@qq.com
Copyright (c) 2021 Eritque arcus
"""
#  Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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

license = """// Copyright (c) 2020 - 2022. Eritque arcus and contributors.
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

ss = "#include <MiraiCP.hpp>\n"


def readin(filename: str):
    global ss
    with open(filename, 'r', encoding="utf-8") as reader:
        ss += "//from " + filename + "\n" \
            + re.sub(r"#include \"(.*)\"", "",
                     reader.read())


for filename in sorted(os.listdir('src/sdk')):
    if filename.endswith('.cpp'):
        readin("src/sdk/" + filename)
for filename in sorted(os.listdir('src/common')):
    if filename.endswith('.cpp'):
        readin("src/common/" + filename)


with open('single_include/MiraiCP.cpp', 'w', encoding='utf-8') as writer:
    writer.write("""// This file is generated automatically by buildScript;
    // When contributing to this repository, please DO NOT edit this file.
    """)
    writer.write(license)
    for a in ss.replace(license, "").split("\n"):
        if a != "":
            writer.write(a + "\n")


with open("single_include/MiraiCP.hpp", 'r', encoding='utf-8') as reader:
    text = reader.read()


with open("single_include/MiraiCP.hpp", 'w', encoding='utf-8') as writer:
    for a in text.replace(license, "").split("\n"):
        if a != "":
            writer.write(a + "\n")


print("header file generated successfully")
