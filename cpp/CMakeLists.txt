add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
cmake_minimum_required(VERSION 3.19)
project(cpp)

set(CMAKE_CXX_STANDARD 20)
set(BUILD_USE_64BITS on)

include_directories(.)
include_directories(include)
include_directories(include/miraicp)
include_directories(include/jni)
include_directories(include/jni/win32)
include_directories(include/jni/win32/bridge)
include_directories(include/utf8)
include_directories(include/utf8/utf8)

set(
        SOURCE_FILES
        include/miraicp/dllmain.cpp
        Procession.cpp
)
set(
        SOURCE_HEADERS
        include/utf8/utf8.h
        include/utf8/utf8/checked.h
        include/utf8/utf8/core.h
        include/utf8/utf8/cpp11.h
        include/utf8/utf8/cpp17.h
        include/utf8/utf8/unchecked.h
        include/jni/win32/bridge/AccessBridgeCallbacks.h
        include/jni/win32/bridge/AccessBridgeCalls.h
        include/jni/win32/bridge/AccessBridgePackages.h
        include/jni/win32/jawt_md.h
        include/jni/win32/jni_md.h
        include/jni/classfile_constants.h
        include/jni/jawt.h
        include/jni/jdwpTransport.h
        include/jni/jni.h
        include/jni/jvmti.h
        include/jni/jvmticmlr.h
        include/json.hpp
        include/miraicp/tech_eritquearcus_miraicp_shared_CPP_lib.h
        include/miraiCP.hpp
)
add_library(
        cpp
        SHARED
        ${SOURCE_FILES}
        ${SOURCE_HEADERS}
)

target_link_libraries(cpp)