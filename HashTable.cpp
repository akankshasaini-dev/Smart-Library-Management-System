// HashTable.cpp - separate chaining. hash(id) = id % bucketCount.
#include "HashTable.h"

#include <algorithm>

using namespace std;

HashTable::HashTable(size_t initialBuckets)
    : buckets(initialBuckets == 0 ? 11 : initialBuckets), itemCount(0) {}

size_t HashTable::indexFor(int id, size_t bucketTotal) {
    return static_cast<size_t>(id) % bucketTotal;   // IDs are always positive
}

bool HashTable::insert(const BookPtr& book) {
    int ignored = 0;
    if (search(book->id, ignored) != nullptr) return false;  // duplicate ID

    // Keep chains short: grow the table when load factor would exceed 0.75.
    if (static_cast<double>(itemCount + 1) / buckets.size() > MAX_LOAD_FACTOR) {
        rehash();
    }
    buckets[indexFor(book->id, buckets.size())].push_back(book);
    ++itemCount;
    return true;
}

BookPtr HashTable::search(int id, int& comparisons) const {
    comparisons = 0;
    const list<BookPtr>& chain = buckets[indexFor(id, buckets.size())];
    for (const BookPtr& book : chain) {
        ++comparisons;
        if (book->id == id) return book;
    }
    return nullptr;
}

bool HashTable::remove(int id) {
    list<BookPtr>& chain = buckets[indexFor(id, buckets.size())];
    for (auto it = chain.begin(); it != chain.end(); ++it) {
        if ((*it)->id == id) {
            chain.erase(it);
            --itemCount;
            return true;
        }
    }
    return false;
}

void HashTable::rehash() {
    size_t newSize = buckets.size() * 2 + 1;
    vector<list<BookPtr>> newBuckets(newSize);
    for (const list<BookPtr>& chain : buckets) {
        for (const BookPtr& book : chain) {
            newBuckets[indexFor(book->id, newSize)].push_back(book);
        }
    }
    buckets.swap(newBuckets);
}

size_t HashTable::longestChain() const {
    size_t longest = 0;
    for (const list<BookPtr>& chain : buckets) longest = max(longest, chain.size());
    return longest;
}

double HashTable::loadFactor() const {
    return static_cast<double>(itemCount) / static_cast<double>(buckets.size());
}
