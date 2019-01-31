cmake_minimum_required(VERSION 3.9)

find_package(LLVM REQUIRED CONFIG)
message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")


list(APPEND CMAKE_PREFIX_PATH "${LLVM_DIR}/share/llvm/cmake")

list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(HandleLLVMOptions)
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})
set(LLVM_RUNTIME_OUTPUT_INTDIR "${CMAKE_BINARY_DIR}/bin/${CMAKE_CFG_INT_DIR}")
set(LLVM_LIBRARY_OUTPUT_INTDIR "${CMAKE_BINARY_DIR}/lib/${CMAKE_CFG_INT_DIR}")

add_subdirectory (src)
