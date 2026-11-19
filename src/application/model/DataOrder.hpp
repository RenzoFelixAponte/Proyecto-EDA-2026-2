#pragma once

#include <stdexcept>
#include <string>

namespace eda::application {

// Orden en que llegan las claves a la estructura.
enum class DataOrder { Random, Sorted, Reversed };

inline std::string toString(DataOrder order) {
    switch (order) {
        case DataOrder::Random:   return "aleatorio";
        case DataOrder::Sorted:   return "ordenado";
        case DataOrder::Reversed: return "inverso";
    }
    return "?";
}

inline DataOrder dataOrderFromString(const std::string& text) {
    if (text == "aleatorio" || text == "random") return DataOrder::Random;
    if (text == "ordenado" || text == "sorted") return DataOrder::Sorted;
    if (text == "inverso" || text == "reversed") return DataOrder::Reversed;
    throw std::invalid_argument("Orden desconocido: " + text);
}

}  // namespace eda::application
