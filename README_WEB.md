# Smart Library Management System — Web Edition

This web edition retains the original C++ `Library`, BST, AVL Tree and Hash Table implementations. The browser UI is served by a small C++ HTTP server and calls the same `Library` methods through JSON endpoints.

## Windows / MSYS2 UCRT64

1. Open the project folder in File Explorer.
2. Double-click `start_windows.bat` (or run it from a UCRT64 terminal).
3. The script compiles the web server and opens `http://localhost:8080`.
4. Keep the terminal window open while using the site. Press Ctrl+C to stop it.

Manual build command from the project root:

```bash
g++ -std=c++17 -Wall -Wextra -Iinclude src/web_server.cpp src/Library.cpp src/AVLTree.cpp src/BST.cpp src/HashTable.cpp src/Utils.cpp -o library_web.exe -lws2_32
./library_web.exe
```

## Features

- Admin dashboard, catalogue search and filters
- Add, update and delete books
- Issue and return books
- Student catalogue portal (demo student identity: Akanksha / S2025A1)
- Live C++ DSA statistics, synchronization check and search-step comparison
- Persistent data in `books.txt`

## Important

- Do not run the old console `library.exe` at the same time as this web server because both use `books.txt`.
- The student portal currently uses a demonstration student account; this first version is a local college-project prototype, not production authentication.
- The server listens on localhost port 8080. Do not expose it to the public internet.
- The DSA dashboard reports real metrics from the C++ structures. Tree drawings are explanatory in this first version; a future enhancement can expose exact node layouts for live tree rendering.
