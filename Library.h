// Library.h - the "brain" of the project. It owns the three data structures
// and keeps them synchronized. It never prints menus (that is main.cpp's job).
#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>
#include <vector>

#include "AVLTree.h"
#include "BST.h"
#include "Book.h"
#include "HashTable.h"

struct SearchComparison {
    bool found;
    int hashSteps;
    int bstSteps;
    int avlSteps;
};

class Library {
public:
    explicit Library(const std::string& dataFilePath);

    // ---- persistence ----
    bool loadFromFile(std::string& report);   // fills all 3 structures from the file
    bool saveToFile() const;                  // writes every book (sorted by ID)

    // ---- operations (each returns true on success, message explains result) ----
    bool addBook(int id, const std::string& title, const std::string& author, std::string& message);
    bool updateBook(int id, const std::string& newTitle, const std::string& newAuthor,
                    int newId, std::string& message);
    bool deleteBook(int id, std::string& message);
    bool issueBook(int id, const std::string& studentId, const std::string& studentName,
                   std::string& message);
    bool returnBook(int id, std::string& message);

    // ---- queries ----
    BookPtr findById(int id) const;                              // Hash Table
    std::vector<BookPtr> findByTitle(const std::string& keyword) const; // AVL in-order scan
    std::vector<BookPtr> allBooksSorted() const;                 // AVL in-order
    std::vector<BookPtr> availableBooks() const;
    std::vector<BookPtr> issuedBooks() const;
    bool contains(int id) const;                                 // Hash Table

    // ---- demo / diagnostics ----
    SearchComparison compareSearch(int id) const;
    bool verifyIntegrity(std::string& report) const;
    const AVLTree& avl() const { return avlIndex; }
    const BST& bst() const { return bstIndex; }
    const HashTable& hash() const { return hashTable; }
    std::size_t totalBooks() const { return hashTable.size(); }

private:
    std::string dataFile;
    HashTable hashTable;   // fast lookup by ID (primary)
    BST bstIndex;          // simple ordered index (educational / comparison)
    AVLTree avlIndex;      // balanced ordered index (sorted display, guaranteed O(log n))

    void insertIntoAll(const BookPtr& book);
    void removeFromAll(int id);
};

#endif
