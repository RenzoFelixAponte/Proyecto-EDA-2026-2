#pragma once

#include <cstddef>
#include <string>

#include "application/model/DataOrder.hpp"

namespace eda::application {

// Resultado agregado de las repeticiones de un escenario y una operacion.
class Summary {
public:
    std::string structure;
    std::string operation;
    std::size_t n = 0;
    DataOrder order = DataOrder::Random;
    int repetitions = 0;
    double meanNs = 0.0;
    double stdDevNs = 0.0;
    double meanHeight = 0.0;
};

}  // namespace eda::application
