file(GLOB Multi_Tests ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(AllTestForPlugin_Multi ${Multi_Tests})

set_target_properties(
        AllTestForPlugin_Multi PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_directories(AllTestForPlugin_Multi PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(AllTestForPlugin_Multi PUBLIC GTest::gtest_main MiraiCP_multi)
target_compile_definitions(AllTestForPlugin_Multi PUBLIC GOOGLE_TEST MIRAICP_LIB_SDK)

add_dependencies(AllTestForPlugin_Multi MiraiCP_multi)

target_include_directories(AllTestForPlugin_Multi PUBLIC ${SDK_INCLUDES_ALL})

gtest_discover_tests(AllTestForPlugin_Multi)
