cmake_minimum_required(VERSION 3.7.0)
project(shibumi)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

find_package(OpenGL REQUIRED)
add_subdirectory(external/nativefiledialog-extended/)

file(GLOB_RECURSE EMU_SRC "src/*.c")

file(GLOB CIMGUI_MEM_EDITOR_SRC
  "${CMAKE_SOURCE_DIR}/external/cimgui_memory_editor/src/*.c")

file(GLOB CIMGUI_SRC
  "${CMAKE_SOURCE_DIR}/external/cimgui/*.cpp"
  "${CMAKE_SOURCE_DIR}/external/cimgui/imgui/*.cpp"
  "${CMAKE_SOURCE_DIR}/external/cimgui/imgui/backends/imgui_impl_sdl.cpp"
  "${CMAKE_SOURCE_DIR}/external/cimgui/imgui/backends/imgui_impl_opengl3.cpp"
)

add_library(cimgui ${CIMGUI_SRC})
add_library(cimgui_memory_editor ${CIMGUI_MEM_EDITOR_SRC})

target_compile_definitions(cimgui PRIVATE 
	IMGUI_IMPL_API=extern\ \"C\")

include(FindPkgConfig)
pkg_check_modules (CAPSTONE REQUIRED capstone)
pkg_check_modules (SDL2 REQUIRED sdl2)

include_directories(
  ${SDL2_INCLUDE_DIRS}
  ${CAPSTONE_INCLUDE_DIRS}
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/utils/
  ${CMAKE_SOURCE_DIR}/include/core/
  ${CMAKE_SOURCE_DIR}/include/core/mem/
  ${CMAKE_SOURCE_DIR}/include/core/mem/mmio/
  ${CMAKE_SOURCE_DIR}/include/core/cpu/
  ${CMAKE_SOURCE_DIR}/include/core/cpu/registers/
  ${CMAKE_SOURCE_DIR}/include/core/scheduler/
  ${CMAKE_SOURCE_DIR}/external/
  ${CMAKE_SOURCE_DIR}/external/nativefiledialog-extended/src/include/
  ${CMAKE_SOURCE_DIR}/external/cimgui/
  ${CMAKE_SOURCE_DIR}/external/cimgui/imgui/
  ${CMAKE_SOURCE_DIR}/external/cimgui/generator/output
  ${CMAKE_SOURCE_DIR}/external/cimgui_memory_editor/src/
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

file(COPY ${CMAKE_SOURCE_DIR}/resources/ DESTINATION ${CMAKE_BINARY_DIR}/resources/)

add_definitions("-DEMU_DIR=\"${CMAKE_SOURCE_DIR}\" -mno-ms-bitfields")

add_executable(${CMAKE_PROJECT_NAME}
  ${EMU_SRC}
)

target_link_libraries(${CMAKE_PROJECT_NAME} ${FLAGS} nfd cimgui cimgui_memory_editor ${CAPSTONE_LIBRARIES})