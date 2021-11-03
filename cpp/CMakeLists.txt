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

execute_process(
        COMMAND python buildScript/addinclude.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

# 构建动态链接库, 第一行是target name不能改, 第二行是表明为动态链接库，也不用改，demo/demo.cpp是处理的cpp，可以改名字，然后要添加你依赖的文件进去。
file(GLOB MIRAI_CPP_PATH ./include/*.cpp)
add_library(
        MiraiCP_multi
        SHARED
        demo/test.cpp
        demo/test2.cpp
        ${MIRAI_CPP_PATH}
)

# 设置输出的dll文件的名字(最后一个参数)，目前是MiraiCP.dll
set_target_properties(MiraiCP_multi PROPERTIES OUTPUT_NAME MiraiCP)
target_include_directories(MiraiCP_multi PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

#----测试相关---
 add_library(
         MiraiCP_single
         SHARED
         demo/test.cpp
         demo/test2.cpp
 )
 set_target_properties(MiraiCP_single PROPERTIES OUTPUT_NAME MiraiCP)
target_include_directories(MiraiCP_single PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/single_include/MiraiCP)

find_package(JNI REQUIRED)
if (JNI_FOUND)
    message (STATUS "FOUND JNI IN ${JAVA_INCLUDE_PATH}")
    set(SOURCE_PATH
            ${CMAKE_CURRENT_SOURCE_DIR}/3rd_include/
            ${CMAKE_CURRENT_SOURCE_DIR}/3rd_include/utf8
            ${CMAKE_CURRENT_SOURCE_DIR}/3rd_include/utf8/utf8
            ${JAVA_INCLUDE_PATH}/
            ${JAVA_INCLUDE_PATH2}/
            )
    target_include_directories(MiraiCP_multi PUBLIC ${SOURCE_PATH})
    target_include_directories(MiraiCP_single PUBLIC ${SOURCE_PATH})
else()
    message(STATUS "Cannot find JNI")
endif()
target_sources(MiraiCP_single PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/single_include/MiraiCP/MiraiCP.cpp)

add_custom_target(
        generateSingleHeader
        COMMAND python buildScript/addinclude.py
        COMMAND python buildScript/third-party/config.py
        COMMAND python buildScript/third-party/amalgamate.py -c buildScript/third-party/config.json -p buildScript/third-party/prologue -s . --verbose=yes
        COMMAND python buildScript/combineSource.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
