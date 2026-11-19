#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "application/model/DataOrder.hpp"

namespace eda::application {

// Parametros del experimento (Tabla 2 del informe).
class BenchmarkConfig {
public:
    std::vector<std::size_t> sizes{1000, 10000, 100000, 1000000};
    std::vector<DataOrder> orders{DataOrder::Random, DataOrder::Sorted, DataOrder::Reversed};
    int repetitions = 10;
    int warmupRuns = 1;
    std::uint64_t baseSeed = 2026;
};

}  // namespace eda::application
