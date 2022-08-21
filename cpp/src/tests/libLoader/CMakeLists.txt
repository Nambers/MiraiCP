file(GLOB libLoader_Tests ${SRC_PATH}/tests/libLoader/*.cpp)
add_executable(AllTestForLibLoader ${libLoader_Tests})

set_target_properties(
        AllTestForLibLoader PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_libraries(AllTestForLibLoader PUBLIC GTest::gtest_main)
target_link_directories(AllTestForLibLoader PUBLIC ${CMAKE_BINARY_DIR})
target_link_options(AllTestForLibLoader PUBLIC -lLoader)
target_compile_definitions(AllTestForLibLoader PUBLIC MIRAICP_LIB_LOADER GOOGLE_TEST)

add_dependencies(AllTestForLibLoader Loader)

target_include_directories(AllTestForLibLoader PUBLIC ${LOADER_INCLUDES_ALL})

gtest_discover_tests(AllTestForLibLoader)
