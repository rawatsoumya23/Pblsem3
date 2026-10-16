// Unit tests for Enums.h: every name must round-trip, unknown text must throw.
// Build: g++ -std=c++17 -Wall -Wextra -g -Iinclude tests/test_enums.cpp -o build/test_enums
#include "core/Enums.h"

#include <cassert>
#include <iostream>

int main() {
    for (const char* name : kSeverityNames)     assert(std::string(toString(severityFromString(name))) == name);
    for (const char* name : kIncidentTypeNames) assert(std::string(toString(incidentTypeFromString(name))) == name);
    for (const char* name : kStatusNames)       assert(std::string(toString(statusFromString(name))) == name);
    for (const char* name : kCategoryNames)     assert(std::string(toString(categoryFromString(name))) == name);
    for (const char* name : kTeamTypeNames)     assert(std::string(toString(teamTypeFromString(name))) == name);

    assert(Severity::Critical > Severity::Serious);   // higher value = more urgent
    assert(severityFromString("Minor") == Severity::Minor);

    bool threw = false;
    try { severityFromString("critical"); } catch (const std::invalid_argument&) { threw = true; }
    assert(threw);                                      // wrong capitals are rejected

    std::cout << "All Enums tests passed.\n";
    return 0;
}
