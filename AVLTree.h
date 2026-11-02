// AVLTree.h - self-balancing BST. Keeps |balance factor| <= 1 using rotations.
#ifndef AVLTREE_H
#define AVLTREE_H

#include <cstddef>
#include <iosfwd>
#include <vector>

#include "Book.h"

class AVLTree {
public:
    AVLTree();
    ~AVLTree();
    AVLTree(const AVLTree&) = delete;
    AVLTree& operator=(const AVLTree&) = delete;

    bool insert(const BookPtr& book);                       // false on duplicate ID
    bool remove(int id);                                    // false if not found
    BookPtr search(int id, int& comparisons) const;
    std::vector<BookPtr> inOrder() const;                   // sorted by ID
    int height() const;                                     // empty tree = 0
    std::size_t size() const { return nodeCount; }
    int rotationCount() const { return rotations; }         // for demonstration
    bool isValid() const;                                   // checks order, heights, balance
    void printSideways(std::ostream& out) const;            // text picture of the tree

private:
    struct Node {
        BookPtr book;
        Node* left;
        Node* right;
        int height;     // height of the subtree rooted here (leaf = 1)
        explicit Node(const BookPtr& b) : book(b), left(nullptr), right(nullptr), height(1) {}
    };

    Node* root;
    std::size_t nodeCount;
    int rotations;

    static int heightOf(const Node* node);
    static int balanceFactor(const Node* node);             // height(left) - height(right)
    static void updateHeight(Node* node);
    Node* rotateRight(Node* y);                             // fixes Left-Left
    Node* rotateLeft(Node* x);                              // fixes Right-Right
    Node* rebalance(Node* node);                            // picks LL / LR / RR / RL case

    Node* insertNode(Node* node, const BookPtr& book, bool& inserted);
    Node* removeNode(Node* node, int id, bool& removed);
    static Node* minNode(Node* node);
    static void inOrderWalk(const Node* node, std::vector<BookPtr>& out);
    static bool validate(const Node* node, long low, long high, int& heightOut);
    static void printNode(const Node* node, int depth, std::ostream& out);
    static void destroy(Node* node);
};

#endif
