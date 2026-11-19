#include "domain/avl/AvlTree.hpp"

#include <algorithm>
#include <cstdlib>

namespace eda::domain {

AvlTree::~AvlTree() { destroy(root_); }

void AvlTree::destroy(AvlNode* node) {
    if (node == nullptr) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

int AvlTree::balanceOf(const AvlNode* node) {
    return heightOf(node->right) - heightOf(node->left);
}

void AvlTree::updateHeight(AvlNode* node) {
    node->height = 1 + std::max(heightOf(node->left), heightOf(node->right));
}

AvlNode* AvlTree::rotateLeft(AvlNode* node) {
    AvlNode* pivot = node->right;
    node->right = pivot->left;
    pivot->left = node;
    updateHeight(node);
    updateHeight(pivot);
    return pivot;
}

AvlNode* AvlTree::rotateRight(AvlNode* node) {
    AvlNode* pivot = node->left;
    node->left = pivot->right;
    pivot->right = node;
    updateHeight(node);
    updateHeight(pivot);
    return pivot;
}

AvlNode* AvlTree::rebalance(AvlNode* node) {
    updateHeight(node);
    const int balance = balanceOf(node);
    if (balance > 1) {                                   // pesado a la derecha
        if (balanceOf(node->right) < 0) node->right = rotateRight(node->right);  // caso RL
        return rotateLeft(node);
    }
    if (balance < -1) {                                  // pesado a la izquierda
        if (balanceOf(node->left) > 0) node->left = rotateLeft(node->left);      // caso LR
        return rotateRight(node);
    }
    return node;
}

bool AvlTree::search(Key key) const {
    const AvlNode* current = root_;
    while (current != nullptr) {
        if (key < current->key) current = current->left;
        else if (current->key < key) current = current->right;
        else return true;
    }
    return false;
}

bool AvlTree::insert(Key key) {
    bool inserted = false;
    root_ = insertAt(root_, key, inserted);
    if (inserted) ++size_;
    return inserted;
}

AvlNode* AvlTree::insertAt(AvlNode* node, Key key, bool& inserted) {
    if (node == nullptr) {
        inserted = true;
        return new AvlNode(key);
    }
    if (key < node->key) node->left = insertAt(node->left, key, inserted);
    else if (node->key < key) node->right = insertAt(node->right, key, inserted);
    else return node;  // duplicado
    return inserted ? rebalance(node) : node;
}

bool AvlTree::remove(Key key) {
    bool removed = false;
    root_ = removeAt(root_, key, removed);
    if (removed) --size_;
    return removed;
}

AvlNode* AvlTree::removeMin(AvlNode* node, AvlNode*& minNode) {
    if (node->left == nullptr) {
        minNode = node;
        return node->right;
    }
    node->left = removeMin(node->left, minNode);
    return rebalance(node);
}

AvlNode* AvlTree::removeAt(AvlNode* node, Key key, bool& removed) {
    if (node == nullptr) return nullptr;
    if (key < node->key) {
        node->left = removeAt(node->left, key, removed);
    } else if (node->key < key) {
        node->right = removeAt(node->right, key, removed);
    } else {
        removed = true;
        AvlNode* left = node->left;
        AvlNode* right = node->right;
        delete node;
        if (right == nullptr) return left;
        AvlNode* successor = nullptr;
        AvlNode* newRight = removeMin(right, successor);
        successor->left = left;
        successor->right = newRight;
        return rebalance(successor);
    }
    return removed ? rebalance(node) : node;
}

int AvlTree::validate(const AvlNode* node, const Key* low, const Key* high, std::size_t& count) {
    if (node == nullptr) return 0;
    if ((low && node->key <= *low) || (high && node->key >= *high)) return -1;
    ++count;
    const int lh = validate(node->left, low, &node->key, count);
    const int rh = validate(node->right, &node->key, high, count);
    if (lh < 0 || rh < 0 || std::abs(rh - lh) > 1) return -1;
    const int h = 1 + std::max(lh, rh);
    return h == node->height ? h : -1;
}

bool AvlTree::checkInvariants() const {
    std::size_t count = 0;
    return validate(root_, nullptr, nullptr, count) >= 0 && count == size_;
}

}  // namespace eda::domain
