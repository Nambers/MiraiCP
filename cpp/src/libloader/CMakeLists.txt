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
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

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