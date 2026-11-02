# Smart Library Management System (C++ console project)

## Folder structure

```
SmartLibrary/
├── include/              <- header files (class declarations)
│   ├── Book.h            record type shared by all structures
│   ├── Utils.h           safe input / string helpers
│   ├── HashTable.h       hash table (separate chaining)
│   ├── BST.h             plain binary search tree
│   ├── AVLTree.h         self-balancing AVL tree
│   └── Library.h         owns the 3 structures + file I/O + rules
├── src/                  <- implementations
│   ├── main.cpp          console menu only (no logic)
│   ├── Library.cpp
│   ├── HashTable.cpp
│   ├── BST.cpp
│   ├── AVLTree.cpp
│   └── Utils.cpp
├── tests/
│   └── test_structures.cpp   randomized self-test of BST / AVL / Hash Table
├── docs/
│   ├── SAMPLE_IO.md      sample inputs and outputs
│   ├── REPORT.md         mini-project report
│   ├── VIVA.md           viva questions and answers
│   └── sample_books.txt  demo data (copy to books.txt)
├── .vscode/tasks.json    one-click build (Ctrl+Shift+B)
└── README.md
```

`books.txt` (the database) is created automatically in the folder you run the program from.

## Architecture: how the three structures work together

One `Book` object is created on the heap. `Library` stores a `shared_ptr` to it in all three structures:

| Structure | Key | Role in this project |
|---|---|---|
| **Hash Table** (chaining, id % buckets) | Book ID | *Primary lookup*: duplicate check, search by ID, issue, return, update/delete lookup. Average O(1). |
| **AVL Tree** | Book ID | *Sorted index*: display all books in sorted order, filter available/issued, title search scan, saving to file. Always O(log n) height. |
| **BST** (unbalanced) | Book ID | *Comparison index*: search by ID using the ordinary BST; menu option 11 shows how many steps it needs versus the AVL and hash table (it degrades to a list on sorted input). |

**Synchronization:** every add / delete / ID change goes through `Library::insertIntoAll()` and `removeFromAll()`, which touch all three structures. Changing a title/author edits the shared `Book` object once, so all three see it. Menu option 13 verifies that sizes, sort order and object identity match.

### Which structure does each operation use?

| Menu option | Structure(s) used |
|---|---|
| 1 Add book | Hash Table (duplicate check) → insert into Hash + BST + AVL |
| 2 Search by ID | Hash Table |
| 3 Search by title | AVL in-order traversal (case-insensitive substring match, results sorted) |
| 4 Update | Hash Table lookup; title/author edit shared object; ID change = remove + insert in all three |
| 5 Delete | Hash Table lookup → remove from Hash + BST + AVL (AVL rebalances) |
| 6 Issue / 7 Return | Hash Table lookup, then flag change on the shared object |
| 8 / 9 / 10 Display | AVL in-order traversal (sorted by ID) |
| 11 Compare search | Hash vs BST vs AVL step counts |
| 12 Statistics | Hash load factor, BST height, AVL height, rotations, AVL picture |
| 13 Verify | Cross-checks all three |
| Save / load | AVL in-order (sorted) to write; each line inserted into all three on load |

## Compile and run in VS Code on Windows

1. **Install a C++ compiler (once).** Install MSYS2 from https://www.msys2.org, open the *MSYS2 UCRT64* terminal and run `pacman -S mingw-w64-ucrt-x86_64-gcc`. Add `C:\msys64\ucrt64\bin` to your Windows **PATH** (Start → "Edit environment variables").
2. **Install VS Code** and its **C/C++** extension (Microsoft).
3. Close and reopen VS Code, then verify in a terminal (Terminal → New Terminal): `g++ --version`
4. **File → Open Folder…** and choose the `SmartLibrary` folder.
5. **Build:** press `Ctrl+Shift+B` (uses `.vscode/tasks.json`), or in the terminal run:
   ```
   g++ -std=c++17 -Wall -Wextra -Iinclude src/main.cpp src/Library.cpp src/AVLTree.cpp src/BST.cpp src/HashTable.cpp src/Utils.cpp -o library.exe
   ```
6. **Run:** `.\library.exe`  (PowerShell) or `library.exe` (cmd).
7. *(Optional demo data)* before the first run: `copy docs\sample_books.txt books.txt`
8. *(Optional self-test)*:
   ```
   g++ -std=c++17 -Iinclude tests/test_structures.cpp src/AVLTree.cpp src/BST.cpp src/HashTable.cpp -o test_structures.exe
   .\test_structures.exe
   ```

Always run `library.exe` from the project folder, because `books.txt` is looked up in the current directory.

## Input rules (validation)
* Book ID: whole number 1–999999 (duplicates rejected).
* Title / author / student name: not empty, and must not contain `|`.
* Student ID: letters and digits only.
* Menu choice: number 0–13. Wrong input just re-asks; it never crashes.
* A book that is currently issued cannot be deleted.
