// ============================================================
// GameEvent.h
//
// Abstract base class for all game events in the simulation.
// ShotEvent, FoulEvent, and ReboundEvent all inherit from this.
//
// Key design decisions:
//   - getPoints() and describe() are pure virtual - every subclass
//     MUST override them with type-specific behavior
//   - Virtual destructor is required to prevent memory leaks when
//     deleting subclass objects through a GameEvent* pointer
//   - All events are stored as GameEvent* in a polymorphic array,
//     so virtual dispatch is the only correct way to call these methods
// ============================================================
#ifndef GAMEEVENT_H
#define GAMEEVENT_H

#include <string>
using namespace std;

class GameEvent {
protected:
    int    team_id; // 1 or 2
    double game_time; // continuous seconds from game start
    string real_clock; // display clock, e.g. "11:45"
    int    player_id; // player involved in this event

public:
    // --- Constructor / Destructor ---
    GameEvent(int team, double gtime, const string& clock, int pid);

    // Virtual destructor - REQUIRED for safe polymorphic deletion
    virtual ~GameEvent();

    // --- Getters (non-virtual - same for all subclasses) ---
    int    getTeamID()    const;
    double getGameTime()  const;
    string getRealClock() const;
    int    getPlayerID()  const;

    // --- Pure virtual interface ---
    // Every subclass must implement these two methods.
   // Enables polymorphic behavior through base-class pointers

    // Returns the point value of this event (0 for misses/rebounds)
    virtual int getPoints() const = 0;

    // Returns a human-readable description of this event
    virtual string describe() const = 0;

    // Returns the event type as a string ("SHOT", "FOUL", "REBOUND")
    virtual string getEventType() const = 0;
};

#endif