// Book.h - the single record type shared by every data structure.
#ifndef BOOK_H
#define BOOK_H

#include <memory>
#include <string>

// One Book object is created on the heap and shared (via shared_ptr) by the
// Hash Table, the BST and the AVL tree. Because they all point to the SAME
// object, changing a title/author is instantly visible in all three.
struct Book {
    int id;                  // unique Book ID (1 - 999999)
    std::string title;
    std::string author;
    bool isIssued;           // true when a student currently holds the book
    std::string studentId;   // valid only when isIssued == true
    std::string studentName; // valid only when isIssued == true

    Book() : id(0), isIssued(false) {}
};

using BookPtr = std::shared_ptr<Book>;

#endif
