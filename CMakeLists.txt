cmake_minimum_required(VERSION 3.7.0)
project(shibumi)

if(CMAKE_VERSION VERSION_LESS "3.7.0")
  set(CMAKE_INCLUDE_CURRENT_DIR ON)
endif()

set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(external/)

add_executable(shibumi
   src/main.cpp
)

include_directories(
   src/shibumi/include/
   src/shibumi/include/core/
   src/shibumi/include/core/cpu/
   src/shibumi/include/core/cpu/instruction/
   src/shibumi/include/core/cpu/registers/
   src/shibumi/include/core/mem/
   src/shibumi/include/core/mem/mmio/
   src/shibumi/include/core/rsp/
   src/shibumi/include/utils/
   src/frontend/
   external/imgui_logger/
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

if(WIN32)
  set(FLAGS -static dependencies gcc stdc++ winpthread winmm version Imm32 Setupapi)
else()
  set(FLAGS dl pthread dependencies)
endif()

target_link_libraries(shibumi ${FLAGS})