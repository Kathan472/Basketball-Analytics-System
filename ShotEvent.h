// ============================================================
// ShotEvent.h
//
// Represents a shot attempt (2PT, 3PT, or FT).
// Inherits from GameEvent.
//
// For field goals (2PT/3PT): point value is determined by
// distance from the basket at the time of the shot:
//   0 -  5 ft  →  1 pt  (layup/dunk zone)
//   5 - 15 ft  →  2 pts (short mid-range)
//  15 - 23 ft  →  3 pts (long mid-range)
//  23 - 30 ft  →  4 pts (beyond arc)
//  30+ ft      →  5 pts (half-court)
//
// For free throws (FT): always worth 1 pt if made.
// Only successful shots contribute points
// ============================================================
#ifndef SHOTEVENT_H
#define SHOTEVENT_H

#include "GameEvent.h"
#include <string>
using namespace std;

class ShotEvent : public GameEvent {
private:
    string shot_type; // "2PT", "3PT", or "FT"
    string outcome; // "MADE" or "MISS"
    double distance; // distance to basket in feet (0.0 for FTs)
    int    zone_value; // 1-5 based on distance (1 for FTs)

    // Computes zone value from distance
    static int computeZone(double dist);

public:
    // Constructor for field goals (distance already computed by Game)
    ShotEvent(int team, double gtime, const string& clock,
              int pid, const string& type, const string& outcome, double distance);

    // Constructor for free throws (distance not applicable)
    ShotEvent(int team, double gtime, const string& clock,
              int pid, const string& outcome);

    virtual ~ShotEvent() override;

    // --- Getters ---
    string getShotType()  const;
    string getOutcome()   const;
    double getDistance()  const;
    int    getZoneValue() const;

    // --- GameEvent interface ---
    // Returns zone_value if MADE, 0 if MISS
    virtual int    getPoints()    const override;
    virtual string describe()     const override;
    virtual string getEventType() const override;
};

#endif