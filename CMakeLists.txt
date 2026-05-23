cmake_minimum_required(VERSION 3.10.0)
project(Butcher VERSION 0.1.0 LANGUAGES CXX)

add_compile_options(-std=c++17)
add_compile_options(-I/usr/include/jsoncpp)

add_executable(Butcher main.cpp src/files/pe.cpp src/files/elf.cpp src/files/common.cpp src/butcher.cpp src/code.cpp src/base/base_x64.cpp src/arch/archive.cpp src/arch/arch_pe.cpp src/arch/arch_elf.cpp src/lang/emit.cpp src/lang/scanner.cpp src/lang/parser.cpp src/lang/language.cpp src/lang/lang_x64.cpp src/lang/lang_c_x64.cpp src/lang/lang_py_x64.cpp)
TARGET_LINK_LIBRARIES(Butcher capstone jsoncpp)

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
