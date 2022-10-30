cmake_minimum_required(VERSION 3.20)
project(YAC VERSION 0.2.0 LANGUAGES CXX C)

# C++17 required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Build options
option(YAC_BUILD_TESTS "Build tests" ON)
option(YAC_ENABLE_LLVM "Enable LLVM backend" ON)
option(YAC_ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(YAC_ENABLE_UBSAN "Enable UndefinedBehaviorSanitizer" OFF)

# Compiler warnings
if(MSVC)
  add_compile_options(/W4)
else()
  add_compile_options(-Wall -Wextra -Wpedantic)
  if(YAC_ENABLE_ASAN)
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
  endif()
  if(YAC_ENABLE_UBSAN)
    add_compile_options(-fsanitize=undefined)
    add_link_options(-fsanitize=undefined)
  endif()
endif()

# Find dependencies (optional for now - we'll integrate parser later)
find_package(FLEX QUIET)
find_package(BISON 3.0 QUIET)

if(FLEX_FOUND AND BISON_FOUND)
  message(STATUS "Flex and Bison found - parser generation enabled")
  set(YAC_ENABLE_PARSER ON)
else()
  message(WARNING "Flex and/or Bison not found - parser generation disabled")
  message(WARNING "  Flex: ${FLEX_FOUND}, Bison: ${BISON_FOUND}")
  set(YAC_ENABLE_PARSER OFF)
endif()

# LLVM (optional for now)
if(YAC_ENABLE_LLVM)
  find_package(LLVM CONFIG)
  if(LLVM_FOUND)
    message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
    message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

    include_directories(${LLVM_INCLUDE_DIRS})
    separate_arguments(LLVM_DEFINITIONS_LIST NATIVE_COMMAND ${LLVM_DEFINITIONS})
    add_definitions(${LLVM_DEFINITIONS_LIST})
    add_definitions(-DYAC_ENABLE_LLVM)

    llvm_map_components_to_libnames(llvm_libs support core irreader)
  else()
    message(WARNING "LLVM not found. LLVM backend will be disabled.")
    set(YAC_ENABLE_LLVM OFF)
  endif()
endif()

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)

# Libraries
add_subdirectory(lib)

# Tools (main compiler)
add_subdirectory(tools)

# Tests
if(YAC_BUILD_TESTS)
  enable_testing()
  add_subdirectory(test)
endif()

# Install rules
install(TARGETS yac
        RUNTIME DESTINATION bin)
