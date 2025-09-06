cmake_minimum_required(VERSION 3.20)


project(mintime LANGUAGES C CXX VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED CXX)

if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang|clang")
  add_compile_options(-O3 -Wall -Wextra -fno-fast-math -ffp-contract=off)
endif()

add_executable(mintime
  src/main.cpp
  src/operations.cpp
  src/scheduler.cpp
  src/cpuinfo.cpp
)

target_include_directories(mintime PRIVATE src)
