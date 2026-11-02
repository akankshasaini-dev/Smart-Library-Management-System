// main.cpp - console menu for the Smart Library Management System.
// This file only handles input/output; all logic lives in Library.
#include <iomanip>
#include <iostream>
#include <string>
#include <vector>

#include "Library.h"
#include "Utils.h"

using namespace std;

static const char* DATA_FILE = "books.txt";   // created in the folder you run the program from
static const int MIN_BOOK_ID = 1;
static const int MAX_BOOK_ID = 999999;

// ---------------------------------------------------------------- display helpers

static string shorten(const string& text, size_t maxLength) {
    if (text.size() <= maxLength) return text;
    return text.substr(0, maxLength - 3) + "...";
}

static void printBooks(const vector<BookPtr>& books) {
    if (books.empty()) {
        cout << "  (no books to display)\n";
        return;
    }
    cout << left << setw(8) << "ID" << setw(32) << "Title" << setw(22) << "Author"
         << setw(11) << "Status" << "Issued To\n";
    cout << string(88, '-') << "\n";
    for (const BookPtr& book : books) {
        cout << left << setw(8) << book->id << setw(32) << shorten(book->title, 30)
             << setw(22) << shorten(book->author, 20)
             << setw(11) << (book->isIssued ? "Issued" : "Available");
        if (book->isIssued) {
            cout << book->studentName << " (" << book->studentId << ")";
        } else {
            cout << "-";
        }
        cout << "\n";
    }
    cout << string(88, '-') << "\n";
    cout << "  Total shown: " << books.size() << "\n";
}

static void saveAfterChange(const Library& library) {
    if (!library.saveToFile()) {
        cout << "  WARNING: could not write " << DATA_FILE << " - changes may be lost!\n";
    }
}

static void printMenu() {
    cout << "\n==========================================\n"
         << "   SMART LIBRARY MANAGEMENT SYSTEM\n"
         << "==========================================\n"
         << " 1.  Add a new book\n"
         << " 2.  Search book by ID        [Hash Table]\n"
         << " 3.  Search book by title     [AVL in-order scan]\n"
         << " 4.  Update book details\n"
         << " 5.  Delete a book\n"
         << " 6.  Issue a book to a student\n"
         << " 7.  Return an issued book\n"
         << " 8.  Display all books (sorted by ID)  [AVL in-order]\n"
         << " 9.  Display available books\n"
         << " 10. Display issued books\n"
         << " 11. Compare search steps (Hash vs BST vs AVL)\n"
         << " 12. Show data structure statistics / AVL tree picture\n"
         << " 13. Verify all data structures are in sync\n"
         << " 0.  Save and exit\n"
         << "------------------------------------------\n";
}

// ---------------------------------------------------------------- menu actions

static void addBookAction(Library& library) {
    cout << "\n--- Add a new book --- (duplicate check: Hash Table; insert: Hash + BST + AVL)\n";
    int id = Utils::readInt("Enter Book ID (" + to_string(MIN_BOOK_ID) + "-" +
                            to_string(MAX_BOOK_ID) + "): ", MIN_BOOK_ID, MAX_BOOK_ID);
    if (library.contains(id)) {
        cout << "  Error: a book with ID " << id << " already exists.\n";
        return;
    }
    string title = Utils::readLine("Enter Title : ");
    string author = Utils::readLine("Enter Author: ");

    string message;
    bool ok = library.addBook(id, title, author, message);
    cout << (ok ? "  " : "  Error: ") << message << "\n";
    if (ok) saveAfterChange(library);
}

static void searchByIdAction(const Library& library) {
    cout << "\n--- Search by Book ID --- (data structure: Hash Table)\n";
    int id = Utils::readInt("Enter Book ID: ", MIN_BOOK_ID, MAX_BOOK_ID);
    BookPtr book = library.findById(id);
    if (!book) {
        cout << "  No book found with ID " << id << ".\n";
        return;
    }
    printBooks({book});
}

static void searchByTitleAction(const Library& library) {
    cout << "\n--- Search by title --- (data structure: AVL tree in-order traversal)\n";
    string keyword = Utils::readLine("Enter title or part of title: ");
    vector<BookPtr> matches = library.findByTitle(keyword);
    if (matches.empty()) {
        cout << "  No book title contains \"" << keyword << "\".\n";
        return;
    }
    printBooks(matches);
}

static void updateBookAction(Library& library) {
    cout << "\n--- Update book --- (lookup: Hash Table; ID change re-inserts into Hash + BST + AVL)\n";
    int id = Utils::readInt("Enter Book ID to update: ", MIN_BOOK_ID, MAX_BOOK_ID);
    BookPtr book = library.findById(id);
    if (!book) {
        cout << "  Error: no book found with ID " << id << ".\n";
        return;
    }
    cout << "  Current details:\n";
    printBooks({book});

    string newTitle = Utils::readLine("New title  (press Enter to keep): ", true);
    string newAuthor = Utils::readLine("New author (press Enter to keep): ", true);
    int newId = id;
    if (Utils::readYesNo("Change the Book ID as well? (y/n): ")) {
        newId = Utils::readInt("Enter new Book ID: ", MIN_BOOK_ID, MAX_BOOK_ID);
    }

    string message;
    bool ok = library.updateBook(id, newTitle, newAuthor, newId, message);
    cout << (ok ? "  " : "  Error: ") << message << "\n";
    if (ok) saveAfterChange(library);
}

