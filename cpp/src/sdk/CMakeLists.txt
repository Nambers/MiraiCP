# common build options for sdk
add_library(sdkCommon INTERFACE)
target_compile_definitions(sdkCommon INTERFACE MIRAICP_LIB_SDK JSON_MultipleHeaders=ON)
add_library(sdkCommonMulti INTERFACE)
target_link_libraries(sdkCommonMulti INTERFACE sdkCommon SDKCommonInclude)

# -- miraicp multi --
file(GLOB MIRAICP_CPP_PATH ${SRC_PATH}/sdk/*.cpp)

add_library(
        MiraiCP_multi
        SHARED
        ${DEMO_PATH}/test_multi.cpp
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

# target_compile_options(MiraiCP_multi PUBLIC -Wall)
target_link_libraries(MiraiCP_multi PRIVATE commonBuild sdkCommonMulti)
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

target_link_libraries(MiraiCP_mock_test PRIVATE commonBuild sdkCommonMulti)
# -- end --

# -- MiraiCP single --
set(SINGLE_HEADER_OUTPUT_PATH ${CMAKE_SOURCE_DIR}/single_include)
# To ensure the source files exist when running cmake. Useful in CI.
if (NOT EXISTS "${SINGLE_HEADER_OUTPUT_PATH}/MiraiCP.hpp" OR NOT EXISTS "${SINGLE_HEADER_OUTPUT_PATH}/MiraiCP.cpp")
    FILE(WRITE "${SINGLE_HEADER_OUTPUT_PATH}/MiraiCP.hpp" "")
    FILE(WRITE "${SINGLE_HEADER_OUTPUT_PATH}/MiraiCP.cpp" "")
endif ()

add_library(
        MiraiCP_single
        SHARED
        ${DEMO_PATH}/test_single.cpp
        ${SINGLE_HEADER_OUTPUT_PATH}/MiraiCP.cpp
)
set_target_properties(
        MiraiCP_single PROPERTIES
        PREFIX lib
        OUTPUT_NAME MiraiCP_single
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)
target_include_directories(MiraiCP_single PRIVATE ${SINGLE_HEADER_OUTPUT_PATH})
add_dependencies(MiraiCP_single generateSingleHeaders)
target_compile_options(MiraiCP_single PUBLIC -w) # disable all warnings!
target_link_libraries(MiraiCP_single PRIVATE commonBuild sdkCommon)
# -- end --
