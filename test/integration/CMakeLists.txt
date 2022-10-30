# Integration tests - test the compiler end-to-end

# Simple test runner
add_executable(run_integration_tests
  test_runner.cpp
)

target_link_libraries(run_integration_tests PRIVATE
  YACLib
)

target_include_directories(run_integration_tests PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

# Add integration test
add_test(
  NAME IntegrationTests
  COMMAND run_integration_tests ${CMAKE_CURRENT_SOURCE_DIR}/fixtures
)
