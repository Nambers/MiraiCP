# 以utf8编译
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
# 最低cmake版本
cmake_minimum_required(VERSION 3.20)
# 声明C++项目
project(MiraiCP LANGUAGES CXX)
# C++ 标准20
set(CMAKE_CXX_STANDARD 20)
# 在64位编译
set(BUILD_USE_64BITS on)
# 构建动态链接库, 第一行是生成的dll名字，demo.cpp是处理的cpp，可以改名字
add_library(
        MiraiCP
        SHARED
        demo.cpp
        )
# 增加MiraiCP依赖
add_subdirectory(include)