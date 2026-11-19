#pragma once

#include "domain/OrderedSet.hpp"

namespace eda::domain {

// Nodo del AVL: clave, dos hijos y la altura del subarbol.
class AvlNode {
public:
    explicit AvlNode(Key key) : key(key) {}

    Key key;
    AvlNode* left = nullptr;
    AvlNode* right = nullptr;
    int height = 1;
};

}  // namespace eda::domain
