file(GLOB libLoader_Tests ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
add_executable(AllTestForLibLoader ${libLoader_Tests})

set_target_properties(
        AllTestForLibLoader PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_directories(AllTestForLibLoader PUBLIC ${CMAKE_BINARY_DIR})
target_link_libraries(AllTestForLibLoader PUBLIC GTest::gtest_main Loader libLoaderCommonInclude)
target_compile_definitions(AllTestForLibLoader PUBLIC MIRAICP_LIB_LOADER GOOGLE_TEST)

add_dependencies(AllTestForLibLoader Loader)

target_include_directories(AllTestForLibLoader PUBLIC ${LOADER_INCLUDES_ALL})

gtest_discover_tests(AllTestForLibLoader)
