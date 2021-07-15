# 以utf8编译
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
# 最低cmake版本
cmake_minimum_required(VERSION 3.5)
# 声明C++项目
project(MiraiCP LANGUAGES CXX)
# C++ 标准20
set(CMAKE_CXX_STANDARD 20)
# 在64位编译
set(BUILD_USE_64BITS on)
# 增加MiraiCP依赖
add_subdirectory(include)
# 提取MiraiCP源代码文件
get_property(SOURCE_FILES GLOBAL PROPERTY "MIRAICP_SOURCE_FILES")
get_property(SOURCE_HEADERS GLOBAL PROPERTY "MIRAICP_SOURCE_HEADERS")
get_property(SOURCE_PATH GLOBAL PROPERTY "MIRAICP_SOURCE_PATH")
# 引入目录依赖
include_directories(${SOURCE_PATH})
# 追加Procession.cpp也就是自定义的处理代码进编译名单(名字可自己改)
list(APPEND
        SOURCE_FILES
        demo.cpp
        )
# 构建动态链接库
add_library(
        MiraiCP
        SHARED
        ${SOURCE_FILES}
        ${SOURCE_HEADERS}
)
target_link_libraries(MiraiCP)