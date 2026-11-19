#pragma once

#include "application/ports/DatasetGenerator.hpp"

namespace eda::infrastructure {

// Adaptador: genera n claves distintas de 64 bits con un PRNG con semilla.
// Las claves presentes son pares y las ausentes impares, asi se garantiza que
// la mitad de las consultas falle sin necesidad de buscar en un conjunto.
class RandomDatasetGenerator final : public application::DatasetGenerator {
public:
    application::Dataset generate(std::size_t n, application::DataOrder order,
                                  std::uint64_t seed) const override;
};

}  // namespace eda::infrastructure
