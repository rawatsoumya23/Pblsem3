#include <cassert>
#include <iostream>
#include <stdexcept>
#include <string>
#include "ds/Stack.h"

int main() {
    // Test 1: a new stack is empty
    Stack<int> s;
    assert(s.empty());
    assert(s.size() == 0);

    // Test 2: last in, first out
    s.push(10);
    s.push(20);
    s.push(30);
    assert(s.size() == 3);
    assert(s.top() == 30);      // top shows the last item pushed
    assert(s.pop() == 30);      // removed in reverse order
    assert(s.pop() == 20);
    assert(s.size() == 1);

    // Test 3: popping an empty stack must throw an error, not crash
    s.pop();                    // removes 10, stack is now empty
    bool threw = false;
    try {
        s.pop();
    } catch (const std::underflow_error&) {
        threw = true;
    }
    assert(threw);

    // Test 4: works with other types too (this is what a template gives us)
    Stack<std::string> route;
    route.push("Main Gate");
    route.push("Admin Office");
    assert(route.pop() == "Admin Office");

    // Test 5: big test - 100000 items in and out
    Stack<int> big;
    for (int i = 0; i < 100000; ++i) big.push(i);
    for (int i = 99999; i >= 0; --i) assert(big.pop() == i);
    assert(big.empty());

    std::cout << "All Stack tests passed\n";
    return 0;
}
