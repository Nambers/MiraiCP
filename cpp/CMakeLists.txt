cmake_minimum_required(VERSION 3.14)
project(MiraiCP_pro LANGUAGES CXX) # MSVC detecting must be after project declaration

#
set(CMAKE_CXX_STANDARD 17) # not working on MSVC
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(commonBuild INTERFACE)

if (MSVC)
    message("MSVC detected")
    # 以utf8编译, for msvc; EHa参数启用标准 C++ stack unwinding; 启用多线程编译；禁止无返回值的函数；禁止unknown-pragmas warning；禁用两个与dll export相关的warning
    add_compile_options(/W4 /utf-8 /EHs /std:c++17 /MP /we4715 /wd4251 /wd4275 /wd4068 /Zc:__cplusplus)
    # Disable RTTI.
    # string(REGEX REPLACE "/GR" "" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /GR-")
else ()
    # 禁止无返回值的函数；禁止unknown-pragmas warning
    add_compile_options(-Wall -Werror=return-type -Wno-unknown-pragmas)
    # Disable RTTI.
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")
    # hide symbol
    add_link_options(-fvisibility=hidden)
endif ()

# optimize for Release build
if (CMAKE_BUILD_TYPE MATCHES ".*Rel.*")
    message("Release mode, enabling maximal optimization")
    if (MSVC)
        add_compile_options(/Ox)
    else ()
        add_compile_options(-O3)
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
add_compile_definitions(JSON_HAS_CPP_17)

# -- automatic add include --
if (WIN32)
    execute_process(
            COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/buildScript/addinclude.py
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif (WIN32)
if (UNIX)
    execute_process(
            COMMAND python3 ${CMAKE_CURRENT_SOURCE_DIR}/buildScript/addinclude.py
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
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
    if (CMAKE_SYSTEM_NAME STREQUAL Android)
        set(JNI_PATH ${JAVA_INCLUDE_PATH})
    else ()
        set(JNI_PATH
                ${JAVA_INCLUDE_PATH}
                ${JAVA_INCLUDE_PATH2}
                )
    endif ()
    set(3RD_PATH
            ${THIRD_PARTY_PATH}/json
            ${THIRD_PARTY_PATH}/json/nlohmann
            ${THIRD_PARTY_PATH}/utf8
            ${THIRD_PARTY_PATH}/utf8/utf8
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
            COMMAND del single_include\\MiraiCP.cpp
            COMMAND del single_include\\MiraiCP.hpp
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif (WIN32)
if (UNIX)
    add_custom_target(
            deleteSingleHeaders
            COMMAND rm single_include/MiraiCP.cpp
            COMMAND rm single_include/MiraiCP.hpp
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    )
endif (UNIX)
# -- end --

# -- gengerate single headers --
if (WIN32)
    set(PYTHON_NAME python)
else ()
    set(PYTHON_NAME python3)
endif ()
add_custom_target(
        generateSingleHeaders
        COMMAND ${PYTHON_NAME} buildScript/generate.py buildScript/config.json
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)
# -- end --

# checks
add_subdirectory(cmake/pthread)
if (CMAKE_SYSTEM_NAME STREQUAL Android)
    message("Android build detected") # TODO(Antares)
endif ()

add_subdirectory(src/sdk)
add_subdirectory(src/libloader)
if (GOOGLE_TEST)
    message(STATUS "loading test")
    add_subdirectory(tests)
endif ()
