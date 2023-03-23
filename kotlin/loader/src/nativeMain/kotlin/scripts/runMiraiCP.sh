#!/bin/bash
executable="./MiraiCP-loader-v2.14.0.kexe"
grep_re=$(LD_LIBRARY_PATH=./ ldd $executable | grep "not found")
if [ "$grep_re" != "" ]; then
  echo "检测到以下依赖缺失"
  echo "Missing dependencies!"
  echo "$grep_re"
else
  LD_LIBRARY_PATH=./ $executable
fi