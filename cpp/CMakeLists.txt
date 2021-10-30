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

#----单一header---
# 执行构建脚本，每次构建前请执行cmake，需要python环境
execute_process(
        COMMAND python include/third-party/config.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

execute_process(
        COMMAND python include/third-party/amalgamate.py -c include/third-party/config.json -p include/third-party/prologue -s . --verbose=yes
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

execute_process(
        COMMAND python buildScript/addinclude.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# 构建动态链接库, 第一行是target name不能改, 第二行是表明为动态链接库，也不用改，demo/demo.cpp是处理的cpp，可以改名字，然后要添加你依赖的文件进去。
add_library(
        MiraiCP
        SHARED
        single_include/MiraiCP/MiraiCP.hpp
        demo/demo.cpp
)

# 设置输出的dll文件的名字(最后一个参数)，目前是MiraiCP.dll
set_target_properties(MiraiCP PROPERTIES OUTPUT_NAME MiraiCP)

add_subdirectory(demo/include)
#----测试相关---
# 设置测试的target(不用可以不用管或者删除)
# add_library(
#        MiraiCP_Test
#        SHARED
#        demo/include/test/test.cpp
#        demo/include/test/test2.cpp
#        ${MIRAI_CPP_PATH}
# )
# set_target_properties(MiraiCP_Test PROPERTIES OUTPUT_NAME MiraiCP)
# add_subdirectory(demo/include/test)
#---测试相关---

# 增加MiraiCP依赖
# add_subdirectory(include)

#add_custom_target(
#        amalgamate
#        COMMAND python include/third-party/config.py
#        COMMAND python include/third-party/amalgamate.py -c include/third-party/config.json -s . --verbose=yes
#        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
#)
