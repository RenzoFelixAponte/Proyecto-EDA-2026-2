cmake_minimum_required(VERSION 3.16)
project(SkipListVsAvl CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()
set(CMAKE_CXX_FLAGS_RELEASE "-O2")
add_compile_options(-Wall -Wextra)

# Nucleo: dominio + aplicacion (sin dependencias de infraestructura)
add_library(core
  src/domain/skiplist/SkipList.cpp
  src/domain/avl/AvlTree.cpp
  src/application/services/StatisticsCalculator.cpp
  src/application/services/BenchmarkService.cpp)
target_include_directories(core PUBLIC src)

# Adaptadores
add_library(adapters
  src/infrastructure/data/RandomDatasetGenerator.cpp
  src/infrastructure/persistence/CsvResultRepository.cpp
  src/infrastructure/cli/CommandLineParser.cpp)
target_link_libraries(adapters PUBLIC core)

add_executable(benchmark src/main.cpp)
target_link_libraries(benchmark PRIVATE adapters)

enable_testing()
add_executable(correctness_tests tests/OrderedSetCorrectnessTest.cpp)
target_link_libraries(correctness_tests PRIVATE core)
add_test(NAME correctness COMMAND correctness_tests)
