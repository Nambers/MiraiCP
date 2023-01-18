# -- miraicp multi --
add_library(
        MiraiCP_multi
        SHARED
        ${SRC_PATH}/../demo/test.cpp
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

# target_compile_options(MiraiCP_multi PUBLIC -Wall)
target_compile_definitions(MiraiCP_multi PUBLIC MIRAICP_LIB_SDK JSON_MultipleHeaders=ON)
# -- end --

# -- MiraiCP mock --
add_library(
        MiraiCP_mock_test
        SHARED
        ${SRC_PATH}/../mockTest/mockTestEntrance.cpp
        ${SRC_PATH}/../mockTest/mockTests.cpp
        ${MIRAICP_CPP_PATH}
        ${MIRAICP_COMMON_PATH}
)
set_target_properties(
        MiraiCP_mock_test PROPERTIES
        PREFIX lib
        OUTPUT_NAME MiraiCP_mock_test
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_include_directories(MiraiCP_mock_test PUBLIC ${SDK_INCLUDES_INTERNAL})

# target_compile_options(MiraiCP_multi PUBLIC -Wall)
target_compile_definitions(MiraiCP_mock_test PUBLIC MIRAICP_LIB_SDK JSON_MultipleHeaders=ON)
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
target_compile_definitions(MiraiCP_single PUBLIC MIRAICP_LIB_SDK JSON_MultipleHeaders=ON)
# -- end --

# 移植性
set_target_properties(MiraiCP_multi PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(MiraiCP_multi PROPERTIES LINK_SEARCH_END_STATIC 1)
set_target_properties(MiraiCP_single PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(MiraiCP_single PROPERTIES LINK_SEARCH_END_STATIC 1)
if (WIN32)
    if (MSVC)
        target_compile_options(MiraiCP_multi PUBLIC /MT)
        target_compile_options(MiraiCP_single PUBLIC /MT)
    else ()
        target_link_libraries(MiraiCP_multi PUBLIC -static)
        target_link_libraries(MiraiCP_single PUBLIC -static)
    endif (MSVC)
endif (WIN32)
if (UNIX)
    target_link_libraries(MiraiCP_multi -static-libstdc++ -static-libgcc)
    target_link_libraries(MiraiCP_single -static-libstdc++ -static-libgcc)
    add_link_options(-fvisibility=hidden)
endif (UNIX)
