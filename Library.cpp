// Library.cpp - business logic + file persistence.
//
// SYNCHRONIZATION RULE: every change goes through insertIntoAll() /
// removeFromAll(), so the Hash Table, BST and AVL tree always hold exactly
// the same set of books.
//
// File format (one book per line, fields separated by '|'):
//   id|title|author|issued(0/1)|studentId|studentName
// Unused student fields are written as "-".
#include "Library.h"

#include <fstream>
#include <sstream>

#include "Utils.h"

using namespace std;

Library::Library(const string& dataFilePath) : dataFile(dataFilePath) {}

// ------------------------------------------------------------------ sync helpers

void Library::insertIntoAll(const BookPtr& book) {
    hashTable.insert(book);
    bstIndex.insert(book);
    avlIndex.insert(book);
}

void Library::removeFromAll(int id) {
    hashTable.remove(id);
    bstIndex.remove(id);
    avlIndex.remove(id);
}

// ------------------------------------------------------------------ persistence

bool Library::saveToFile() const {
    ofstream out(dataFile.c_str());
    if (!out) return false;
    for (const BookPtr& book : avlIndex.inOrder()) {   // sorted order by ID
        out << book->id << '|' << book->title << '|' << book->author << '|'
            << (book->isIssued ? 1 : 0) << '|'
            << (book->isIssued ? book->studentId : "-") << '|'
            << (book->isIssued ? book->studentName : "-") << '\n';
    }
    return static_cast<bool>(out);
}

