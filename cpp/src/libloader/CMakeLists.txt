#set(LOADER_JVM_INCLUDES_INTERNAL
#        ${JNI_PATH}
#        ${3RD_PATH}
#        ${SRC_PATH}/libloader/common
#        ${SRC_PATH}/libloader/jvm
#        ${SRC_PATH}/common)
#set(LOADER_JVM_INCLUDES_ALL ${LOADER_JVM_INCLUDES_INTERNAL} PARENT_SCOPE)
#set(LOADER_NATIVE_INCLUDES_INTERNAL
#        ${3RD_PATH}
#        ${SRC_PATH}/libloader/common
#        ${SRC_PATH}/libloader/native
#        ${SRC_PATH}/common)
#set(LOADER_JVM_INCLUDES_ALL ${LOADER_JVM_INCLUDES_INTERNAL} PARENT_SCOPE)
#file(GLOB LOADER_COMMON_PATH ${SRC_PATH}/libloader/common/*.cpp)
#file(GLOB LOADER_JVM_PATH ${SRC_PATH}/libloader/jvm/*.cpp)
#file(GLOB LOADER_NATIVE_PATH ${SRC_PATH}/libloader/native/*.cpp)
#add_subdirectory(jvm)
#add_subdirectory(native)

file(GLOB LOADER_PATH ${SRC_PATH}/libloader/*.cpp)
# -- MiraiCP libLoader --
add_library(Loader
        SHARED
        ${LOADER_PATH}
        ${MIRAICP_COMMON_PATH}
        )

target_include_directories(Loader PUBLIC
        ${JNI_PATH}
        ${3RD_PATH}
        ${SRC_PATH}/libloader
        ${SRC_PATH}/common
        )

set_target_properties(Loader PROPERTIES
        PREFIX lib
        OUTPUT_NAME Loader
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

target_compile_definitions(Loader PUBLIC MIRAICP_LIB_LOADER JSON_MultipleHeaders=ON)

# 移植性
if (WIN32)
    if (MSVC)
        target_compile_options(Loader PUBLIC /MT)
    else ()
        target_link_libraries(Loader PUBLIC -static)
    endif (MSVC)
endif (WIN32)
if (UNIX)
    target_link_libraries(Loader -static-libstdc++ -static-libgcc)
    target_compile_options(Loader PUBLIC -rdynamic)
endif (UNIX)
# -- end --

# -- MiraiCP libLoader native --
add_library(LoaderNative
        STATIC
        ${LOADER_PATH}
        ${MIRAICP_COMMON_PATH}
        )

target_include_directories(LoaderNative PUBLIC
        ${JNI_PATH}
        ${3RD_PATH}
        ${SRC_PATH}/libloader
        ${SRC_PATH}/common
        )

set_target_properties(LoaderNative PROPERTIES
        PREFIX lib
        OUTPUT_NAME LoaderNative
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

target_compile_definitions(LoaderNative PUBLIC MIRAICP_LIB_LOADER LOADER_NATIVE JSON_MultipleHeaders=ON)

# 移植性
if (WIN32)
    if (MSVC)
        target_compile_options(LoaderNative PUBLIC /MT)
    else ()
        target_link_libraries(LoaderNative PUBLIC -static)
    endif (MSVC)
endif (WIN32)
if (UNIX)
    target_link_libraries(LoaderNative -static-libstdc++ -static-libgcc)
    target_compile_options(LoaderNative PUBLIC -rdynamic)
endif (UNIX)
# -- end --