static void deleteBookAction(Library& library) {
    cout << "\n--- Delete book --- (removed from Hash Table, BST and AVL tree)\n";
    int id = Utils::readInt("Enter Book ID to delete: ", MIN_BOOK_ID, MAX_BOOK_ID);
    BookPtr book = library.findById(id);
    if (!book) {
        cout << "  Error: no book found with ID " << id << ". Nothing deleted.\n";
        return;
    }
    printBooks({book});
    if (!Utils::readYesNo("Really delete this book? (y/n): ")) {
        cout << "  Deletion cancelled.\n";
        return;
    }
    string message;
    bool ok = library.deleteBook(id, message);
    cout << (ok ? "  " : "  Error: ") << message << "\n";
    if (ok) saveAfterChange(library);
}

static void issueBookAction(Library& library) {
    cout << "\n--- Issue book --- (lookup: Hash Table)\n";
    int id = Utils::readInt("Enter Book ID to issue: ", MIN_BOOK_ID, MAX_BOOK_ID);
    BookPtr book = library.findById(id);
    if (!book) {
        cout << "  Error: no book found with ID " << id << ".\n";
        return;
    }
    if (book->isIssued) {
        cout << "  Error: book is already issued to " << book->studentName
             << " (" << book->studentId << ").\n";
        return;
    }
    string studentId = Utils::readStudentId("Enter Student ID  : ");
    string studentName = Utils::readLine("Enter Student name: ");

    string message;
    bool ok = library.issueBook(id, studentId, studentName, message);
    cout << (ok ? "  " : "  Error: ") << message << "\n";
    if (ok) saveAfterChange(library);
}

static void returnBookAction(Library& library) {
    cout << "\n--- Return book --- (lookup: Hash Table)\n";
    int id = Utils::readInt("Enter Book ID to return: ", MIN_BOOK_ID, MAX_BOOK_ID);
    string message;
    bool ok = library.returnBook(id, message);
    cout << (ok ? "  " : "  Error: ") << message << "\n";
    if (ok) saveAfterChange(library);
}

static void compareSearchAction(const Library& library) {
    cout << "\n--- Compare search steps for one Book ID ---\n";
    int id = Utils::readInt("Enter Book ID: ", MIN_BOOK_ID, MAX_BOOK_ID);
    SearchComparison result = library.compareSearch(id);
    cout << "  Book " << (result.found ? "FOUND" : "NOT FOUND") << ". Comparisons needed:\n"
         << "    Hash Table (chain scan) : " << result.hashSteps << "\n"
         << "    BST                     : " << result.bstSteps << "\n"
         << "    AVL tree                : " << result.avlSteps << "\n"
         << "  (BST can be slow if IDs were added in sorted order; AVL stays balanced.)\n";
}

static void showStatisticsAction(const Library& library) {
    cout << "\n--- Data structure statistics ---\n"
         << "  Books stored          : " << library.totalBooks() << "\n"
         << "  Hash table buckets    : " << library.hash().bucketCount()
         << " (load factor " << fixed << setprecision(2) << library.hash().loadFactor()
         << ", longest chain " << library.hash().longestChain() << ")\n"
         << "  BST height            : " << library.bst().height() << "\n"
         << "  AVL height            : " << library.avl().height() << "\n"
         << "  AVL rotations so far  : " << library.avl().rotationCount()
         << " (this session)\n";
    cout.unsetf(ios::floatfield);
    cout << "\n  AVL tree picture (rotated 90 degrees, root at far left; h = height, bf = balance factor):\n\n";
    if (library.totalBooks() > 40) {
        cout << "  (tree has more than 40 books - picture skipped to keep the screen readable)\n";
    } else {
        library.avl().printSideways(cout);
    }
}

static void verifyAction(const Library& library) {
    cout << "\n--- Integrity check ---\n";
    string report;
    library.verifyIntegrity(report);
    cout << report;
}

// ---------------------------------------------------------------- main

int main() {
    Library library(DATA_FILE);

    string loadReport;
    library.loadFromFile(loadReport);
    cout << loadReport << "\n";

    while (true) {
        printMenu();
        int choice = Utils::readInt("Enter your choice (0-13): ", 0, 13);

        switch (choice) {
            case 1:  addBookAction(library); break;
            case 2:  searchByIdAction(library); break;
            case 3:  searchByTitleAction(library); break;
            case 4:  updateBookAction(library); break;
            case 5:  deleteBookAction(library); break;
            case 6:  issueBookAction(library); break;
            case 7:  returnBookAction(library); break;
            case 8:
                cout << "\n--- All books sorted by ID (AVL in-order traversal) ---\n";
                printBooks(library.allBooksSorted());
                break;
            case 9:
                cout << "\n--- Available books ---\n";
                printBooks(library.availableBooks());
                break;
            case 10:
                cout << "\n--- Issued books ---\n";
                printBooks(library.issuedBooks());
                break;
            case 11: compareSearchAction(library); break;
            case 12: showStatisticsAction(library); break;
            case 13: verifyAction(library); break;
            case 0:
                if (library.saveToFile()) {
                    cout << "\nData saved to " << DATA_FILE << ". Goodbye!\n";
                } else {
                    cout << "\nWARNING: could not save data to " << DATA_FILE << ".\n";
                }
                return 0;
        }
    }
}
