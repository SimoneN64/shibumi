cmake_minimum_required(VERSION 3.7.0)
project(shibumi)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/nativefiledialog-extended/)
add_subdirectory(external/Gui/)
include(FetchContent)
include(FindPkgConfig)
pkg_check_modules (CAPSTONE REQUIRED capstone)

FetchContent_Declare(fmt
  GIT_REPOSITORY https://github.com/fmtlib/fmt.git
  GIT_TAG master
)
FetchContent_MakeAvailable(fmt)

file(GLOB_RECURSE GUI_SRC "src/frontend/*.cpp" "src/frontend/*.hpp")
file(GLOB_RECURSE EMU_SRC "src/shibumi/src/*.c")

include_directories(
  ${CAPSTONE_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/src/frontend/
  ${CMAKE_SOURCE_DIR}/src/shibumi/include
  ${CMAKE_SOURCE_DIR}/src/shibumi/include/utils/
  ${CMAKE_SOURCE_DIR}/src/shibumi/include/core/
  ${CMAKE_SOURCE_DIR}/src/shibumi/include/core/rsp/
  ${CMAKE_SOURCE_DIR}/src/shibumi/include/core/mem/
  ${CMAKE_SOURCE_DIR}/src/shibumi/include/core/mem/mmio/
  ${CMAKE_SOURCE_DIR}/src/shibumi/include/core/cpu/
  ${CMAKE_SOURCE_DIR}/src/shibumi/include/core/cpu/registers/
  ${CMAKE_SOURCE_DIR}/src/shibumi/include/core/scheduler/
  ${CMAKE_SOURCE_DIR}/external/
  ${CMAKE_SOURCE_DIR}/external/nativefiledialog-extended/src/include/
  ${CMAKE_SOURCE_DIR}/external/imgui/backends/
  ${CMAKE_SOURCE_DIR}/external/imgui_memory_editor/imgui_memory_editor/
  ${CMAKE_SOURCE_DIR}/external/imgui_logger/
)

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_compile_options(-g)
  add_compile_definitions(DEBUG)
  #add_link_options(-fsanitize=address)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  add_compile_options(-O3)
endif()

add_compile_options(-mno-ms-bitfields)

file(COPY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_BINARY_DIR}/resources/)
file(COPY ${CMAKE_SOURCE_DIR}/tests/ DESTINATION ${CMAKE_BINARY_DIR}/tests/)

add_executable(${CMAKE_PROJECT_NAME}
  ${EMU_SRC}
  ${GUI_SRC}
  src/main.cpp
)

if(WIN32)
  set(FLAGS -static nfd fmt::fmt gui ${CAPSTONE_LIBRARIES} gcc stdc++ winpthread winmm version Imm32 Setupapi)
else()
  set(FLAGS dl pthread nfd fmt::fmt gui ${CAPSTONE_LIBRARIES})
endif()

target_link_libraries(${CMAKE_PROJECT_NAME} ${FLAGS})