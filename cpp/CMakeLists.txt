cmake_minimum_required(VERSION 3.10)

# 以utf8编译, for msvc
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")

# fix msvc `message : 函数是 extern "C" 并且指定了 /EHc`
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/EHa>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/EHa>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/std:c++17>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/MP>")

#
set(CMAKE_CXX_STANDARD 17) # not working on MSVC
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(MiraiCP_pro LANGUAGES CXX)

#
set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
file(GLOB MIRAICP_CPP_PATH ${SRC_PATH}/include/*.cpp)
file(GLOB MIRAICP_COMMON_PATH ${SRC_PATH}/common/*.cpp)
file(GLOB LOADER_PATH ${SRC_PATH}/libloader/*.cpp)
option(GOOGLE_TEST "GOOGLE_TEST" ON)

# -- automatic add include --
if (WIN32)
    execute_process(
            COMMAND python ../buildScript/addinclude.py
            WORKING_DIRECTORY ${SRC_PATH}
    )
endif (WIN32)
if (UNIX)
    execute_process(
            COMMAND python3 ../buildScript/addinclude.py
            WORKING_DIRECTORY ${SRC_PATH}
    )
endif (UNIX)
# -- end --

# -- jni required --
set(JAVA_AWT_LIBRARY NotNeeded)
# set(JAVA_JVM_LIBRARY NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)
find_package(JNI REQUIRED)
if (JNI_FOUND)
    message(STATUS "FOUND JNI IN ${JAVA_INCLUDE_PATH}")
    set(JNI_PATH
            ${JAVA_INCLUDE_PATH}
            ${JAVA_INCLUDE_PATH2}
            )
    set(3RD_PATH
            ${SRC_PATH}/3rd_include/json
            ${SRC_PATH}/3rd_include/json/nlohmann
            ${SRC_PATH}/3rd_include/utf8
            ${SRC_PATH}/3rd_include/utf8/utf8
            )
else ()
    message(STATUS "Cannot find JNI")
endif ()
# -- end --

# -- delete outdated single headers task --
if (WIN32)
    # must use `\\`
    add_custom_target(
            deleteSingleHeaders
            COMMAND del single_include\\MiraiCP\\MiraiCP.cpp
            COMMAND del single_include\\MiraiCP\\MiraiCP.hpp
            WORKING_DIRECTORY ${SRC_PATH}
    )
endif (WIN32)
if (UNIX)
    add_custom_target(
            deleteSingleHeaders
            COMMAND rm single_include/MiraiCP/MiraiCP.cpp
            COMMAND rm single_include/MiraiCP/MiraiCP.hpp
            WORKING_DIRECTORY ${SRC_PATH}
    )
endif (UNIX)
# -- end --

# -- gengerate single headers --
if (WIN32)
    add_custom_target(
            generateSingleHeaders
            COMMAND python ../buildScript/addinclude.py
            COMMAND python ../buildScript/third-party/config.py
            COMMAND python ../buildScript/third-party/amalgamate.py -c ../buildScript/third-party/config.json -p ../buildScript/third-party/prologue -s . --verbose=yes
            COMMAND python ../buildScript/combineSource.py
            WORKING_DIRECTORY ${SRC_PATH}
    )
endif (WIN32)
if (UNIX)
    add_custom_target(
            generateSingleHeaders
            COMMAND python3 ../buildScript/addinclude.py
            COMMAND python3 ../buildScript/third-party/config.py
            COMMAND python3 ../buildScript/third-party/amalgamate.py -c ../buildScript/third-party/config.json -p ../buildScript/third-party/prologue -s . --verbose=yes
            COMMAND python3 ../buildScript/combineSource.py
            WORKING_DIRECTORY ${SRC_PATH}
    )
endif (UNIX)
# -- end --

add_subdirectory(src/include)
add_subdirectory(src/miraicp-core)
add_subdirectory(src/libloader)
if (GOOGLE_TEST)
    message(STATUS "loading test")
    add_subdirectory(src/tests)
endif ()
