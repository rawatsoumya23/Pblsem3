#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include "core/Enums.h"
#include "ds/Vector.h"

// ---------------------------------------------------------------------------
// Equipment each incident type requires (team decision).
// A team without this item cannot be sent, even as a backup.
//   Fire                        -> Fire Extinguisher
//   Medical, Fall, Electrical   -> Stretcher
//   Collapse, Vehicle           -> Rescue Kit
// ---------------------------------------------------------------------------
inline std::string requiredEquipment(IncidentType type) {
    switch (type) {
        case IncidentType::Fire:       return "Fire Extinguisher";
        case IncidentType::Medical:
        case IncidentType::Fall:
        case IncidentType::Electrical: return "Stretcher";
        case IncidentType::Collapse:
        case IncidentType::Vehicle:    return "Rescue Kit";
    }
    return "";
}

// ---------------------------------------------------------------------------
// RescueTeam: ABSTRACT base class. One row of the MySQL rescue_teams table,
// plus its rows from team_equipment.
//
// Polymorphism: canHandle() is different for each kind of team, and the
// dispatcher calls it without knowing which kind of team it has.
// ---------------------------------------------------------------------------
class RescueTeam {
public:
    RescueTeam(int id, std::string name, int baseZoneId, bool available, int workload)
        : id_(id), name_(std::move(name)), baseZoneId_(baseZoneId),
          available_(available), workload_(workload) {
        if (id_ <= 0) throw std::invalid_argument("Team ID must be positive");
        if (workload_ < 0) throw std::invalid_argument("Workload cannot be negative");
    }

    virtual ~RescueTeam() = default;
    RescueTeam(const RescueTeam&) = delete;
    RescueTeam& operator=(const RescueTeam&) = delete;

    // ---- Each kind of team answers these differently ----
    virtual TeamType type() const = 0;
    virtual bool canHandle(IncidentType incident) const = 0;

    // ---- Equipment ----
    void addEquipment(const std::string& item) { equipment_.push_back(item); }
    bool hasEquipment(const std::string& item) const {
        for (const std::string& e : equipment_) {   // linear search: teams carry only a few items
            if (e == item) return true;
        }
        return false;
    }

    // ---- Availability and workload ----
    // assign() and release() are the ONLY ways to change availability,
    // so a team can never be "busy" without its workload being counted.
    void assign() {
        if (!available_) throw std::logic_error("Team " + name_ + " is already busy");
        available_ = false;
        ++workload_;
    }
    void release() { available_ = true; }

    // ---- Getters ----
    int id() const { return id_; }
    const std::string& name() const { return name_; }
    int baseZoneId() const { return baseZoneId_; }
    bool available() const { return available_; }
    int workload() const { return workload_; }   // jobs handled today

private:
    int id_;
    std::string name_;
    int baseZoneId_;
    bool available_;
    int workload_;
    Vector<std::string> equipment_;
};

class MedicalTeam : public RescueTeam {
public:
    using RescueTeam::RescueTeam;
    TeamType type() const override { return TeamType::Medical; }
    bool canHandle(IncidentType t) const override {
        return t == IncidentType::Medical || t == IncidentType::Fall || t == IncidentType::Electrical;
    }
};

class FireTeam : public RescueTeam {
public:
    using RescueTeam::RescueTeam;
    TeamType type() const override { return TeamType::Fire; }
    bool canHandle(IncidentType t) const override { return t == IncidentType::Fire; }
};

// General teams can be sent to ANY type of incident, which makes them the backup.
// The equipment check still applies (e.g. no fire extinguisher -> not sent to a fire).
class GeneralTeam : public RescueTeam {
public:
    using RescueTeam::RescueTeam;
    TeamType type() const override { return TeamType::General; }
    bool canHandle(IncidentType) const override { return true; }
};

// Factory: create the right team class from the database "specialization" value.
inline std::unique_ptr<RescueTeam> makeTeam(TeamType type, int id, const std::string& name,
                                            int baseZoneId, bool available, int workload) {
    switch (type) {
        case TeamType::Medical: return std::make_unique<MedicalTeam>(id, name, baseZoneId, available, workload);
        case TeamType::Fire:    return std::make_unique<FireTeam>(id, name, baseZoneId, available, workload);
        case TeamType::General: return std::make_unique<GeneralTeam>(id, name, baseZoneId, available, workload);
    }
    throw std::invalid_argument("Unknown team type");
}
