# -- MiraiCP libLoader --
add_library(Loader
        SHARED
        ${LOADER_PATH}
        ${MIRAICP_COMMON_PATH}
        )

set(LOADER_INCLUDES_INTERNAL
        ${JNI_PATH}
        ${3RD_PATH}
        ${SRC_PATH}/libloader
        ${SRC_PATH}/common)
set(LOADER_INCLUDES_ALL ${LOADER_INCLUDES_INTERNAL} PARENT_SCOPE)

target_include_directories(Loader PUBLIC ${LOADER_INCLUDES_INTERNAL}
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
    target_link_libraries(Loader -static)
endif (WIN32)
if (UNIX)
    target_link_libraries(Loader -static-libstdc++ -static-libgcc)
    target_compile_options(Loader PUBLIC -rdynamic)
endif (UNIX)