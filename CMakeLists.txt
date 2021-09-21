cmake_minimum_required(VERSION 3.7.0)
project(shibumi)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

#set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
#set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)

#find_package(glfw3 3.3 REQUIRED)
#find_package(OpenGL REQUIRED)
#add_subdirectory(include/external/nativefiledialog-extended/)

file(GLOB_RECURSE EMU_SRC "src/*.c")

#set(GLAD_SRC include/external/glad/src/glad.c)
#file(GLOB CIMGUI_SRC
#  "include/external/cimgui/*.cpp"
#  "include/external/cimgui/imgui/*.cpp"
#  "include/external/cimgui/imgui/backends/imgui_impl_glfw.cpp"
#  "include/external/cimgui/imgui/backends/imgui_impl_opengl3.cpp"
#)

#add_library(cimgui ${CIMGUI_SRC})

#target_compile_definitions(cimgui PRIVATE 
#	IMGUI_IMPL_API=extern\ \"C\"
#	IMGUI_IMPL_OPENGL_LOADER_GLAD)

include_directories(
  ${CMAKE_SOURCE_DIR}/include
  ${CMAKE_SOURCE_DIR}/include/utils/
  ${CMAKE_SOURCE_DIR}/include/core/
  ${CMAKE_SOURCE_DIR}/include/core/mem/
  ${CMAKE_SOURCE_DIR}/include/core/mem/mmio/
  ${CMAKE_SOURCE_DIR}/include/core/cpu/
  #${CMAKE_SOURCE_DIR}/include/core/scheduler/
  #${CMAKE_SOURCE_DIR}/include/external/
  #${CMAKE_SOURCE_DIR}/include/external/glad/include/
  #${CMAKE_SOURCE_DIR}/include/external/glad/include/KHR/
  #${CMAKE_SOURCE_DIR}/include/external/glad/include/glad/
  #${CMAKE_SOURCE_DIR}/include/external/nativefiledialog-extended/
  #${CMAKE_SOURCE_DIR}/include/external/cimgui/
  #${CMAKE_SOURCE_DIR}/include/external/cimgui/imgui/
  #${CMAKE_SOURCE_DIR}/include/external/cimgui/generator/output
)

#if(WIN32)
#  set(FLAGS -static glfw OpenGL::GL gcc stdc++ winpthread winmm version Imm32 Setupapi)
#else()
#  set(FLAGS pthread glfw OpenGL::GL)
#endif()

if (CMAKE_BUILD_TYPE MATCHES Debug)
  add_compile_options(-g)
  add_compile_definitions(DEBUG)
  #add_link_options(-fsanitize=address)
elseif(CMAKE_BUILD_TYPE MATCHES Release)
  add_compile_options(-O3)
endif()

add_executable(${CMAKE_PROJECT_NAME}
  ${EMU_SRC}
  #${GLAD_SRC}
)

#target_link_libraries(${CMAKE_PROJECT_NAME} ${FLAGS} ${CMAKE_DL_LIBS} nfd cimgui capstone)