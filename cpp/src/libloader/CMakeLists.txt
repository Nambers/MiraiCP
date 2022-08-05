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
        ${SRC_PATH}
        )
set_target_properties(Loader PROPERTIES
        PREFIX lib
        OUTPUT_NAME Loader
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
# https://stackoverflow.com/questions/45114078/get-address-for-symbol-in-current-process-in-windows
target_compile_options(Loader PUBLIC -DMIRAICP_LIB_LOADER -DJSON_MultipleHeaders=ON)

# 移植性
if (WIN32)
    target_link_libraries(Loader -static)
endif (WIN32)
if (UNIX)
    target_link_libraries(Loader -static-libstdc++ -static-libgcc)
    target_compile_options(Loader PUBLIC -rdynamic)
endif (UNIX)