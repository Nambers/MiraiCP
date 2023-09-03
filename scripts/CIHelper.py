#!/usr/bin/env python3
'''
File: scripts/CIHelper.py
Project: MiraiCP
File Created: Monday, 21st August 2023
Author: eritque-arcus (
Modified: Antares (antares0982@gmail.com)
-----
Copyright 2023 (c) eritque-arcus and Antares
'''

import re
import sys
import os
from typing import Dict, List, Tuple


class SupportedSkips:
    skip_info: List[Tuple[str, List[str]]] = [
        ("WINDOWS", ["win", "windows"]), ("LINUX", ["linux"])]
    flags: Dict[str, int] = dict()
    match_dict: Dict[str, int] = dict()

    @classmethod
    def gen_flags(cls):
        flag = 1
        for attr in cls.skip_info:
            cls.flags[attr[0]] = flag
            short_names = attr[1]
            for short_name in short_names:
                cls.match_dict[short_name] = flag
            flag <<= 1

    @classmethod
    def get_all_flag(cls):
        return sum(cls.flags.values())


def parse_to_workflow(msg: str) -> None:
    github_env_file = os.environ.get("GITHUB_ENV")
    if github_env_file is None:
        print(msg)
    else:
        with open(github_env_file, "a") as f:
            f.write(msg + "\n")


def skip_env(msg: str) -> None:
    SupportedSkips.gen_flags()
    #
    msg = msg.strip()
    #
    flag = 0
    if msg.endswith("noqa"):
        flag = SupportedSkips.get_all_flag()
    else:
        _rep = ".*noqa\((.*?)\)$"
        match = re.match(_rep, msg)
        if match is not None:
            rep = match.group(1)
            if rep:
                platforms: List[str] = rep.split(',')
                for platform in platforms:
                    platform = platform.strip().lower()
                    flag |= SupportedSkips.match_dict.get(platform, 0)
    #
    for print_name, _ in SupportedSkips.skip_info:
        if flag & SupportedSkips.flags[print_name]:
            parse_to_workflow(f"SKIP_{print_name}=true")


if __name__ == "__main__":
    args = sys.argv
    assert len(args) >= 2, "args length must >= 2"
    cmd = args[1]
    if cmd == "skip_env":
        args_str = ' '.join(args[2:])
        skip_env(args_str)
    else:
        print("cmd not match!", file=sys.stderr)
        exit(1)
