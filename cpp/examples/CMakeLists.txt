file(GLOB MIRAICP_EXAMPLE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
# for code highlight only
# don't make this target
add_library(
        EXAMPLE
        SHARED
        ${MIRAICP_EXAMPLE_PATH}
)
target_include_directories(EXAMPLE PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/../single_include
        ${3RD_PATH}
)
# Notice: If you want examples have a correct highlight and include, you need to make `MiraiCP_single` target at first.