set(SOURCE_PATH
        ${CMAKE_CURRENT_SOURCE_DIR}/
        ${CMAKE_CURRENT_SOURCE_DIR}/miraicp
        ${CMAKE_CURRENT_SOURCE_DIR}/jni
        ${CMAKE_CURRENT_SOURCE_DIR}/jni/win32
        ${CMAKE_CURRENT_SOURCE_DIR}/jni/win32/bridge
        ${CMAKE_CURRENT_SOURCE_DIR}/utf8
        ${CMAKE_CURRENT_SOURCE_DIR}/utf8/utf8
        )
target_include_directories(MiraiCP PUBLIC ${SOURCE_PATH})