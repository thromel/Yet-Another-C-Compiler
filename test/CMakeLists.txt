# Testing infrastructure

# Check if GoogleTest is available
find_package(GTest QUIET)

if(GTest_FOUND)
  message(STATUS "GoogleTest found - building unit tests")

  # Unit tests
  add_executable(yac_unit_tests
    unit/test_ast.cpp
    unit/test_type.cpp
    unit/test_diagnostic.cpp
  )

  target_link_libraries(yac_unit_tests PRIVATE
    YACLib
    GTest::GTest
    GTest::Main
  )

  target_include_directories(yac_unit_tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../include
  )

  # Register tests with CTest
  include(GoogleTest)
  gtest_discover_tests(yac_unit_tests)

else()
  message(WARNING "GoogleTest not found - skipping unit tests")
  message(WARNING "Install GoogleTest or use -DYAC_BUILD_TESTS=OFF")
endif()

# Integration tests (don't require GoogleTest)
add_subdirectory(integration)