bool Library::loadFromFile(string& report) {
    ifstream in(dataFile.c_str());
    if (!in) {
        report = "No data file found. Starting with an empty library.";
        return true;                                   // first run is not an error
    }

    size_t loaded = 0, skipped = 0;
    string line;
    while (getline(in, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();  // Windows line endings
        if (Utils::trim(line).empty()) continue;

        vector<string> fields;
        stringstream parts(line);
        string field;
        while (getline(parts, field, '|')) fields.push_back(field);

        bool valid = fields.size() >= 6;
        int id = 0;
        if (valid) {
            try {
                id = stoi(fields[0]);
            } catch (...) {
                valid = false;
            }
        }
        if (valid && (id < 1 || id > 999999)) valid = false;
        if (valid && (fields[3] != "0" && fields[3] != "1")) valid = false;
        if (valid && fields[1].empty()) valid = false;
        if (valid && contains(id)) valid = false;           // duplicate ID in file
        if (!valid) {
            ++skipped;
            continue;
        }
        BookPtr book = make_shared<Book>();
        book->id = id;
        book->title = fields[1];
        book->author = fields[2];
        book->isIssued = (fields[3] == "1");
        if (book->isIssued) {
            book->studentId = fields[4];
            book->studentName = fields[5];
        }
        insertIntoAll(book);
        ++loaded;
    }

    ostringstream message;
    message << "Loaded " << loaded << " book(s) from file.";
    if (skipped > 0) message << " Skipped " << skipped << " invalid/duplicate line(s).";
    report = message.str();
    return true;
}

// ------------------------------------------------------------------ operations

bool Library::addBook(int id, const string& title, const string& author, string& message) {
    if (contains(id)) {                                 // Hash Table: O(1) duplicate check
        message = "A book with ID " + to_string(id) + " already exists.";
        return false;
    }
    BookPtr book = make_shared<Book>();
    book->id = id;
    book->title = title;
    book->author = author;
    insertIntoAll(book);                                // Hash + BST + AVL
    message = "Book added successfully.";
    return true;
}

bool Library::updateBook(int id, const string& newTitle, const string& newAuthor,
                         int newId, string& message) {
    BookPtr book = findById(id);                        // Hash Table lookup
    if (!book) {
        message = "No book found with ID " + to_string(id) + ".";
        return false;
    }
    if (newId != id && contains(newId)) {               // new ID must stay unique
        message = "Cannot change ID: another book already uses ID " + to_string(newId) + ".";
        return false;
    }

    if (newId != id) {
        // The ID is the KEY of all three structures, so the book must be removed
        // under the old key and re-inserted under the new one.
        removeFromAll(id);
        book->id = newId;
        insertIntoAll(book);
    }
    // Title/author are not keys; the three structures share the same Book object,
    // so editing it once updates all of them.
    if (!newTitle.empty()) book->title = newTitle;
    if (!newAuthor.empty()) book->author = newAuthor;
    message = "Book updated successfully.";
    return true;
}

bool Library::deleteBook(int id, string& message) {
    BookPtr book = findById(id);
    if (!book) {
        message = "No book found with ID " + to_string(id) + ". Nothing deleted.";
        return false;
    }
    if (book->isIssued) {
        message = "Book is currently issued to " + book->studentName +
                  ". Return it before deleting.";
        return false;
    }
    removeFromAll(id);                                  // Hash + BST + AVL
    message = "Book deleted successfully.";
    return true;
}

bool Library::issueBook(int id, const string& studentId, const string& studentName,
                        string& message) {
    BookPtr book = findById(id);
    if (!book) {
        message = "No book found with ID " + to_string(id) + ".";
        return false;
    }
    if (book->isIssued) {
        message = "Book is already issued to " + book->studentName + " (" + book->studentId + ").";
        return false;
    }
    book->isIssued = true;
    book->studentId = studentId;
    book->studentName = studentName;
    message = "Book issued to " + studentName + " (" + studentId + ").";
    return true;
}

bool Library::returnBook(int id, string& message) {
    BookPtr book = findById(id);
    if (!book) {
        message = "No book found with ID " + to_string(id) + ".";
        return false;
    }
    if (!book->isIssued) {
        message = "This book is not issued, so it cannot be returned.";
        return false;
    }
    message = "Book returned by " + book->studentName + " (" + book->studentId + ").";
    book->isIssued = false;
    book->studentId.clear();
    book->studentName.clear();
    return true;
}

// ------------------------------------------------------------------ queries

BookPtr Library::findById(int id) const {
    int steps = 0;
    return hashTable.search(id, steps);
}

bool Library::contains(int id) const { return findById(id) != nullptr; }

vector<BookPtr> Library::allBooksSorted() const { return avlIndex.inOrder(); }

vector<BookPtr> Library::findByTitle(const string& keyword) const {
    vector<BookPtr> matches;
    string needle = Utils::toLower(Utils::trim(keyword));
    if (needle.empty()) return matches;
    for (const BookPtr& book : avlIndex.inOrder()) {    // results come out sorted by ID
        if (Utils::toLower(book->title).find(needle) != string::npos) matches.push_back(book);
    }
    return matches;
}

vector<BookPtr> Library::availableBooks() const {
    vector<BookPtr> result;
    for (const BookPtr& book : avlIndex.inOrder()) {
        if (!book->isIssued) result.push_back(book);
    }
    return result;
}

vector<BookPtr> Library::issuedBooks() const {
    vector<BookPtr> result;
    for (const BookPtr& book : avlIndex.inOrder()) {
        if (book->isIssued) result.push_back(book);
    }
    return result;
}

// ------------------------------------------------------------------ diagnostics

SearchComparison Library::compareSearch(int id) const {
    SearchComparison result;
    result.hashSteps = result.bstSteps = result.avlSteps = 0;
    result.found = hashTable.search(id, result.hashSteps) != nullptr;
    bstIndex.search(id, result.bstSteps);
    avlIndex.search(id, result.avlSteps);
    return result;
}

bool Library::verifyIntegrity(string& report) const {
    ostringstream text;
    bool ok = true;

    text << "  Books in Hash Table : " << hashTable.size() << "\n";
    text << "  Books in BST        : " << bstIndex.size() << "\n";
    text << "  Books in AVL tree   : " << avlIndex.size() << "\n";
    if (hashTable.size() != bstIndex.size() || bstIndex.size() != avlIndex.size()) {
        text << "  PROBLEM: structure sizes differ!\n";
        ok = false;
    }

    vector<BookPtr> fromBst = bstIndex.inOrder();
    vector<BookPtr> fromAvl = avlIndex.inOrder();
    if (fromBst.size() != fromAvl.size()) {
        ok = false;
    } else {
        for (size_t i = 0; i < fromAvl.size(); ++i) {
            if (fromBst[i] != fromAvl[i]) {              // same sorted order, same objects
                text << "  PROBLEM: BST and AVL differ at position " << i << "\n";
                ok = false;
                break;
            }
        }
    }
    for (const BookPtr& book : fromAvl) {
        if (findById(book->id) != book) {
            text << "  PROBLEM: book " << book->id << " is missing from the Hash Table\n";
            ok = false;
            break;
        }
    }
    if (!avlIndex.isValid()) {
        text << "  PROBLEM: AVL tree is not a valid balanced BST!\n";
        ok = false;
    }
    text << (ok ? "  RESULT: all three data structures are consistent.\n"
                : "  RESULT: inconsistency detected.\n");
    report = text.str();
    return ok;
}
