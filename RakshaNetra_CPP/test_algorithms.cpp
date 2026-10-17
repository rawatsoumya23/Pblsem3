#include <cassert>
#include <cstdlib>
#include <iostream>
#include <string>
#include "ds/Algorithms.h"

struct Named {
    std::string name;
    int id = 0;
};

int main() {
    auto ascending = [](int a, int b) { return a < b; };
    auto self = [](int x) { return x; };

    // Test 1: empty and single-item lists
    Vector<int> empty;
    mergeSort(empty, ascending);
    assert(empty.empty());
    assert(binarySearch(empty, 5, self) == -1);
    Vector<int> one;
    one.push_back(7);
    mergeSort(one, ascending);
    assert(one[0] == 7);

    // Test 2: normal sort
    Vector<int> v;
    int values[] = {5, 1, 9, 3, 7, 2, 8, 2};
    for (int x : values) v.push_back(x);
    mergeSort(v, ascending);
    int sorted[] = {1, 2, 2, 3, 5, 7, 8, 9};
    for (std::size_t i = 0; i < v.size(); ++i) assert(v[i] == sorted[i]);

    // Test 3: descending order just by changing the comparator
    mergeSort(v, [](int a, int b) { return a > b; });
    assert(v[0] == 9 && v[v.size() - 1] == 1);

    // Test 4: STABLE - equal names keep their original order (by ID here)
    Vector<Named> people;
    people.push_back({"Rohit", 1});
    people.push_back({"Aman", 2});
    people.push_back({"Rohit", 3});
    people.push_back({"Aman", 4});
    mergeSort(people, [](const Named& a, const Named& b) { return a.name < b.name; });
    assert(people[0].id == 2 && people[1].id == 4);   // Aman(2) before Aman(4)
    assert(people[2].id == 1 && people[3].id == 3);   // Rohit(1) before Rohit(3)

    // Test 5: binary search on a sorted list
    Vector<int> ids;
    for (int i = 101; i <= 112; ++i) ids.push_back(i);
    assert(binarySearch(ids, 101, self) == 0);        // first
    assert(binarySearch(ids, 112, self) == 11);       // last
    assert(binarySearch(ids, 107, self) == 6);        // middle
    assert(binarySearch(ids, 100, self) == -1);       // smaller than all
    assert(binarySearch(ids, 999, self) == -1);       // larger than all

    // Test 6: binary search with a key function (search people by name after sorting)
    long idx = binarySearch(people, std::string("Rohit"), [](const Named& n) { return n.name; });
    assert(idx == 2 || idx == 3);

    // Test 7: linear search on an unsorted list
    Vector<int> unsorted;
    unsorted.push_back(40); unsorted.push_back(10); unsorted.push_back(30);
    assert(linearSearch(unsorted, [](int x) { return x == 30; }) == 2);
    assert(linearSearch(unsorted, [](int x) { return x == 99; }) == -1);

    // Test 8: big test - 100000 random numbers end up in order
    Vector<int> big;
    std::srand(7);
    for (int i = 0; i < 100000; ++i) big.push_back(std::rand() % 100000);
    mergeSort(big, ascending);
    for (std::size_t i = 1; i < big.size(); ++i) assert(big[i - 1] <= big[i]);

    std::cout << "All Algorithms tests passed\n";
    return 0;
}
