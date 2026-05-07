// ============================================================
// FoulEvent.h
//
// Represents a shooting foul. Inherits from GameEvent.
//
// FoulEvent::getPoints() returns 0 — always.
//
// Free throw points are credited through the ShotEvent records that immediately follow the foul in play_events.txt.
// If getPoints() returned ft_made here, the simulation loop would
// count those points twice (once here, once per FT ShotEvent).
//
// FoulEvent exists to carry foul metadata for Option 24 (foul summary). It is not a scoring event.
//
// ft_awarded: how many FTs the fouled player is entitled to take.
// Determined by the zone of the shot that was fouled.
// Set in the constructor.
//
// ft_made: how many of those FTs actually went in.
//        Starts at 0. Updated by setFTMade() as each
//        FT MADE record comes through in buildGameEvents().
// ============================================================
#ifndef FOULEVENT_H
#define FOULEVENT_H

#include "GameEvent.h"
#include <string>
using namespace std;

class FoulEvent : public GameEvent {
private:
    int    foul_zone;    // zone value of the shot that was fouled (1-5)
    int    ft_awarded;   // free throws awarded (== foul_zone)
    int    ft_made;      // free throws actually made (built up via setFTMade)
    double distance;     // distance of fouled player from basket

    // Returns zone 1-5 based on distance (same bands as ShotEvent)
    static int zoneFromDist(double dist);

public:
    FoulEvent(int team, double gtime, const string& clock,
              int pid, double distance);

    virtual ~FoulEvent() override;

    // --- Getters ---
    int    getFoulZone()   const;
    int    getFTAwarded()  const;
    int    getFTMade()     const;
    double getDistance()   const;

    // Called by buildGameEvents() to record actual FT results
    void   setFTMade(int made);

    // --- GameEvent interface ---
    // getPoints() ALWAYS returns 0.
    // (FT points come from ShotEvent records, not here.)
    virtual int    getPoints()    const override;
    virtual string describe()     const override;
    virtual string getEventType() const override;
};

#endif