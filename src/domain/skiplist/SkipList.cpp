#include "domain/skiplist/SkipList.hpp"

#include <stdexcept>

namespace eda::domain {

SkipList::SkipList(double p, int maxLevel, std::uint64_t seed)
    : p_(p),
      maxLevel_(maxLevel),
      head_(0, maxLevel),
      rng_(seed),
      coin_(p),
      update_(maxLevel, nullptr) {
    if (p <= 0.0 || p >= 1.0) throw std::invalid_argument("p debe estar en (0, 1)");
    if (maxLevel < 1) throw std::invalid_argument("maxLevel debe ser >= 1");
}

SkipList::~SkipList() {
    SkipListNode* node = head_.forward[0];
    while (node != nullptr) {
        SkipListNode* next = node->forward[0];
        delete node;
        node = next;
    }
}

int SkipList::randomLevel() {
    int level = 1;
    while (level < maxLevel_ && coin_(rng_)) ++level;
    return level;
}

SkipListNode* SkipList::findPredecessors(Key key, std::vector<SkipListNode*>& update) const {
    auto* current = const_cast<SkipListNode*>(&head_);
    for (int i = level_ - 1; i >= 0; --i) {
        while (current->forward[i] != nullptr && current->forward[i]->key < key) {
            current = current->forward[i];
        }
        update[i] = current;
    }
    return current->forward[0];
}

bool SkipList::search(Key key) const {
    const SkipListNode* current = &head_;
    for (int i = level_ - 1; i >= 0; --i) {
        while (current->forward[i] != nullptr && current->forward[i]->key < key) {
            current = current->forward[i];
        }
    }
    current = current->forward[0];
    return current != nullptr && current->key == key;
}

bool SkipList::insert(Key key) {
    SkipListNode* candidate = findPredecessors(key, update_);
    if (candidate != nullptr && candidate->key == key) return false;

    const int newLevel = randomLevel();
    if (newLevel > level_) {
        for (int i = level_; i < newLevel; ++i) update_[i] = &head_;
        level_ = newLevel;
    }

    auto* node = new SkipListNode(key, newLevel);
    for (int i = 0; i < newLevel; ++i) {
        node->forward[i] = update_[i]->forward[i];
        update_[i]->forward[i] = node;
    }
    ++size_;
    return true;
}

bool SkipList::remove(Key key) {
    SkipListNode* target = findPredecessors(key, update_);
    if (target == nullptr || target->key != key) return false;

    for (int i = 0; i < target->level(); ++i) {
        update_[i]->forward[i] = target->forward[i];
    }
    delete target;
    while (level_ > 1 && head_.forward[level_ - 1] == nullptr) --level_;
    --size_;
    return true;
}

bool SkipList::checkInvariants() const {
    // Cada nivel debe estar estrictamente ordenado y ser sublista del nivel inferior.
    std::size_t count = 0;
    for (const SkipListNode* n = head_.forward[0]; n != nullptr; n = n->forward[0]) {
        ++count;
        if (n->forward[0] != nullptr && n->forward[0]->key <= n->key) return false;
    }
    if (count != size_) return false;

    for (int i = 1; i < level_; ++i) {
        const SkipListNode* lower = head_.forward[i - 1];
        for (const SkipListNode* n = head_.forward[i]; n != nullptr; n = n->forward[i]) {
            while (lower != nullptr && lower != n) lower = lower->forward[i - 1];
            if (lower == nullptr) return false;
        }
    }
    return true;
}

}  // namespace eda::domain
