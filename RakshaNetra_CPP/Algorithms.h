#pragma once
#include <cstddef>
#include "ds/Vector.h"

// Sorting and searching, written from scratch.
//
// Used in RakshaNetra for:
//   mergeSort     - worker list sorted by name, zone risk ranking in analytics
//   binarySearch  - finding an item in a list that is already sorted
//   linearSearch  - small or unsorted lists
//
// "less(a, b)" means "a should come BEFORE b".

// ---------------------------------------------------------------------------
// Merge sort: O(n log n) always, and STABLE (equal items keep their order).
// Idea: split the list in half, sort each half, then merge the two sorted halves.
// ---------------------------------------------------------------------------
template <typename T, typename Less>
void mergeSortRange(Vector<T>& v, Vector<T>& tmp, std::size_t lo, std::size_t hi, Less less) {
    if (hi - lo < 2) return;                    // 0 or 1 item: already sorted
    std::size_t mid = lo + (hi - lo) / 2;
    mergeSortRange(v, tmp, lo, mid, less);      // sort left half  [lo, mid)
    mergeSortRange(v, tmp, mid, hi, less);      // sort right half [mid, hi)

    // Merge the two sorted halves into tmp
    std::size_t i = lo, j = mid, k = lo;
    while (i < mid && j < hi) {
        // Take from the RIGHT only if it is strictly smaller -> equal items keep left-first order (stable)
        if (less(v[j], v[i])) tmp[k++] = v[j++];
        else                  tmp[k++] = v[i++];
    }
    while (i < mid) tmp[k++] = v[i++];          // copy what is left of the left half
    while (j < hi)  tmp[k++] = v[j++];          // copy what is left of the right half

    for (std::size_t x = lo; x < hi; ++x) v[x] = tmp[x];   // copy back
}

template <typename T, typename Less>
void mergeSort(Vector<T>& v, Less less) {
    if (v.size() < 2) return;
    Vector<T> tmp(v);                           // working space of the same size
    mergeSortRange(v, tmp, 0, v.size(), less);
}

// ---------------------------------------------------------------------------
// Binary search: O(log n). The Vector MUST already be sorted by key (ascending).
// keyOf(item) gives the key of an item. Returns the index, or -1 if not found.
// ---------------------------------------------------------------------------
template <typename T, typename K, typename KeyOf>
long binarySearch(const Vector<T>& v, const K& key, KeyOf keyOf) {
    std::size_t lo = 0, hi = v.size();          // search in [lo, hi)
    while (lo < hi) {
        std::size_t mid = lo + (hi - lo) / 2;   // written this way to avoid overflow
        if (keyOf(v[mid]) < key)      lo = mid + 1;
        else if (key < keyOf(v[mid])) hi = mid;
        else                          return static_cast<long>(mid);
    }
    return -1;
}

// ---------------------------------------------------------------------------
// Linear search: O(n). Works on any list, sorted or not.
// Returns the index of the first item where match(item) is true, or -1.
// ---------------------------------------------------------------------------
template <typename T, typename Match>
long linearSearch(const Vector<T>& v, Match match) {
    for (std::size_t i = 0; i < v.size(); ++i) {
        if (match(v[i])) return static_cast<long>(i);
    }
    return -1;
}
