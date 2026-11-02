// AVLTree.cpp - self-balancing binary search tree.
//
// Height of a node  : 1 + max(height(left), height(right)); empty subtree = 0.
// Balance factor    : height(left) - height(right). Allowed values: -1, 0, +1.
// After every insert/delete we walk back up and fix any node whose balance
// factor became +2 or -2 using one or two rotations.
#include "AVLTree.h"

#include <algorithm>
#include <cstdlib>
#include <iostream>

using namespace std;

AVLTree::AVLTree() : root(nullptr), nodeCount(0), rotations(0) {}

AVLTree::~AVLTree() { destroy(root); }

void AVLTree::destroy(Node* node) {
    if (node == nullptr) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

int AVLTree::heightOf(const Node* node) { return node == nullptr ? 0 : node->height; }

int AVLTree::balanceFactor(const Node* node) {
    return node == nullptr ? 0 : heightOf(node->left) - heightOf(node->right);
}

void AVLTree::updateHeight(Node* node) {
    node->height = 1 + max(heightOf(node->left), heightOf(node->right));
}

/* Right rotation (used for Left-Left case):
         y                x
        / \              / \
       x   C    ==>     A   y
      / \                  / \
     A   B                B   C
   x becomes the new root of the subtree; B moves from x's right to y's left. */
AVLTree::Node* AVLTree::rotateRight(Node* y) {
    Node* x = y->left;
    Node* middle = x->right;
    x->right = y;
    y->left = middle;
    updateHeight(y);    // y is now lower, so update it first
    updateHeight(x);
    ++rotations;
    return x;           // x is the new root of this subtree
}

// Left rotation (used for Right-Right case) - the mirror image.
AVLTree::Node* AVLTree::rotateLeft(Node* x) {
    Node* y = x->right;
    Node* middle = y->left;
    y->left = x;
    x->right = middle;
    updateHeight(x);
    updateHeight(y);
    ++rotations;
    return y;
}

AVLTree::Node* AVLTree::rebalance(Node* node) {
    updateHeight(node);
    int balance = balanceFactor(node);

    if (balance > 1) {                                  // left side too tall
        if (balanceFactor(node->left) < 0) {            // Left-Right case
            node->left = rotateLeft(node->left);
        }
        return rotateRight(node);                       // Left-Left case
    }
    if (balance < -1) {                                 // right side too tall
        if (balanceFactor(node->right) > 0) {           // Right-Left case
            node->right = rotateRight(node->right);
        }
        return rotateLeft(node);                        // Right-Right case
    }
    return node;                                        // already balanced
}

bool AVLTree::insert(const BookPtr& book) {
    bool inserted = false;
    root = insertNode(root, book, inserted);
    if (inserted) ++nodeCount;
    return inserted;
}

AVLTree::Node* AVLTree::insertNode(Node* node, const BookPtr& book, bool& inserted) {
    if (node == nullptr) {
        inserted = true;
        return new Node(book);
    }
    if (book->id < node->book->id) {
        node->left = insertNode(node->left, book, inserted);
    } else if (book->id > node->book->id) {
        node->right = insertNode(node->right, book, inserted);
    } else {
        return node;                                    // duplicate: nothing changes
    }
    return rebalance(node);                             // fix heights/balance on the way up
}

bool AVLTree::remove(int id) {
    bool removed = false;
    root = removeNode(root, id, removed);
    if (removed) --nodeCount;
    return removed;
}

AVLTree::Node* AVLTree::minNode(Node* node) {
    while (node->left != nullptr) node = node->left;
    return node;
}

AVLTree::Node* AVLTree::removeNode(Node* node, int id, bool& removed) {
    if (node == nullptr) return nullptr;                // id not in tree

    if (id < node->book->id) {
        node->left = removeNode(node->left, id, removed);
    } else if (id > node->book->id) {
        node->right = removeNode(node->right, id, removed);
    } else {
        removed = true;
        if (node->left == nullptr || node->right == nullptr) {
            Node* child = (node->left != nullptr) ? node->left : node->right;
            delete node;
            return child;                               // may be nullptr (leaf); child is balanced already
        }
        Node* successor = minNode(node->right);         // two children: use in-order successor
        node->book = successor->book;
        bool ignored = false;
        node->right = removeNode(node->right, successor->book->id, ignored);
    }
    return rebalance(node);                             // fix heights/balance on the way up
}

BookPtr AVLTree::search(int id, int& comparisons) const {
    comparisons = 0;
    const Node* current = root;
    while (current != nullptr) {
        ++comparisons;
        if (id == current->book->id) return current->book;
        current = (id < current->book->id) ? current->left : current->right;
    }
    return nullptr;
}

vector<BookPtr> AVLTree::inOrder() const {
    vector<BookPtr> result;
    inOrderWalk(root, result);
    return result;
}

void AVLTree::inOrderWalk(const Node* node, vector<BookPtr>& out) {
    if (node == nullptr) return;
    inOrderWalk(node->left, out);
    out.push_back(node->book);
    inOrderWalk(node->right, out);
}

int AVLTree::height() const { return heightOf(root); }

// Self-check used by the integrity menu option and by the test program.
bool AVLTree::validate(const Node* node, long low, long high, int& heightOut) {
    if (node == nullptr) {
        heightOut = 0;
        return true;
    }
    long id = node->book->id;
    if (id <= low || id >= high) return false;          // BST ordering broken
    int leftHeight = 0, rightHeight = 0;
    if (!validate(node->left, low, id, leftHeight)) return false;
    if (!validate(node->right, id, high, rightHeight)) return false;
    if (abs(leftHeight - rightHeight) > 1) return false; // AVL balance broken
    heightOut = 1 + max(leftHeight, rightHeight);
    return heightOut == node->height;                    // stored height correct?
}

bool AVLTree::isValid() const {
    int h = 0;
    return validate(root, -1L, 2000000000L, h);
}

// Prints the tree rotated 90 degrees: right subtree on top, left at the bottom.
void AVLTree::printNode(const Node* node, int depth, ostream& out) {
    if (node == nullptr) return;
    printNode(node->right, depth + 1, out);
    int bf = balanceFactor(node);
    out << string(static_cast<size_t>(depth) * 8, ' ') << node->book->id
        << " [h=" << node->height << ", bf=" << (bf > 0 ? "+" : "") << bf << "]\n";
    printNode(node->left, depth + 1, out);
}

void AVLTree::printSideways(ostream& out) const {
    if (root == nullptr) {
        out << "  (tree is empty)\n";
        return;
    }
    printNode(root, 0, out);
}
