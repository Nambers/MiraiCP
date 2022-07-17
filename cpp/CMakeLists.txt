cmake_minimum_required(VERSION 3.10)
# 以utf8编译, for msvc
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/EHa>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/EHa>")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(MiraiCP_pro LANGUAGES CXX)
set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

file(GLOB MIRAICP_CPP_PATH ${SRC_PATH}/include/*.cpp)
file(GLOB MIRAICP_COMMON_PATH ${SRC_PATH}/common/*.cpp)
file(GLOB LOADER_PATH ${SRC_PATH}/libloader/*.cpp)

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
#set(JAVA_JVM_LIBRARY NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)
find_package(JNI REQUIRED)
if (JNI_FOUND)
    message(STATUS "FOUND JNI IN ${JAVA_INCLUDE_PATH}")
    set(JNI_PATH
            ${JAVA_INCLUDE_PATH}
            ${JAVA_INCLUDE_PATH2}
            )
    set(3RD_PATH
            ${SRC_PATH}/3rd_include/
            ${SRC_PATH}/3rd_include/utf8
            ${SRC_PATH}/3rd_include/utf8/utf8
            )
else ()
    message(STATUS "Cannot find JNI")
endif ()
if (WIN32)
    # ADD_DEFINITIONS(-D_JVM_DLL_PATH=\"${JAVA_INCLUDE_PATH}/../bin/server/jvm.dll\")
else (WIN32)
    # ADD_DEFINITIONS(-D_JVM_DLL_PATH=\"\")
endif (WIN32)
# -- end --


# -- miraicp multi --
add_library(
        MiraiCP_multi
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/test2.cpp
        ${MIRAICP_CPP_PATH}
        ${MIRAICP_COMMON_PATH}
)
set_target_properties(MiraiCP_multi PROPERTIES
        PREFIX lib
        OUTPUT_NAME MiraiCP_multi)
target_include_directories(MiraiCP_multi PUBLIC
        ${SRC_PATH}/include
        ${SRC_PATH}/common
        ${3RD_PATH}
        )
target_compile_options(MiraiCP_multi PUBLIC -w)
# -- end --

# -- MiraiCP single --
if (NOT EXISTS "${SRC_PATH}/single_include/MiraiCP/MiraiCP.hpp" OR NOT EXISTS "${SRC_PATH}/single_include/MiraiCP/MiraiCP.cpp")
    FILE(WRITE "${SRC_PATH}/single_include/MiraiCP/MiraiCP.hpp" "")
    FILE(WRITE "${SRC_PATH}/single_include/MiraiCP/MiraiCP.cpp" "")
endif ()
add_library(
        MiraiCP_single
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/test2.cpp
        ${SRC_PATH}/single_include/MiraiCP/MiraiCP.cpp
)
add_dependencies(MiraiCP_single generateSingleHeaders)
set_target_properties(MiraiCP_single PROPERTIES
        PREFIX lib
        OUTPUT_NAME MiraiCP_single)
target_include_directories(MiraiCP_single PUBLIC
        ${SRC_PATH}/single_include/MiraiCP
        ${3RD_PATH}
        )
target_compile_options(MiraiCP_single PUBLIC -w)
# -- end --

# -- MiraiCP core --
add_executable(MiraiCP_core
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/MiraiCP-core.cpp
        ${SRC_PATH}/miraicp-core/core.cpp
        ${MIRAICP_CPP_PATH}
        ${MIRAICP_COMMON_PATH}
        )
target_include_directories(MiraiCP_core PUBLIC
        ${JNI_PATH}
        ${3RD_PATH}
        ${SRC_PATH}/miraicp-core
        ${SRC_PATH}/include
        ${SRC_PATH}/common
        ${SRC_PATH}
        )
target_compile_options(MiraiCP_core PUBLIC -DMIRAICP_CORE)
if (UNIX)
    #    target_link_libraries(MiraiCP_core ${JAVA_JVM_LIBRARY})
endif (UNIX)
# -- end --

# -- MiraiCP libLoader --
add_library(Loader
        SHARED
        ${LOADER_PATH}
        ${MIRAICP_COMMON_PATH}
        )
target_include_directories(Loader PUBLIC
        ${JNI_PATH}
        ${3RD_PATH}
        ${SRC_PATH}/include
        ${SRC_PATH}/common
        ${SRC_PATH}
        )
# https://stackoverflow.com/questions/45114078/get-address-for-symbol-in-current-process-in-windows
target_compile_options(Loader PUBLIC -DMIRAICP_LIB_LOADER)
if (UNIX)
    target_link_libraries(Loader  -static-libstdc++ -static-libgcc)
    target_compile_options(Loader PUBLIC -rdynamic)
endif (UNIX)
if (WIN32)
    target_link_libraries(Loader -static)
endif (WIN32)
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
