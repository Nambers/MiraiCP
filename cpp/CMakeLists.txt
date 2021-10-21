# 最低cmake版本
cmake_minimum_required(VERSION 3.10)
# 以utf8编译
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
# C++ 标准17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# 声明C++项目
project(MiraiCP_pro LANGUAGES CXX)
# 在64位编译
set(BUILD_USE_64BITS on)
# 构建动态链接库, 第一行是target name不能改, 第二行是表明为动态链接库，也不用改，demo.cpp是处理的cpp，可以改名字，然后要添加你依赖的文件进去。
add_library(
        MiraiCP
        SHARED
        include/miraiCP.cpp
        demo.cpp)
# 设置输出的dll文件的名字(最后一个参数)，目前是MiraiCP.dll
set_target_properties(MiraiCP PROPERTIES OUTPUT_NAME MiraiCP)
#----测试相关---
# 设置测试的target(不用可以不用管或者删除)
add_library(
        MiraiCP_Test
        SHARED
        include/miraiCP.cpp
        include/test/test.cpp
        include/test/test2.cpp)
set_target_properties(MiraiCP_Test PROPERTIES OUTPUT_NAME MiraiCP)
add_subdirectory(include/test)
#---测试相关---
# 增加MiraiCP依赖
add_subdirectory(include)
