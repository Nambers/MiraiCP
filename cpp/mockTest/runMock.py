#!/usr/bin/python3

'''
File: runMock.py
Project: mockTest
File Created: Friday, 19th January 2023
Author: Antares (antares0982@gmail.com)
-----
Copyright 2023 (c) Antares
'''

import os
import subprocess
import sys
from typing import List

codepage = 'utf-8' if sys.platform != 'win32' else 'cp936'
currentFilePath = os.path.dirname(os.path.abspath(__file__))
logfile = "mock_errlog.txt"
prefix = "bash " if sys.platform != 'win32' else ""


def writelog(e: subprocess.CalledProcessError):
    ot: str = e.stdout.decode(codepage, errors='ignore')
    with open(os.path.join(currentFilePath, logfile), 'a', encoding='utf-8') as f:
        f.write(ot)
        f.write('\n\n')


cfg_content = r"""
{
  "accounts": [],
  "cppPaths": [
    {
      "path": "{}/libMiraiCP_mock_test.so"
    }
  ]
}
"""


def runmock(name: str, kwargs: dict) -> bool:
    global prefix
    try:
        subprocess.check_output(
            f"{prefix}gradlew :shared:test --tests \"tech.eritquearcus.miraicp.shared.test.events.{name}\"",
            shell=True,
            stderr=subprocess.STDOUT,
            **kwargs
        )
        return True
    except subprocess.CalledProcessError as e:
        print(
            f"{name} failed with return code {e.returncode}. Log has been written.",
            file=sys.stderr
        )
        writelog(e)
        return False


def find_build_folder(_cppPath: str, _cppFiles: List[str]):
    if "cmake-build-debug" in _cppFiles and os.path.isdir(os.path.join(_cppPath, "cmake-build-debug")):
        return os.path.join(_cppPath, "cmake-build-debug")
    elif "build" in _cppFiles and os.path.isdir(os.path.join(_cppPath, "build")):
        return os.path.join(_cppPath, "build")
    elif "cmake-build-release" in _cppFiles and os.path.isdir(os.path.join(_cppPath, "cmake-build-release")):
        return os.path.join(_cppPath, "cmake-build-release")


def setup_lib_path(_lib_path):
    mock_cfg_dir = os.path.join(_lib_path, "testFileFromKt4Mock")
    if not os.path.exists(mock_cfg_dir):
        os.mkdir(mock_cfg_dir)
    cfg_file = os.path.join(mock_cfg_dir, "config.json")
    if not os.path.exists(cfg_file):
        with open(cfg_file, 'w', encoding='utf-8') as f:
            f.write(cfg_content.format(_lib_path))

    subprocess.check_output(
        f"cmake --build {_lib_path} --target MiraiCP_mock_test",
        shell=True,
        stderr=subprocess.STDOUT,
    )
    subprocess.check_output(
        f"cmake --build {_lib_path} --target Loader",
        shell=True,
        stderr=subprocess.STDOUT,
    )


if __name__ == "__main__":
    if os.path.exists(os.path.join(currentFilePath, logfile)):
        os.remove(os.path.join(currentFilePath, logfile))

    cppPath = os.path.join(currentFilePath, os.path.pardir)
    cppFiles = os.listdir(cppPath)

    libPath = find_build_folder(cppPath, cppFiles)
    if libPath is None:
        print("Error: no build folder detected", file=sys.stderr)
        exit(1)

    setup_lib_path(libPath)

    kotlinPath = os.path.join(os.path.join(cppPath, os.path.pardir), "kotlin")

    env = os.environ.copy()
    env.update({"libpath": libPath})

    testsName = ["EventsTest", "OperationTest", "MessageTest"]

    # taskPool = pool.Pool(processes=min(os.cpu_count(), len(testsName)))
    testResult = list(map(
        runmock,
        testsName,
        ({'cwd': kotlinPath, 'env': env} for _ in enumerate(testsName))
    ))
    passed = all(testResult)

    if passed:
        print("All check passed.")
    else:
        failed = sum(not x for x in testResult)
        print(f"{failed} tests failed.")
