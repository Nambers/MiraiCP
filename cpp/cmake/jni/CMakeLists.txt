# -- jni required --
set(JAVA_AWT_LIBRARY NotNeeded)
# set(JAVA_JVM_LIBRARY NotNeeded)
set(JAVA_AWT_INCLUDE_PATH NotNeeded)
find_package(JNI REQUIRED)
if (JNI_FOUND)
    message(STATUS "FOUND JNI IN ${JAVA_INCLUDE_PATH}")
    if (CMAKE_SYSTEM_NAME STREQUAL Android)
        set(JNI_PATH ${JAVA_INCLUDE_PATH} PARENT_SCOPE)
    else ()
        set(JNI_PATH
                ${JAVA_INCLUDE_PATH}
                ${JAVA_INCLUDE_PATH2}
                PARENT_SCOPE
                )
    endif ()
    set(3RD_PATH
            ${THIRD_PARTY_PATH}/json
            ${THIRD_PARTY_PATH}/json/nlohmann
            ${THIRD_PARTY_PATH}/utf8
            ${THIRD_PARTY_PATH}/utf8/utf8
            PARENT_SCOPE
            )
else ()
    message(STATUS "Cannot find JNI")
endif ()
# -- end --
