project(dependencies)

add_subdirectory(Gui/)
add_subdirectory(nativefiledialog-extended/)

include(FetchContent)
include(FindPkgConfig)
pkg_check_modules (CAPSTONE REQUIRED capstone)

FetchContent_Declare(fmt
   GIT_REPOSITORY https://github.com/fmtlib/fmt.git
   GIT_TAG master
   )
FetchContent_MakeAvailable(fmt)

add_library(dependencies
   imgui_logger/imgui_logger.h
   imgui_memory_editor/imgui_memory_editor/imgui_memory_editor.h
)

target_include_directories(dependencies PUBLIC
   ${CAPSTONE_INCLUDE_DIRS}
)

target_link_libraries(dependencies PUBLIC gui nfd ${CAPSTONE_LIBRARIES} fmt::fmt)