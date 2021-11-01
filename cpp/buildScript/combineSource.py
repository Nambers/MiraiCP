#!/usr/bin/python3
"""
Filename: MiraiCP/cpp/include/addinclude.py
Path: MiraiCP/cpp/include
Author: 1930893235@qq.com
Copyright (c) 2021 Eritque arcus
"""
import os
import re

ss = "#include <MiraiCP.hpp>\n"
for filename in sorted(os.listdir('include')):
    if filename.endswith('.cpp'):
        ss += "//from " + filename + "\n" + re.sub(r"#include \"(.*)\"", "",
                                                   open("include/" + filename, 'r', encoding="utf-8").read())
open('single_include/MiraiCP/miraiCP.cpp', 'w', encoding='utf-8').write(ss)
print("header file generated successfully")
