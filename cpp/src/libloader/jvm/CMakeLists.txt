# -- MiraiCP libLoader --
add_library(Loader
        SHARED
        ${LOADER_COMMON_PATH}
        ${LOADER_JVM_PATH}
        ${MIRAICP_COMMON_PATH}
        )


target_include_directories(Loader PUBLIC
        ${LOADER_JVM_INCLUDES_INTERNAL}
        )
set_target_properties(Loader PROPERTIES
        PREFIX lib
        OUTPUT_NAME Loader
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

# https://stackoverflow.com/questions/45114078/get-address-for-symbol-in-current-process-in-windows
target_compile_definitions(Loader PUBLIC MIRAICP_LIB_LOADER JSON_MultipleHeaders=ON)
# target_compile_options(Loader PUBLIC -Wall)

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