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

codepage = 'utf-8' if sys.platform != 'win32' else 'cp936'
currentFilePath = os.path.dirname(os.path.abspath(__file__))
logfile = "mock_errlog.txt"
prefix = "bash " if sys.platform != 'win32' else ""

def writelog(e: subprocess.CalledProcessError):
    ot: str = e.stdout.decode(codepage, errors='ignore')
    with open(os.path.join(currentFilePath, logfile), 'a', encoding='utf-8') as f:
        f.write(ot)
        f.write('\n\n')


def runmock(name: str, kwargs: dict) -> bool:
    global kotlinPath, env
    try:
        subprocess.check_output(
            f"{prefix}gradlew :shared:test --tests \"tech.eritquearcus.miraicp.shared.test.events.{name}\"",
            shell=True,
            stderr=subprocess.PIPE,
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


if __name__ == "__main__":
    if os.path.exists(os.path.join(currentFilePath, logfile)):
        os.remove(os.path.join(currentFilePath, logfile))

    cppPath = os.path.join(currentFilePath, os.path.pardir)
    cppFiles = os.listdir(cppPath)

    if "cmake-build-debug" in cppFiles and os.path.isdir(os.path.join(cppPath, "cmake-build-debug")):
        libPath = os.path.join(cppPath, "cmake-build-debug")
    elif "build" in cppFiles and os.path.isdir(os.path.join(cppPath, "build")):
        libPath = os.path.join(cppPath, "build")
    elif "cmake-build-release" in cppFiles and os.path.isdir(os.path.join(cppPath, "cmake-build-release")):
        libPath = os.path.join(cppPath, "cmake-build-release")
    else:
        print("Error: no build folder detected", file=sys.stderr)
        exit(1)

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
