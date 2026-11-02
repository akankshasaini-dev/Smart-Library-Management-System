// BST.cpp - ordinary Binary Search Tree (no balancing).
// Worst case (IDs inserted in sorted order) it degenerates into a linked list: O(n).
#include "BST.h"

#include <algorithm>

using namespace std;

BST::BST() : root(nullptr), nodeCount(0) {}

BST::~BST() { destroy(root); }

void BST::destroy(Node* node) {
    if (node == nullptr) return;
    destroy(node->left);
    destroy(node->right);
    delete node;
}

bool BST::insert(const BookPtr& book) {
    bool inserted = false;
    root = insertNode(root, book, inserted);
    if (inserted) ++nodeCount;
    return inserted;
}

BST::Node* BST::insertNode(Node* node, const BookPtr& book, bool& inserted) {
    if (node == nullptr) {
        inserted = true;
        return new Node(book);
    }
    if (book->id < node->book->id) {
        node->left = insertNode(node->left, book, inserted);
    } else if (book->id > node->book->id) {
        node->right = insertNode(node->right, book, inserted);
    }
    // equal ID: duplicate, do nothing (inserted stays false)
    return node;
}

BookPtr BST::search(int id, int& comparisons) const {
    comparisons = 0;
    const Node* current = root;
    while (current != nullptr) {
        ++comparisons;
        if (id == current->book->id) return current->book;
        current = (id < current->book->id) ? current->left : current->right;
    }
    return nullptr;
}

bool BST::remove(int id) {
    bool removed = false;
    root = removeNode(root, id, removed);
    if (removed) --nodeCount;
    return removed;
}

BST::Node* BST::minNode(Node* node) {
    while (node->left != nullptr) node = node->left;
    return node;
}

BST::Node* BST::removeNode(Node* node, int id, bool& removed) {
    if (node == nullptr) return nullptr;            // id not in tree

    if (id < node->book->id) {
        node->left = removeNode(node->left, id, removed);
    } else if (id > node->book->id) {
        node->right = removeNode(node->right, id, removed);
    } else {
        removed = true;
        if (node->left == nullptr || node->right == nullptr) {
            // Case 1 & 2: zero or one child -> replace node by its child
            Node* child = (node->left != nullptr) ? node->left : node->right;
            delete node;
            return child;
        }
        // Case 3: two children -> copy in-order successor, then delete the successor
        Node* successor = minNode(node->right);
        node->book = successor->book;
        bool ignored = false;
        node->right = removeNode(node->right, successor->book->id, ignored);
    }
    return node;
}

vector<BookPtr> BST::inOrder() const {
    vector<BookPtr> result;
    inOrderWalk(root, result);
    return result;
}

void BST::inOrderWalk(const Node* node, vector<BookPtr>& out) {
    if (node == nullptr) return;
    inOrderWalk(node->left, out);
    out.push_back(node->book);
    inOrderWalk(node->right, out);
}

int BST::heightOf(const Node* node) {
    if (node == nullptr) return 0;
    return 1 + max(heightOf(node->left), heightOf(node->right));
}

int BST::height() const { return heightOf(root); }
