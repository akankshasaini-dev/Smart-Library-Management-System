@echo off
cd /d "%~dp0"
echo Building Smart Library web application...
g++ -std=c++17 -Wall -Wextra -Iinclude src/web_server.cpp src/Library.cpp src/AVLTree.cpp src/BST.cpp src/HashTable.cpp src/Utils.cpp -o library_web.exe -lws2_32
if errorlevel 1 (
  echo.
  echo Build failed. Check the compiler errors above.
  pause
  exit /b 1
)
echo.
echo Starting server at http://localhost:8080
echo Keep this window open while using the website.
start "" http://localhost:8080
library_web.exe
pause
