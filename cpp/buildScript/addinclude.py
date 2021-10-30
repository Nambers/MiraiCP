#!/usr/bin/python3
"""
Filename: MiraiCP/cpp/include/addinclude.py
Path: MiraiCP/cpp/include
Author: antares0982@gmail.com
Copyright (c) 2021 Antares
"""

import os

ss = "#ifndef MIRAICP_HPP_H\n#define MIRAICP_HPP_H\n\n"

fnames = os.listdir('include')
fnames.sort()

for filename in fnames:
    if filename.endswith('.h'):
        ss += f"#include \"{filename}\"\n"

ss += "\n#endif //MIRAICP_HPP_H"

with open('include/miraiCP.hpp', 'w', encoding='utf-8') as f:
    f.write(ss)

print("header file generated successfully")
