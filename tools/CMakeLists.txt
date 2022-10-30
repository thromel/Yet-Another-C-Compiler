# YAC Compiler Driver

add_executable(yac
  yac/main.cpp
)

target_link_libraries(yac PRIVATE
  YACLib
)

target_include_directories(yac PRIVATE
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
