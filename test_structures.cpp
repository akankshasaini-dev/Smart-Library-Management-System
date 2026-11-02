// test_structures.cpp - randomized stress test for BST, AVL tree and Hash Table.
// Compare every result against std::set and check the AVL invariants.
// Build:  g++ -std=c++17 -Iinclude tests/test_structures.cpp src/AVLTree.cpp src/BST.cpp src/HashTable.cpp -o test_structures
#include <cstdlib>
#include <iostream>
#include <set>

#include "AVLTree.h"
#include "BST.h"
#include "HashTable.h"

using namespace std;

static int failures = 0;
#define CHECK(condition) do { if (!(condition)) { ++failures; cout << "FAILED line " << __LINE__ << ": " #condition "\n"; } } while (0)

static BookPtr makeBook(int id) {
    BookPtr book = make_shared<Book>();
    book->id = id;
    book->title = "T" + to_string(id);
    return book;
}

int main() {
    srand(12345);
    AVLTree avl;
    BST bst;
    HashTable table;
    set<int> reference;

    for (int step = 0; step < 20000; ++step) {
        int id = 1 + rand() % 500;
        bool doInsert = (rand() % 3) != 0;    // 2/3 inserts, 1/3 deletes
        int steps = 0;
        if (doInsert) {
            bool expected = reference.insert(id).second;
            BookPtr book = makeBook(id);
            CHECK(avl.insert(book) == expected);
            CHECK(bst.insert(book) == expected);
            CHECK(table.insert(book) == expected);
        } else {
            bool expected = reference.erase(id) > 0;
            CHECK(avl.remove(id) == expected);
            CHECK(bst.remove(id) == expected);
            CHECK(table.remove(id) == expected);
        }
        CHECK(avl.isValid());
        CHECK(avl.size() == reference.size());
        CHECK(bst.size() == reference.size());
        CHECK(table.size() == reference.size());
        bool present = reference.count(id) > 0;
        CHECK((avl.search(id, steps) != nullptr) == present);
        CHECK((bst.search(id, steps) != nullptr) == present);
        CHECK((table.search(id, steps) != nullptr) == present);
    }

    // sorted order must match std::set
    vector<BookPtr> sortedAvl = avl.inOrder();
    vector<BookPtr> sortedBst = bst.inOrder();
    CHECK(sortedAvl.size() == reference.size());
    size_t i = 0;
    for (int id : reference) {
        CHECK(sortedAvl[i]->id == id);
        CHECK(sortedBst[i]->id == id);
        ++i;
    }

    // ascending inserts: BST degenerates, AVL stays logarithmic
    AVLTree avl2;
    BST bst2;
    for (int id = 1; id <= 1023; ++id) {
        BookPtr book = makeBook(id);
        avl2.insert(book);
        bst2.insert(book);
    }
    cout << "Ascending 1..1023  ->  AVL height " << avl2.height() << ", BST height " << bst2.height() << "\n";
    CHECK(avl2.height() == 10);
    CHECK(bst2.height() == 1023);
    CHECK(avl2.isValid());

    cout << (failures == 0 ? "ALL TESTS PASSED\n" : "SOME TESTS FAILED\n");
    return failures == 0 ? 0 : 1;
}
