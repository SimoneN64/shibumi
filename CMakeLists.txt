cmake_minimum_required(VERSION 3.21)
project(shibumi)

find_package(SDL2 REQUIRED)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_subdirectory(src/)
add_subdirectory(external/)
add_executable(shibumi src/main.c)

include_directories(${SDL2_INCLUDE_DIRECTORIES})

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
  set(FLAGS -static SDL2main SDL2 gcc stdc++ winpthread winmm version Imm32 Setupapi)
else()
  set(FLAGS dl SDL2 pthread)
endif()

target_link_libraries(shibumi core nfd ${FLAGS})