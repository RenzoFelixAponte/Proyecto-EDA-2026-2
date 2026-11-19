#pragma once

#include "domain/OrderedSet.hpp"
#include "domain/avl/AvlNode.hpp"

namespace eda::domain {

// AVL Tree (Adel'son-Vel'skii y Landis, 1962): BST con |factor de balance| <= 1.
class AvlTree final : public OrderedSet {
public:
    AvlTree() = default;
    ~AvlTree() override;

    AvlTree(const AvlTree&) = delete;
    AvlTree& operator=(const AvlTree&) = delete;

    bool insert(Key key) override;
    bool search(Key key) const override;
    bool remove(Key key) override;

    std::size_t size() const override { return size_; }
    std::size_t height() const override { return static_cast<std::size_t>(heightOf(root_)); }
    bool checkInvariants() const override;
    std::string name() const override { return "AVL"; }

private:
    static int heightOf(const AvlNode* node) { return node ? node->height : 0; }
    static int balanceOf(const AvlNode* node);
    static void updateHeight(AvlNode* node);
    static AvlNode* rotateLeft(AvlNode* node);
    static AvlNode* rotateRight(AvlNode* node);
    static AvlNode* rebalance(AvlNode* node);
    static void destroy(AvlNode* node);

    AvlNode* insertAt(AvlNode* node, Key key, bool& inserted);
    AvlNode* removeAt(AvlNode* node, Key key, bool& removed);
    AvlNode* removeMin(AvlNode* node, AvlNode*& minNode);
    // Devuelve la altura real o -1 si se rompe algun invariante.
    static int validate(const AvlNode* node, const Key* low, const Key* high, std::size_t& count);

    AvlNode* root_ = nullptr;
    std::size_t size_ = 0;
};

}  // namespace eda::domain
