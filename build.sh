#!/usr/bin/env sh
# Compila sin CMake (alternativa rapida). Uso: ./build.sh
set -e
mkdir -p build
FLAGS="-std=c++17 -O2 -Wall -Wextra -Isrc"
CORE="src/domain/skiplist/SkipList.cpp src/domain/avl/AvlTree.cpp \
      src/application/services/StatisticsCalculator.cpp src/application/services/BenchmarkService.cpp"
ADAPTERS="src/infrastructure/data/RandomDatasetGenerator.cpp \
          src/infrastructure/persistence/CsvResultRepository.cpp src/infrastructure/cli/CommandLineParser.cpp"
g++ $FLAGS $CORE $ADAPTERS src/main.cpp -o build/benchmark
g++ $FLAGS src/domain/skiplist/SkipList.cpp src/domain/avl/AvlTree.cpp \
    tests/OrderedSetCorrectnessTest.cpp -o build/correctness_tests
echo "Listo: build/benchmark y build/correctness_tests"
