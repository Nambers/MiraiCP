# 移植性
set_target_properties(commonBuild PROPERTIES LINK_SEARCH_START_STATIC 1)
set_target_properties(commonBuild PROPERTIES LINK_SEARCH_END_STATIC 1)
if (WIN32)
    if (MSVC)
        target_compile_options(commonBuild INTERFACE /MT)
    else ()
        target_link_libraries(commonBuild INTERFACE -static)
    endif (MSVC)
endif (WIN32)
if (UNIX AND NOT (CMAKE_SYSTEM_NAME STREQUAL Android))
    target_link_libraries(commonBuild INTERFACE -static-libstdc++ -static-libgcc)
endif ()
# -- end --
