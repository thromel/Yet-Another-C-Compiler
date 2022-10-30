# YAC Library

# Basic library (diagnostics, source locations)
add_library(YACBasic
  Basic/Diagnostic.cpp
)
target_include_directories(YACBasic PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# AST library
add_library(YACAST
  AST/AST.cpp
  AST/ASTVisitor.cpp
)
target_include_directories(YACAST PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_link_libraries(YACAST PUBLIC YACBasic)

# Type library
add_library(YACType
  Type/Type.cpp
)
target_include_directories(YACType PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)

# Parse library
add_library(YACParse
  Parse/Token.cpp
  Parse/Lexer.cpp
  Parse/Parser.cpp
)
target_include_directories(YACParse PUBLIC
  ${CMAKE_CURRENT_SOURCE_DIR}/../include
)
target_link_libraries(YACParse PUBLIC
  YACBasic
  YACAST
  YACType
)

# Combined library for convenience
add_library(YACLib
  INTERFACE
)
target_link_libraries(YACLib INTERFACE
  YACBasic
  YACAST
  YACType
  YACParse
)
