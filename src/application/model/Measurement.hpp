#pragma once

#include <cstddef>
#include <string>

#include "application/model/DataOrder.hpp"

namespace eda::application {

// Una medicion cruda: una operacion, en una repeticion, de un escenario.
class Measurement {
public:
    std::string structure;
    std::string operation;  // insert | search | remove
    std::size_t n = 0;
    DataOrder order = DataOrder::Random;
    int repetition = 0;
    double nsPerOp = 0.0;
    std::size_t height = 0;  // altura/niveles tras construir la estructura
};

}  // namespace eda::application
