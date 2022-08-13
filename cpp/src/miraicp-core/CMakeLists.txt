# -- MiraiCP core --
add_executable(
        MiraiCP_core
        ${SRC_PATH}/../demo/MiraiCP-core.cpp
        ${SRC_PATH}/miraicp-core/core.cpp
        ${MIRAICP_CPP_PATH}
        ${MIRAICP_COMMON_PATH}
)
set_target_properties(
        MiraiCP_core PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        OUTPUT_NAME MiraiCP_core
)

target_include_directories(
        MiraiCP_core PUBLIC
        ${SRC_PATH}/miraicp-core
        ${SRC_PATH}/include
        ${SRC_PATH}/common
        ${JNI_PATH}
        ${3RD_PATH}
)

if (WIN32)
    target_compile_definitions(MiraiCP_core PUBLIC MIRAICP_CORE _JVM_DLL_PATH=\"${JAVA_INCLUDE_PATH}/../bin/server/jvm.dll\")
else (WIN32)
    target_compile_definitions(MiraiCP_core PUBLIC MIRAICP_CORE _JVM_DLL_PATH=\"\")
endif (WIN32)
# -- end --