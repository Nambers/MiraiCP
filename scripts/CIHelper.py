# -*- utf-8 -*-
# author: eritque-arcus
# date: 08/21/23

import sys, json, re

def genMatrix(msg: str):
    global winOS, linuxOS
    assert(len(msg) > 0)
    matrix = {
        "os":[winOS, linuxOS], 
        "include":[
            {"os": winOS, "targetName": "MSVCX64"},
            {"os": linuxOS, "targetName": "LinuxX64"}
            ]
        }
    if msg.endswith("noqa"):
        matrix = {"os": []}
    else:
        result = re.findall(R"noqa\(([a-z,]+)\)$", msg)
        if len(result) > 0:
            OSs = result[0].split(',')
            for OS in OSs:
                if OS == "win" or OS == "windows":
                    matrix["os"].remove(winOS)
                    matrix["include"] = [item for item in matrix["include"] if item["os"] != winOS]
                elif OS == "linux":
                    matrix["os"].remove(linuxOS)
                    matrix["include"] = [item for item in matrix["include"] if item["os"] != linuxOS]
                else:
                    print("Error: Can't find specific noqa OS name:" + OS +"!!!!", file=sys.stderr)
    print("matrix=" + json.dumps(matrix))

if __name__ == "__main__":
    winOS = "windows-2019"
    linuxOS = "ubuntu-20.04"
    args = sys.argv
    assert(len(args) >= 2)
    cmd = args[1]
    if cmd == "genMatrix":
        assert(len(args) == 3)
        genMatrix(args[2])
    else:
        print("cmd not match!", file=sys.stderr)
        exit(1)