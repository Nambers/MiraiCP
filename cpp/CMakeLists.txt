add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
cmake_minimum_required(VERSION 3.19)
project(cpp)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_USE_64BITS on)

include_directories(.)
include_directories(include)
include_directories(include/win32)
include_directories(include/win32/bridge)
include_directories("include/utf8")

set(
        SOURCE_TEMPLATES
)

set(
        SOURCE_FILES
        dllmain.cpp
        pch.cpp
        Procession.cpp
        tools.cpp
)
set(
        SOURCE_HEADERS
        include/utf8.h
        include/utf8/checked.h
        include/utf8/core.h
        include/utf8/cpp11.h
        include/utf8/cpp17.h
        include/utf8/unchecked.h
        include/win32/bridge/AccessBridgeCallbacks.h
        include/win32/bridge/AccessBridgeCalls.h
        include/win32/bridge/AccessBridgePackages.h
        include/win32/jawt_md.h
        include/win32/jni_md.h
        include/classfile_constants.h
        include/jawt.h
        include/jdwpTransport.h
        include/jni.h
        include/jvmti.h
        include/jvmticmlr.h
        constants.h
        framework.h
        pch.h
        tech_eritquearcus_miraicp_shared_CPP_lib.h
        tools.h
)
add_library(
        cpp
        SHARED
        ${SOURCE_FILES}
        ${SOURCE_TEMPLATES}
        ${SOURCE_HEADERS}
)

target_link_libraries(cpp)