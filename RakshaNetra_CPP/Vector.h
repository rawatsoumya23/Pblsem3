#pragma once
// Vector<T>: a dynamic array written from scratch (no std::vector).
//
// Design decisions (be ready to explain these in the viva):
//  * Storage is a heap array allocated with new T[capacity]. This is simple and
//    safe for students, but it means T must be default-constructible and
//    move-assignable. (std::vector instead uses raw memory + placement new,
//    which avoids that requirement but is much harder to get right.)
//  * Capacity doubles when full (4 -> 8 -> 16 ...), so push_back is
//    O(1) amortised: n pushes cost O(n) element moves in total.
//  * Rule of five: destructor, copy constructor, move constructor, and one
//    assignment operator taking its argument BY VALUE (copy-and-swap), which
//    covers both copy and move assignment and is safe for self-assignment.
//  * Pointers/references into the Vector become invalid after any push_back
//    that triggers growth. Store IDs/indices across operations, not pointers.

#include <cstddef>
#include <stdexcept>
#include <string>
#include <utility>

template <typename T>
class Vector {
public:
    Vector() = default;

    ~Vector() { delete[] data_; }

    // Deep copy: the new Vector owns its own array.
    Vector(const Vector& other) : size_(other.size_), capacity_(other.size_) {
        if (capacity_ == 0) return;
        data_ = new T[capacity_];
        try {
            for (std::size_t i = 0; i < size_; ++i) data_[i] = other.data_[i];
        } catch (...) {          // an element copy threw: don't leak the array
            delete[] data_;
            throw;
        }
    }

    // Move: steal the other Vector's array and leave it empty but valid.
    Vector(Vector&& other) noexcept
        : data_(other.data_), size_(other.size_), capacity_(other.capacity_) {
        other.data_ = nullptr;
        other.size_ = 0;
        other.capacity_ = 0;
    }

    // Copy-and-swap: 'other' is already a copy (or a moved-from temporary),
    // so swapping with it is all that is needed. Self-assignment is safe.
    Vector& operator=(Vector other) noexcept {
        swap(other);
        return *this;
    }

    void swap(Vector& other) noexcept {
        std::swap(data_, other.data_);
        std::swap(size_, other.size_);
        std::swap(capacity_, other.capacity_);
    }

    // Adds a copy of value at the end. O(1) amortised.
    void push_back(const T& value) {
        // Copy FIRST: 'value' might be one of our own elements (v.push_back(v[0])),
        // and grow() would free the memory it refers to.
        T copy(value);
        if (size_ == capacity_) grow();
        data_[size_++] = std::move(copy);
    }

    // Adds value at the end by moving it (needed for move-only types like unique_ptr).
    void push_back(T&& value) {
        T tmp(std::move(value));
        if (size_ == capacity_) grow();
        data_[size_++] = std::move(tmp);
    }

    // Removes the last element. Throws std::underflow_error if empty.
    void pop_back() {
        if (size_ == 0) throw std::underflow_error("Vector::pop_back on empty vector");
        --size_;
        data_[size_] = T{};      // release whatever the old element held
    }

    // Removes the element at index, shifting later elements left. O(n).
    // Throws std::out_of_range if index >= size().
    void removeAt(std::size_t index) {
        checkIndex(index, "removeAt");
        for (std::size_t i = index; i + 1 < size_; ++i) data_[i] = std::move(data_[i + 1]);
        --size_;
        data_[size_] = T{};
    }

    // Removes all elements; keeps the allocated capacity for reuse.
    void clear() {
        for (std::size_t i = 0; i < size_; ++i) data_[i] = T{};
        size_ = 0;
    }

    // Ensures capacity >= newCapacity. Never shrinks.
    void reserve(std::size_t newCapacity) {
        if (newCapacity <= capacity_) return;
        T* bigger = new T[newCapacity];
        try {
            for (std::size_t i = 0; i < size_; ++i) bigger[i] = std::move(data_[i]);
        } catch (...) {
            delete[] bigger;
            throw;
        }
        delete[] data_;
        data_ = bigger;
        capacity_ = newCapacity;
    }

    // Unchecked access: fast, but an invalid index is undefined behaviour.
    T& operator[](std::size_t i) { return data_[i]; }
    const T& operator[](std::size_t i) const { return data_[i]; }

    // Checked access: throws std::out_of_range if i >= size().
    T& at(std::size_t i) { checkIndex(i, "at"); return data_[i]; }
    const T& at(std::size_t i) const { checkIndex(i, "at"); return data_[i]; }

    // Last element. Throws std::underflow_error if empty.
    T& back() {
        if (size_ == 0) throw std::underflow_error("Vector::back on empty vector");
        return data_[size_ - 1];
    }
    const T& back() const {
        if (size_ == 0) throw std::underflow_error("Vector::back on empty vector");
        return data_[size_ - 1];
    }

    std::size_t size() const noexcept { return size_; }
    std::size_t capacity() const noexcept { return capacity_; }
    bool empty() const noexcept { return size_ == 0; }

    // Allow range-based for loops: for (auto& x : v) { ... }
    T* begin() noexcept { return data_; }
    T* end() noexcept { return data_ + size_; }
    const T* begin() const noexcept { return data_; }
    const T* end() const noexcept { return data_ + size_; }

private:
    T* data_ = nullptr;
    std::size_t size_ = 0;
    std::size_t capacity_ = 0;

    void grow() { reserve(capacity_ == 0 ? 4 : capacity_ * 2); }

    void checkIndex(std::size_t i, const char* where) const {
        if (i >= size_)
            throw std::out_of_range(std::string("Vector::") + where + ": index " +
                                    std::to_string(i) + " >= size " + std::to_string(size_));
    }
};
