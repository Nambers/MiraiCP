# -- miraicp multi --
add_library(
        MiraiCP_multi
        SHARED
        ${SRC_PATH}/../demo/test.cpp
        ${SRC_PATH}/../demo/test2.cpp
        ${MIRAICP_CPP_PATH}
        ${MIRAICP_COMMON_PATH}
)
set_target_properties(
        MiraiCP_multi PROPERTIES
        PREFIX lib
        OUTPUT_NAME MiraiCP_multi
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

set(SDK_INCLUDES_INTERNAL
        ${SRC_PATH}/include
        ${SRC_PATH}/common
        ${3RD_PATH})
set(SDK_INCLUDES_ALL ${SDK_INCLUDES_INTERNAL} PARENT_SCOPE)

target_include_directories(MiraiCP_multi PUBLIC ${SDK_INCLUDES_INTERNAL})

target_compile_options(MiraiCP_multi PUBLIC -w)
target_compile_definitions(MiraiCP_multi PUBLIC JSON_MultipleHeaders=ON MIRAICP_LIB_SDK)
# -- end --

# -- MiraiCP single --
if (NOT EXISTS "${SRC_PATH}/single_include/MiraiCP/MiraiCP.hpp" OR NOT EXISTS "${SRC_PATH}/single_include/MiraiCP/MiraiCP.cpp")
    FILE(WRITE "${SRC_PATH}/single_include/MiraiCP/MiraiCP.hpp" "")
    FILE(WRITE "${SRC_PATH}/single_include/MiraiCP/MiraiCP.cpp" "")
endif ()


add_library(
        MiraiCP_single
        SHARED
        ${SRC_PATH}/../demo/test.cpp
        ${SRC_PATH}/../demo/test2.cpp
        ${SRC_PATH}/single_include/MiraiCP/MiraiCP.cpp
)
target_include_directories(
        MiraiCP_single PUBLIC
        ${SRC_PATH}/single_include/MiraiCP
        ${3RD_PATH}
)
set_target_properties(
        MiraiCP_single PROPERTIES
        PREFIX lib
        OUTPUT_NAME MiraiCP_single
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

add_dependencies(MiraiCP_single generateSingleHeaders)
target_compile_options(MiraiCP_single PUBLIC -w)
target_compile_definitions(MiraiCP_single PUBLIC JSON_MultipleHeaders=ON MIRAICP_LIB_SDK)
# -- end --

# 移植性
if (WIN32)
    target_link_libraries(MiraiCP_multi -static)
    target_link_libraries(MiraiCP_single -static)
endif (WIN32)
if (UNIX)
    target_link_libraries(MiraiCP_multi -static-libstdc++ -static-libgcc)
    target_link_libraries(MiraiCP_single -static-libstdc++ -static-libgcc)
endif (UNIX)
