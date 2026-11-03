FROM gcc:13

WORKDIR /app

COPY . .

RUN g++ -std=c++17 -O2 -I. \
    web_server.cpp \
    Utils.cpp \
    BST.cpp \
    Library.cpp \
    HashTable.cpp \
    AVLTree.cpp \
    -o smart-library

EXPOSE 10000

CMD ["./smart-library"]
