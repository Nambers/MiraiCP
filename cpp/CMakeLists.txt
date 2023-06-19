cmake_minimum_required(VERSION 3.14)
project(MiraiCP_pro LANGUAGES CXX) # MSVC detecting must be after project declaration

#
set(CMAKE_CXX_STANDARD 17) # not working on MSVC
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(commonBuild INTERFACE)
# for sharing including path
add_library(SDKCommonInclude INTERFACE)
add_library(libLoaderCommonInclude INTERFACE)

if (MSVC)
    message("MSVC detected")
    # 以utf8编译, for msvc; EHa参数启用标准 C++ stack unwinding; 启用多线程编译；禁止无返回值的函数；禁止unknown-pragmas warning；禁用两个与dll export相关的warning
    target_compile_options(commonBuild INTERFACE /W4 /utf-8 /EHs /std:c++17 /MP /we4715 /wd4251 /wd4275 /wd4068 /Zc:__cplusplus)
    # Disable RTTI.
    # string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
else ()
    # 禁止无返回值的函数；禁止unknown-pragmas warning
    target_compile_options(commonBuild INTERFACE -Wall -Werror=return-type -Wno-unknown-pragmas)
    # Disable RTTI.
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    # hide symbol
    target_link_options(commonBuild INTERFACE -fvisibility=hidden)
endif ()

# optimize for Release build
if (CMAKE_BUILD_TYPE MATCHES ".*Rel.*")
    message("Release mode, enabling maximal optimization")
    if (MSVC)
        target_compile_options(commonBuild INTERFACE /Ox)
    else ()
        target_compile_options(commonBuild INTERFACE -O3)
    endif (MSVC)
endif (CMAKE_BUILD_TYPE MATCHES ".*Rel.*")

#
set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)
set(INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)
set(DEMO_PATH ${CMAKE_CURRENT_SOURCE_DIR}/demo)
set(THIRD_PARTY_PATH ${CMAKE_CURRENT_SOURCE_DIR}/3rd_include)
file(GLOB MIRAICP_COMMON_PATH ${SRC_PATH}/common/*.cpp)

option(GOOGLE_TEST "GOOGLE_TEST" ON)

#
target_compile_definitions(commonBuild INTERFACE JSON_HAS_CPP_17)

# checks
add_subdirectory(cmake/jni)
add_subdirectory(cmake/pthread)
add_subdirectory(cmake/static)
if (CMAKE_SYSTEM_NAME STREQUAL Android)
    message("Android build detected") # TODO(Antares)
endif ()

# -- share included paths --
# should be included after JNI check since ${3RD_PATH}

target_include_directories(SDKCommonInclude INTERFACE
        ${3RD_PATH}
        ${INCLUDE_PATH}/common
        ${SRC_PATH}/common
        ${INCLUDE_PATH}/sdk
        ${SRC_PATH}/sdk)
target_include_directories(libLoaderCommonInclude INTERFACE
        ${3RD_PATH}
        ${INCLUDE_PATH}/common
        ${SRC_PATH}/common
        ${JNI_PATH}
        ${SRC_PATH}/libloader)

# -- set command names according to platform --
if (WIN32)
    set(PYTHON_NAME python)
    set(DEL_COMMANDNAME del)
else ()
    set(PYTHON_NAME python3)
    set(DEL_COMMANDNAME rm)
endif ()

# -- delete outdated single headers task --
add_custom_target(
        deleteSingleHeaders
        COMMAND ${DEL_COMMANDNAME} MiraiCP.cpp
        COMMAND ${DEL_COMMANDNAME} MiraiCP.hpp
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/single_include
)
# -- end --

# -- gengerate single headers --
add_custom_target(
        generateSingleHeaders
        COMMAND ${PYTHON_NAME} buildScript/generate.py buildScript/config.json
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
# -- end --

# add targets
add_subdirectory(src)

# tests
if (GOOGLE_TEST)
    message(STATUS "loading test")
    add_subdirectory(tests)
endif ()
