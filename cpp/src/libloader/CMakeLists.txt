file(GLOB_RECURSE LOADER_PATH ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

# common build options for loader
add_library(loaderCommon INTERFACE)
target_link_libraries(loaderCommon INTERFACE libLoaderCommonInclude)
target_compile_definitions(loaderCommon INTERFACE MIRAICP_LIB_LOADER JSON_MultipleHeaders=ON)

# -- MiraiCP libLoader --
add_library(
        Loader
        SHARED
        ${LOADER_PATH}
        ${MIRAICP_COMMON_PATH}
)

set_target_properties(
        Loader PROPERTIES
        PREFIX lib
        OUTPUT_NAME Loader
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}
)

target_link_libraries(Loader PRIVATE commonBuild loaderCommon)