message("Android build detected, testing API level")
include(CheckCXXSourceCompiles)
try_run(test_run_result # exit code
        test_compile_result # boolean
        ${CMAKE_CURRENT_BINARY_DIR} # output binary
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake_CheckApiLevel.cpp # source
        RUN_OUTPUT_VARIABLE ANDROID_API_LEVEL # stdout
        )
if (test_compile_result AND ("${test_run_result}" EQUAL 0))
    message("API level is: ${ANDROID_API_LEVEL}")
    if (${ANDROID_API_LEVEL} GREATER_EQUAL 26)
        add_compile_definitions(TERMUX_THREAD_SUPPORTED)
        message("pthread utilities supported")
    else ()
        message("pthread utilities not supported")
    endif ()
else ()
    message("Failed to check Android API version")
endif ()
