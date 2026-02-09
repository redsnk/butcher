cmake_minimum_required(VERSION 3.10.0)
project(Butcher VERSION 0.1.0 LANGUAGES CXX)

add_executable(Butcher main.cpp src/files/pe.cpp src/files/elf.cpp src/files/common.cpp src/butcher.cpp src/base/base_x64.cpp src/arch/arch_pe.cpp src/arch/arch_elf.cpp src/lang/language.cpp src/lang/lang_x64.cpp src/lang/lang_c_x64.cpp src/lang/lang_py_x64.cpp)
TARGET_LINK_LIBRARIES(Butcher capstone)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
