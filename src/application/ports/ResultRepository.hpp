#pragma once

#include <vector>

#include "application/model/Measurement.hpp"
#include "application/model/Summary.hpp"

namespace eda::application {

// Puerto de salida: donde se guardan los resultados (CSV, consola, BD...).
class ResultRepository {
public:
    virtual ~ResultRepository() = default;
    virtual void saveMeasurements(const std::vector<Measurement>& measurements) = 0;
    virtual void saveSummaries(const std::vector<Summary>& summaries) = 0;
};

}  // namespace eda::application
