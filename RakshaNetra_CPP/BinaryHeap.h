#pragma once
#include <cstddef>
#include <stdexcept>
#include <utility>   // std::move, std::swap
#include "ds/Vector.h"

// BinaryHeap<T, Less>: a priority queue. top() and pop() always give the item
// with the HIGHEST priority, no matter what order the items were added in.
//
// "Less" is a small comparator object you write yourself:
//     less(a, b) returns true if a has LOWER priority than b.
// This one class gives both kinds of heap:
//     - max-heap (biggest first): less(a, b) = a < b   -> emergencies (most severe first)
//     - min-heap (smallest first): less(a, b) = a > b  -> Dijkstra (shortest distance first)
//
// How it is stored: a complete binary tree kept inside a Vector.
//
//              [0]                 index i has:
//            /     \                 parent      (i - 1) / 2
//         [1]       [2]              left child  2*i + 1
//        /   \     /   \             right child 2*i + 2
//      [3]  [4]  [5]  [6]
//
// Heap rule: every parent has priority >= its children, so the top is at [0].
//
// Time complexity:
//   push, pop     O(log n)   (an item moves up or down at most one level per step)
//   top, size     O(1)
template <typename T, typename Less>
class BinaryHeap {
private:
    Vector<T> a_;   // the tree, stored level by level
    Less less_;     // the comparator

    // A new item was put at position i (the bottom). Move it UP while it has
    // higher priority than its parent.
    void siftUp(std::size_t i) {
        while (i > 0) {
            std::size_t parent = (i - 1) / 2;
            if (!less_(a_[parent], a_[i])) break;   // parent already has >= priority: done
            std::swap(a_[parent], a_[i]);
            i = parent;
        }
    }

    // The item at position i may have lower priority than its children.
    // Move it DOWN, swapping with the higher-priority child each time.
    void siftDown(std::size_t i) {
        std::size_t n = a_.size();
        while (true) {
            std::size_t left = 2 * i + 1;
            std::size_t right = 2 * i + 2;
            std::size_t best = i;
            if (left < n && less_(a_[best], a_[left])) best = left;
            if (right < n && less_(a_[best], a_[right])) best = right;   // right may not exist
            if (best == i) break;   // i is higher than both children: done
            std::swap(a_[i], a_[best]);
            i = best;
        }
    }

public:
    BinaryHeap() = default;
    explicit BinaryHeap(Less less) : less_(less) {}

    // Add an item: put it at the bottom, then move it up to its correct place.
    void push(const T& value) {
        a_.push_back(value);
        siftUp(a_.size() - 1);
    }

    // Look at the highest-priority item without removing it. Throws if empty.
    const T& top() const {
        if (empty()) throw std::underflow_error("top() called on an empty BinaryHeap");
        return a_[0];
    }

    // Remove and return the highest-priority item. Throws if empty.
    // Steps: take the top, move the LAST item to the top, then move it down.
    T pop() {
        if (empty()) throw std::underflow_error("pop() called on an empty BinaryHeap");
        T result = std::move(a_[0]);
        std::size_t last = a_.size() - 1;
        if (last > 0) a_[0] = std::move(a_[last]);   // (skip when only one item: nothing to move)
        a_.pop_back();
        if (!a_.empty()) siftDown(0);
        return result;
    }

    bool empty() const { return a_.empty(); }
    std::size_t size() const { return a_.size(); }
};
