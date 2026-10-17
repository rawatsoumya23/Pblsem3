// Unit tests for Vector<T>.
// Build: g++ -std=c++17 -Wall -Wextra -g -Iinclude tests/test_vector.cpp -o build/test_vector
#include "ds/Vector.h"

#include <iostream>
#include <memory>
#include <string>

static int failures = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        if (!(cond)) {                                                           \
            std::cerr << "FAIL " << __FILE__ << ":" << __LINE__ << "  " #cond "\n"; \
            ++failures;                                                          \
        }                                                                        \
    } while (0)

// Returns true if f() throws exactly an exception of type E.
template <typename E, typename F>
bool throws(F f) {
    try { f(); } catch (const E&) { return true; } catch (...) { return false; }
    return false;
}

static void testEmpty() {
    Vector<int> v;
    CHECK(v.empty());
    CHECK(v.size() == 0);
    CHECK(throws<std::out_of_range>([&] { v.at(0); }));
    CHECK(throws<std::underflow_error>([&] { v.pop_back(); }));
    CHECK(throws<std::underflow_error>([&] { v.back(); }));
}

static void testGrowth() {
    Vector<int> v;
    for (int i = 0; i < 100; ++i) v.push_back(i);   // grows 4 -> 8 -> ... -> 128
    CHECK(v.size() == 100);
    CHECK(v.capacity() >= 100);
    bool allCorrect = true;
    for (int i = 0; i < 100; ++i) if (v[i] != i) allCorrect = false;
    CHECK(allCorrect);
    CHECK(throws<std::out_of_range>([&] { v.at(100); }));
}

static void testDeepCopy() {
    Vector<std::string> a;
    a.push_back("Mohan Singh");
    a.push_back("Kavita Joshi");
    Vector<std::string> b(a);          // copy constructor
    b[0] = "Changed";
    CHECK(a[0] == "Mohan Singh");      // original must be untouched
    CHECK(b.size() == 2);

    Vector<std::string> c;
    c = a;                             // copy assignment
    c.push_back("Extra");
    CHECK(a.size() == 2);
    CHECK(c.size() == 3);
}

static void testSelfAssignment() {
    Vector<std::string> a;
    a.push_back("Tunnel Section");
    Vector<std::string>& sameObject = a;
    a = sameObject;                    // must not destroy its own data
    CHECK(a.size() == 1);
    CHECK(a[0] == "Tunnel Section");
}

static void testMove() {
    Vector<int> a;
    for (int i = 0; i < 10; ++i) a.push_back(i);
    Vector<int> b(std::move(a));       // move constructor
    CHECK(b.size() == 10);
    CHECK(a.empty() && a.capacity() == 0);   // source left empty but valid

    Vector<int> c;
    c = std::move(b);                  // move assignment
    CHECK(c.size() == 10);
    CHECK(c[9] == 9);
}

static void testMoveOnlyType() {
    // unique_ptr cannot be copied; this checks growth uses moves, not copies.
    Vector<std::unique_ptr<int>> v;
    for (int i = 0; i < 20; ++i) v.push_back(std::make_unique<int>(i));
    CHECK(v.size() == 20);
    CHECK(*v[19] == 19);
    v.pop_back();
    v.removeAt(0);
    CHECK(v.size() == 18);
    CHECK(*v[0] == 1);
}

static void testRemoveAt() {
    Vector<int> v;
    v.push_back(10); v.push_back(20); v.push_back(30); v.push_back(40);
    v.removeAt(1);                     // [10, 30, 40]
    CHECK(v.size() == 3);
    CHECK(v[0] == 10 && v[1] == 30 && v[2] == 40);
    v.removeAt(2);                     // remove last: [10, 30]
    CHECK(v.size() == 2 && v.back() == 30);
    CHECK(throws<std::out_of_range>([&] { v.removeAt(5); }));
}

static void testPushOwnElement() {
    // v.push_back(v[0]) when full: the reference points into memory that
    // grow() frees. push_back must copy the value before growing.
    Vector<std::string> v;
    for (int i = 0; i < 4; ++i) v.push_back("a long string that defeats small-string optimisation");
    CHECK(v.size() == v.capacity());   // next push forces growth
    v.push_back(v[0]);
    CHECK(v.size() == 5);
    CHECK(v[4] == v[0]);
}

static void testClearAndReuse() {
    Vector<int> v;
    for (int i = 0; i < 50; ++i) v.push_back(i);
    std::size_t capBefore = v.capacity();
    v.clear();
    CHECK(v.empty());
    CHECK(v.capacity() == capBefore);  // capacity kept for reuse
    v.push_back(7);
    CHECK(v.size() == 1 && v[0] == 7);
}

static void testRangeFor() {
    Vector<int> v;
    for (int i = 1; i <= 5; ++i) v.push_back(i);
    int sum = 0;
    for (int x : v) sum += x;
    CHECK(sum == 15);
}

static void testLarge() {
    Vector<int> v;
    const int n = 100000;
    for (int i = 0; i < n; ++i) v.push_back(i);
    CHECK(v.size() == static_cast<std::size_t>(n));
    CHECK(v[0] == 0 && v[n - 1] == n - 1);
}

int main() {
    testEmpty();
    testGrowth();
    testDeepCopy();
    testSelfAssignment();
    testMove();
    testMoveOnlyType();
    testRemoveAt();
    testPushOwnElement();
    testClearAndReuse();
    testRangeFor();
    testLarge();

    if (failures == 0) {
        std::cout << "All Vector tests passed.\n";
        return 0;
    }
    std::cout << failures << " Vector test(s) FAILED.\n";
    return 1;
}
