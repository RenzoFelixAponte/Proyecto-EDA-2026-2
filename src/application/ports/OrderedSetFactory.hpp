#pragma once

#include <cstdint>
#include <memory>
#include <string>

#include "domain/OrderedSet.hpp"

namespace eda::application {

// Puerto de salida: crea instancias nuevas de una estructura.
// Agregar una estructura nueva = agregar un adaptador que implemente esto.
class OrderedSetFactory {
public:
    virtual ~OrderedSetFactory() = default;
    virtual std::unique_ptr<domain::OrderedSet> create(std::uint64_t seed) const = 0;
    virtual std::string name() const = 0;
};

}  // namespace eda::application
