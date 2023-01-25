# -- miraicp multi --
file(GLOB MIRAICP_CPP_PATH ${SRC_PATH}/sdk/*.cpp)

add_library(
        MiraiCP_multi
        SHARED
        ${DEMO_PATH}/test.cpp
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
        ${INCLUDE_PATH}/sdk
        ${INCLUDE_PATH}/common
        ${3RD_PATH})
set(SDK_INCLUDES_ALL ${SDK_INCLUDES_INTERNAL} PARENT_SCOPE)

target_include_directories(MiraiCP_multi PUBLIC ${SDK_INCLUDES_INTERNAL})

# target_compile_options(MiraiCP_multi PUBLIC -Wall)
target_compile_definitions(MiraiCP_multi PUBLIC MIRAICP_LIB_SDK JSON_MultipleHeaders=ON)
target_link_libraries(MiraiCP_multi PRIVATE commonBuild)
# -- end --

# -- MiraiCP mock --
set(MOCK_PATH ${CMAKE_SOURCE_DIR}/mockTest)
add_library(
        MiraiCP_mock_test
        SHARED
        ${MOCK_PATH}/mockTestEntrance.cpp
        ${MOCK_PATH}/mockTests.cpp
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
target_link_libraries(MiraiCP_mock_test PRIVATE commonBuild)
# -- end --

# -- MiraiCP single --
set(SINGLE_HEADER_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/single_include)
if (NOT EXISTS "${SINGLE_HEADER_OUTPUT_PATH}/MiraiCP.hpp" OR NOT EXISTS "${SINGLE_HEADER_OUTPUT_PATH}/MiraiCP.cpp")
    FILE(WRITE "${SINGLE_HEADER_OUTPUT_PATH}/MiraiCP.hpp" "")
    FILE(WRITE "${SINGLE_HEADER_OUTPUT_PATH}/MiraiCP.cpp" "")
endif ()


add_library(
        MiraiCP_single
        SHARED
        ${DEMO_PATH}/test.cpp
        ${SINGLE_HEADER_OUTPUT_PATH}/MiraiCP.cpp
)
target_include_directories(
        MiraiCP_single PUBLIC
        ${SINGLE_HEADER_OUTPUT_PATH}
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
target_compile_options(MiraiCP_single PUBLIC -w) # disable all warnings!
target_compile_definitions(MiraiCP_single PUBLIC MIRAICP_LIB_SDK JSON_MultipleHeaders=ON)
target_link_libraries(MiraiCP_single PRIVATE commonBuild)
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
if (UNIX AND NOT (CMAKE_SYSTEM_NAME STREQUAL Android))
    target_link_libraries(MiraiCP_multi PRIVATE -static-libstdc++ -static-libgcc)
    target_link_libraries(MiraiCP_single PRIVATE -static-libstdc++ -static-libgcc)
    add_link_options(-fvisibility=hidden)
endif ()
