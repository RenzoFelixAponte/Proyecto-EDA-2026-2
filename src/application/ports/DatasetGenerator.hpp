#pragma once

#include <cstddef>
#include <cstdint>

#include "application/model/DataOrder.hpp"
#include "application/model/Dataset.hpp"

namespace eda::application {

// Puerto de salida: produce las claves y consultas de un escenario.
class DatasetGenerator {
public:
    virtual ~DatasetGenerator() = default;
    virtual Dataset generate(std::size_t n, DataOrder order, std::uint64_t seed) const = 0;
};

}  // namespace eda::application
