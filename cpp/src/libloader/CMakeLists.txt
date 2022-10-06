set(LOADER_JVM_INCLUDES_INTERNAL
        ${JNI_PATH}
        ${3RD_PATH}
        ${SRC_PATH}/libloader/common
        ${SRC_PATH}/libloader/jvm
        ${SRC_PATH}/common)
set(LOADER_JVM_INCLUDES_ALL ${LOADER_JVM_INCLUDES_INTERNAL} PARENT_SCOPE)
set(LOADER_NATIVE_INCLUDES_INTERNAL
        ${3RD_PATH}
        ${SRC_PATH}/libloader/common
        ${SRC_PATH}/libloader/native
        ${SRC_PATH}/common)
set(LOADER_JVM_INCLUDES_ALL ${LOADER_JVM_INCLUDES_INTERNAL} PARENT_SCOPE)
file(GLOB LOADER_COMMON_PATH ${SRC_PATH}/libloader/common/*.cpp)
file(GLOB LOADER_JVM_PATH ${SRC_PATH}/libloader/jvm/*.cpp)
file(GLOB LOADER_NATIVE_PATH ${SRC_PATH}/libloader/native/*.cpp)
add_subdirectory(jvm)
add_subdirectory(native)