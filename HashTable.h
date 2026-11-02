// HashTable.h - hash table with separate chaining, keyed by Book ID.
#ifndef HASHTABLE_H
#define HASHTABLE_H

#include <cstddef>
#include <list>
#include <vector>

#include "Book.h"

class HashTable {
public:
    explicit HashTable(std::size_t initialBuckets = 11);

    bool insert(const BookPtr& book);                       // false if ID already exists
    BookPtr search(int id, int& comparisons) const;         // average O(1)
    bool remove(int id);                                    // false if ID not found

    std::size_t size() const { return itemCount; }
    std::size_t bucketCount() const { return buckets.size(); }
    std::size_t longestChain() const;
    double loadFactor() const;

private:
    static constexpr double MAX_LOAD_FACTOR = 0.75;

    std::vector<std::list<BookPtr>> buckets; // each bucket is a chain of books
    std::size_t itemCount;

    static std::size_t indexFor(int id, std::size_t bucketTotal);
    void rehash();                                          // grow when too full
};

#endif
