#pragma once

#include <array>
#include <cstdint>
#include <random>

#include "domain/OrderedSet.hpp"
#include "domain/skiplist/SkipListNode.hpp"

namespace eda::domain {

// Skip List de Pugh (1990): niveles enlazados con altura aleatoria por nodo.
class SkipList final : public OrderedSet {
public:
    static constexpr int kDefaultMaxLevel = 20;

    explicit SkipList(double p = 0.5, int maxLevel = kDefaultMaxLevel,
                      std::uint64_t seed = 42);
    ~SkipList() override;

    SkipList(const SkipList&) = delete;
    SkipList& operator=(const SkipList&) = delete;

    bool insert(Key key) override;
    bool search(Key key) const override;
    bool remove(Key key) override;

    std::size_t size() const override { return size_; }
    std::size_t height() const override { return static_cast<std::size_t>(level_); }
    bool checkInvariants() const override;
    std::string name() const override { return "SkipList"; }

private:
    int randomLevel();
    // Llena update[i] con el ultimo nodo de nivel i cuya clave es < key.
    SkipListNode* findPredecessors(Key key, std::vector<SkipListNode*>& update) const;

    double p_;
    int maxLevel_;
    int level_ = 1;  // niveles en uso actualmente
    std::size_t size_ = 0;
    SkipListNode head_;
    std::mt19937_64 rng_;
    std::bernoulli_distribution coin_;
    mutable std::vector<SkipListNode*> update_;  // buffer reutilizable
};

}  // namespace eda::domain
