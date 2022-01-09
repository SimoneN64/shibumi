cmake_minimum_required(VERSION 3.7.0)
project(shibumi)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
add_subdirectory(external/nativefiledialog-extended/)
add_subdirectory(external/Gui/)
include(FindPkgConfig)
pkg_check_modules (CAPSTONE REQUIRED capstone)
pkg_check_modules (SDL2 REQUIRED sdl2)

file(GLOB_RECURSE GUI_SRC "src/frontend/*.cpp" "src/frontend/*.hpp")
file(GLOB_RECURSE EMU_SRC "src/shibumi/src/*.c")

include_directories(
  ${SDL2_INCLUDE_DIRS}
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

if(WIN32)
  set(FLAGS -static ${SDL2_LIBRARIES} OpenGL::GL gcc stdc++ winpthread winmm version Imm32 Setupapi)
else()
  set(FLAGS dl pthread ${SDL2_LIBRARIES} OpenGL::GL)
endif()

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

target_link_libraries(${CMAKE_PROJECT_NAME} ${FLAGS} nfd gui ${CAPSTONE_LIBRARIES})