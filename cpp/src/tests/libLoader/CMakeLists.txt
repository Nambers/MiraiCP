file(GLOB libLoader_Tests ${SRC_PATH}/tests/libLoader/*.cpp)
add_executable(AllTestForLibLoader ${libLoader_Tests} ${LOADER_PATH}
        ${MIRAICP_COMMON_PATH})

target_link_libraries(AllTestForLibLoader PUBLIC GTest::gtest_main)

target_include_directories(AllTestForLibLoader PUBLIC
        ${JNI_PATH}
        ${3RD_PATH}
        ${SRC_PATH}/libloader
        ${SRC_PATH}/common
        ${SRC_PATH})
target_compile_options(AllTestForLibLoader PUBLIC -DMIRAICP_LIB_LOADER)
gtest_discover_tests(AllTestForLibLoader)
