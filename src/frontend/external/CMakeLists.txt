project(dependencies)

add_subdirectory(Gui)
add_subdirectory(nativefiledialog-extended)

include(FetchContent)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

add_library(dependencies
  imgui_logger/imgui_logger.h
  imgui_memory_editor/imgui_memory_editor/imgui_memory_editor.h
  stb_image.h
)

target_include_directories(dependencies PUBLIC
    .
    Gui/
    imgui_logger/
    imgui_memory_editor/imgui_memory_editor/
    nativefiledialog-extended/src/include/
)

target_link_libraries(dependencies PUBLIC gui nfd fmt::fmt)