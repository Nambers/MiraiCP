# --- MiraiCP loader native --
add_library(Native
        STATIC
        ${LOADER_PATH}
        MiraiCP.cpp)
set(LOADER_INCLUDES_INTERNAL
        ${JNI_PATH}
        ${3RD_PATH}
        ${SRC_PATH}/libloader
        ${SRC_PATH}/common)
set(LOADER_INCLUDES_ALL ${LOADER_INCLUDES_INTERNAL} PARENT_SCOPE)
target_include_directories(Native PUBLIC ${LOADER_INCLUDES_INTERNAL}
        )
set_target_properties(Native PROPERTIES
        PREFIX lib
        OUTPUT_NAME MiraiCP
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
target_compile_definitions(Native PUBLIC MIRAICP_LIB_LOADER JSON_MultipleHeaders=ON)
# 移植性
if (WIN32)
    if (MSVC)
        target_compile_options(Native PUBLIC /MT)
    else ()
        target_link_libraries(Native PUBLIC -static)
    endif (MSVC)
endif (WIN32)
if (UNIX)
    target_link_libraries(Native -static-libstdc++ -static-libgcc)
    target_compile_options(Native PUBLIC -rdynamic)
endif (UNIX)