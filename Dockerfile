FROM gcc:13

WORKDIR /app

COPY . .

RUN g++ -std=c++17 -O2 -Iinclude \
    src/web_server.cpp \
    src/Utils.cpp \
    src/BST.cpp \
    src/Library.cpp \
    src/HashTable.cpp \
    src/AVLTree.cpp \
    -o smart-library

EXPOSE 10000

CMD ["./smart-library"]
