#pragma once
#include <cstddef>
#include <string>
#include <utility>   // std::move

// HashMap<K, V>: stores (key -> value) pairs and finds a value by its key
// in O(1) time on average. Example: worker ID 105 -> that worker's record.
//
// How it works ("separate chaining"):
//   - There is an array of "buckets". Each bucket is a small linked list.
//   - A hash function turns the key into a number; that number picks the bucket.
//   - To find a key, we only search ONE short bucket, not the whole data.
//
//   bucket 0: -> (104, Vikas) -> nullptr
//   bucket 1: -> (105, Arjun) -> (113, Neha) -> nullptr    <- two keys in one bucket ("collision")
//   bucket 2: -> nullptr
//   ...
//
// When there are more items than buckets (load factor > 1), the table doubles
// its bucket count and moves every entry to its new bucket ("rehash"), so the
// buckets stay short.
//
// Used in RakshaNetra for:
//   - worker lookup by ID (identity verification, gate check-in, emergency reporter check)
//
// Time complexity:
//   insert, find, erase    O(1) on average, O(n) in the worst case (all keys in one bucket)
//
// Pointers returned by find() stay valid after a rehash, because entries are
// re-linked into new buckets, not copied. They become invalid only when that
// key is erased.

// ---- Hash functions: turn a key into a number ----
inline std::size_t hashKey(int key) {
    return static_cast<std::size_t>(static_cast<unsigned int>(key));   // negative IDs also work
}

inline std::size_t hashKey(const std::string& key) {
    // FNV-1a: mixes every character into the result, so similar names spread out.
    std::size_t h = 2166136261u;
    for (unsigned char c : key) {
        h ^= c;
        h *= 16777619u;
    }
    return h;
}

template <typename K, typename V>
class HashMap {
private:
    struct Entry {
        K key;
        V value;
        Entry* next;   // next entry in the same bucket
    };

    Entry** buckets_ = nullptr;   // array of bucket heads
    std::size_t bucketCount_ = 0; // always a power of two: 8, 16, 32, ...
    std::size_t size_ = 0;        // number of stored entries

    // Which bucket a key belongs to. Because bucketCount_ is a power of two,
    // "& (bucketCount_ - 1)" gives the same result as "% bucketCount_", but faster.
    std::size_t indexFor(const K& key) const {
        return hashKey(key) & (bucketCount_ - 1);
    }

    // Double the number of buckets and re-link every entry into its new bucket.
    void rehash(std::size_t newBucketCount) {
        Entry** newBuckets = new Entry*[newBucketCount]();   // () sets all to nullptr
        for (std::size_t b = 0; b < bucketCount_; ++b) {
            Entry* e = buckets_[b];
            while (e != nullptr) {
                Entry* next = e->next;                        // remember before re-linking
                std::size_t i = hashKey(e->key) & (newBucketCount - 1);
                e->next = newBuckets[i];                      // put e at the front of its new bucket
                newBuckets[i] = e;
                e = next;
            }
        }
        delete[] buckets_;
        buckets_ = newBuckets;
        bucketCount_ = newBucketCount;
    }

public:
    HashMap() : buckets_(new Entry*[8]()), bucketCount_(8) {}

    // Destructor: delete every entry in every bucket, then the bucket array.
    ~HashMap() {
        clear();
        delete[] buckets_;
    }

    // Copying is switched off (same reason as the other containers).
    HashMap(const HashMap&) = delete;
    HashMap& operator=(const HashMap&) = delete;

    // Add a new key. Returns false (and changes nothing) if the key already exists.
    // A worker ID must be unique, so a duplicate is rejected, not overwritten.
    bool insert(const K& key, V value) {
        if (find(key) != nullptr) return false;
        if (size_ + 1 > bucketCount_) rehash(bucketCount_ * 2);   // keep load factor <= 1
        std::size_t i = indexFor(key);
        buckets_[i] = new Entry{key, std::move(value), buckets_[i]};   // add at the front of the bucket
        ++size_;
        return true;
    }

    // Return a pointer to the value for this key, or nullptr if the key is not present.
    V* find(const K& key) {
        for (Entry* e = buckets_[indexFor(key)]; e != nullptr; e = e->next) {
            if (e->key == key) return &e->value;
        }
        return nullptr;
    }
    const V* find(const K& key) const {
        for (const Entry* e = buckets_[indexFor(key)]; e != nullptr; e = e->next) {
            if (e->key == key) return &e->value;
        }
        return nullptr;
    }

    bool contains(const K& key) const { return find(key) != nullptr; }

    // Remove a key. Returns true if it was removed, false if it was not present.
    // Same "prev + cur" idea as LinkedList::removeIf.
    bool erase(const K& key) {
        std::size_t i = indexFor(key);
        Entry* prev = nullptr;
        Entry* cur = buckets_[i];
        while (cur != nullptr) {
            if (cur->key == key) {
                if (prev == nullptr) buckets_[i] = cur->next;   // first in its bucket
                else prev->next = cur->next;                    // middle or last
                delete cur;
                --size_;
                return true;
            }
            prev = cur;
            cur = cur->next;
        }
        return false;
    }

    // Call action(key, value) for every entry. The order is NOT sorted;
    // to show workers sorted by name or ID, copy them into a Vector and sort it.
    template <typename F>
    void forEach(F action) const {
        for (std::size_t b = 0; b < bucketCount_; ++b) {
            for (const Entry* e = buckets_[b]; e != nullptr; e = e->next) {
                action(e->key, e->value);
            }
        }
    }

    // Remove every entry (the bucket array is kept).
    void clear() {
        for (std::size_t b = 0; b < bucketCount_; ++b) {
            Entry* e = buckets_[b];
            while (e != nullptr) {
                Entry* next = e->next;
                delete e;
                e = next;
            }
            buckets_[b] = nullptr;
        }
        size_ = 0;
    }

    std::size_t size() const { return size_; }
    bool empty() const { return size_ == 0; }
    std::size_t bucketCount() const { return bucketCount_; }
};
