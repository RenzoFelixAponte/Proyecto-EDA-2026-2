#pragma once

#include "application/ports/OrderedSetFactory.hpp"
#include "domain/skiplist/SkipList.hpp"

namespace eda::infrastructure {

// Adaptador: crea Skip Lists con p y MaxLevel configurables.
class SkipListFactory final : public application::OrderedSetFactory {
public:
    explicit SkipListFactory(double p = 0.5, int maxLevel = domain::SkipList::kDefaultMaxLevel)
        : p_(p), maxLevel_(maxLevel) {}

    std::unique_ptr<domain::OrderedSet> create(std::uint64_t seed) const override {
        return std::make_unique<domain::SkipList>(p_, maxLevel_, seed);
    }

    std::string name() const override {
        return p_ == 0.5 ? "SkipList" : "SkipList(p=" + std::to_string(p_).substr(0, 4) + ")";
    }

private:
    double p_;
    int maxLevel_;
};

}  // namespace eda::infrastructure
