file(GLOB Multi_Tests ${SRC_PATH}/tests/plugin/multi/*.cpp)
add_executable(AllTestForPlugin_Multi ${Multi_Tests})

target_link_libraries(AllTestForPlugin_Multi PUBLIC GTest::gtest_main)
target_link_directories(AllTestForPlugin_Multi PUBLIC ${CMAKE_BINARY_DIR})
target_link_options(AllTestForPlugin_Multi PUBLIC -lMiraiCP_multi)
target_compile_definitions(AllTestForPlugin_Multi PUBLIC GOOGLE_TEST MIRAICP_LIB_SDK)

add_dependencies(AllTestForPlugin_Multi MiraiCP_multi)

target_include_directories(
        AllTestForPlugin_Multi PUBLIC
        ${SRC_PATH}/include
        ${SRC_PATH}/common
        ${3RD_PATH}
)

gtest_discover_tests(AllTestForPlugin_Multi)
