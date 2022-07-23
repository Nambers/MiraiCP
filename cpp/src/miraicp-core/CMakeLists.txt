# -- MiraiCP core --
add_executable(MiraiCP_core
        ${SRC_PATH}/../demo/MiraiCP-core.cpp
        ${SRC_PATH}/miraicp-core/core.cpp
        ${MIRAICP_CPP_PATH}
        ${MIRAICP_COMMON_PATH}
        )
target_include_directories(MiraiCP_core PUBLIC
        ${JNI_PATH}
        ${3RD_PATH}
        ${SRC_PATH}/miraicp-core
        ${SRC_PATH}/include
        ${SRC_PATH}/common
        ${SRC_PATH}
        )
if (WIN32)
    target_compile_options(MiraiCP_core PUBLIC -DMIRAICP_CORE -D_JVM_DLL_PATH=\"${JAVA_INCLUDE_PATH}/../bin/server/jvm.dll\")
else (WIN32)
    target_compile_options(MiraiCP_core PUBLIC -DMIRAICP_CORE -D_JVM_DLL_PATH=\"\")
endif (WIN32)
# -- end --