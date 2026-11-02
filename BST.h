// BST.h - plain (unbalanced) Binary Search Tree ordered by Book ID.
#ifndef BST_H
#define BST_H

#include <cstddef>
#include <vector>

#include "Book.h"

class BST {
public:
    BST();
    ~BST();
    BST(const BST&) = delete;
    BST& operator=(const BST&) = delete;

    bool insert(const BookPtr& book);                       // false on duplicate ID
    bool remove(int id);                                    // false if not found
    BookPtr search(int id, int& comparisons) const;
    std::vector<BookPtr> inOrder() const;                   // sorted by ID
    int height() const;                                     // empty tree = 0
    std::size_t size() const { return nodeCount; }

private:
    struct Node {
        BookPtr book;
        Node* left;
        Node* right;
        explicit Node(const BookPtr& b) : book(b), left(nullptr), right(nullptr) {}
    };

    Node* root;
    std::size_t nodeCount;

    Node* insertNode(Node* node, const BookPtr& book, bool& inserted);
    Node* removeNode(Node* node, int id, bool& removed);
    static Node* minNode(Node* node);
    static void inOrderWalk(const Node* node, std::vector<BookPtr>& out);
    static int heightOf(const Node* node);
    static void destroy(Node* node);
};

#endif
