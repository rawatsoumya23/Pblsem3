#pragma once
#include <cstddef>
#include <stdexcept>

// Stack<T>: Last-In-First-Out (LIFO) container, built from linked nodes.
// Think of a pile of plates: you add on top, and you remove from the top.
//
// Used in RakshaNetra for:
//   - undoing the last action (the most recent action is undone first)
//   - printing a Dijkstra route in the correct order
//
// Every operation here is O(1) (constant time).
template <typename T>
class Stack {
private:
    // One plate in the pile: it stores a value and points to the plate below it.
    struct Node {
        T value;
        Node* next;   // the node below this one (nullptr for the bottom node)
    };

    Node* top_ = nullptr;     // the topmost node; nullptr means the stack is empty
    std::size_t size_ = 0;    // how many items are in the stack

public:
    Stack() = default;

    // Destructor: runs automatically when the stack is destroyed.
    // It deletes every node, so no memory is leaked.
    ~Stack() {
        while (!empty()) pop();
    }

    // Copying is switched off. A plain copy would make two stacks point to the
    // SAME nodes, and both destructors would delete them -> crash (double delete).
    Stack(const Stack&) = delete;
    Stack& operator=(const Stack&) = delete;

    // Put a new item on top.
    void push(const T& value) {
        top_ = new Node{value, top_};   // new node points to the old top
        ++size_;
    }

    // Remove the top item and return it. Throws if the stack is empty.
    T pop() {
        if (empty()) throw std::underflow_error("pop() called on an empty Stack");
        Node* old = top_;        // remember the current top
        T value = old->value;    // save its value
        top_ = old->next;        // the node below becomes the new top
        delete old;              // free the memory of the removed node
        --size_;
        return value;
    }

    // Look at the top item without removing it. Throws if the stack is empty.
    const T& top() const {
        if (empty()) throw std::underflow_error("top() called on an empty Stack");
        return top_->value;
    }

    bool empty() const { return top_ == nullptr; }
    std::size_t size() const { return size_; }
};
