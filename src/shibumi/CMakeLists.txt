project(core)

file(GLOB_RECURSE SOURCES "src/*.c")
file(GLOB_RECURSE HEADERS "include/*.h")

add_library(core
   ${SOURCES}
   ${HEADERS}
)