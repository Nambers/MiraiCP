file(GLOB LOADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# -- MiraiCP libLoader --
add_library(Loader
        SHARED
        ${LOADER_PATH}
        ${MIRAICP_COMMON_PATH}
        )

target_include_directories(Loader PUBLIC
        ${JNI_PATH}
        ${3RD_PATH}
        ${INCLUDE_PATH}/libloader
        ${INCLUDE_PATH}/common
        )

set_target_properties(Loader PROPERTIES
        PREFIX lib
        OUTPUT_NAME Loader
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

target_compile_definitions(Loader PUBLIC MIRAICP_LIB_LOADER JSON_MultipleHeaders=ON)

target_link_libraries(Loader PRIVATE commonBuild)

# 移植性
set_target_properties(Loader PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(Loader PROPERTIES LINK_SEARCH_END_STATIC 1)
if (WIN32)
    if (MSVC)
        target_compile_options(Loader PUBLIC /MT)
    else ()
        target_link_libraries(Loader PUBLIC -static)
    endif (MSVC)
endif (WIN32)
if (UNIX)
    target_link_libraries(Loader PRIVATE -static-libstdc++ -static-libgcc)
    # target_compile_options(Loader PUBLIC -rdynamic)
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
        ${INCLUDE_PATH}/libloader
        ${INCLUDE_PATH}/common
        )

set_target_properties(LoaderNative PROPERTIES
        PREFIX lib
        OUTPUT_NAME LoaderNative
        ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

target_compile_definitions(LoaderNative PUBLIC MIRAICP_LIB_LOADER LOADER_NATIVE JSON_MultipleHeaders=ON)
target_link_libraries(LoaderNative PRIVATE commonBuild)

# 移植性
set_target_properties(LoaderNative PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(LoaderNative PROPERTIES LINK_SEARCH_END_STATIC 1)
if (WIN32)
    if (MSVC)
        target_compile_options(LoaderNative PUBLIC /MT)
    else ()
        target_link_libraries(LoaderNative PUBLIC -static)
    endif (MSVC)
endif (WIN32)
if (UNIX AND NOT (CMAKE_SYSTEM_NAME STREQUAL Android))
    target_link_libraries(LoaderNative PRIVATE -static-libstdc++ -static-libgcc)
    add_link_options(-fvisibility=hidden)
endif ()
# -- end --