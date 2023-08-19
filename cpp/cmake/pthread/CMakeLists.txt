message("Testing pthreads")
if (NOT MSVC)
    include(CheckCXXSourceCompiles)
    try_run(test_run_set # exit code
            test_compile_set # boolean
            ${CMAKE_CURRENT_BINARY_DIR} # output binary
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake_CheckSetName.cpp # source
            COMPILE_OUTPUT_VARIABLE SET_COMPILE_OUTPUT
            RUN_OUTPUT_VARIABLE SET_RUN_OUTPUT
    )
    try_run(test_run_get # exit code
            test_compile_get # boolean
            ${CMAKE_CURRENT_BINARY_DIR} # output binary
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake_CheckGetName.cpp # source
            COMPILE_OUTPUT_VARIABLE GET_COMPILE_OUTPUT
            RUN_OUTPUT_VARIABLE GET_RUN_OUTPUT
    )
    try_run(test_run_cancel # exit code
            test_compile_cancel # boolean
            ${CMAKE_CURRENT_BINARY_DIR} # output binary
            ${CMAKE_CURRENT_SOURCE_DIR}/cmake_CheckCancel.cpp # source
            COMPILE_OUTPUT_VARIABLE CANCEL_COMPILE_OUTPUT
            RUN_OUTPUT_VARIABLE CANCEL_RUN_OUTPUT
    )

    if (test_compile_set AND ("${test_run_set}" EQUAL 0))
        target_compile_definitions(commonBuild INTERFACE PTHREAD_SET_SUPPORTED)
    else ()
        message(WARNING "pthread_setname_np() is not supported\ncompile output:\n${SET_COMPILE_OUTPUT}\nrun output:\n${SET_RUN_OUTPUT}")
    endif ()

    if (test_compile_get AND ("${test_run_get}" EQUAL 0))
        target_compile_definitions(commonBuild INTERFACE PTHREAD_GET_SUPPORTED)
    else ()
        message(WARNING "pthread_getname_np() is not supported\ncompile output:\n${GET_COMPILE_OUTPUT}\nrun output:\n${GET_RUN_OUTPUT}")
    endif ()

    if (test_compile_cancel AND ("${test_run_cancel}" EQUAL 0))
        target_compile_definitions(commonBuild INTERFACE PTHREAD_CANCEL_SUPPORTED)
    else ()
        message(WARNING "pthread_cancel() is not supported\ncompile output:\n${CANCEL_COMPILE_OUTPUT}\nrun output:\n${CANCEL_RUN_OUTPUT}")
    endif ()
else ()
    message("MSVC detected, ignore pthread tests")
endif ()
