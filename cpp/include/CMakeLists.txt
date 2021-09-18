find_package(JNI)

if (JNI_FOUND)
    message (STATUS "FOUND JNI IN ${JAVA_INCLUDE_PATH}")
    set(SOURCE_PATH
            ${CMAKE_CURRENT_SOURCE_DIR}/
            ${CMAKE_CURRENT_SOURCE_DIR}/miraicp
            ${CMAKE_CURRENT_SOURCE_DIR}/utf8
            ${CMAKE_CURRENT_SOURCE_DIR}/utf8/utf8
            ${JAVA_INCLUDE_PATH}/
            ${JAVA_INCLUDE_PATH2}/
            )
    target_include_directories(MiraiCP PUBLIC ${SOURCE_PATH})
else()
    message(STATUS "Cannot find JNI")
endif()