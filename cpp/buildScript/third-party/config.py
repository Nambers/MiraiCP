#!/usr/bin/python3
"""
Filename: MiraiCP/cpp/buildScript/third-party/config.py
Path: MiraiCP/cpp/buildScript/third-party
Author: antares0982@gmail.com
Copyright (c) 2021 Antares
"""
# workpath: MiraiCP/cpp/src
import json
import os

d = dict()
d['project'] = "Mirai C++ SDK"
d['target'] = "single_include/MiraiCP/MiraiCP.hpp"
d['sources'] = list()
d['include_paths'] = ["include"]

fnames = os.listdir('include')
fnames.sort()

msg = "Files to be added:\n"

for filename in fnames:
    if filename.endswith('.h'):
        d['sources'].append("include/" + filename)
        msg += filename + "\n"

dirpath = os.path.dirname(os.path.abspath(__file__))

with open(os.path.join(dirpath, "config.json"), 'w', encoding='utf-8') as f:
    json.dump(d, f, ensure_ascii=False, indent=4)

print(msg)
