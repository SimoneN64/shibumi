project(mem)

set(SOURCES
  mmio/intr.c
  mmio/mi.c
  mmio/pi.c
  mmio/ri.c
  mmio/si.c
  mmio/vi.c
  mmio/ai.c
  mmio/pif.c
)

add_library(mem
  ${SOURCES}
)

target_include_directories(mem PUBLIC
  ../../../include/core/
  ../../../include/core/mem/
  ../../../include/core/mem/mmio/
)