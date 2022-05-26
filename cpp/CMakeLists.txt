cmake_minimum_required(VERSION 3.10)
# 以utf8编译, for msvc
add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(MiraiCP_pro LANGUAGES CXX)
set(SRC_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src)

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
    set(SOURCE_PATH
            ${SRC_PATH}/3rd_include/
            ${SRC_PATH}/3rd_include/utf8
            ${SRC_PATH}/3rd_include/utf8/utf8
            ${JAVA_INCLUDE_PATH}
            ${JAVA_INCLUDE_PATH2}
            )
else ()
    message(STATUS "Cannot find JNI")
endif ()
if (WIN32)
    ADD_DEFINITIONS(-D_JVM_DLL_PATH=\"${JAVA_INCLUDE_PATH}/../bin/server/jvm.dll\")
else (WIN32)
    ADD_DEFINITIONS(-D_JVM_DLL_PATH=\"\")
endif (WIN32)
# -- end --

# -- miraicp multi --
file(GLOB MIRAICP_CPP_PATH ${CMAKE_CURRENT_SOURCE_DIR}/src/include/*.cpp)
add_library(
        MiraiCP_multi
        SHARED
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/test.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/test2.cpp
        ${MIRAICP_CPP_PATH}
)
set_target_properties(MiraiCP_multi PROPERTIES
        PREFIX lib
        OUTPUT_NAME MiraiCP_multi)
target_include_directories(MiraiCP_multi PUBLIC
        ${SRC_PATH}/include
        ${SOURCE_PATH}
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
        ${SOURCE_PATH}
        )
target_compile_options(MiraiCP_single PUBLIC -w)
# -- end --

# -- MiraiCP core --
add_executable(MiraiCP_core
        ${CMAKE_CURRENT_SOURCE_DIR}/demo/MiraiCP-core.cpp
        ${SRC_PATH}/miraicp-core/core.cpp
        ${MIRAICP_CPP_PATH}
        )
target_include_directories(MiraiCP_core PUBLIC
        ${SOURCE_PATH}
        ${SRC_PATH}/miraicp-core
        ${SRC_PATH}/include
        ${SRC_PATH}
        )
if (UNIX)
    target_link_libraries(MiraiCP_core ${JAVA_JVM_LIBRARY})
endif (UNIX)
# -- end --

# -- MiraiCP libloader --
file(GLOB LOADER_PATH ${SRC_PATH}/libloader/*.cpp)
add_library(loader
        SHARED
        ${LOADER_PATH}
        )
target_include_directories(loader PUBLIC
        ${SOURCE_PATH}
        ${SRC_PATH}/include
        ${SRC_PATH}
        )
if (UNIX)
    target_link_libraries(loader ${JAVA_JVM_LIBRARY} dl)
endif (UNIX)
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
