#pragma once

#include <vector>

#include "domain/OrderedSet.hpp"

namespace eda::domain {

// Nodo de la Skip List: una clave y un puntero "forward" por cada nivel.
class SkipListNode {
public:
    SkipListNode(Key key, int level) : key(key), forward(level, nullptr) {}

    int level() const { return static_cast<int>(forward.size()); }

    Key key;
    std::vector<SkipListNode*> forward;
};

}  // namespace eda::domain
