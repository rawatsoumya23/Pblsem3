#pragma once
#include <memory>
#include <stdexcept>
#include <string>
#include "core/Enums.h"

// The PPE (personal protective equipment) a person is wearing at the gate.
struct PPE {
    bool helmet = true;
    bool vest = true;
    bool boots = true;
};

// ---------------------------------------------------------------------------
// Person: ABSTRACT base class for everyone on site.
//
// OOP concepts shown here:
//   Abstraction    - Person cannot be created directly (it has pure virtual functions).
//   Encapsulation  - data is private; it can only be changed through checked functions.
//   Inheritance    - Worker, Staff and Contractor are kinds of Person.
//   Polymorphism   - code that holds a Person* calls category() or
//                    mayWorkAloneInRestrictedZone() and automatically gets the
//                    version for the real type (Worker, Staff or Contractor).
//
// One row of the MySQL "workers" table becomes one Person object.
// ---------------------------------------------------------------------------
class Person {
public:
    Person(int id, std::string name, std::string job, int zoneId, PPE ppe, bool idRevoked)
        : id_(id), name_(std::move(name)), job_(std::move(job)),
          zoneId_(zoneId), ppe_(ppe), idRevoked_(idRevoked) {
        if (id_ <= 0) throw std::invalid_argument("Person ID must be positive");
        if (name_.empty()) throw std::invalid_argument("Person name must not be empty");
    }

    // A base class with virtual functions needs a virtual destructor, so that
    // deleting a Worker through a Person* runs the Worker's destructor too.
    virtual ~Person() = default;

    // Copying a Person through a base-class reference would "slice" off the
    // derived part, so copying is switched off. Persons are stored by pointer.
    Person(const Person&) = delete;
    Person& operator=(const Person&) = delete;

    // ---- Pure virtual: every derived class MUST provide these ----
    virtual Category category() const = 0;
    virtual std::string badge() const = 0;   // badge colour shown at verification

    // ---- Virtual with a default: derived classes MAY change it ----
    // Contractors override this to return false (site safety rule).
    virtual bool mayWorkAloneInRestrictedZone() const { return true; }

    // ---- Getters (read-only access) ----
    int id() const { return id_; }
    const std::string& name() const { return name_; }
    const std::string& job() const { return job_; }
    int zoneId() const { return zoneId_; }
    const PPE& ppe() const { return ppe_; }
    bool idRevoked() const { return idRevoked_; }

    // ---- Setters with checks (encapsulation) ----
    void setZone(int zoneId) {
        if (zoneId <= 0) throw std::invalid_argument("Zone ID must be positive");
        zoneId_ = zoneId;
    }
    void setPPE(PPE ppe) { ppe_ = ppe; }
    void setJob(const std::string& job) {
        if (job.empty()) throw std::invalid_argument("Job must not be empty");
        job_ = job;
    }
    void revokeId() { idRevoked_ = true; }
    void restoreId() { idRevoked_ = false; }

private:
    int id_;
    std::string name_;
    std::string job_;     // job title from the database "role" column (Electrician, Mason, ...)
    int zoneId_;
    PPE ppe_;
    bool idRevoked_;
};

// ---------------------------------------------------------------------------
// Derived classes. Each one gives its own answer to the virtual functions.
// ---------------------------------------------------------------------------
class Worker : public Person {
public:
    using Person::Person;   // reuse the Person constructor
    Category category() const override { return Category::Worker; }
    std::string badge() const override { return "Yellow"; }
};

class Staff : public Person {
public:
    using Person::Person;
    Category category() const override { return Category::Staff; }
    std::string badge() const override { return "Blue"; }
};

class Contractor : public Person {
public:
    using Person::Person;
    Category category() const override { return Category::Contractor; }
    std::string badge() const override { return "Orange"; }
    // Site rule: contractors must not work alone in a restricted zone.
    bool mayWorkAloneInRestrictedZone() const override { return false; }
};

// ---------------------------------------------------------------------------
// Factory: create the right kind of Person from the database "category" value.
// The loader calls this for each row, so the rest of the program never needs
// to know which derived class it is dealing with.
// ---------------------------------------------------------------------------
inline std::unique_ptr<Person> makePerson(Category category, int id, const std::string& name,
                                          const std::string& job, int zoneId, PPE ppe,
                                          bool idRevoked) {
    switch (category) {
        case Category::Worker:     return std::make_unique<Worker>(id, name, job, zoneId, ppe, idRevoked);
        case Category::Staff:      return std::make_unique<Staff>(id, name, job, zoneId, ppe, idRevoked);
        case Category::Contractor: return std::make_unique<Contractor>(id, name, job, zoneId, ppe, idRevoked);
    }
    throw std::invalid_argument("Unknown category");
}
