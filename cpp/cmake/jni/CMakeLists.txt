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
else ()
    message(STATUS "Cannot find JNI")
endif ()
# -- end --
