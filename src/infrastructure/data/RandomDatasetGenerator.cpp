#include "infrastructure/data/RandomDatasetGenerator.hpp"

#include <algorithm>
#include <functional>
#include <random>
#include <unordered_set>

namespace eda::infrastructure {

using application::DataOrder;
using application::Dataset;
using domain::Key;

Dataset RandomDatasetGenerator::generate(std::size_t n, DataOrder order, std::uint64_t seed) const {
    std::mt19937_64 rng(seed);
    // Rango [0, 2^61): al multiplicar por 2 (o 2k+1) no hay desborde.
    std::uniform_int_distribution<Key> dist(0, (Key{1} << 61) - 1);

    Dataset data;
    data.insertKeys.reserve(n);
    std::unordered_set<Key> used;
    used.reserve(n * 2);
    while (data.insertKeys.size() < n) {
        const Key key = dist(rng) * 2;  // par => presente
        if (used.insert(key).second) data.insertKeys.push_back(key);
    }

    switch (order) {
        case DataOrder::Random:   break;  // ya estan en orden aleatorio
        case DataOrder::Sorted:   std::sort(data.insertKeys.begin(), data.insertKeys.end()); break;
        case DataOrder::Reversed:
            std::sort(data.insertKeys.begin(), data.insertKeys.end(), std::greater<>());
            break;
    }

    data.queries.reserve(n);
    std::uniform_int_distribution<std::size_t> pick(0, n - 1);
    for (std::size_t i = 0; i < n; ++i) {
        if (i % 2 == 0) data.queries.push_back(data.insertKeys[pick(rng)]);  // presente
        else data.queries.push_back(dist(rng) * 2 + 1);                      // ausente
    }
    std::shuffle(data.queries.begin(), data.queries.end(), rng);
    return data;
}

}  // namespace eda::infrastructure
