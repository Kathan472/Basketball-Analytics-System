// ============================================================
// ReboundEvent.h
//
// Represents a rebound (offensive or defensive).
// Inherits from GameEvent.
// Rebounds always return 0 from getPoints().
// ============================================================
#ifndef REBOUNDEVENT_H
#define REBOUNDEVENT_H

#include "GameEvent.h"
#include <string>
using namespace std;

class ReboundEvent : public GameEvent {
private:
    string rebound_type; // "OFF" (offensive) or "DEF" (defensive)

public:
    ReboundEvent(int team, double gtime, const string& clock, int pid, const string& type);

    virtual ~ReboundEvent() override;

    // --- Getter ---
    string getReboundType() const;

    // --- GameEvent interface ---
    virtual int    getPoints()    const override;   // always 0
    virtual string describe()     const override;
    virtual string getEventType() const override;
};

#endif