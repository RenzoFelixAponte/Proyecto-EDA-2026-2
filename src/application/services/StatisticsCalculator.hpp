#pragma once

#include <vector>

#include "application/model/Measurement.hpp"
#include "application/model/Summary.hpp"

namespace eda::application {

// Agrupa mediciones por (estructura, operacion, n, orden) y calcula media y
// desviacion estandar muestral.
class StatisticsCalculator {
public:
    std::vector<Summary> summarize(const std::vector<Measurement>& measurements) const;
};

}  // namespace eda::application
