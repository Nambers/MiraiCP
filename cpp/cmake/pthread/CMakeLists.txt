message("Testing pthreads")
include(CheckCXXSourceCompiles)
try_run(test_run_set # exit code
        test_compile_set # boolean
        ${CMAKE_CURRENT_BINARY_DIR} # output binary
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake_CheckSetName.cpp # source
        )
try_run(test_run_get # exit code
        test_compile_get # boolean
        ${CMAKE_CURRENT_BINARY_DIR} # output binary
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake_CheckGetName.cpp # source
        )
try_run(test_run_cancel # exit code
        test_compile_cancel # boolean
        ${CMAKE_CURRENT_BINARY_DIR} # output binary
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake_CheckCancel.cpp # source
        )

if (test_compile_set AND ("${test_run_set}" EQUAL 0))
    target_compile_definitions(commonBuild INTERFACE PTHREAD_SET_SUPPORTED)
else ()
    message("pthread_setname_np() is not supported")
endif ()

if (test_compile_get AND ("${test_run_get}" EQUAL 0))
    target_compile_definitions(commonBuild INTERFACE PTHREAD_GET_SUPPORTED)
else ()
    message("pthread_getname_np() is not supported")
endif ()

if (test_compile_cancel AND ("${test_run_cancel}" EQUAL 0))
    target_compile_definitions(commonBuild INTERFACE PTHREAD_CANCEL_SUPPORTED)
else ()
    message("pthread_cancel() is not supported")
endif ()
