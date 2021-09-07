# 以utf8编译
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
# 最低cmake版本
cmake_minimum_required(VERSION 3.5)
# 声明C++项目
project(MiraiCP LANGUAGES CXX)
# C++ 标准20
set(CMAKE_CXX_STANDARD 17)
# 在64位编译
set(BUILD_USE_64BITS on)
# 构建动态链接库, 第一行是target name不能改, 第二行是表明为动态链接库，也不用改，demo.cpp是处理的cpp，可以改名字，然后要添加你依赖的文件进去。
add_library(
        MiraiCP
        SHARED
        demo.cpp
        )
# 设置输出的dll文件的名字(最后一个参数)，目前是MiraiCP.dll
set_target_properties(MiraiCP PROPERTIES OUTPUT_NAME MiraiCP)
# 增加MiraiCP依赖
add_subdirectory(include)
