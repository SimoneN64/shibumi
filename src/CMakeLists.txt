project(core)

file(GLOB_RECURSE SOURCES "shibumi/src/*.c")
file(GLOB_RECURSE HEADERS "shibumi/include/*.h")

include(FindPkgConfig)
pkg_check_modules (CAPSTONE REQUIRED capstone)

add_library(core
  ${SOURCES}
  ${HEADERS}
  context.cpp
  context.hpp
)

target_include_directories(core PUBLIC
  ${CAPSTONE_INCLUDE_DIRS}
  .
  shibumi/
  shibumi/include/
  shibumi/include/core/
  shibumi/include/core/cpu/
  shibumi/include/core/cpu/instruction/
  shibumi/include/core/cpu/registers/
  shibumi/include/core/mem/
  shibumi/include/core/mem/mmio/
  shibumi/include/core/rsp/
  shibumi/include/utils/
)

target_link_libraries(core PUBLIC
  ${CAPSTONE_LIBRARIES}
)