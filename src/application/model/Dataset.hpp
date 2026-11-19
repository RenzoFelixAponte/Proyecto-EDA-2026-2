#pragma once

#include <vector>

#include "domain/OrderedSet.hpp"

namespace eda::application {

// Datos de un escenario: claves a insertar (y luego eliminar en el mismo orden)
// y consultas de busqueda (50% presentes, 50% ausentes, en orden aleatorio).
class Dataset {
public:
    std::vector<domain::Key> insertKeys;
    std::vector<domain::Key> queries;
};

}  // namespace eda::application
