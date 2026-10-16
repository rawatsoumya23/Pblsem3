#pragma once
// Shared enumerations for RakshaNetra.
//
// IMPORTANT: the text names below must match the MySQL ENUM values exactly
// (same spelling, same capitals). The database loader converts the text it
// reads into these types with the ...FromString() functions, which throw
// std::invalid_argument for any unknown text, so bad data is caught at load
// time instead of silently becoming a wrong value.

#include <stdexcept>
#include <string>

// Higher value = more urgent. The max-heap uses this ordering directly.
enum class Severity { Minor = 1, Moderate = 2, Serious = 3, Critical = 4 };
enum class IncidentType { Fire, Collapse, Medical, Vehicle, Fall, Electrical };
enum class IncidentStatus { Pending, Dispatched, Escalated, Resolved };
enum class Category { Worker, Staff, Contractor };
enum class TeamType { Medical, Fire, General };

// ---- Name tables (index = enum value; Severity is offset by 1) ----
inline constexpr const char* kSeverityNames[] = {"Minor", "Moderate", "Serious", "Critical"};
inline constexpr const char* kIncidentTypeNames[] = {"Fire", "Collapse", "Medical",
                                                     "Vehicle", "Fall", "Electrical"};
inline constexpr const char* kStatusNames[] = {"Pending", "Dispatched", "Escalated", "Resolved"};
inline constexpr const char* kCategoryNames[] = {"Worker", "Staff", "Contractor"};
inline constexpr const char* kTeamTypeNames[] = {"Medical", "Fire", "General"};

// ---- Enum -> text ----
inline const char* toString(Severity s)       { return kSeverityNames[static_cast<int>(s) - 1]; }
inline const char* toString(IncidentType t)   { return kIncidentTypeNames[static_cast<int>(t)]; }
inline const char* toString(IncidentStatus s) { return kStatusNames[static_cast<int>(s)]; }
inline const char* toString(Category c)       { return kCategoryNames[static_cast<int>(c)]; }
inline const char* toString(TeamType t)       { return kTeamTypeNames[static_cast<int>(t)]; }

// ---- Text -> enum (throws std::invalid_argument on unknown text) ----
namespace enum_detail {
template <typename E, int N>
E parse(const std::string& text, const char* const (&names)[N], int offset, const char* typeName) {
    for (int i = 0; i < N; ++i)
        if (text == names[i]) return static_cast<E>(i + offset);
    throw std::invalid_argument(std::string("Unknown ") + typeName + ": '" + text + "'");
}
}  // namespace enum_detail

inline Severity severityFromString(const std::string& t) {
    return enum_detail::parse<Severity>(t, kSeverityNames, 1, "severity");
}
inline IncidentType incidentTypeFromString(const std::string& t) {
    return enum_detail::parse<IncidentType>(t, kIncidentTypeNames, 0, "incident type");
}
inline IncidentStatus statusFromString(const std::string& t) {
    return enum_detail::parse<IncidentStatus>(t, kStatusNames, 0, "status");
}
inline Category categoryFromString(const std::string& t) {
    return enum_detail::parse<Category>(t, kCategoryNames, 0, "category");
}
inline TeamType teamTypeFromString(const std::string& t) {
    return enum_detail::parse<TeamType>(t, kTeamTypeNames, 0, "team type");
}
