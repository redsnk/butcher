cmake_minimum_required(VERSION 3.10.0)
project(Butcher VERSION 0.1.0 LANGUAGES CXX)

add_executable(Butcher main.cpp src/files/pe.cpp src/butcher.cpp src/base/base_x64.cpp src/arch/pe_x64.cpp)
TARGET_LINK_LIBRARIES(Butcher capstone)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
