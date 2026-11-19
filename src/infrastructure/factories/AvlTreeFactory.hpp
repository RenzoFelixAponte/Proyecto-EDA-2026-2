#pragma once

#include "application/ports/OrderedSetFactory.hpp"
#include "domain/avl/AvlTree.hpp"

namespace eda::infrastructure {

// Adaptador: crea AVL Trees (la semilla no se usa: el AVL es determinista).
class AvlTreeFactory final : public application::OrderedSetFactory {
public:
    std::unique_ptr<domain::OrderedSet> create(std::uint64_t /*seed*/) const override {
        return std::make_unique<domain::AvlTree>();
    }
    std::string name() const override { return "AVL"; }
};

}  // namespace eda::infrastructure
