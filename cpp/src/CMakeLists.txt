target_include_directories(
        commonBuild INTERFACE
        ${3RD_PATH}
        ${INCLUDE_PATH}/common
        ${SRC_PATH}/common
)


add_subdirectory(sdk)
add_subdirectory(libloader)
