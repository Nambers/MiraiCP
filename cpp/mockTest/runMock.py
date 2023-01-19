#!/usr/bin/python3

import os
import subprocess

if __name__ == "__main__":
    cppPath = os.path.join(os.path.dirname(
        os.path.abspath(__file__)), os.path.pardir)
    cppFiles = os.listdir(cppPath)

    if "cmake-build-debug" in cppFiles and os.path.isdir(os.path.join(cppPath, "cmake-build-debug")):
        libPath = os.path.join(cppPath, "cmake-build-debug")
    elif "build" in cppFiles and os.path.isdir(os.path.join(cppPath, "build")):
        libPath = os.path.join(cppPath, "build")
    elif "cmake-build-release" in cppFiles and os.path.isdir(os.path.join(cppPath, "cmake-build-release")):
        libPath = os.path.join(cppPath, "cmake-build-release")
    else:
        print("error: no build folder")
        exit(1)

    kotlinPath = os.path.join(os.path.join(cppPath, os.path.pardir), "kotlin")
    env = os.environ.copy()
    env.update({"libpath": libPath})

    passed = True

    try:
        Event = subprocess.check_output("gradlew :shared:cleanJvmTest :shared:jvmTest --tests \"tech.eritquearcus.miraicp.shared.test.events.EventsTest\"",
                                        shell=True, cwd=kotlinPath, env=env, encoding='utf-8')
    except subprocess.CalledProcessError as e:
        passed = False
        print("Error: ")
        print(e.output)
    try:
        Op = subprocess.check_output("gradlew :shared:cleanJvmTest :shared:jvmTest --tests \"tech.eritquearcus.miraicp.shared.test.events.OperationTest\"",
                                     shell=True, cwd=kotlinPath, env=env, encoding='utf-8')
    except subprocess.CalledProcessError as e:
        passed = False
        print("Error: ")
        print(e.output)
    try:
        Msg = subprocess.check_output("gradlew :shared:cleanJvmTest :shared:jvmTest --tests \"tech.eritquearcus.miraicp.shared.test.events.MessageTest\"",
                                      shell=True, cwd=kotlinPath, env=env, encoding='utf-8')
    except subprocess.CalledProcessError as e:
        passed = False
        print("Error: ")
        print(e.output)

    if passed:
        print("All check passed.")
