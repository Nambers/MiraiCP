file(GLOB Multi_Tests ${SRC_PATH}/tests/plugin/multi/*.cpp)
add_executable(AllTestForPlugin_Multi ${Multi_Tests} ${MIRAICP_CPP_PATH}
        ${MIRAICP_COMMON_PATH})

target_link_libraries(AllTestForPlugin_Multi PUBLIC GTest::gtest_main)

target_include_directories(AllTestForPlugin_Multi PUBLIC ${SRC_PATH}/include
        ${SRC_PATH}/common
        ${3RD_PATH})
gtest_discover_tests(AllTestForPlugin_Multi)
