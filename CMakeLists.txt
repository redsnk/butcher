cmake_minimum_required(VERSION 3.10.0)
project(Butcher VERSION 0.1.0 LANGUAGES C)

add_executable(Butcher main.c src/files/pe.c src/butcher.c src/arch/pe_x64.c)
TARGET_LINK_LIBRARIES(Butcher capstone)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
