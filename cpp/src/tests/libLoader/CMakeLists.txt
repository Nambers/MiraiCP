file(GLOB libLoader_Tests ${SRC_PATH}/tests/libLoader/*.cpp)
add_executable(AllTestForLibLoader ${libLoader_Tests})#  ${LOADER_PATH} ${MIRAICP_COMMON_PATH})

target_link_libraries(AllTestForLibLoader PUBLIC GTest::gtest_main)
target_link_directories(AllTestForLibLoader PUBLIC ${CMAKE_BINARY_DIR})
target_link_options(AllTestForLibLoader PUBLIC -lLoader)

add_dependencies(AllTestForLibLoader Loader)

target_include_directories(AllTestForLibLoader PUBLIC
        ${JNI_PATH}
        ${3RD_PATH}
        ${SRC_PATH}/libloader
        ${SRC_PATH}/common
        ${SRC_PATH}
        )

gtest_discover_tests(AllTestForLibLoader)
