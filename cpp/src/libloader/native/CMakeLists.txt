# -- MiraiCP libLoader --
add_library(LoaderNative
        STATIC
        ${LOADER_COMMON_PATH}
        ${LOADER_NATIVE_PATH}
        ${MIRAICP_COMMON_PATH}
        )


target_include_directories(LoaderNative PUBLIC
        ${LOADER_NATIVE_INCLUDES_INTERNAL}
        )
set_target_properties(LoaderNative PROPERTIES
        PREFIX lib
        OUTPUT_NAME LoaderNative
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# https://stackoverflow.com/questions/45114078/get-address-for-symbol-in-current-process-in-windows
target_compile_definitions(LoaderNative PUBLIC MIRAICP_LIB_LOADER JSON_MultipleHeaders=ON LOADER_NATIVE)
# target_compile_options(Loader PUBLIC -Wall)

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