project(core)

file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")

include(FindPkgConfig)
pkg_check_modules (CAPSTONE REQUIRED capstone)

add_library(core
   ${SOURCES}
   ${HEADERS}
)

target_include_directories(core PUBLIC
  ${CAPSTONE_INCLUDE_DIRS}
  .
  include/
  include/core/
  include/core/cpu/
  include/core/cpu/instruction/
  include/core/cpu/registers/
  include/core/mem/
  include/core/mem/mmio/
  include/core/rsp/
  include/utils/
)

target_link_libraries(core PUBLIC
  ${CAPSTONE_LIBRARIES}
)