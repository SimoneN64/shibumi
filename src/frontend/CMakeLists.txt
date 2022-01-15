project(frontend)

file(GLOB SOURCES "*.cpp")
file(GLOB HEADERS "*.hpp")

include(FindPkgConfig)
pkg_check_modules (CAPSTONE REQUIRED capstone)
add_subdirectory(external/)
add_subdirectory(../shibumi/ ../)

add_library(frontend
   ${HEADERS}
   ${SOURCES}
)

target_include_directories(frontend PUBLIC
    ${CAPSTONE_INCLUDE_DIRS}
    .
    ../shibumi/include/
    ../shibumi/include/
    ../shibumi/include/core/
    ../shibumi/include/core/cpu/
    ../shibumi/include/core/cpu/instruction/
    ../shibumi/include/core/cpu/registers/
    ../shibumi/include/core/mem/
    ../shibumi/include/core/mem/mmio/
    ../shibumi/include/core/rsp/
    ../shibumi/include/utils/
)

target_link_libraries(frontend PUBLIC ${CAPSTONE_LIBRARIES} core dependencies)